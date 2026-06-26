# Latite Client

[![Discord](https://img.shields.io/discord/885656043521179680)](https://discord.gg/GpV3w5tyBs)
[![GitHub release](https://img.shields.io/github/v/release/LatiteClient/Latite)](https://github.com/LatiteClient/Latite/releases/latest)
[![Crowdin](https://badges.crowdin.net/LatiteClient/localized.svg)](https://crowdin.com/project/LatiteClient)
[![Latite Nightly](https://github.com/LatiteClient/Latite/actions/workflows/nightly-build.yml/badge.svg)](https://github.com/LatiteClient/Latite/actions/workflows/nightly-build.yml)

**Latite Client** is a legitimate DLL modification for Minecraft Windows 10/11 Edition featuring a clean UI, 30+ customizable mods, and a powerful plugin system.

![Image of Latite Client mod menu](https://github.com/user-attachments/assets/86ce2fea-af96-4264-93e1-040545912636)

## Features

- **Clean Interface**: Intuitive UI with search and filtering
- **30+ Mods**: Motion Blur, Keystrokes, Zoom, FPS Counter, Toggle Sprint, and more
- **Plugin System**: Add community plugins or create your own using JavaScript/TypeScript
- **Customization**: Accent colors, module positions, keybinds, and font settings
- **[Launcher](https://github.com/LatiteClient/Launcher)**: Opens Minecraft when needed and injects Latite automatically

## Installation

### Recommended Method

1. Download the [Latite Client Launcher](https://github.com/LatiteClient/Launcher/releases/latest/download/Latite.Client.Launcher_x64-setup.exe)
2. Run the installer.
3. Open **Latite Client Launcher**.
4. Press **Launch** to start Minecraft and inject Latite.

### Alternative Options

- [DLL Download](https://github.com/LatiteClient/Latite/releases/latest/download/Latite.dll) (for using Latite with other Injector's)
- **Jiayi Launcher Import**:  
  Use this command (Win + R) to import Latite in Jiayi Launcher:
  ```bash
  jiayi://addmod/https://github.com/LatiteClient/Latite/releases/latest/download/Latite.dll
  ```

[Watch Installation Tutorial](https://youtu.be/h3v849ayuZY)

## Plugins

Enhance your experience with community-made plugins:

```console
.plugin install [plugin-name]
.plugin load [plugin-name]
```

- [Available Plugins](https://latite.net/plugins/)
- [Plugin Documentation](https://latitescripting.github.io)
- [Plugin Repository](https://github.com/LatiteScripting/Scripts)

## Building

### Prerequisites

Install the following before cloning the repository:

- [Git](https://git-scm.com/download/win)
- [Visual Studio 2022 or Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) with the **Desktop development with C++** workload. Make sure the MSVC x64/x86 build tools, a Windows SDK, CMake, and Ninja are selected in the Visual Studio Installer.
- [MinGW-w64](https://www.mingw-w64.org/). Latite uses MinGW's `ld.exe` to embed the files in `assets/` into the DLL. MinGW's `bin` directory, such as `C:\Program Files\mingw64\bin`, must be in your system's PATH.
- `clang-format`. Install it through Visual Studio, LLVM, or another package manager, then make sure `clang-format` is available in your PATH.

### Build From the Command Line

1. Open **x64 Native Tools Command Prompt for VS 2022** or **Developer PowerShell for VS 2022**. A normal terminal will only work if the MSVC developer environment has already been initialized.
2. Confirm that the required tools are available:

```powershell
cl
cmake --version
ninja --version
ld
```

Example output:
```powershell
PS C:\Users\Plextora\source\repos\Latite> cl
Microsoft (R) C/C++ Optimizing Compiler Version 19.44.35222 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

usage: cl [ option... ] filename... [ /link linkoption... ]
PS C:\Users\Plextora\source\repos\Latite> cmake --version
cmake version 4.2.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
PS C:\Users\Plextora\source\repos\Latite> ninja --version
1.13.2
PS C:\Users\Plextora\source\repos\Latite> ld
C:\Program Files\mingw64\bin\ld.exe: no input files
```

3. Clone Latite and enter the repository:

```powershell
git clone git@github.com:LatiteClient/Latite.git
cd Latite
```

4. Configure and build the Release preset:

```powershell
cmake --preset x64-release
cmake --build out/build/x64-release
```

The first configure may take a few minutes while CMake downloads dependencies. The completed DLL should be in:

```text
out/build/x64-release/Latite.dll
```

After changing source files, only the `cmake --build` command normally needs to be run again. CMake automatically regenerates the project when its build files change.

### Other Build Configurations

Replace `x64-release` in both commands and the output path with one of these
presets:

| Preset               | Output                                      | Purpose                                                                    |
| -------------------- | ------------------------------------------- | -------------------------------------------------------------------------- |
| `x64-debug`          | `LatiteDebug.dll` and `LatiteDebug.pdb`     | Development build with minimal optimization and full debugging information |
| `x64-release`        | `Latite.dll`                                | Optimized build intended for normal use                                    |
| `x64-relwithdebinfo` | `Latite.dll` and `Latite.pdb`               | Optimized build with debugging information                                 |
| `x64-nightly`        | `LatiteNightly.dll` and `LatiteNightly.pdb` | Experimental configuration used by the nightly workflow                    |

For example, to create a Debug build:

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

## Contributing

We welcome people to contribute code via making a PR (Pull Request) to the Client or [Launcher](https://github.com/LatiteClient/Launcher). Just make sure to ping us in our [Discord Server](https://discord.gg/GpV3w5tyBs) if we don't get to reviewing your PR in a timely manner :)

### Formatting

Latite uses `clang-format` for C++ formatting. Run formatting commands from the repository root so `clang-format` can find `.clang-format`.

All commits should be formatted using `clang-format`. Therefore, it's highly recommended to install the repository Git hook that checks staged C++ files before committing:

```powershell
git config core.hooksPath tools/hooks
```
#### Useful clang-format commands:

Format one file:

```powershell
clang-format -i src\client\Latite.h
```

Format all Latite source files:

```powershell
Get-ChildItem src -Recurse -Include *.h,*.hpp,*.cpp,*.cxx,*.cc |
    ForEach-Object { clang-format -i $_.FullName }
```

Or from Git Bash:

```bash
find src -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.cc' \) -print0 |
    xargs -0 clang-format -i
```

Check formatting without modifying files:

```powershell
Get-ChildItem src -Recurse -Include *.h,*.hpp,*.cpp,*.cxx,*.cc |
    ForEach-Object { clang-format --dry-run --Werror $_.FullName }
```

Or from Git Bash:

```bash
find src -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.cc' \) -print0 |
    xargs -0 clang-format --dry-run --Werror
```

## FAQ

<details>
<summary>Why is it flagged as a virus?</summary>
This is a false positive due to DLL injection techniques. Latite is completely safe. <a href="https://latite.net/#faq">Learn more</a>
</details>

<details>
<summary>Can I use this on mobile?</summary>
<strong>No.</strong> Check out our Android project <a href="https://atlasclient.net">Atlas Client</a> instead.
</details>

[View Full FAQ](https://latite.net/#faq)

## Community

- [Discord Server](https://discord.gg/GpV3w5tyBs)
- [Twitter](https://twitter.com/LatiteClient)
- [YouTube](https://youtube.com/@LatiteClientMC)

> **Note: These are the only official social medias Latite Client has. If an entity is claiming to be Latite Client and does not have the same socials as the ones listed above, they are impersonating us.**

## License

By using Latite Client, you agree to our [License Terms](https://raw.githubusercontent.com/LatiteClient/Latite/refs/heads/master/LICENSE).

---

**Disclaimer**: Latite Client is not affiliated with Mojang or Microsoft in any way, shape, or form. Use at your own risk on multiplayer servers.
