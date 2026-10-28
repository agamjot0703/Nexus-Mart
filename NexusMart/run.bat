@echo off
if not exist nexus_mart.exe call build.bat
if not exist nexus_mart.exe exit /b 1
nexus_mart.exe
