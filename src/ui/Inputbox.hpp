#pragma once

#include "ui/Component.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS
{
    namespace UI
    {
        class InputBox : public Component
        {
        public:
            using Ptr = std::shared_ptr<InputBox>;
            static Ptr Create(std::shared_ptr<Graphics::Font> defaultFont);

            InputBox(std::shared_ptr<Graphics::Font> defaultFont);
            virtual ~InputBox() = default;
            InputBox(const InputBox&) = default;
            InputBox& operator=(const InputBox&) = default;
            InputBox(InputBox&&) = default;
            InputBox& operator=(InputBox&&) = default;

            void SetPreviewText(const std::string &previewText);
            void SetText(const std::string &text);
            void SetPreviewTextColor(const glm::vec4 &previewColor);
            void SetTextColor(const glm::vec4 &textColor);
            void SetFont(std::shared_ptr<Graphics::Font> font);
            void SetFontSize(float fontSize);

            const std::string& GetText() const;

            void Update(const Timer &timer) override;
            void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

            void OnMouseDown(Input::InputKey mouseCode) override;
            void OnMouseUp(Input::InputKey mouseCode) override;
            void OnMouseMove(float x, float y) override;
            void OnChar(uint32_t c) override;
            void OnKeyRepeat(Input::InputKey keyCode) override;
            void OnKeyUp(Input::InputKey keyCode) override;
            void OnKeyDown(Input::InputKey keyCode) override;

        private:
            void OnKey(Input::InputKey key, bool repeat);

            void RecalcCharWidths();

        private:
            std::string previewText, text;

            glm::vec4 previewTextColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), textColor = glm::vec4(0, 0, 0, 1);
            glm::vec4 backColor = glm::vec4(1, 1, 1, 1);
            std::shared_ptr<Graphics::Font> font;
            float fontSize = 16;

            float fontHeight;
            std::vector<float> charWidths;

            bool isInBounds;

            bool hasFocus = false;
            int caretPosition = 0;
            float blinkTime = 0.7f;

            bool ctrlDown = false;

        };
    }
}
