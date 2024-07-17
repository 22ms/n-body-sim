// Source: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"

namespace camera {

    const glm::vec3 VECTOR_ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 VECTOR_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 VECTOR_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    Camera::Camera(glm::vec3 position, float zoom, float movementSpeed, float mouseSensitivity)
        : Position(position)
        , Zoom(zoom)
        , MovementSpeed(movementSpeed)
        , MouseSensitivity(mouseSensitivity)
    {
        WorldUp = camera::VECTOR_UP;
        Front = camera::VECTOR_FRONT;
        Yaw = -90.0f;
        Pitch = 0.0f;
        updateCameraVectors();
        applyMovementSpeed(MovementSpeed);
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = AppliedMovementSpeed * deltaTime;
        switch (direction) {
            case CameraMovement::FORWARD:
                Position += Front * velocity;
                break;
            case CameraMovement::BACKWARD:
                Position -= Front * velocity;
                break;
            case CameraMovement::LEFT:
                Position -= Right * velocity;
                break;
            case CameraMovement::RIGHT:
                Position += Right * velocity;
                break;
            default:
                break;
        }
    }

    void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void Camera::ProcessMouseScroll(float yoffset)
    {
        MovementSpeed += yoffset/10;
        if (MovementSpeed < 1.0f)
            MovementSpeed = 1.0f;
        if (MovementSpeed > 5.0f)
            MovementSpeed = 5.0f;
        applyMovementSpeed(MovementSpeed);
    }

    void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    void Camera::applyMovementSpeed(float movementSpeed) {
        AppliedMovementSpeed = movementSpeed * movementSpeed;
    }

}
