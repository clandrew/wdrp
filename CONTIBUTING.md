# Contributing

The plugin is built using Visual Studio 2017 on a Windows 10 environment. It uses the v141 toolset and SDK version 10.0.17134.0. Other environments might work too.

# Pre-requisites

### C++ Build Tools
You will need the C++ compiler. The compiler is included with the Build Tools for Visual Studio (located under "Tools for Visual Studio 2019")

https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019

When the installer is shown, select:

- C++ CMake tools for Windows (this should also select MSVC v142 - VS 2019 C++ x64/x86 build tools)
- Windows 10 SDK (10.0.17134.0)
- MSVC v141 - VS 2017 C++ x64/x86 build tools (v14.16)

### Discord RPC

The discord-rpc library is required for the header files and the binaries that will be included in the distribution output.  The `win32-dynamic` located in the release is what is to be used, extracted to the `discord-rpc` folder of this repo.  

https://github.com/discordapp/discord-rpc

### Winamp Header Files

We need to communicate with Winamp. The main file needed is `wa_ipc.h` which is included in the SDK of Winamp.  

http://download.nullsoft.com/winamp/plugin-dev/WA5.55_SDK.exe

By default this is extracted to C:\Program Files (x86)\Winamp SDK\

#### Winamp Misc

The Winamp communication is documented: http://wiki.winamp.com/wiki/Articles#Plug-in_Articles
An upcoming and interesting Winamp SDK is *WACUP (WinAmp Community Update Project)*: https://getwacup.com/

### 7-Zip
To archive a release build and use the `CreateRelease.cmd` you will need 7-zip installed and the path configured correctly via the `ZIP_UTILITY`.

# Building 

## Visual Studio

Visual Studio 2017 project file is included with the project, `DiscrodRichPresence.vcxproj`

## build.bat

A batch file exists that calls cl.exe and rc.exe, `build.bat` which is used by VS Code to build but can be used without VS Code.

## VS Code

Opening the `DiscordRichPresence` sub-folder as the workspace will enable VS Code to build the project 'automatically' via the presence of `tasks.json` under `.vscode`.

The default build command using CTRL+SHIFT+B (or clicking Build in bottom left) will build in Release mode and output the dll with no further action.  To build in Debug or use the "Copy to Installation" script, use the `> Tasks: Run Task` command to see the other build options.

### Extensions
Microsoft C++ extension is highly recommend (required?) to install: https://code.visualstudio.com/docs/languages/cpp#_install-the-microsoft-cc-extension
To create the build tasks, easy-cpp was used: https://marketplace.visualstudio.com/items?itemName=ACharLuk.easy-cpp-projects

### Additional
Default build is setup via the `.vscode` folder allowing for CTRL+SHIFT+B to build the project or using the other build commands via the command interface in VS Code

A 'Hello World' starting page is located: https://code.visualstudio.com/docs/cpp/config-msvc 
