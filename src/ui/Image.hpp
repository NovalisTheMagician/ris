#pragma once

#include "ui/Component.hpp"

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

            void SetImage(int image);
            void SetPosition(const glm::vec2 &position);
            void SetSize(const glm::vec2 &size);
            void SetColor(const glm::vec4 &color);

            void Update() override;
            void Draw(const glm::vec2 &parentPosition) override;

        private:
            int image = 1;
            glm::vec4 color = glm::vec4(1, 1, 1, 1);
            glm::vec2 position, size;

        };
        using ImagePtr = std::shared_ptr<Image>;
    }
}
