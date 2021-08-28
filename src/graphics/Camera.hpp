#pragma once

#include <glm/glm.hpp>

#include "graphics/Transform.hpp"

namespace RIS::Graphics
{
    class Camera
    {
    public:
        Camera();
        Camera(float fov, float aspect);
        Camera(float fov, float aspect, float nearPlane, float farPlane);

        void SetFoV(float fov);
        void SetAspect(float aspect);
        void SetNear(float nearPlane);
        void SetFar(float farPlane);

        float GetFov() const;

        Transform& GetTransform();

        glm::vec3 Direction() const;
        glm::vec3 Up() const;
        glm::vec3 Right() const;

        glm::vec3 YawDirection() const;

        void AddYaw(float amount);
        void AddPitch(float amount);
        void AddRoll(float amount);

        void SetYaw(float amount);
        void SetPitch(float amount);
        void SetRoll(float amount);

        glm::vec3 GetAngles() const;

        glm::vec3& Position();

        glm::mat4 ViewProj() const;
        glm::mat4 View() const;
        glm::mat4 Projection() const;

    private:
        float fov, aspect, nearPlane, farPlane;

        glm::mat4 projection;

        Transform transform;

        float yaw, pitch, roll;

    };
}
