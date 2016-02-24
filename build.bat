@echo off

if exist resources.res set EX=resources.res
if not exist resources.res set EX=""

if '%1'=='' g++ -std=c++11 -Isrc -Isrc/win32 -DTRAFFICSIM_WIN32 -oTrafficSimulator src\*.cpp src\win32\*.cpp %EX% -lopengl32 -lglu32 -lcomctl32 -mwindows
if '%1'=='debug' g++ -g -std=c++11 -Isrc -Isrc/win32 -DTRAFFICSIM_WIN32 -oTrafficSimulatorDebug src\*.cpp src\win32\*.cpp %EX% -lopengl32 -lglu32 -lgdi32 -lcomctl32
if '%1'=='res' windres resources.rc -o resources.res --output-format=coff
