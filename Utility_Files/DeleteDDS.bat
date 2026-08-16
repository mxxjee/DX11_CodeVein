@echo off
setlocal enabledelayedexpansion

set COUNT=0

for /r "%~dp0" %%f in (*.dds) do (
    set /a COUNT+=1
    echo [!COUNT!] Deleting: %%f
    del "%%f"
)

echo.
echo Total %COUNT% files deleted.
pause