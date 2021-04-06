#pragma once

#include "ui/Component.hpp"

#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>

namespace RIS::UI
{
    class Image : public Component<Image>
    {
    public:
        Image(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize);
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;

        Image& SetImage(Graphics::Texture::Ptr image);
        Image& SetColor(const glm::vec4 &color);

        void Draw(Graphics::SpriteRenderer &renderer);

    private:
        Graphics::Texture::Ptr image;
        glm::vec4 color = glm::vec4(1, 1, 1, 1);

    };
}
