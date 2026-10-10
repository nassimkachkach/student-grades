@echo off
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -no_logo
if errorlevel 1 exit /b %errorlevel%

if not exist build mkdir build

cl /std:c++17 /EHsc /W4 /nologo /I. /Fo:build\ main.cpp Student.cpp StudentFileReader.cpp RandomStudentGenerator.cpp Sorting.cpp /Fe:build\main.exe
exit /b %errorlevel%