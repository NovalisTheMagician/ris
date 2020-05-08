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
            InputBox() = default;
            virtual ~InputBox() = default;
            InputBox(const InputBox&) = default;
            InputBox& operator=(const InputBox&) = default;
            InputBox(InputBox&&) = default;
            InputBox& operator=(InputBox&&) = default;

            void SetPosition(const glm::vec2 &position);
            void SetSize(const glm::vec2 &size);

            void SetPreviewText(const std::string &previewText);
            void SetText(const std::string &text);
            void SetPreviewTextColor(const glm::vec4 &previewColor);
            void SetTextColor(const glm::vec4 &textColor);
            void SetFont(int font, float fontSize);

            std::string GetText() const;

            void Update() override;
            void Draw(const glm::vec2 &parentPosition) override;

        private:
            std::string previewText, text;
            glm::vec2 position, size;

            glm::vec4 previewTextColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), textColor = glm::vec4(0, 0, 0, 1);
            int font;
            float fontSize;

        };
        using InputBoxPtr = std::shared_ptr<InputBox>;
    }
}
