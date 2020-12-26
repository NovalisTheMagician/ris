#pragma once

#include "ui/Component.hpp"

#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS::UI
{
    class Image : public Component
    {
    public:
        using Ptr = std::shared_ptr<Image>;
        static Ptr Create();

        Image() = default;
        virtual ~Image() = default;
        Image(const Image&) = default;
        Image& operator=(const Image&) = default;
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;

        void SetImage(std::shared_ptr<Graphics::Texture> image);
        void SetColor(const glm::vec4 &color);

        void Update(const Timer &timer) override;
        void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        std::shared_ptr<Graphics::Texture> image;
        glm::vec4 color = glm::vec4(1, 1, 1, 1);

    };
}
