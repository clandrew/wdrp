@echo on
setlocal

set ZIP_UTILITY="E:\Program Files\7-Zip\7z.exe"


mkdir Distribute
copy Release\DiscordRichPresence.dll Distribute\gen_DiscordRichPresence.dll

%ZIP_UTILITY% a Distribute.zip Distribute\
