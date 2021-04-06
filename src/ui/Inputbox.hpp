#pragma once

#include "ui/Component.hpp"

#include <glm/glm.hpp>

#include <string>

namespace RIS::UI
{
    class Inputbox : public Component<Inputbox>
    {
    public:
        Inputbox(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize);
        Inputbox(const Inputbox&) = delete;
        Inputbox& operator=(const Inputbox&) = delete;
        Inputbox(Inputbox&&) = default;
        Inputbox& operator=(Inputbox&&) = default;

        Inputbox& SetPreviewText(const std::string &previewText);
        Inputbox& SetText(const std::string &text);
        Inputbox& SetPreviewTextColor(const glm::vec4 &previewColor);
        Inputbox& SetTextColor(const glm::vec4 &textColor);
        Inputbox& SetFont(Graphics::Font::Ptr font);
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

    private:
        void OnKey(Input::InputKey key, bool repeat);

        void RecalcCharWidths();

    private:
        std::string previewText, text;

        glm::vec4 previewTextColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), textColor = glm::vec4(0, 0, 0, 1);
        glm::vec4 backColor = glm::vec4(1, 1, 1, 1);

        float fontHeight;
        std::vector<float> charWidths;

        bool isInBounds;

        bool hasFocus = false;
        int caretPosition = 0;
        float blinkTime = 0.7f;

        bool ctrlDown = false;

    };
}
