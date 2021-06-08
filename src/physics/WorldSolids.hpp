#pragma once

#include <vector>
#include <memory>

#include "physics/Brush.hpp"

namespace RIS::Physics
{
    class WorldSolids
    {
    public:
        using Ptr = std::shared_ptr<WorldSolids>;

    public:
        WorldSolids(std::vector<Brush> &&brushes);

        bool Collides(const glm::vec3 &position) const;
        bool Collides(const glm::vec3 &position, float radius) const;
        bool Collides(const glm::vec3 &oldPosition, const glm::vec3 &newPosition, glm::vec3 &adjustedPos) const;
        bool Collides(const glm::vec3 &oldPosition, const glm::vec3 &newPosition, float radius, glm::vec3 &adjustedPos) const;

    private:
        std::vector<Brush> brushes;

    };
}
