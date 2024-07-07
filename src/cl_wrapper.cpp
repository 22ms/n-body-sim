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

void CLWrapper::Initialize(GLFWwindow* window, unsigned int* posBO, vxvyvz* velocities, int* N, float* timeScale)
{
    m_PosGLBO = posBO;
    m_Velocities = velocities;
    m_N = N;
    m_TimeScale = timeScale;

    cl_int status = clGetPlatformIDs(1, &m_Platform, NULL);
    printf("clGetPlatformIDs: %d\n", status);

    status = clGetDeviceIDs(m_Platform, CL_DEVICE_TYPE_GPU, 1, &m_Device, NULL);
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
        CL_CONTEXT_PLATFORM, (cl_context_properties)m_Platform,
        0
    };

    m_Context = clCreateContext(props, 1, &m_Device, NULL, NULL, &status);
    printf("context status: %d\n", status);

    m_CmdQueue = clCreateCommandQueue(m_Context, m_Device, 0, &status);
    printf("cmd queue status: %d\n", status);

    m_VelCLBO = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 4*sizeof(float)*(*m_N), NULL, &status);
    printf("_velCLBO status: %d\n", status);

    status = clEnqueueWriteBuffer(m_CmdQueue, m_VelCLBO, CL_FALSE, 0, 4*sizeof(float)*(*m_N), m_Velocities, 0, NULL, NULL);
    printf("_velCLBO buffer status: %d\n", status);

    m_PosCLBO = clCreateFromGLBuffer(m_Context, CL_MEM_READ_WRITE, *m_PosGLBO, &status );
    printf("_posCLBO buffer status: %d\n", status);

    m_NSquared = new Kernel(m_Context, m_Device, "kernels/n_squared.cl", "n_squared");  
}

void CLWrapper::SimulateTimestep() {
    float timestep = 0.0001 * (*m_TimeScale);
    float minimumSqDistance = 0.001;

    size_t globalWorkSize[3] = { (size_t)(*m_N), 1, 1 };
    size_t localWorkSize[3] = { 1024, 1, 1 };

    cl_int status = clSetKernelArg(m_NSquared->GetKernel(), 0, sizeof(float), &timestep);
    if (status != 0) {
        printf("kernel 1 args 0 status: %d\n", status);
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 1, sizeof(float), &minimumSqDistance);
    if (status != 0) {
        printf("kernel 1 args 1 status: %d\n", status);
    }
    
    status = clSetKernelArg(m_NSquared->GetKernel(), 2, sizeof(cl_mem), &m_PosCLBO);
    if (status != 0) {
        printf("kernel 1 args 2 status: %d\n", status);
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 3, sizeof(cl_mem), &m_VelCLBO);
    if (status != 0) {
        printf("kernel 1 args 3 status: %d\n", status);
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 4, localWorkSize[0] * sizeof(cl_float4), NULL);
    if (status != 0) {
        printf("kernel args 5 status: %d\n", status);
    }

    glFinish();
    status = clEnqueueAcquireGLObjects(m_CmdQueue, 1, &m_PosCLBO, 0, NULL, NULL );
    if (status != 0) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
    }

    clFinish(m_CmdQueue);

    status = clEnqueueNDRangeKernel(m_CmdQueue, m_NSquared->GetKernel(), 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (status != 0) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
    }

    clFinish(m_CmdQueue);

    clEnqueueReleaseGLObjects(m_CmdQueue, 1, &m_PosCLBO, 0, NULL, NULL);
    if (status != 0) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
    }

    clFinish(m_CmdQueue);
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
    clGetDeviceInfo(m_Device, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
    char* extensions = new char[extensionSize];
    clGetDeviceInfo(m_Device, CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);

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