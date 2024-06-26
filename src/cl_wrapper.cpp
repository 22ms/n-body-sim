#include "cl_wrapper.h"
#include "utilities.h"
#include "kernel.h"

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

CLWrapper::CLWrapper(GLFWwindow* window, int* N, unsigned int* posBO)
{
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

    _velocities = new xyz [*N];
    _velCLBO = clCreateBuffer(_context, CL_MEM_READ_WRITE, 3*sizeof(float)*(*N), NULL, &status);
    printf("_velCLBO status: %d\n", status);

    status = clEnqueueWriteBuffer(_cmdQueue, _velCLBO, CL_FALSE, 0, 3*sizeof(float)*(*N), _velocities, 0, NULL, NULL);
    printf("_velCLBO buffer status: %d\n", status);

    _posCLBO = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, *_posGLBO, &status );
    printf("_posCLBO buffer status: %d\n", status);

    _kernel_1 = Kernel(_context, _device, "kernels/test.cl");
    
}

void CLWrapper::simulateTimestep() {
    
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