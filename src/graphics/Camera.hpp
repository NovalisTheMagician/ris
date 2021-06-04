#pragma once

#include <glm/glm.hpp>

#include "graphics/Transform.hpp"

namespace RIS::Graphics
{
    class Camera
    {
    public:
        Camera() = default;
        Camera(float fov, float aspect);

        void SetFoV(float fov);
        void SetAspect(float aspect);

        Transform& GetTransform();

        glm::vec3 Direction() const;
        glm::vec3 Up() const;
        glm::vec3 Right() const;

        glm::vec3 YawDirection() const;

        float& Yaw();
        float& Pitch();
        float& Roll();

        void AddYaw(float amount);
        void AddPitch(float amount);
        void AddRoll(float amount);

        glm::mat4 ViewProj() const;
        glm::mat4 View() const;
        glm::mat4 Projection() const;

    private:
        float fov, aspect;

        glm::mat4 projection;
        glm::mat4 view;

        Transform transform;

        glm::vec3 position;

        float yaw, pitch, roll;

    };
}
