#pragma once

#include <glm/glm.hpp>

namespace RIS::Graphics::Colors
{
    constexpr glm::vec4 Transparent = {0, 0, 0, 0};
    constexpr glm::vec4 White       = {1, 1, 1, 1};
    constexpr glm::vec4 Black       = {0, 0, 0, 1};
    constexpr glm::vec4 Grey        = {0.5f, 0.5f, 0.5f, 1};
    constexpr glm::vec4 LightGrey   = {0.85f, 0.85f, 0.85f, 1};
    constexpr glm::vec4 DarkGrey    = {0.15f, 0.15f, 0.15f, 1};
    constexpr glm::vec4 Red         = {1, 0, 0, 1};
    constexpr glm::vec4 Green       = {0, 1, 0, 1};
    constexpr glm::vec4 Blue        = {0, 0, 1, 1};
    constexpr glm::vec4 Cyan        = {0, 1, 1, 1};
    constexpr glm::vec4 Yellow      = {1, 1, 0, 1};
    constexpr glm::vec4 Magenta     = {1, 0, 1, 1};
    constexpr glm::vec4 CornflowerBlue = {0.392f, 0.584f, 0.929f, 1};
}
