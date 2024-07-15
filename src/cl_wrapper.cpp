#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <stdio.h>
#include <string.h>

#ifdef __unix__
#include <GL/glx.h>
#endif

#include "cl_wrapper.hpp"
#include "gl_wrapper.hpp"
#include "kernel.hpp"
#include "utilities.hpp"
#include "config.hpp"
#include "world_state.hpp"

namespace clwrapper {

    // "Public"

    cl_command_queue cmdQueue;

    // "Private"

    static cl_mem velBuffer;
    static cl_mem posBuffer;

    static cl_context context;
    static cl_device_id device;
    static cl_platform_id platform;

    static std::unique_ptr<Kernel> nSquared;

    int calculateWorkGroupSize();
    bool isCLExtensionSupported(const std::string& extension);

    void Initialize()
    {
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

        #ifdef _WIN32
        cl_context_properties props[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0
        };
        #elif __unix__
        cl_context_properties props[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)glXGetCurrentDisplay(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0
        };
        #endif

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

        velBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, 4 * sizeof(float) * config::simulation::MAX_N, NULL, &status);
        if (status != CL_SUCCESS) {
            printf("velBuffer buffer creation status: %d\n", status);
            std::terminate();
        }

        UpdateCLBuffers();
        nSquared = std::make_unique<Kernel>(Kernel(context, device, "kernels/n_squared.cl", "n_squared"));
    }

    void UpdateCLBuffers () {
        cl_int status;

        glFinish();
        clFinish(cmdQueue);

        status = clEnqueueWriteBuffer(cmdQueue, velBuffer, CL_TRUE, 0, 4 * sizeof(float) * config::simulation::MAX_N, &glwrapper::Velocities[0], 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            printf("velBuffer buffer enqueue status: %d\n", status);
            std::terminate();
        }

        posBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, glwrapper::PosBuffer, &status);
        if (status != CL_SUCCESS) {
            printf("posBuffer buffer creation status: %d\n", status);
            std::terminate();
        }
    }

    void SimulateTimestep()
    {
        float timestep = 0.0001 * (*worldstate::CurrentTimeScalePtr);
        float minimumSqDistance = 0.0001;

        size_t globalWorkSize[3] = { (size_t)(*worldstate::CurrentNPtr), 1, 1 };
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

    int calculateWorkGroupSize()
    {
        size_t maxWorkGroupSize;
        cl_int err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
        if (err != CL_SUCCESS) {
            printf("Unable to get device info\n");
            std::terminate();
        }
        if (maxWorkGroupSize > *worldstate::CurrentNPtr) {
            return *worldstate::CurrentNPtr;
        }
        return maxWorkGroupSize;
    }

    bool isCLExtensionSupported(const std::string& extension)
    {
        // Check if the extension is empty
        if (extension.empty())
            return false;

        // Check if the extension contains a space, which is invalid
        if (extension.find(' ') != std::string::npos)
            return false;

        // Get the size of the extensions string
        size_t extensionSize;
        cl_int result = clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, nullptr, &extensionSize);
        if (result != CL_SUCCESS)
            return false;

        // Get the extensions string
        std::unique_ptr<char[]> extensions(new char[extensionSize]);
        result = clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, extensionSize, extensions.get(), nullptr);
        if (result != CL_SUCCESS)
            return false;

        // Convert the extensions C-string to a std::string for easier manipulation
        std::string extensionsStr(extensions.get(), extensionSize);

        // Check if the extension is in the extensions string
        size_t pos = extensionsStr.find(extension);
        while (pos != std::string::npos) {
            // Check if the found extension is correctly delimited
            size_t endPos = pos + extension.length();
            if ((endPos == extensionsStr.length() || std::isspace(extensionsStr[endPos])) &&
                (pos == 0 || std::isspace(extensionsStr[pos - 1]))) {
                return true;
            }
            pos = extensionsStr.find(extension, endPos);
        }

        return false;
    }

}