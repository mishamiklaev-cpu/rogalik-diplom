@echo off
setlocal

REM Path to clang-format
set CF="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-format.exe"

REM Format project files only
cd /d "%~dp0Rogalique"

for %%f in (*.cpp *.h) do (
    echo Formatting %%f
    %CF% -i -style=file %%f
)

echo Done!
pause
