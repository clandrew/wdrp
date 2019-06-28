@echo on
setlocal

set ZIP_UTILITY="E:\Program Files\7-Zip\7z.exe"

del Distribute.zip
rmdir /S /Q Distribute

mkdir Distribute
mkdir Distribute\DiscordRichPresence

copy DiscordRuntime\discord-rpc.dll Distribute\DiscordRichPresence\discord-rpc.dll
copy DiscordRuntime\send-presence.exe Distribute\DiscordRichPresence\send-presence.exe
copy settings.ini Distribute\DiscordRichPresence\settings.ini
copy Release\DiscordRichPresence.dll Distribute\gen_DiscordRichPresence.dll


%ZIP_UTILITY% a Distribute.zip Distribute\
