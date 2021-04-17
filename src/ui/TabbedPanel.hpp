#pragma once

#include "ui/Panel.hpp"

#include "graphics/Font.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <string_view>

namespace RIS::UI
{
    std::vector<std::reference_wrapper<Panel>> MakeTabbable(Panel& parentPanel, const std::vector<std::string_view> &names, const glm::vec2 &buttonSize, float margin = 5);
}
