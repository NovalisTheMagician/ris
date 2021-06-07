#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace RIS::Physics
{
    struct Plane
    {
        glm::vec3 normal;
        glm::vec3 point;
        float d;
    };

    struct Brush
    {
        std::vector<Plane> planes;
    };
}
