# ECSMS (Experimental / Simulation Control System for Mobile Communications)  

## Building  
1. Create build directory;  
2. Configure project using CMake-GUI or CMake CLI;  
3. Open project in Visual Studio running build/ecsms.sln or pushing "Open Projecct" buttion in CMake-GUI.  

### Build and run example  
Expected that commands performed in the root project directory.  
``` bash
mkdir build && cd build
cmake ..
cmake --build . -j12
./bin/Debug/test_application.exe
```
