#define CL_TARGET_OPENCL_VERSION 120

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "cl_wrapper.hpp"
#include "kernel.hpp"
#include "utilities.hpp"

void CLWrapper::Initialize(GLFWwindow* window, unsigned int* posBO, vxvyvz* velocities, int* N, float* timeScale)
{
    m_PosGLBO = posBO;
    m_Velocities = velocities;
    m_N = N;
    m_TimeScale = timeScale;
    m_PreviousN = *m_N;

    cl_int status = clGetPlatformIDs(1, &m_Platform, NULL);
    if (status != CL_SUCCESS) {
        printf("clGetPlatformIDs: %d\n", status);
        std::terminate();
    }

    status = clGetDeviceIDs(m_Platform, CL_DEVICE_TYPE_GPU, 1, &m_Device, NULL);
    if (status != CL_SUCCESS) {
        printf("clGetDeviceIDs: %d\n", status);
        std::terminate();
    }

    if (!isCLExtensionSupported("cl_khr_gl_sharing")) {
        fprintf(stderr, "cl_khr_gl_sharing is not supported.\n");
        std::terminate();
    }

    // TODO: I do not know if these properties need to be set, will test when code runs
    cl_context_properties props[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)m_Platform,
        0
    };

    m_Context = clCreateContext(props, 1, &m_Device, NULL, NULL, &status);
    if (status != CL_SUCCESS) {
        printf("context status: %d\n", status);
        std::terminate();
    }

    m_CmdQueue = clCreateCommandQueue(m_Context, m_Device, 0, &status);
    if (status != CL_SUCCESS) {
        printf("cmd queue status: %d\n", status);
        std::terminate();
    }

    updateBuffers();
    m_NSquared = new Kernel(m_Context, m_Device, "kernels/n_squared.cl", "n_squared");
}

void CLWrapper::updateBuffers()
{
    cl_int status;

    printf("Updating buffer...\n");
    m_VelCLBO = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 4 * sizeof(float) * (*m_N), NULL, &status);
    if (status != CL_SUCCESS) {
        printf("m_VelCLBO buffer creation status: %d\n", status);
        std::terminate();
    }

    status = clEnqueueWriteBuffer(m_CmdQueue, m_VelCLBO, CL_FALSE, 0, 4 * sizeof(float) * (*m_N), m_Velocities, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("m_VelCLBO buffer enqueue status: %d\n", status);
        std::terminate();
    }

    m_PosCLBO = clCreateFromGLBuffer(m_Context, CL_MEM_READ_WRITE, *m_PosGLBO, &status);
    if (status != CL_SUCCESS) {
        printf("m_PosCLBO buffer creation status: %d\n", status);
        std::terminate();
    }
    printf("Buffer updated!\n");
}

void CLWrapper::SimulateTimestep()
{
    if (m_PreviousN != *m_N) {
        glFinish();
        clFinish(m_CmdQueue);
        updateBuffers();
        m_PreviousN = *m_N;
    }

    float timestep = 0.0001 * (*m_TimeScale);
    float minimumSqDistance = 0.0001;

    size_t globalWorkSize[3] = { (size_t)(*m_N), 1, 1 };
    size_t localWorkSize[3] = { (size_t)calculateWorkGroupSize(), 1, 1 };

    cl_int status = clSetKernelArg(m_NSquared->GetKernel(), 0, sizeof(float), &timestep);
    if (status != CL_SUCCESS) {
        printf("kernel 1 args 0 status: %d\n", status);
        std::terminate();
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 1, sizeof(float), &minimumSqDistance);
    if (status != CL_SUCCESS) {
        printf("kernel 1 args 1 status: %d\n", status);
        std::terminate();
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 2, sizeof(cl_mem), &m_PosCLBO);
    if (status != CL_SUCCESS) {
        printf("kernel 1 args 2 status: %d\n", status);
        std::terminate();
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 3, sizeof(cl_mem), &m_VelCLBO);
    if (status != CL_SUCCESS) {
        printf("kernel 1 args 3 status: %d\n", status);
        std::terminate();
    }

    status = clSetKernelArg(m_NSquared->GetKernel(), 4, localWorkSize[0] * sizeof(cl_float4), NULL);
    if (status != CL_SUCCESS) {
        printf("kernel 1 args 4 status: %d\n", status);
        std::terminate();
    }

    glFinish();
    status = clEnqueueAcquireGLObjects(m_CmdQueue, 1, &m_PosCLBO, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
        std::terminate();
    }

    clFinish(m_CmdQueue);

    status = clEnqueueNDRangeKernel(m_CmdQueue, m_NSquared->GetKernel(), 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
        std::terminate();
    }

    clFinish(m_CmdQueue);

    clEnqueueReleaseGLObjects(m_CmdQueue, 1, &m_PosCLBO, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("clEnqueueAcquireGLObjects status: %d\n", status);
        std::terminate();
    }

    clFinish(m_CmdQueue);
}

int CLWrapper::calculateWorkGroupSize()
{
    size_t maxWorkGroupSize;
    cl_int err = clGetDeviceInfo(m_Device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
    if (err != CL_SUCCESS) {
        printf("Unable to get device info\n");
        std::terminate();
    }
    if (maxWorkGroupSize > *m_N) {
        return *m_N;
    }
    return maxWorkGroupSize;
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