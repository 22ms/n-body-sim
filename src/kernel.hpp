#ifndef KERNEL_H
#define KERNEL_H

#define CL_TARGET_OPENCL_VERSION 120

typedef struct _cl_context* cl_context;
typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_kernel* cl_kernel;
typedef struct _cl_program* cl_program;

class Kernel // NOTE: as of right now, this encapsulation is a bit over engineered, but I wanted to be consistent with the OpenGL Shader encapsulation
{
public:
    Kernel(cl_context context, cl_device_id device, const char* path, const char* name);
    cl_kernel getKernel();
private:
    cl_program _program;
    cl_kernel _kernel;
};

#endif