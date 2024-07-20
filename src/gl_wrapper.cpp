#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <stdio.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_wrapper.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "cl_wrapper.hpp"
#include "shader.hpp"
#include "world_gens/world_generators.hpp"

namespace glwrapper {
    // External variables
    GLFWwindow* Window;
    std::unique_ptr<camera::Camera> MainCamera;

    unsigned int ParticleBuffer;
    float DeltaTime;

    // Internal variables
    static std::unique_ptr<Shader> particleShader;
    static std::unique_ptr<Shader> gridShader;
    static std::unique_ptr<worldgens::WorldGenerator> previousWorldGeneratorPtr;

    static unsigned int particleAttributes;
    static unsigned int previousN;

    static float* particleArray = nullptr;
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

    void Initialize()
    {
        previousN = *state::simulation::NPtr;
        previousWorldGeneratorPtr = state::simulation::WorldGeneratorPtr->Clone();

        glfwSetErrorCallback(errorCallback);
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Window = glfwCreateWindow(state::window::WIDTH, state::window::HEIGHT, state::window::TITLE.c_str(), nullptr, nullptr);

        glfwMakeContextCurrent(Window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            fprintf(stderr, "Failed to initialize GLAD\n");
            std::terminate();
        }
        
        glfwSwapInterval(1); // Enable vsync
        glViewport(0, 0, state::window::WIDTH, state::window::HEIGHT);
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

        state::simulation::WorldGeneratorPtr->Generate(particleArray, *state::simulation::NPtr);

        glGenBuffers(1, &ParticleBuffer);
        glGenVertexArrays(1, &particleAttributes);

        glBindVertexArray(particleAttributes);
        glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer);
        glBufferData(GL_ARRAY_BUFFER, (4 + 3) * state::simulation::MAX_N * sizeof(float), particleArray, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (4 + 3) * sizeof(float), (void*)(0));
        glEnableVertexAttribArray(0);
        // Velocity attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (4 + 3) * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(1);

        particleShader = std::make_unique<Shader>("shaders/particle.vert", "shaders/particle.frag");

        MainCamera = std::make_unique<camera::Camera>(
            camera::Camera(
                glm::vec3(0.0f, 0.0f, 10.0f),
                45.0f,
                1.0f,
                0.1f
        ));

        lastX = state::window::WIDTH / 2.0f;
        lastY = state::window::HEIGHT / 2.0f;
    }

    void Render()
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        DeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        processKeyInput();

        glm::mat4 projection = glm::perspective(glm::radians(MainCamera->Zoom), (float)(state::window::WIDTH) / (float)(state::window::HEIGHT), 0.1f, 1000.0f);
        glm::mat4 view = MainCamera->GetViewMatrix();
        MainCamera->MouseSensitivity = *state::rendering::MouseSensitivityPtr;

        particleShader->SetMat4("projection", projection);
        particleShader->SetMat4("view", view);
        particleShader->SetFloat("pointSize", *state::rendering::PointSizePtr);

        if (*state::simulation::NPtr != previousN || !state::simulation::WorldGeneratorPtr->IsSameType(*previousWorldGeneratorPtr)) {
            state::simulation::WorldGeneratorPtr->Generate(particleArray, *state::simulation::NPtr);
            particleShader->SetInt("N", *state::simulation::NPtr);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * (*state::simulation::NPtr) * (4 + 3), particleArray);
            clwrapper::UpdateInteropBuffer();

            previousN = *state::simulation::NPtr;
            previousWorldGeneratorPtr = state::simulation::WorldGeneratorPtr->Clone();
            *state::simulation::ElapsedTimePtr = 0.0f;
        }
        particleShader->Use();
        glBindVertexArray(particleAttributes);
        glDrawArrays(GL_POINTS, 0, *state::simulation::NPtr);

        GLenum err = glGetError();
        if (err != 0) {
            fprintf(stderr, "OpenGL error: %d\n", err);
            std::terminate();
        }
    }

    void SwapBuffers()
    {
        glfwSwapBuffers(Window);
    }

    void Cleanup() {
        glFinish();
        glDeleteVertexArrays(1, &particleAttributes);
        glDeleteBuffers(1, &ParticleBuffer);
        glfwDestroyWindow(Window);
        glfwTerminate();

        delete[] particleArray;
        particleArray = nullptr;
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(Window);
    }

    static void processKeyInput() {
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

    static void errorCallback(int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    static void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height) {
        glViewport(0, 0, width, height);
    }

    static void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn) {
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

    static void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            captureMouse = true;
            glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            captureMouse = false;
            glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    static void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset) {
        MainCamera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}