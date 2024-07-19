// Source: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
#pragma once

#include <glm/glm.hpp>

namespace camera {
    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    extern const glm::vec3 VECTOR_ZERO;
    extern const glm::vec3 VECTOR_UP;
    extern const glm::vec3 VECTOR_FRONT;

    class Camera {
    public:
        Camera(
            glm::vec3 position = VECTOR_ZERO,
            float zoom = 45.0f,
            float movementSpeed = 1.0f, 
            float mouseSensitivity = 0.1f
        );

        glm::mat4 GetViewMatrix();
        void ProcessKeyboard(CameraMovement direction, float deltaTime);
        void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
        void ProcessMouseScroll(float yoffset);

        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        
        float Yaw;
        float Pitch;
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;
    private:
        void updateCameraVectors();
    };
}