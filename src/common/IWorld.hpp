#pragma once

#include "common/Timer.hpp"

#include <string>

#include <glm/glm.hpp>

namespace RIS
{
    using ActorId = int;

    struct ActorInfo
    {
        std::string archeType;
        glm::vec3 position;
        glm::vec3 orientation;
        int health;
    };

    class IWorld
    {
    public:
        virtual ~IWorld() = default;

        virtual void PostInit() = 0;

        virtual void Load(const std::string &worldName) = 0;

        virtual ActorId CreateActor(const ActorInfo &info) = 0;
        virtual void DestroyActor(ActorId id) = 0;

        virtual void Update(const Timer &timer) = 0;
        virtual void Draw() = 0;
    };
}
