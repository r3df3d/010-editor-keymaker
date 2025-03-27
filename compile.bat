@echo off
windres resource.rc -O coff -o resource.o
g++ -O2 -s -o Keygen.exe keygen.cpp resource.o -mwindows -lcomctl32 -lgdi32 -ladvapi32
if %ERRORLEVEL% EQU 0 (echo Done!) else (echo Failed!)
