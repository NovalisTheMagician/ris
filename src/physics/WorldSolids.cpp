#include "physics/WorldSolids.hpp"

#include <vector>
#include <algorithm>

namespace RIS::Physics
{
    WorldSolids::WorldSolids(std::vector<Brush> &&brushes)
        : brushes(std::move(brushes))
    {}

    bool WorldSolids::Collides(const glm::vec3 &position) const
    {
        for(const auto &brush : brushes)
        {
            bool isInside = true;
            for(const auto &plane : brush.planes)
            {
                float side = glm::dot(position, plane.normal) + plane.d;
                isInside &= side < 0;
            }
            if(isInside)
            {
                return true;
            }
        }
        return false;
    }

    bool WorldSolids::Collides(const glm::vec3 &position, float radius) const
    {
        for(const auto &brush : brushes)
        {
            bool isInside = true;
            for(const auto &plane : brush.planes)
            {
                glm::vec3 p = position - (plane.normal * radius);
                float side = glm::dot(p, plane.normal) + plane.d;
                isInside &= side < 0;
            }
            if(isInside)
            {
                return true;
            }
        }
        return false;
    }

    bool WorldSolids::Collides(const glm::vec3 &oldPosition, const glm::vec3 &newPosition, glm::vec3 &adjustedPos) const
    {
        glm::vec3 lineDir = (newPosition - oldPosition);

        for(const auto &brush : brushes)
        {
            bool isInside = true;
            for(const auto &plane : brush.planes)
            {
                float side = glm::dot(newPosition, plane.normal) + plane.d;
                isInside &= side < 0;
            }
            if(isInside)
            {
                //check which planes we crossed
                std::vector<glm::vec3> adjPoses;
                for(const auto &plane : brush.planes)
                {
                    float newSide = glm::dot(newPosition, plane.normal) + plane.d;
                    float oldSide = glm::dot(oldPosition, plane.normal) + plane.d;
                    if(newSide * oldSide < 0)
                    {
                        //float t = (plane.d - glm::dot(plane.normal, oldPosition)) / glm::dot(plane.normal, lineDir);
                        float t = (-(glm::dot(plane.normal, oldPosition) + plane.d)) / glm::dot(plane.normal, lineDir);
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

    bool WorldSolids::Collides(const glm::vec3 &oldPosition, const glm::vec3 &newPosition, float radius, glm::vec3 &adjustedPos) const
    {
        glm::vec3 lineDir = (newPosition - oldPosition);

        for(const auto &brush : brushes)
        {
            bool isInside = true;
            for(const auto &plane : brush.planes)
            {
                glm::vec3 np = newPosition - (plane.normal * radius);
                float side = glm::dot(np, plane.normal) + plane.d;
                isInside &= side < 0;
            }
            if(isInside)
            {
                //check which planes we crossed
                std::vector<glm::vec3> adjPoses;
                for(const auto &plane : brush.planes)
                {
                    glm::vec3 np = newPosition - (plane.normal * radius);
                    glm::vec3 op = oldPosition - (plane.normal * radius);
                    
                    float newSide = glm::dot(np, plane.normal) + plane.d;
                    float oldSide = glm::dot(op, plane.normal) + plane.d;

                    if(newSide * oldSide < 0)
                    {
                        float t = (-(glm::dot(plane.normal, op) + plane.d)) / glm::dot(plane.normal, lineDir);
                        if(t <= 1.0f && t >= 0.0f)
                        {
                            glm::vec3 newPos = op + (lineDir * t);
                            newPos += plane.normal * radius;
                            adjPoses.push_back(newPos);
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
