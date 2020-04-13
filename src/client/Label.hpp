#pragma once

#include "Component.hpp"

#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS
{
    class UILabel : public Component
    {
    public:
        UILabel(const SystemLocator &systems);
        virtual ~UILabel();

        void SetFont(int font, float fontSize);
        void SetTextColor(const glm::vec4 &color);
        void SetPosition(const glm::vec2 &position);
        void SetVisible(bool visible);
        void SetText(const std::string &text);

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        glm::vec2 position;
        std::string text = "";

        int font = 1;
        float fontSize = -1;
        glm::vec4 fontColor = glm::vec4(1, 1, 1, 1);
        bool isVisible = true;

    };
    using LabelPtr = std::shared_ptr<UILabel>;
}

#define MakeLabel(x) std::make_shared<UILabel>(x)
