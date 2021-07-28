### Clone the repo
* `git clone https://github.com/vsytch/OpenCL-Hello-World.git --recursive`

### Build steps
* Launch command prompt:
    * `cmd.exe`

* Initialize x64 MSVC:
    * `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat x64`

* Initialize vcpkg and install required packages:
    * `.\vcpkg\bootstrap-vcpkg.bat`
    * `.\vcpkg\vcpkg install opencl:x64-windows`
    * `.\vcpkg\vcpkg install fmt:x64-windows`

* Configure CMake:
    * `mkdir build && cd build`
    * `cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake ..`

* Build the project:
    * `cmake --build . --parallel`

### Run the example:
* Enable verbose logging for the AMD OpenCL driver
    * `set AMD_LOG_LEVEL=5`
* `.\main.exe`
