typedef struct _cl_device_id* cl_device_id;
typedef struct _cl_platform_id* cl_platform_id;

class CLWrapper {
public:
    CLWrapper();
private:
    bool isCLExtensionSupported(const char* extension);

    cl_device_id _device;
    cl_platform_id _platform;
};