#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

#define WM_TRAY_MSG    (WM_USER + 1)
#define IDM_BLOCK      101
#define IDM_AUTOSTART  102
#define IDM_EXIT       103

static struct {
    HHOOK     hook;
    HANDLE    mutex;
    BOOL      block_shift;
    BOOL      auto_start;
    ULONGLONG down_ts;
} g = { 0 };

// =》注册表开机自启逻辑

const wchar_t* REG_RUN_PATH = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const wchar_t* APP_NAME = L"Caps2Lang";

BOOL IsAutoStartEnabled() {
    HKEY hKey;
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, REG_RUN_PATH, 0, KEY_READ, &hKey);
    if (res != ERROR_SUCCESS) return FALSE;

    WCHAR path[MAX_PATH];
    DWORD size = sizeof(path);
    res = RegQueryValueEx(hKey, APP_NAME, NULL, NULL, (LPBYTE)path, &size);
    RegCloseKey(hKey);
    return (res == ERROR_SUCCESS);
}

void SetAutoStart(BOOL enable) {
    HKEY hKey;
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, REG_RUN_PATH, 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS) return;

    if (enable) {
        WCHAR path[MAX_PATH];
        GetModuleFileName(NULL, path, MAX_PATH);
        RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (LPBYTE)path, (lstrlen(path) + 1) * sizeof(WCHAR));
    }
    else {
        RegDeleteValue(hKey, APP_NAME);
    }
    RegCloseKey(hKey);
}


static void SendKeyStroke(BYTE vk) {
    INPUT in[2] = { 0 };
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = vk;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (!(p->flags & LLKHF_INJECTED)) {
            // 处理 CapsLock 键
            if (p->vkCode == VK_CAPITAL) {
                if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                    if (!g.down_ts) g.down_ts = GetTickCount64();
                    return 1;
                }
                if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                    ULONGLONG dur = GetTickCount64() - g.down_ts;
                    g.down_ts = 0;
                    // 长按超过 500ms 触发原生 CapsLock，短按触发 Left Shift
                    SendKeyStroke(dur > 500 ? VK_CAPITAL : VK_LSHIFT);
                    return 1;
                }
            }
            // 拦截系统 Shift (如果开启了拦截)
            if (g.block_shift && (p->vkCode == VK_LSHIFT || p->vkCode == VK_RSHIFT)) {
                return 1;
            }
        }
    }
    return CallNextHookEx(g.hook, nCode, wParam, lParam);
}

// --- 窗口及托盘逻辑 ---

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        g.auto_start = IsAutoStartEnabled(); // 启动时同步注册表状态
        NOTIFYICONDATA nid = { sizeof(nid), hWnd, 1 };
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAY_MSG;
        nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
        lstrcpyn(nid.szTip, L"Caps2Lang - 运行中", 64);
        Shell_NotifyIcon(NIM_ADD, &nid);
        break;
    }
    case WM_TRAY_MSG:
        if (lp == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            if (hMenu) {
                AppendMenu(hMenu, g.block_shift ? MF_CHECKED : MF_UNCHECKED, IDM_BLOCK, L"禁用系统 Shift 切换");
                AppendMenu(hMenu, g.auto_start ? MF_CHECKED : MF_UNCHECKED, IDM_AUTOSTART, L"开机自启动");
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, IDM_EXIT, L"退出程序");

                SetForegroundWindow(hWnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
                PostMessage(hWnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);
            }
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDM_BLOCK:
            g.block_shift = !g.block_shift;
            break;
        case IDM_AUTOSTART:
            g.auto_start = !g.auto_start;
            SetAutoStart(g.auto_start);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
        break;
    case WM_DESTROY: {
        NOTIFYICONDATA nid = { sizeof(nid), hWnd, 1 };
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    }
    default: return DefWindowProc(hWnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmd, int nShow) {
    // 1. 单实例保证
    g.mutex = CreateMutex(NULL, TRUE, L"Global\\Caps2Lang_SingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);


    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"C2L_WND";
    if (!RegisterClass(&wc)) return 0;

    HWND hWnd = CreateWindowEx(0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInst, NULL);
    if (!hWnd) return 0;

    g.hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInst, 0);
    if (!g.hook) {
        MessageBox(NULL, L"无法安装键盘钩子，请尝试以管理员身份运行。", L"错误", MB_ICONERROR);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理
    if (g.hook) UnhookWindowsHookEx(g.hook);
    if (g.mutex) {
        ReleaseMutex(g.mutex);
        CloseHandle(g.mutex);
    }
    return 0;
}