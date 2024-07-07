#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_wrapper.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "utilities.hpp"

void GLWrapper::Initialize(int width, int height, const char* title, int* N)
{
    m_Width = width;
    m_Height = height;
    m_N = N;
    m_PreviousN = *m_N;

    glfwSetErrorCallback(errorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwMakeContextCurrent(Window);
    glfwSwapInterval(1); // Enable vsync
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);
    glfwSetCursorPosCallback(Window, mouseCallback);
    glfwSetMouseButtonCallback(Window, mouseButtonCallback);
    glfwSetScrollCallback(Window, scrollCallback);

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLenum err = glewInit();
    fillVertexBuffers();

    glGenBuffers(1, &m_PosGLBO);
    glGenVertexArrays(1, &m_PosGLAO);

    glBindVertexArray(m_PosGLAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PosGLBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*m_N) * 4, m_Positions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    m_Shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
    m_Shader->Use();

    MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    MainCameraSpeed = &(MainCamera->MovementSpeed);
    LastX = m_Width / 2.0f;
    LastY = m_Height / 2.0f;
    FirstMouse = true;
    CaptureMouse = false;
    m_DeltaTime = 0.0f;
    m_LastFrameTime = 0.0f;
}

unsigned int* GLWrapper::GetPosGLBO()
{
    return &m_PosGLBO;
}

vxvyvz* GLWrapper::GetVelocities()
{
    return m_Velocities;
}

bool GLWrapper::ShouldClose()
{
    return glfwWindowShouldClose(Window);
}

void GLWrapper::Render()
{
    float currentFrameTime = static_cast<float>(glfwGetTime());
    m_DeltaTime = currentFrameTime - m_LastFrameTime;
    m_LastFrameTime = currentFrameTime;

    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processKeyInput(Window);

    glm::mat4 projection = glm::perspective(glm::radians(MainCamera->Zoom), (float)m_Width / (float)m_Height, 0.1f, 100.0f);
    m_Shader->SetMat4("projection", projection);

    glm::mat4 view = MainCamera->GetViewMatrix();
    m_Shader->SetMat4("view", view);

    if (*m_N != m_PreviousN) {
        fillVertexBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*m_N) * 4, m_Positions, GL_DYNAMIC_DRAW);
    }
    glDrawArrays(GL_POINTS, 0, *m_N);

    m_PreviousN = *m_N;

    GLenum err = glGetError();
    if (err != 0) {
        printf("OpenGL error: 0x%x\n", err);
        std::terminate();
    }
}

void GLWrapper::SwapBuffers()
{
    glfwSwapBuffers(Window);
}

void GLWrapper::fillVertexBuffers()
{
    if (m_Positions != nullptr) {
        delete[] m_Positions;
    }

    if (m_Velocities != nullptr) {
        delete[] m_Velocities;
    }

    m_Positions = new xyzm[*m_N];
    m_Velocities = new vxvyvz[*m_N];

    float endRadius = 1.0f;
    float spacing = endRadius / *m_N;

    // set positions
    for (int i = 0; i < *m_N; i++) {
        double theta = acos(2 * rand() / double(RAND_MAX) - 1); // Polar angle
        double phi = rand() / double(RAND_MAX) * 2 * M_PI;      // Azimuthal angle
        double radius = endRadius;                              // Radius of the sphere

        m_Positions[i].x = radius * sin(theta) * cos(phi);
        m_Positions[i].y = radius * sin(theta) * sin(phi);
        m_Positions[i].z = radius * cos(theta);
        m_Positions[i].m = 1;
    }

    // set velocities
    for (int i = 0; i < *m_N; i++) {
        m_Velocities[i].vx = 0;
        m_Velocities[i].vy = 0;
        m_Velocities[i].vz = 0;
    }
}

void GLWrapper::errorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void GLWrapper::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLWrapper::mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    GLWrapper& instance = GLWrapper::GetInstance();

    if (instance.FirstMouse) {
        instance.LastX = xpos;
        instance.LastY = ypos;
        instance.FirstMouse = false;
    }

    float xoffset = xpos - instance.LastX;
    float yoffset = instance.LastY - ypos; // reversed since y-coordinates go from bottom to top

    instance.LastX = xpos;
    instance.LastY = ypos;

    if (instance.CaptureMouse) {
        instance.MainCamera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void GLWrapper::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLWrapper& instance = GLWrapper::GetInstance();
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        instance.CaptureMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        instance.CaptureMouse = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void GLWrapper::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    GLWrapper::GetInstance().MainCamera->ProcessMouseScroll(yoffset);
}

void GLWrapper::processKeyInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(FORWARD, m_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(BACKWARD, m_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(LEFT, m_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(RIGHT, m_DeltaTime);
}