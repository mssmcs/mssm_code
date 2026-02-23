@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "PS1_SCRIPT=%SCRIPT_DIR%new_app.ps1"

if not exist "%PS1_SCRIPT%" (
  echo Cannot find %PS1_SCRIPT%
  exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%PS1_SCRIPT%" %*
exit /b %ERRORLEVEL%
