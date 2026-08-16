@echo off
setlocal enabledelayedexpansion
set NVTT="C:\Program Files\NVIDIA Corporation\NVIDIA Texture Tools\nvtt_export.exe"
set COUNT=0
for /r "%~dp0" %%f in (*.png) do (
    set /a COUNT+=1
    echo [!COUNT!] Converting: %%f
    %NVTT% "%%f" -f bc7 --no-mips --export-transfer-function linear -o "%%~dpnf.dds"
)
echo.
echo Total %COUNT% files converted.
pause