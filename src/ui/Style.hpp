#pragma once

#include "graphics/Font.hpp"
#include "graphics/Texture.hpp"
#include "graphics/SpriteRenderer.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <string>

namespace RIS::UI
{
    struct Region
    {
        glm::vec2 topLeft;
        glm::vec2 botRight;
    };

    enum class ButtonState
    {
        Normal,
        Hover,
        Click,
        Disable
    };

    struct ButtonStyle
    {
        Graphics::Font font;
        std::optional<Region> normalState;
        std::optional<Region> hoverState;
        std::optional<Region> clickState;
        std::optional<Region> disableState;
    };

    class Style
    {
    public:
        void DrawButton(const glm::vec2 &position, const glm::vec2 &size, ButtonState state);
        void DrawPanel(const glm::vec2 &position, const glm::vec2 &size);
        void DrawInputbox(const glm::vec2 &position, const glm::vec2 &size);
    
    private:
        Graphics::Texture::Ptr uiAtlas;
        Graphics::Font::Ptr font;

    };
}
