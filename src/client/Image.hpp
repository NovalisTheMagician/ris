#pragma once

#include "Component.hpp"

#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS
{
    class UIImage : public Component
    {
    public:
        UIImage(const SystemLocator &systems);
        virtual ~UIImage();

        void SetImage(int image);
        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);
        void SetColor(const glm::vec4 &color);

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        int image = 1;
        glm::vec4 color = glm::vec4(1, 1, 1, 1);
        glm::vec2 position, size;

    };
    using ImagePtr = std::shared_ptr<UIImage>;
}

#define MakeImage(x) std::make_shared<UIImage>(x)
