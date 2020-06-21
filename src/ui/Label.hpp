#pragma once

#include "ui/Component.hpp"

#include "graphics/Font.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS
{
    namespace UI
    {
        class Label : public Component
        {
        public:
            using Ptr = std::shared_ptr<Label>;
            static Ptr Create(std::shared_ptr<Graphics::Font> defaultFont);

            Label(std::shared_ptr<Graphics::Font> defaultFont);
            virtual ~Label() = default;
            Label(const Label&) = default;
            Label& operator=(const Label&) = default;
            Label(Label&&) = default;
            Label& operator=(Label&&) = default;

            void SetFont(std::shared_ptr<Graphics::Font> font);
            void SetFontSize(float fontSize);
            void SetTextColor(const glm::vec4 &color);
            void SetVisible(bool visible);
            void SetText(const std::string &text);

            void Update() override;
            void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

        private:
            std::string text = "";

            std::shared_ptr<Graphics::Font> font;
            float fontSize = -1;
            glm::vec4 fontColor = glm::vec4(1, 1, 1, 1);
            bool isVisible = true;

        };
    }
}
