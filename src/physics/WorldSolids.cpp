#include "physics/WorldSolids.hpp"

namespace RIS::Physics
{
    WorldSolids::WorldSolids(std::vector<Brush> &&brushes)
        : brushes(std::move(brushes))
    {}

    bool WorldSolids::Collides(const glm::vec3 &position, glm::vec3 &adjustedPos) const
    {
        for(const auto &brush : brushes)
        {
            bool isInside = true;
            for(const auto &plane : brush.planes)
            {
                float side = glm::dot(position, plane.normal) + plane.d;
                isInside &= side <= 0;
            }
            if(isInside)
                return true;
        }
        return false;
    }
}
