#include "ui/Image.hpp"

namespace RIS
{
    namespace UI
    {
        void Image::SetImage(int image)
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

        void Image::Draw(const glm::vec2 &parentPosition)
        {
            glm::vec2 pos = parentPosition + position;
            //renderer.SetTexture(image, 0);
            //renderer.DrawQuad(position, size, {1, 1, 1, 1});
        }
    }
}
