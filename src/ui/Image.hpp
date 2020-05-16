#pragma once

#include "ui/Component.hpp"

#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS
{
    namespace UI
    {
        class Image : public Component
        {
        public:
            Image() = default;
            virtual ~Image() = default;
            Image(const Image&) = default;
            Image& operator=(const Image&) = default;
            Image(Image&&) = default;
            Image& operator=(Image&&) = default;

            void SetImage(std::shared_ptr<Graphics::Texture> image);
            void SetPosition(const glm::vec2 &position);
            void SetSize(const glm::vec2 &size);
            void SetColor(const glm::vec4 &color);

            void Update() override;
            void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

        private:
            std::shared_ptr<Graphics::Texture> image;
            glm::vec4 color = glm::vec4(1, 1, 1, 1);
            glm::vec2 position, size;

        };
        using ImagePtr = std::shared_ptr<Image>;
    }
}
