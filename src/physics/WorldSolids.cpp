#include "physics/WorldSolids.hpp"

#include <vector>
#include <algorithm>

namespace RIS::Physics
{
    WorldSolids::WorldSolids(std::vector<Brush> &&brushes)
        : brushes(std::move(brushes))
    {}

    bool WorldSolids::Collides(const glm::vec3 &oldPosition, const glm::vec3 &newPosition, glm::vec3 &adjustedPos) const
    {
        glm::vec3 lineDir = (newPosition - oldPosition);

        for(const auto &brush : brushes)
        {
            bool isInside = true;
            //std::vector<glm::vec3> adjPoses;
            for(const auto &plane : brush.planes)
            {
                float side = glm::dot(newPosition, plane.normal) + plane.d;
                isInside &= side <= 0;
                /*
                float oldSide = glm::dot(oldPosition, plane.normal) + plane.d;

                if((side <= 0) != (oldSide <= 0))
                {
                    float t = (glm::dot(plane.normal, plane.point) - glm::dot(plane.normal, oldPosition)) / glm::dot(plane.normal, lineDir);
                    if(t <= 1.0f && t > 0.0f)
                    {
                        adjPoses.push_back(oldPosition + (lineDir * t));
                    }
                }
                */
            }
            if(isInside)
            {
                //check which planes we crossed
                std::vector<glm::vec3> adjPoses;
                for(const auto &plane : brush.planes)
                {
                    float newSide = glm::dot(newPosition, plane.normal) + plane.d;
                    float oldSide = glm::dot(oldPosition, plane.normal) + plane.d;
                    if((newSide <= 0) != (oldSide <= 0))
                    {
                        float t = (plane.d - glm::dot(plane.normal, oldPosition)) / glm::dot(plane.normal, lineDir);
                        if(t <= 1.0f && t >= 0.0f)
                        {
                            adjPoses.push_back(oldPosition + (lineDir * t));
                        }
                    }
                }

                auto minElement = std::min_element(std::begin(adjPoses), std::end(adjPoses), [oldPosition](const auto &first, const auto &smallest) { return glm::distance(first, oldPosition) < glm::distance(smallest, oldPosition); });
                if(minElement != std::end(adjPoses))
                    adjustedPos = *minElement;

                return true;
            }
        }
        return false;
    }
}
