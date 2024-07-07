#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <CL/cl.h>

#include "kernel.hpp"

Kernel::Kernel(cl_context context, cl_device_id device, const char* path, const char* name){
    cl_int status;
    std::string code;
    std::ifstream file;

    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        std::terminate();
    }

    m_Program = clCreateProgramWithSource(context, 1,
                                        (const char**)&code,
                                        NULL, &status);

    if (status != 0)
    {
        std::printf("Failed to create CL program from source. Error code: %d\n", status);
        std::terminate();
    }

    status = clBuildProgram(m_Program, 1, &device, NULL, NULL, NULL);
    if (status != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(m_Program, device, CL_PROGRAM_BUILD_LOG,
                              sizeof(buildLog), buildLog, NULL);

        std::cerr << "Error in kernel: " << std::endl;
        std::cerr << buildLog;
        std::terminate();
    }

    m_Kernel = clCreateKernel(m_Program, name, &status);
    if (status != 0)
    {
        std::printf("Failed to create CL kernel. Error code: %d\n", status);
        std::terminate();
    }
}

cl_kernel Kernel::GetKernel() {
    return m_Kernel;
}