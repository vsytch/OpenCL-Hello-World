cmake_minimum_required(VERSION 3.20)

project(opencl-hello-world)

find_package(fmt CONFIG REQUIRED)
find_package(OpenCL REQUIRED)

add_executable(main main.cpp)

set_target_properties(main PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

target_link_libraries(main PRIVATE OpenCL::OpenCL fmt::fmt)
