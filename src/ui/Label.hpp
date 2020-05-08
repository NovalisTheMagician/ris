#pragma once

#include "ui/Component.hpp"

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
            Label() = default;
            virtual ~Label() = default;
            Label(const Label&) = default;
            Label& operator=(const Label&) = default;
            Label(Label&&) = default;
            Label& operator=(Label&&) = default;

            void SetFont(int font, float fontSize);
            void SetTextColor(const glm::vec4 &color);
            void SetPosition(const glm::vec2 &position);
            void SetVisible(bool visible);
            void SetText(const std::string &text);

            void Update() override;
            void Draw(const glm::vec2 &parentPosition) override;

        private:
            glm::vec2 position;
            std::string text = "";

            int font = 1;
            float fontSize = -1;
            glm::vec4 fontColor = glm::vec4(1, 1, 1, 1);
            bool isVisible = true;

        };
        using LabelPtr = std::shared_ptr<Label>;
    }
}
