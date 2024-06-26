#include "kernel.h"

#include <CL/cl.h>

Kernel::Kernel(cl_context context, cl_device_id device, const char* kernelPath){
    std::string kernelCode;
    std::ifstream kernelFile;
    cl_program program;

    kernelFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        kernelFile.open(kernelPath);
        std::stringstream kernelStream;
        kernelStream << kernelFile.rdbuf();
        kernelFile.close();
        kernelCode = kernelStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    program = clCreateProgramWithSource(context, 1,
                                        (const char**)&kernelCode,
                                        NULL, NULL);

    if (program == NULL)
    {
        std::cerr << "Failed to create CL program from source." << std::endl;
        return;
    }

    cl_int errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              sizeof(buildLog), buildLog, NULL);

        std::cerr << "Error in kernel: " << std::endl;
        std::cerr << buildLog;
        clReleaseProgram(program);
        return;
    }
}

cl_program Kernel::getProgram() {
    return _program;
}