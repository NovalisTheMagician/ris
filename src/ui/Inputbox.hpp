#pragma once

#include "ui/Component.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS::UI
{
    class Inputbox
    {
    public:
        Inputbox(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont);
        Inputbox(const Inputbox&) = delete;
        Inputbox& operator=(const Inputbox&) = delete;
        Inputbox(Inputbox&&) = default;
        Inputbox& operator=(Inputbox&&) = default;

        Inputbox& SetName(const std::string &name);
        std::string GetName() const;
        Inputbox& SetAnchor(Anchor anchor);
        Anchor GetAnchor() const;
        Inputbox& SetPosition(const glm::vec2 &position);
        Inputbox& SetSize(const glm::vec2 &size);
        Inputbox& SetPreviewText(const std::string &previewText);
        Inputbox& SetText(const std::string &text);
        Inputbox& SetPreviewTextColor(const glm::vec4 &previewColor);
        Inputbox& SetTextColor(const glm::vec4 &textColor);
        Inputbox& SetFont(std::shared_ptr<Graphics::Font> font);
        Inputbox& SetFontSize(float fontSize);

        const std::string& GetText() const;

        void Update(const Timer &timer);
        void Draw(Graphics::SpriteRenderer &renderer);

        void OnMouseDown(Input::InputKey mouseCode);
        void OnMouseUp(Input::InputKey mouseCode);
        void OnMouseMove(float x, float y);
        void OnChar(uint32_t c);
        void OnKeyRepeat(Input::InputKey keyCode);
        void OnKeyUp(Input::InputKey keyCode);
        void OnKeyDown(Input::InputKey keyCode);

        void OnMouseWheel(float x, float y) {};

    private:
        void OnKey(Input::InputKey key, bool repeat);

        void RecalcCharWidths();

    private:
        std::string name;
        glm::vec2 position;
        glm::vec2 size = glm::vec2(64, 32);
        Anchor anchor = Anchor::TopLeft;
        Graphics::Framebuffer &parentFramebuffer;
        Graphics::Font::Ptr font;

        std::string previewText, text;

        glm::vec4 previewTextColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), textColor = glm::vec4(0, 0, 0, 1);
        glm::vec4 backColor = glm::vec4(1, 1, 1, 1);
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
