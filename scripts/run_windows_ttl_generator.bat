@echo off
cd /d "%~dp0"
lv2_ttl_generator.exe godot-distrho_dsp.dll
exit /b %errorlevel%
