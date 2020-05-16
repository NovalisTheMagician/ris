#include "ui/Image.hpp"

namespace RIS
{
    namespace UI
    {
        void Image::SetImage(std::shared_ptr<Graphics::Texture> image)
        {
            this->image = image;
        }

        void Image::SetColor(const glm::vec4 &color)
        {
            this->color = color;
        }

        void Image::SetPosition(const glm::vec2 &position)
        {
            this->position = position;
        }

        void Image::SetSize(const glm::vec2 &size)
        {
            this->size = size;
        }

        void Image::Update()
        {

        }

        void Image::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
        {
            glm::vec2 pos = parentPosition + position;
            if(image)
                renderer.DrawTexture(*image, position, size, {1, 1, 1, 1});
        }
    }
}
