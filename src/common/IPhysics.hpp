#pragma once

#include <glm/glm.hpp>

namespace RIS
{
    class IPhysics
    {
    public:
        virtual ~IPhysics() = default;

        virtual void SetGlobalParameters(const glm::vec3 &gravity) = 0;
        virtual void SetTimeStep(float timeStep) = 0;
        virtual void Update() = 0;
    };
}
