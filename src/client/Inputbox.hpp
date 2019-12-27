#pragma once

#include "Component.hpp"

#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS
{
    class UIInputBox : public Component
    {
    public:
        UIInputBox(const SystemLocator &systems);
        virtual ~UIInputBox();

        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);

        void SetPreviewText(const std::string &previewText);
        void SetText(const std::string &text);
        void SetPreviewTextColor(const glm::vec4 &previewColor);
        void SetTextColor(const glm::vec4 &textColor);
        void SetFont(int font, float fontSize);

        std::string GetText() const;

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        std::string previewText, text;
        glm::vec2 position, size;

        glm::vec4 previewTextColor, textColor;
        int font;
        float fontSize;

    };
    using InputBoxPtr = std::shared_ptr<UIInputBox>;
}

#define MakeInputBox(x) std::make_shared<UIInputBox>(x)
