#include <glad/glad.h>

#ifdef _WIN32
#include <CL/cl.h>
#include <CL/cl_gl.h>
#elif __linux__
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glx.h>
#elif __APPLE__
#include <OpenCL/opencl.h>
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLDevice.h>
#endif

#include <GLFW/glfw3.h>
#include <exception>
#include <stdio.h>
#include <string.h>

#include "cl_wrapper.hpp"
#include "gl_wrapper.hpp"
#include "kernel.hpp"
#include "utilities.hpp"
#include "config.hpp"
#include "world_state.hpp"

namespace clwrapper {

    // External variables
    cl_command_queue cmdQueue;

    // Internal variables
    static cl_mem interopParticleBuffer;

    static cl_context context;
    static cl_device_id device;
    static cl_platform_id platform;

    static std::unique_ptr<Kernel> nSquared;

    static int calculateWorkGroupSize();
    static bool isCLExtensionSupported(const std::string& extension);

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

        #ifdef __APPLE__
        if (!isCLExtensionSupported("cl_APPLE_gl_sharing")) {
            fprintf(stderr, "cl_APPLE_gl_sharing is not supported.\n");
            std::terminate();
        }
        #else
        if (!isCLExtensionSupported("cl_khr_gl_sharing")) {
            fprintf(stderr, "cl_khr_gl_sharing is not supported.\n");
            std::terminate();
        }
        #endif

        #ifdef _WIN32
        cl_context_properties props[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0
        };
        context = clCreateContext(props, 1, &device, NULL, NULL, &status);
        #elif __unix__
        cl_context_properties props[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)glXGetCurrentDisplay(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
            0
        };
        context = clCreateContext(props, 1, &device, NULL, NULL, &status);
        #elif __APPLE__
        cl_context_properties props[] = {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
            0
        };
        context = clCreateContext(props, 0, 0, NULL, NULL, &status);
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

        UpdateCLBuffers();
        nSquared = std::make_unique<Kernel>(Kernel(context, device, "kernels/n_squared.cl", "n_squared"));
    }

    void UpdateCLBuffers () {
        cl_int status;

        glFinish();
        clFinish(cmdQueue);

        interopParticleBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, glwrapper::ParticleBuffer, &status);
        if (status != CL_SUCCESS) {
            printf("clParticleBuffer buffer creation status: %d\n", status);
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

        status = clSetKernelArg(nSquared->GetKernel(), 2, sizeof(cl_mem), &interopParticleBuffer);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 2 status: %d\n", status);
            std::terminate();
        }

        status = clSetKernelArg(nSquared->GetKernel(), 3, localWorkSize[0] * sizeof(cl_float4), NULL);
        if (status != CL_SUCCESS) {
            printf("kernel 1 args 3 status: %d\n", status);
            std::terminate();
        }

        glFinish();
        status = clEnqueueAcquireGLObjects(cmdQueue, 1, &interopParticleBuffer, 0, NULL, NULL);
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

        clEnqueueReleaseGLObjects(cmdQueue, 1, &interopParticleBuffer, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            printf("clEnqueueAcquireGLObjects status: %d\n", status);
            std::terminate();
        }

        clFinish(cmdQueue);
    }

    void Cleanup () {
        clReleaseCommandQueue(cmdQueue);
        clReleaseMemObject(interopParticleBuffer);
        clReleaseContext(context);
    }

    static int calculateWorkGroupSize()
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

    static bool isCLExtensionSupported(const std::string& extension)
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