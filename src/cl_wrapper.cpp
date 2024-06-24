#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <stdio.h>
#include <string.h>

#include "cl_wrapper.h"
#include "utilities.h"

CLWrapper::CLWrapper()
{
    // get the platform id:
    cl_int status = clGetPlatformIDs(1, &_platform, NULL);
    printf("clGetPlatformIDs: %d", status);

    // get the device id:
    status = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_GPU, 1, &_device, NULL);
    printf("clGetDeviceIDs: %d", status);
    // check if the opengl sharing extension is supported
    // (no point going on if it isn’t):
    // (we need the Device in order to ask, so we can't do it any sooner than right here)if( IsCLExtensionSupported( "cl_khr_gl_sharing" ) )
    if (isCLExtensionSupported("cl_khr_gl_sharing")) {
        fprintf(stderr, "cl_khr_gl_sharing is supported.\n");
    } else {
        fprintf(stderr, "cl_khr_gl_sharing is not supported -- sorry.\n");
        return;
    }
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