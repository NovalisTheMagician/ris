#include "Image.hpp"

namespace RIS
{
    UIImage::UIImage(const SystemLocator &systems)
        : Component(systems), image(1)
    {
    }

    UIImage::~UIImage()
    {
    }

    void UIImage::SetImage(int image)
    {
        this->image = image;
    }

    void UIImage::SetPosition(const glm::vec2 &position)
    {
        this->position = position;
    }

    void UIImage::SetSize(const glm::vec2 &size)
    {
        this->size = size;
    }

    void UIImage::Update()
    {

    }

    void UIImage::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        glm::vec2 pos = parentPosition + position;
        renderer.SetTexture(image, 0);
        renderer.DrawQuad(position, size, {1, 1, 1, 1});
    }
}
