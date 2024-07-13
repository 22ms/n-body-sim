#include <GL/glew.h>
#include <CL/cl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "gl_wrapper.hpp"
#include "shader.hpp"
#include "utilities.hpp"
#include "world_generators.hpp"
#include "globals.hpp"

namespace glwrapper {

    // "Public"

    GLFWwindow* Window = nullptr;
    Camera* MainCamera = nullptr;
    Position* Positions = nullptr;
    Velocity* Velocities = nullptr;

    unsigned int PosBuffer;
    int Width;
    int Height;
    float DeltaTime;
    float* MainCameraSpeedPtr = nullptr;

    // "Private"

    static Shader* shader = nullptr;
    worldgenerators::WorldGenerator** worldGeneratorPtr = nullptr;
    worldgenerators::WorldGenerator* previousWorldGeneratorPtr;

    static unsigned int posAttribute;

    static unsigned int* nPtr = nullptr;
    static unsigned int previousN;

    static float lastX;
    static float lastY;
    static float lastFrameTime;

    static bool firstMouse;
    static bool captureMouse;

    void processKeyInput();
    void (*bufferUpdateCallback)(int);

    void errorCallback(int error, const char* description);
    void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height);
    void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn);
    void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods);
    void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset);

    void Initialize(int width, int height, const char* title, unsigned int* nPtr, void (*bufferUpdateCallback)(int), worldgenerators::WorldGenerator** worldGeneratorPtr)
    {
        glwrapper::Width = width;
        glwrapper::Height = height;
        glwrapper::nPtr = nPtr;
        glwrapper::bufferUpdateCallback = bufferUpdateCallback;
        glwrapper::worldGeneratorPtr = worldGeneratorPtr;

        previousN = *glwrapper::nPtr;
        previousWorldGeneratorPtr = *glwrapper::worldGeneratorPtr;

        glfwSetErrorCallback(errorCallback);
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Window = glfwCreateWindow(Width, Height, title, nullptr, nullptr);

        glfwMakeContextCurrent(Window);
        glfwSwapInterval(1); // Enable vsync
        glViewport(0, 0, Width, Height);
        glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);
        glfwSetCursorPosCallback(Window, mouseCallback);
        glfwSetMouseButtonCallback(Window, mouseButtonCallback);
        glfwSetScrollCallback(Window, scrollCallback);

        glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLenum err = glewInit();
        (*worldGeneratorPtr)->Generate(Positions, Velocities, *nPtr);

        glGenBuffers(1, &PosBuffer);
        glGenVertexArrays(1, &posAttribute);

        glBindVertexArray(posAttribute);
        glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_N * 4, Positions, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
        shader->Use();

        MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
        MainCameraSpeedPtr = &(MainCamera->MovementSpeed);
        lastX = Width / 2.0f;
        lastY = Height / 2.0f;
        firstMouse = true;
        captureMouse = false;
        DeltaTime = 0.0f;
        lastFrameTime = 0.0f;
    }

    void Render(cl_command_queue cmdQueue)
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        DeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        processKeyInput();

        glm::mat4 projection = glm::perspective(glm::radians(MainCamera->Zoom), (float)Width / (float)Height, 0.1f, 1000.0f);
        shader->SetMat4("projection", projection);

        glm::mat4 view = MainCamera->GetViewMatrix();
        shader->SetMat4("view", view);

        if (*nPtr != previousN || *worldGeneratorPtr != previousWorldGeneratorPtr) {
            glFinish();
            clFinish(cmdQueue);

            (*worldGeneratorPtr)->Generate(Positions, Velocities, *nPtr);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * (*nPtr) * 4, Positions);
            bufferUpdateCallback(*nPtr);

            glFinish();
            clFinish(cmdQueue);

            previousN = *nPtr;
            previousWorldGeneratorPtr = *worldGeneratorPtr;
        }
        glDrawArrays(GL_POINTS, 0, *nPtr);

        GLenum err = glGetError();
        if (err != 0) {
            printf("OpenGL error: 0x%x\n", err);
            std::terminate();
        }
    }

    void SwapBuffers()
    {
        glfwSwapBuffers(Window);
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(Window);
    }

    void processKeyInput()
    {
        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(Window, true);

        if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(FORWARD, DeltaTime);
        if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(BACKWARD, DeltaTime);
        if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(LEFT, DeltaTime);
        if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(RIGHT, DeltaTime);
    }

    void errorCallback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        if (captureMouse) {
            MainCamera->ProcessMouseMovement(xoffset, yoffset);
        }
    }

    void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            captureMouse = true;
            glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            captureMouse = false;
            glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset)
    {
        MainCamera->ProcessMouseScroll(yoffset);
    }
} // namespace glwrapper