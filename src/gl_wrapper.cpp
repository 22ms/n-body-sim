#ifdef __APPLE__
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <OpenCL/opencl.hpp>
#else
#include <CL/cl.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <stdio.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "gl_wrapper.hpp"
#include "cl_wrapper.hpp"
#include "shader.hpp"
#include "utilities.hpp"
#include "config.hpp"
#include "world_state.hpp"
#include "world_gens/world_generators.hpp"

namespace glwrapper {

    // External variables
    GLFWwindow* Window;
    std::unique_ptr<camera::Camera> MainCamera;
    std::vector<utilities::Position> Positions (config::simulation::MAX_N);
    std::vector<utilities::Velocity> Velocities (config::simulation::MAX_N);

    unsigned int PosBuffer;
    int CurrentWidth;
    int CurrentHeight;
    float DeltaTime;

    // Internal variables
    static std::unique_ptr<Shader> shader;
    static std::unique_ptr<worldgens::WorldGenerator> previousWorldGeneratorPtr;

    static unsigned int posAttribute;
    static unsigned int previousN;

    static float lastX;
    static float lastY;
    static float lastFrameTime;

    static bool firstMouse = true;
    static bool captureMouse = false;

    static void processKeyInput();

    static void errorCallback(int error, const char* description);
    static void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height);
    static void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn);
    static void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset);

    void Test(utilities::Position* positions, utilities::Velocity* velocities) {
        for (int i = 0; i < config::simulation::MAX_N; i++) {
            printf("pos x %d: %g\n", i, positions[i].x);
            printf("vel x %d: %g\n", i, velocities[i].x);
        }
    }

    void Initialize()
    {
        glwrapper::CurrentWidth = config::window::Width;
        glwrapper::CurrentHeight = config::window::Height;

        previousN = *worldstate::CurrentNPtr;
        previousWorldGeneratorPtr = worldstate::CurrentWorldGeneratorPtr->Clone();

        glfwSetErrorCallback(errorCallback);
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Window = glfwCreateWindow(CurrentWidth, CurrentHeight, config::window::Title.c_str(), nullptr, nullptr);

        glfwMakeContextCurrent(Window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            printf("Failed to initialize GLAD\n");
            std::terminate();
        }
        
        glfwSwapInterval(1); // Enable vsync
        glViewport(0, 0, CurrentWidth, CurrentHeight);
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

        worldstate::CurrentWorldGeneratorPtr->Generate(Positions, Velocities, *worldstate::CurrentNPtr);

        glGenBuffers(1, &PosBuffer);
        glGenVertexArrays(1, &posAttribute);

        glBindVertexArray(posAttribute);
        glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * config::simulation::MAX_N * 4, &Positions[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        shader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
        shader->Use();

        MainCamera = std::make_unique<camera::Camera>(
            camera::Camera(
                glm::vec3(0.0f, 0.0f, 3.0f),
                45.0f,
                *worldstate::MainCameraSpeedPtr,
                0.1f
        ));

        lastX = CurrentWidth / 2.0f;
        lastY = CurrentHeight / 2.0f;
    }

    void Render()
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        DeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        processKeyInput();

        glm::mat4 projection = glm::perspective(glm::radians(MainCamera->Zoom), (float)CurrentWidth / (float)CurrentHeight, 0.1f, 1000.0f);
        shader->SetMat4("projection", projection);

        glm::mat4 view = MainCamera->GetViewMatrix();
        shader->SetMat4("view", view);

        if (*worldstate::CurrentNPtr != previousN || !worldstate::CurrentWorldGeneratorPtr->IsSameType(*previousWorldGeneratorPtr)) {
            worldstate::CurrentWorldGeneratorPtr->Generate(Positions, Velocities, *worldstate::CurrentNPtr);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * (*worldstate::CurrentNPtr) * 4, &Positions[0]);
            clwrapper::UpdateCLBuffers();

            previousN = *worldstate::CurrentNPtr;
            previousWorldGeneratorPtr = worldstate::CurrentWorldGeneratorPtr->Clone();
        }
        glDrawArrays(GL_POINTS, 0, *worldstate::CurrentNPtr);

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

    void Cleanup() {
        glFinish();
        glDeleteVertexArrays(1, &posAttribute);
        glDeleteBuffers(1, &PosBuffer);
        glfwDestroyWindow(Window);
        glfwTerminate();
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(Window);
    }

    static void processKeyInput()
    {
        if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(Window, true);

        if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(camera::CameraMovement::FORWARD, DeltaTime);
        if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(camera::CameraMovement::BACKWARD, DeltaTime);
        if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(camera::CameraMovement::LEFT, DeltaTime);
        if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
            MainCamera->ProcessKeyboard(camera::CameraMovement::RIGHT, DeltaTime);
    }

    static void errorCallback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    static void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    static void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn)
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

    static void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods)
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

    static void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset)
    {
        MainCamera->ProcessMouseScroll(yoffset);
    }
}