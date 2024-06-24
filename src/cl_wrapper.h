#define CL_TARGET_OPENCL_VERSION 120

typedef struct _cl_mem* cl_mem;
typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_platform_id* cl_platform_id;

class GLFWwindow;
struct xyzw;

class CLWrapper {
public:
    CLWrapper(GLFWwindow* window, int* N); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    void simulateTimestep();
private:
    bool isCLExtensionSupported(const char* extension);

    xyzw* _hVel = nullptr;
    cl_mem _dVel;
    cl_device_id _device;
    cl_platform_id _platform;
};