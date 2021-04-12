#pragma once

#include <glm/glm.hpp>

#include "ui/Panel.hpp"

#include "graphics/Texture.hpp"

namespace RIS::UI
{
    struct ScrollButtons
    {
        Graphics::Texture::Ptr upNormal, upHover, upClick;
        Graphics::Texture::Ptr downNormal, downHover, downClick;
    };

    Panel& MakeScrollable(Panel& parentPanel, glm::vec2 scrollSize, const ScrollButtons &buttonTextures, glm::vec2 scrollStep = glm::vec2(16, 16), float scrollAreaWidth = 16.0f);
}
