#pragma once

#include "ui/Component.hpp"

#include "graphics/Font.hpp"
#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include <functional>

namespace RIS
{
    namespace UI
    {
        using ButtonFunc = std::function<void()>;

        class Button : public Component
        {
        public:
            using Ptr = std::shared_ptr<Button>;
            static Ptr Create(std::shared_ptr<Graphics::Font> defaultFont);

            Button(std::shared_ptr<Graphics::Font> defaultFont);
            virtual ~Button() = default;
            Button(const Button&) = default;
            Button& operator=(const Button&) = default;
            Button(Button&&) = default;
            Button& operator=(Button&&) = default;

            void SetText(const std::string &text);
            void SetFont(std::shared_ptr<Graphics::Font> font);
            void SetFontSize(float size);
            void SetTextColor(const glm::vec4 &color);

            void SetColors(const glm::vec4 &normal, const glm::vec4 &hover, const glm::vec4 &down);
            void SetImages(std::shared_ptr<Graphics::Texture> normal, std::shared_ptr<Graphics::Texture> hover, std::shared_ptr<Graphics::Texture> down);

            void SetNormalColor(const glm::vec4 &color);
            void SetNormalImage(std::shared_ptr<Graphics::Texture> image);
            void SetHoverColor(const glm::vec4 &color);
            void SetHoverImage(std::shared_ptr<Graphics::Texture> image);
            void SetDownColor(const glm::vec4 &color);
            void SetDownImage(std::shared_ptr<Graphics::Texture> image);

            void SetCallback(ButtonFunc func);

            void Update(const Timer &timer) override;
            void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

            void OnMouseMove(float x, float y) override;
            void OnMouseDown(Input::InputKey button) override;
            void OnMouseUp(Input::InputKey button) override;

        private:
            std::string text = "";

            std::shared_ptr<Graphics::Font> font;
            float fontSize = -1;
            glm::vec4 textColor = glm::vec4(0, 0, 0, 1);

            glm::vec4 normalColor = glm::vec4(1, 1, 1, 1);
            glm::vec4 hoverColor = glm::vec4(0.7f, 0.7f, 0.7f, 1);
            glm::vec4 downColor = glm::vec4(0.5f, 0.5f, 0.5f, 1);
            std::shared_ptr<Graphics::Texture> normalImage, hoverImage, downImage;

            bool isInBounds = false;
            bool isClickedDown = false;

            ButtonFunc callback;

        };
    }
}
