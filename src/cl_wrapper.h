#define CL_TARGET_OPENCL_VERSION 120

typedef struct _cl_mem* cl_mem;
typedef struct _cl_context* cl_context;
typedef struct _cl_command_queue* cl_command_queue;
typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_platform_id* cl_platform_id;

class GLFWwindow;
struct xyz;

class CLWrapper {
public:
    CLWrapper(GLFWwindow* window, int* N, unsigned int* posGLBO); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    void simulateTimestep();
private:
    bool isCLExtensionSupported(const char* extension);

    unsigned int* _posGLBO = nullptr;

    xyz* _velocities = nullptr;
    cl_mem _velCLBO;
    cl_mem _posCLBO;

    cl_context _context;
    cl_command_queue _cmdQueue;
    cl_device_id _device;
    cl_platform_id _platform;
};