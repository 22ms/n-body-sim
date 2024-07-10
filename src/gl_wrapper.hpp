#ifndef GL_WRAPPER_HPP
#define GL_WRAPPER_HPP

#define GLEW_BUILD

class Shader;
class Camera;

struct GLFWwindow;
struct xyzm;
struct vxvyvz;

void glInitialize(int initialWidth, int initialHeight, const char* title, int* nPtr);
void glRender();
void glSwapBuffers();
bool glShouldClose();

extern GLFWwindow* glWindow;
extern Camera* glMainCamera;
extern xyzm* glPositions;
extern vxvyvz* glVelocities;

extern unsigned int glPosBuffer;
extern int glWidth;
extern int glHeight;
extern float glDeltaTime;

#endif