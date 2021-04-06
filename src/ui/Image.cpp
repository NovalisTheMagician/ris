#include "ui/Image.hpp"

namespace RIS::UI
{
    Image::Image(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize)
        : Component(parentFramebuffer, defaultFont, parentSize)
    {}

    Image& Image::SetImage(std::shared_ptr<Graphics::Texture> image)
    {
        this->image = image;
        return *this;
    }

    Image& Image::SetColor(const glm::vec4 &color)
    {
        this->color = color;
        return *this;
    }

    void Image::Draw(Graphics::SpriteRenderer &renderer)
    {
        if(image)
            renderer.DrawTexture(*image, position, size, {1, 1, 1, 1});
    }
}
