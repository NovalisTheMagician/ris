#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace RIS::Physics
{
    struct Plane
    {
        glm::vec3 normal;
        float d;
    };

    struct Brush
    {
        std::vector<Plane> planes;
    };
}
