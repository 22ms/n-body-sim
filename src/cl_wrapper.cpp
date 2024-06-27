#include "cl_wrapper.hpp"
#include "utilities.hpp"
#include "kernel.hpp"

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

CLWrapper::CLWrapper(GLFWwindow* window, int* N, unsigned int* posBO)
{
    _N = N;
    _posGLBO = posBO;

    cl_int status = clGetPlatformIDs(1, &_platform, NULL);
    printf("clGetPlatformIDs: %d\n", status);

    status = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_GPU, 1, &_device, NULL);
    printf("clGetDeviceIDs: %d\n", status);

    if (isCLExtensionSupported("cl_khr_gl_sharing")) {
        fprintf(stderr, "cl_khr_gl_sharing is supported.\n");
    } else {
        fprintf(stderr, "cl_khr_gl_sharing is not supported -- sorry.\n");
        return;
    }

    // TODO: I do not know if these properties need to be set, will test when code runs
    cl_context_properties props[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)_platform,
        0
    };

    _context = clCreateContext(props, 1, &_device, NULL, NULL, &status);
    printf("context status: %d\n", status);

    _cmdQueue = clCreateCommandQueue(_context, _device, 0, &status);
    printf("cmd queue status: %d\n", status);

    float timestep = 0.0001;
    float minimum_sq_distance = 0.0001;
    int n_thread = 64;

    _velocities = new vxvyvz [*_N];
    for (int i = 0; i < *_N; i++) {
        _velocities[i].vx = 0.0f;
        _velocities[i].vy = 0.0f;
        _velocities[i].vz = 0.0f;
    }

    _velCLBO = clCreateBuffer(_context, CL_MEM_READ_WRITE, 4*sizeof(float)*(*_N), NULL, &status);
    printf("_velCLBO status: %d\n", status);

    status = clEnqueueWriteBuffer(_cmdQueue, _velCLBO, CL_FALSE, 0, 4*sizeof(float)*(*_N), _velocities, 0, NULL, NULL);
    printf("_velCLBO buffer status: %d\n", status);

    _posCLBO = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, *_posGLBO, &status );
    printf("_posCLBO buffer status: %d\n", status);

    _kernel_1 = new Kernel(_context, _device, "kernels/n_squared.cl", "n_squared");

    status = clSetKernelArg(_kernel_1->getKernel(), 0, sizeof(float), &timestep);
    printf("kernel 1 args 0 status: %d\n", status);

    status = clSetKernelArg(_kernel_1->getKernel(), 1, sizeof(float), &minimum_sq_distance);
    printf("kernel 1 args 1 status: %d\n", status);
    
    status = clSetKernelArg(_kernel_1->getKernel(), 2, sizeof(cl_mem), &_posCLBO);
    printf("kernel 1 args 2 status: %d\n", status);

    status = clSetKernelArg(_kernel_1->getKernel(), 3, sizeof(cl_mem), &_velCLBO);
    printf("kernel 1 args 3 status: %d\n", status);

    status = clSetKernelArg(_kernel_1->getKernel(), 4, n_thread * sizeof(cl_float4), NULL);
    printf("kernel args 5 status: %d\n", status);
    
}

void CLWrapper::simulateTimestep() {
    cl_int status;
    size_t globalWorkSize[3] = { *_N, 1, 1 };
    size_t localWorkSize[3] = { 1, 1, 1 };

    glFinish();
    status = clEnqueueAcquireGLObjects(_cmdQueue, 1, &_posCLBO, 0, NULL, NULL );
    if (status != 0) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
    }

    clFinish(_cmdQueue);

    status = clEnqueueNDRangeKernel(_cmdQueue, _kernel_1->getKernel(), 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (status != 0) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
    }

    clFinish(_cmdQueue);

    clEnqueueReleaseGLObjects(_cmdQueue, 1, &_posCLBO, 0, NULL, NULL);
    if (status != 0) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
    }

    clFinish(_cmdQueue);
}

bool CLWrapper::isCLExtensionSupported(const char* extension)
{
    // see if the extension is bogus:
    if (extension == NULL || extension[0] == '\0')
        return false;

    char* where = (char*)strchr(extension, ' ');
    if (where != NULL)
        return false;

    // get the full list of extensions:
    size_t extensionSize;
    clGetDeviceInfo(_device, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
    char* extensions = new char[extensionSize];
    clGetDeviceInfo(_device, CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);

    for (char* start = extensions;;) {
        where = (char*)strstr((const char*)start, extension);
        if (where == 0) {
            delete[] extensions;
            return false;
        }
        char* terminator = where + strlen(extension); // points to what should be the separator
        if (*terminator == ' ' || *terminator == '\0' || *terminator == '\r' || *terminator == '\n') {
            delete[] extensions;
            return true;
        }
        start = terminator;
    }
}