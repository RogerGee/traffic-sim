@if '%1'=='' g++ -std=c++11 -Isrc -Isrc/win32 -DTRAFFICSIM_WIN32 -oTrafficSimulator src\*.cpp src\win32\*.cpp -lopengl32 -lglu32 -mwindows
@if '%1'=='debug' g++ -g -std=c++11 -Isrc -Isrc/win32 -DTRAFFICSIM_WIN32 -oTrafficSimulator src\*.cpp src\win32\*.cpp -lopengl32 -lglu32 -lgdi32
