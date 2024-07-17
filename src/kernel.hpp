#pragma once

typedef struct _cl_context* cl_context;
typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_kernel* cl_kernel;
typedef struct _cl_program* cl_program;

class Kernel // NOTE: as of right now, this encapsulation is a bit over engineered, but I wanted to be consistent with the OpenGL Shader encapsulation
{
public:
    Kernel(cl_context context, cl_device_id device, std::string path, const char* name);
    cl_kernel GetKernel();
private:
    cl_program m_Program;
    cl_kernel m_Kernel;
};