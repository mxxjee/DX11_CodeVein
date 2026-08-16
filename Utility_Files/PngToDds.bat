@echo off
setlocal enabledelayedexpansion
set NVTT="C:\Program Files\NVIDIA Corporation\NVIDIA Texture Tools\nvtt_export.exe"
set COUNT=0
for /r "%~dp0" %%f in (*.png) do (
    set /a COUNT+=1
    set "FNAME=%%~nf"
    set "FORMAT=bc7"
    if "!FNAME:~-2!"=="_N" set "FORMAT=bc5"
    echo [!COUNT!] Converting: %%f  [!FORMAT!]
    %NVTT% "%%f" -f !FORMAT! --export-transfer-function linear -o "%%~dpnf.dds"
    if errorlevel 1 echo        [ERROR] Failed: %%f
)
echo.
echo Total %COUNT% files converted.
pause