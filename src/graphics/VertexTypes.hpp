#pragma once

#include <glm/glm.hpp>

namespace RIS
{
    namespace VertexType
    {
        struct SpriteVertex
        {
            glm::vec2 position;
            glm::vec2 texCoords;
        };

        struct ModelVertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
            glm::i8vec4 joints;
            glm::vec4 weights;
        };

        struct TerrainVertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord0;
            glm::vec2 texCoord1;
        };
    }
}
