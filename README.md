# Caps2Lang

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-0078d7.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)
![Memory](https://img.shields.io/badge/RAM-%3C1MB-green.svg)
![Status](https://img.shields.io/badge/Status-Stable-brightgreen.svg)
🎮 为游戏玩家设计的 CapsLock 极简中英切换工具，解决全屏游戏切输入法卡顿问题。 
🚀 A tiny, lag-free CapsLock-to-Language switcher for gamers. Stop stuttering when switching IM in full-screen games.

### 为什么需要它？

在《CS2》、《Valorant》等全屏游戏中，系统默认的输入法切换逻辑（如 Shift 或 Win+Space）经常会触发系统级 UI，导致游戏瞬时卡顿甚至切回桌面。本工具通过底层 Hook 将 CapsLock 模拟为硬切换指令，实现无感状态转换。

### 功能特性

* **短按 CapsLock**：强制切换中/英（模拟 Left Shift 切换）。
* **长按 CapsLock ( >500ms )**：触发原生的“大小写锁定”功能，不破坏原有习惯。
* **屏蔽物理 Shift**：可选禁用系统原生的 Shift 切换逻辑，彻底杜绝游戏内误触导致的输入法弹窗。
* **极度轻量**：纯 C 语言编写，内存占用 <1MB，无安装包，单执行文件运行。
* **开机自启**：支持右键菜单一键设置。

### 使用方法

1. 下载 `Caps2Lang.exe`。
2. 直接运行。程序会静默运行在系统托盘（右下角）。
3. **注意**：建议右键以**管理员身份运行**，否则在某些高权限的游戏窗口中键盘钩子可能失效。

### 托盘菜单

右键点击托盘图标：

* **禁用系统 Shift 切换**：开启后，按左/右 Shift 键将不再触发输入法切换。
* **开机自启动**：自动写入当前用户的注册表启动项。
* **退出**：完全卸载键盘钩子并关闭程序。

### 编译环境

* **语言**：C (Win32 API)
* **编译器**：Visual Studio (推荐) 或 MinGW
* **链接库**：`user32.lib`, `shell32.lib`, `advapi32.lib`
* **字符集**：Unicode

### 免责声明

本工具使用全局键盘钩子（WH_KEYBOARD_LL）捕获按键，不涉及内存注入。但在极少数反作弊系统严苛的游戏中，请自行评估使用风险。

### 许可证

MIT

### VirusTotal扫描结果
[![VirusTotal Scan](https://img.shields.io/badge/VirusTotal-Scan_Result-blue)](https://www.virustotal.com/gui/file/87f3d9115a11121de1748001f5180300a0614fa325ccb960ed3d302c4d588d96?nocache=1)

# Caps2Lang
🎮 A tiny, lag-free CapsLock switcher for gamers — stop IM stutters in full-screen games.

## Why This?
In full-screen games like *CS2* or *Valorant*, default OS input method (IM) switches (e.g., Shift or Win+Space) often trigger system UI pops — causing game lag, frame drops, or even desktop switches. This tool uses a low-level keyboard hook to turn CapsLock into a "hard switch" command, so you can toggle languages without disrupting gameplay.


## Features
- **Short-press CapsLock (<500ms):** Force-switch between Chinese/English (simulates Left Shift, ignores current IM state).
- **Long-press CapsLock (>500ms):** Trigger native Caps Lock (no breaking old habits).
- **Optional Shift Block:** Disable OS-native Shift-based IM switching — no more accidental IM pops mid-game.
- **Extremely Lightweight:** Written in pure C (Win32 API), <1MB RAM usage, no installer (single .exe).
- **Auto-Start:** One-click setup via tray menu.


## How to Use
1. Download `Caps2Lang.exe`.
2. Run it (right-click → *Run as administrator* — required for hook access in high-privilege game windows).
3. The app runs silently in your system tray (bottom-right corner).


## Tray Menu (Right-Click Icon)
- **Block System Shift Switch:** Enable to stop Left/Right Shift from triggering IM changes.
- **Auto-Start on Boot:** Adds the app to your user-level startup registry.
- **Exit:** Unhooks the keyboard listener and closes the app.


## Build Environment
- **Language:** C (Win32 API)
- **Compiler:** Visual Studio (recommended) or MinGW
- **Libraries Linked:** `user32.lib`, `shell32.lib`, `advapi32.lib`
- **Character Set:** Unicode


## Disclaimer
This tool uses a global keyboard hook (`WH_KEYBOARD_LL`) — no memory injection is involved. However, use at your own risk in games with strict anti-cheat systems (some may flag hook-based tools).


## License
MIT License

## VirusTotal scan results
[![VirusTotal Scan](https://img.shields.io/badge/VirusTotal-Scan_Result-blue)](https://www.virustotal.com/gui/file/87f3d9115a11121de1748001f5180300a0614fa325ccb960ed3d302c4d588d96?nocache=1)
