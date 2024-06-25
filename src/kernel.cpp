#include "kernel.h"

Kernel::Kernel(const char* kernelPath){
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::ifstream kernelFile;
    // ensure ifstream objects can throw exceptions:
    kernelFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        kernelFile.open(kernelPath);
        std::stringstream kernelStream;
        kernelStream << kernelFile.rdbuf();
        kernelFile.close();
        vertexCode = kernelStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vertexCode = vertexCode.c_str();
}