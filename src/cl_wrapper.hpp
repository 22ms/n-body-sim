#ifndef CL_WRAPPER_HPP
#define CL_WRAPPER_HPP

#define CL_TARGET_OPENCL_VERSION 120

typedef struct _cl_mem* cl_mem;
typedef struct _cl_context* cl_context;
typedef struct _cl_command_queue* cl_command_queue;
typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_platform_id* cl_platform_id;

class GLFWwindow;
class Kernel;
struct vxvyvz;

class CLWrapper {
    // Singleton pattern: https://stackoverflow.com/a/1008289/16951338
public:
    static CLWrapper& GetInstance()
    {
        static CLWrapper instance;
        return instance;
    }
private:
    CLWrapper() {};
    CLWrapper(CLWrapper const&);
    void operator=(CLWrapper const&);

    // Member variables
public:
    void Initialize(GLFWwindow* window, unsigned int* posGLBO, vxvyvz* velocities, int* N, float* timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    void SimulateTimestep();
private:
    bool isCLExtensionSupported(const char* extension);

    unsigned int* m_PosGLBO = nullptr;
    int* m_N = nullptr;
    float* m_TimeScale = nullptr;

    vxvyvz* m_Velocities = nullptr;
    cl_mem m_VelCLBO;
    cl_mem m_PosCLBO;

    cl_context m_Context;
    cl_command_queue m_CmdQueue;
    cl_device_id m_Device;
    cl_platform_id m_Platform;

    Kernel* m_NSquared;
};

#endif