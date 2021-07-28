#define CL_TARGET_OPENCL_VERSION 200
#include <CL/opencl.h>

#include <fmt/core.h>

#include <array>
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>

int main(char** argv, int argc)
{
    auto status = cl_int{};

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clGetPlatformIDs.html
    // Query the number of platforms
    auto num_platforms = cl_uint{};
    status = clGetPlatformIDs(0, nullptr, &num_platforms);
    if (status != CL_SUCCESS)
    {
        fmt::print("clGetPlatformIDs failed with error code {}\n", status);
        return status;
    }

    // Query all the platforms
    auto platforms = std::vector<cl_platform_id>(num_platforms);
    status = clGetPlatformIDs(num_platforms, platforms.data(), nullptr);
    if (status != CL_SUCCESS)
    {
        fmt::print("clGetPlatformIDs failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clGetPlatformInfo.html
    auto amd_platform = cl_platform_id{};
    constexpr auto amd_platform_vendor = std::string_view{"Advanced Micro Devices, Inc."};
    for (const auto &platform: platforms)
    {
        // Query the size of platform's vendor name
        auto param_value_size = std::size_t{};
        status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, std::size_t{}, nullptr, &param_value_size);
        if (status != CL_SUCCESS)
        {
            fmt::print("clGetPlatformInfo failed with error code {}\n", status);
            return status;
        }

        // Query the platform's vendor name
        auto platform_vendor = std::string{};
        platform_vendor.resize(param_value_size);
        status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, platform_vendor.size(), platform_vendor.data(), nullptr);
        if (status != CL_SUCCESS)
        {
            fmt::print("clGetPlatformInfo failed with error code {}\n", status);
            return status;
        }

        // Remove extra trailing zeros
        platform_vendor.erase(platform_vendor.find_last_not_of('\0') + 1, std::string::npos);

        // Pick the AMD platform
        if (platform_vendor == amd_platform_vendor)
        {
            amd_platform = platform;
            break;
        }
    }

    if (amd_platform == cl_platform_id{})
    {
        fmt::print("Could not find the AMD platform\n");
        return -1;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clGetDeviceIDs.html
    // Query the number of GPUs
    auto num_devices = cl_uint{};
    status = clGetDeviceIDs(amd_platform, CL_DEVICE_TYPE_GPU, cl_uint{}, nullptr, &num_devices);
    if (status != CL_SUCCESS)
    {
        fmt::print("clGetDeviceIDs failed with error code {}\n", status);
        return status;
    }

    // Query all the GPUs
    auto devices = std::vector<cl_device_id>(num_devices);
    status = clGetDeviceIDs(amd_platform, CL_DEVICE_TYPE_GPU, num_devices, devices.data(), nullptr);
    if (status != CL_SUCCESS)
    {
        fmt::print("clGetDeviceIDs failed with error code {}\n", status);
        return status;
    }

    // Pick first available device, for simplicity sake
    const auto device = devices[0];

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clCreateContext.html
    // Create a context on the default device
    const auto context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);
    if (status != CL_SUCCESS)
    {
        fmt::print("clCreateContext failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clCreateProgramWithSource.html
    // Create an OpenCL program
    constexpr std::string_view hello_world =
#include "kernel.cl"
    ;

    constexpr auto strings = std::array{hello_world.data()};
    constexpr auto lengths = std::array{hello_world.size()};
    const auto program = clCreateProgramWithSource(context, 1, const_cast<const char**>(strings.data()), lengths.data(), &status);
    if (status != CL_SUCCESS)
    {
        fmt::print("clCreateProgramWithSource failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clBuildProgram.html
    // Build the OpenCL program
    status = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (status != CL_SUCCESS)
    {
        fmt::print("clBuildProgram failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clCreateKernel.html
    // Extract the kernel from the program
    const auto kernel = clCreateKernel(program, "hello_world", &status);
    if (status != CL_SUCCESS)
    {
        fmt::print("clCreateKernel failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clCreateCommandQueueWithProperties.html
    // Create a queue
    const auto queue = clCreateCommandQueueWithProperties(context, device, nullptr, &status);
    if (status != CL_SUCCESS)
    {
        fmt::print("clCreateCommandQueueWithProperties failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clEnqueueNDRangeKernel.html
    // Enqueue the kernel
    constexpr auto work_dim = cl_uint{3};
    constexpr auto global_size = std::array<std::size_t, work_dim>{1, 1, 1};
    constexpr auto local_size = std::array<std::size_t, work_dim>{1, 1, 1};
    status = clEnqueueNDRangeKernel(queue, kernel, work_dim, nullptr, global_size.data(), local_size.data(), cl_uint{}, nullptr, nullptr);
    if (status != CL_SUCCESS)
    {
        fmt::print("clEnqueueNDRangeKernel failed with error code {}\n", status);
        return status;
    }

    // https://www.khronos.org/registry/OpenCL/sdk/2.0/docs/man/xhtml/clFinish.html
    // Flush the queue and block
    status = clFinish(queue);
    if (status != CL_SUCCESS)
    {
        fmt::print("clEnqueueNDRangeKernel failed with error code {}\n", status);
        return status;
    }

    return 0;
}