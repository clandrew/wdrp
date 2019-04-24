@echo off
setlocal

:: This script is available as a convenience to deploy the plugin locally for testing as part of the build operation.

:: Enable the copying behavior below
:: set DEPLOY_PLUGIN_LOCALLY=1
set DEPLOY_PLUGIN_LOCALLY=0
if NOT "%DEPLOY_PLUGIN_LOCALLY%"=="1" goto :end

:: Set the environment variables below as needed.
set REPOSITORY_DIR=C:\Users\Claire\source\repos\wdrp
set BUILD_CONFIGURATION=Debug
set WINAMP_INSTALL_DIR=E:\Program Files (x86)\Winamp
set DISCORD_RPC_LIBRARY_DIR=C:\Users\Claire\source\repos\Resources\discord-rpc
set COPY_SYMBOLS=0

set BUILT_BINARY_DIR=%REPOSITORY_DIR%\%BUILD_CONFIGURATION%
set WINAMP_PLUGINS_DIR=%WINAMP_INSTALL_DIR%\Plugins

copy  /Y "%BUILT_BINARY_DIR%\DiscordRichPresence.dll" "%WINAMP_PLUGINS_DIR%\gen_DiscordRichPresence.dll"

if NOT "%COPY_SYMBOLS%"=="1" goto :afterCopySymbols
copy  /Y "%BUILT_BINARY_DIR%\DiscordRichPresence.pdb" "%WINAMP_PLUGINS_DIR%\gen_DiscordRichPresence.pdb"
:afterCopySymbols

mkdir "%WINAMP_PLUGINS_DIR%\DiscordRichPresence"
copy /Y "%DISCORD_RPC_LIBRARY_DIR%\win32-dynamic\bin\*.*" "%WINAMP_PLUGINS_DIR%\DiscordRichPresence"

:end
