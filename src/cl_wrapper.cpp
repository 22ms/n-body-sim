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
#include "globals.hpp"

namespace clwrapper {

    // "Public"

    cl_command_queue cmdQueue;

    // "Private"

    static unsigned int glPosBuffer;
    static unsigned int n;

    static float* timeScalePtr = nullptr;

    static cl_mem velBuffer;
    static cl_mem posBuffer;

    static cl_context context;
    static cl_device_id device;
    static cl_platform_id platform;

    static Kernel* nSquared;
    static Velocity** velocities = nullptr;

    int calculateWorkGroupSize();
    bool isCLExtensionSupported(const char* extension);

    void Initialize(unsigned int glPosBuffer, Velocity** velocities, unsigned int n, float* timeScalePtr)
    {
        clwrapper::glPosBuffer = glPosBuffer;
        clwrapper::velocities = velocities;
        clwrapper::n = n;
        clwrapper::timeScalePtr = timeScalePtr;

        cl_int status = clGetPlatformIDs(1, &platform, NULL);
        if (status != CL_SUCCESS) {
            printf("clGetPlatformIDs: %d\n", status);
            std::terminate();
        }

        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
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
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0
        };

        context = clCreateContext(props, 1, &device, NULL, NULL, &status);
        if (status != CL_SUCCESS) {
            printf("context status: %d\n", status);
            std::terminate();
        }

        cmdQueue = clCreateCommandQueue(context, device, 0, &status);
        if (status != CL_SUCCESS) {
            printf("cmd queue status: %d\n", status);
            std::terminate();
        }

        velBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * sizeof(float) * MAX_N, NULL, &status);
        if (status != CL_SUCCESS) {
            printf("velBuffer buffer creation status: %d\n", status);
            std::terminate();
        }

        UpdateCLBuffers(n);
        nSquared = new Kernel(context, device, "kernels/n_squared.cl", "n_squared");
    }

    void SimulateTimestep()
    {
        float timestep = 0.0001 * (*timeScalePtr);
        float minimumSqDistance = 0.0001;

        size_t globalWorkSize[3] = { (size_t)(n), 1, 1 };
        size_t localWorkSize[3] = { (size_t)calculateWorkGroupSize(), 1, 1 };

        cl_int status = clSetKernelArg(nSquared->GetKernel(), 0, sizeof(float), &timestep);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 0 status: %d\n", status);
            std::terminate();
        }

        status = clSetKernelArg(nSquared->GetKernel(), 1, sizeof(float), &minimumSqDistance);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 1 status: %d\n", status);
            std::terminate();
        }

        status = clSetKernelArg(nSquared->GetKernel(), 2, sizeof(cl_mem), &posBuffer);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 2 status: %d\n", status);
            std::terminate();
        }

        status = clSetKernelArg(nSquared->GetKernel(), 3, sizeof(cl_mem), &velBuffer);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 3 status: %d\n", status);
            std::terminate();
        }

        status = clSetKernelArg(nSquared->GetKernel(), 4, localWorkSize[0] * sizeof(cl_float4), NULL);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 4 status: %d\n", status);
            std::terminate();
        }

        glFinish();
        status = clEnqueueAcquireGLObjects(cmdQueue, 1, &posBuffer, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            printf("clEnqueueAcquireGLObjects status: %d\n", status);
            std::terminate();
        }

        clFinish(cmdQueue);

        status = clEnqueueNDRangeKernel(cmdQueue, nSquared->GetKernel(), 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            printf("clEnqueueAcquireGLObjects status: %d\n", status);
            std::terminate();
        }

        clFinish(cmdQueue);

        clEnqueueReleaseGLObjects(cmdQueue, 1, &posBuffer, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            printf("clEnqueueAcquireGLObjects status: %d\n", status);
            std::terminate();
        }

        clFinish(cmdQueue);
    }

    void UpdateCLBuffers (int n) {
        clwrapper::n = n;
        cl_int status;

        glFinish();
        clFinish(cmdQueue);

        status = clEnqueueWriteBuffer(cmdQueue, velBuffer, CL_TRUE, 0, 4 * sizeof(float) * MAX_N, *velocities, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            printf("velBuffer buffer enqueue status: %d\n", status);
            std::terminate();
        }

        posBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, glPosBuffer, &status);
        if (status != CL_SUCCESS) {
            printf("glPosBuffer buffer creation status: %d\n", status);
            std::terminate();
        }
    }

    int calculateWorkGroupSize()
    {
        size_t maxWorkGroupSize;
        cl_int err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
        if (err != CL_SUCCESS) {
            printf("Unable to get device info\n");
            std::terminate();
        }
        if (maxWorkGroupSize > n) {
            return n;
        }
        return maxWorkGroupSize;
    }

    bool isCLExtensionSupported(const char* extension)
    {
        // see if the extension is bogus:
        if (extension == NULL || extension[0] == '\0')
            return false;

        char* where = (char*)strchr(extension, ' ');
        if (where != NULL)
            return false;

        // get the full list of extensions:
        size_t extensionSize;
        clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
        char* extensions = new char[extensionSize];
        clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);

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

}