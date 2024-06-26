#ifndef KERNEL_H
#define KERNEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

typedef struct _cl_context* cl_context;
typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_program* cl_program;

class Kernel // NOTE: as of right now, this encapsulation is a bit over engineered, but I wanted to be consistent with the OpenGL Shader encapsulation
{
public:
    Kernel(cl_context context, cl_device_id device, const char* kernelPath);
    cl_program getProgram();
private:
    cl_program _program = NULL;
};

#endif