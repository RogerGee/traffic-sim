@g++ -std=c++11 -Isrc -Isrc/win32 -DTRAFFICSIM_WIN32 -oTrafficSimulator src\*.cpp src\win32\*.cpp -lopengl32 -mwindows
