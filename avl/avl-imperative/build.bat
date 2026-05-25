@echo off
g++ -std=c++17 -Wall -o avl src/main.cpp src/avl.cpp
if %errorlevel% == 0 (
    echo Build successful! Run with: avl.exe
) else (
    echo Build failed.
)
pause
