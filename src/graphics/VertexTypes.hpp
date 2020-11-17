#pragma once

#include <glm/glm.hpp>

#include <graphics/VertexArray.hpp>

namespace RIS
{
    namespace VertexType
    {
        struct SpriteVertex
        {
            glm::vec2 position;
            glm::vec2 texCoords;
        };
        constexpr Graphics::AttribFormat SpriteVertexFormat[2] = {{0, 2, GL_FLOAT, offsetof(VertexType::SpriteVertex, position), false, 0}, {1, 2, GL_FLOAT, offsetof(VertexType::SpriteVertex, texCoords), false, 0}};

        struct ModelVertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
            glm::i16vec4 joints;
            glm::vec4 weights;
        };
        constexpr Graphics::AttribFormat ModelVertexFormat[5] = {{0, 3, GL_FLOAT, 0, false, 0},
                                                                 {1, 3, GL_FLOAT, 0, false, 1},
                                                                 {2, 2, GL_FLOAT, 0, false, 2},
                                                                 {3, 4, GL_UNSIGNED_SHORT, 0, false, 3},
                                                                 {4, 4, GL_FLOAT, 0, false, 4}};

        struct MapVertex
        {
            
        };
    }
}
