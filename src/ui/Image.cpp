#include "ui/Image.hpp"

namespace RIS::UI
{
    Image::Ptr Image::Create()
    {
        return std::make_shared<Image>();
    }

    void Image::SetImage(std::shared_ptr<Graphics::Texture> image)
    {
        this->image = image;
    }

    void Image::SetColor(const glm::vec4 &color)
    {
        this->color = color;
    }

    void Image::Update(const Timer &timer)
    {

    }

    void Image::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
    {
        glm::vec2 pos = parentPosition + position;
        if(image)
            renderer.DrawTexture(*image, pos, size, {1, 1, 1, 1});
    }
}
