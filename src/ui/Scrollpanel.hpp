#pragma once

#include <glm/glm.hpp>

#include "ui/Panel.hpp"

namespace RIS::UI
{
    Panel& MakeScrollable(Panel& parentPanel, glm::vec2 scrollSize, glm::vec2 scrollStep = glm::vec2(16, 16), float scrollAreaWidth = 16.0f);
}
