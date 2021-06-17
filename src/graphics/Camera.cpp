#include "graphics/Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace RIS::Graphics
{
    Camera::Camera()
        : Camera(glm::radians(75.0f), 1.0f)
    {}

    Camera::Camera(float fov, float aspect)
        : Camera(fov, aspect, 0.1f, 10000.0f)
    {}

    Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
        : fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane), projection(glm::perspective(fov, aspect, nearPlane, farPlane)), yaw(0), pitch(0), roll(0)
    {}

    void Camera::SetFoV(float fov)
    {
        this->fov = fov;
        projection = glm::perspective(fov, aspect, nearPlane, farPlane);
    }

    void Camera::SetAspect(float aspect)
    {
        this->aspect = aspect;
        projection = glm::perspective(fov, aspect, nearPlane, farPlane);
    }

    void Camera::SetNear(float nearPlane)
    {
        this->nearPlane = nearPlane;
        projection = glm::perspective(fov, aspect, nearPlane, farPlane);
    }

    void Camera::SetFar(float farPlane)
    {
        this->farPlane = farPlane;
        projection = glm::perspective(fov, aspect, nearPlane, farPlane);
    }

    Transform& Camera::GetTransform()
    {
        return transform;
    }

    glm::vec3 Camera::Direction() const
    {
        glm::vec3 ref(0, 0, -1);
        return transform.rotation * ref;
    }

    glm::vec3 Camera::Up() const
    {
        glm::vec3 ref(0, 1, 0);
        return transform.rotation * ref;
    }

    glm::vec3 Camera::Right() const
    {
        glm::vec3 ref(1, 0, 0);
        return transform.rotation * ref;
    }

    glm::vec3 Camera::YawDirection() const
    {
        glm::quat rot({0, yaw, 0});
        glm::vec3 ref(0, 0, -1);
        return rot * ref;
    }

    void Camera::AddYaw(float amount)
    {
        yaw += amount;
        while(yaw >= glm::two_pi<float>())
            yaw -= glm::two_pi<float>();
        while(yaw < 0)
            yaw += glm::two_pi<float>();
        transform.rotation = glm::quat({pitch, yaw, roll});
    }

    void Camera::AddPitch(float amount)
    {
        pitch += amount;
        if(pitch >= glm::half_pi<float>())
            pitch = glm::half_pi<float>();
        if(pitch <= -glm::half_pi<float>())
            pitch = -glm::half_pi<float>();
        transform.rotation = glm::quat({pitch, yaw, roll});
    }

    void Camera::AddRoll(float amount)
    {
        roll += amount;
        while(roll >= glm::two_pi<float>())
            roll -= glm::two_pi<float>();
        while(roll < 0)
            roll += glm::two_pi<float>();
        transform.rotation = glm::quat({pitch, yaw, roll});
    }

    void Camera::SetYaw(float amount)
    {
        yaw = amount;
        while(yaw >= glm::two_pi<float>())
            yaw -= glm::two_pi<float>();
        while(yaw < 0)
            yaw += glm::two_pi<float>();
        transform.rotation = glm::quat({pitch, yaw, roll});
    }

    void Camera::SetPitch(float amount)
    {
        pitch = amount;
        if(pitch >= glm::half_pi<float>())
            pitch = glm::half_pi<float>();
        if(pitch <= -glm::half_pi<float>())
            pitch = -glm::half_pi<float>();
        transform.rotation = glm::quat({pitch, yaw, roll});
    }

    void Camera::SetRoll(float amount)
    {
        roll = amount;
        while(roll >= glm::two_pi<float>())
            roll -= glm::two_pi<float>();
        while(roll < 0)
            roll += glm::two_pi<float>();
        transform.rotation = glm::quat({pitch, yaw, roll});
    }

    glm::vec3& Camera::Position()
    {
        return transform.position;
    }

    glm::mat4 Camera::ViewProj() const
    {
        return Projection() * View();
    }

    glm::mat4 Camera::View() const
    {
        glm::vec3 direction = Direction();
        glm::vec3 up = Up();
        return glm::lookAt(transform.position, transform.position + direction, up);
    }

    glm::mat4 Camera::Projection() const
    {
        return projection;
    }
}
