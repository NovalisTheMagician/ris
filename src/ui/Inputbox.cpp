#include "window/Window.hpp"
#include "RIS.hpp"

#include "ui/Inputbox.hpp"

#include <utf8.h>

#include <cmath>
#include "misc/MathHelper.hpp"

namespace RIS
{
    namespace UI
    {
        InputBox::Ptr InputBox::Create(std::shared_ptr<Graphics::Font> defaultFont)
        {
            return std::make_shared<InputBox>(defaultFont);
        }

        InputBox::InputBox(std::shared_ptr<Graphics::Font> defaultFont)
            : font(defaultFont)
        {}

        void InputBox::SetPreviewText(const std::string &previewText)
        {
            this->previewText = previewText;
        }

        void InputBox::SetText(const std::string &text)
        {
            this->text = text;
        }
        
        void InputBox::SetPreviewTextColor(const glm::vec4 &previewColor)
        {
            this->previewTextColor = previewColor;
        }
        
        void InputBox::SetTextColor(const glm::vec4 &textColor)
        {
            this->textColor = textColor;
        }
        
        void InputBox::SetFont(std::shared_ptr<Graphics::Font> font)
        {
            this->font = font;
        }
        
        void InputBox::SetFontSize(float fontSize)
        {
            this->fontSize = fontSize;
        }
        
        const std::string& InputBox::GetText() const
        {
            return text;
        }

        void InputBox::Update(const Timer &timer)
        {

        }

        void InputBox::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
        {
            parentPos = parentPosition;
            std::string caret = "|";
            if(!hasFocus)
                caret = "";

            glm::vec2 pos = position + parentPosition;
            glm::vec2 textPos = pos + glm::vec2(2, size.y / 4);

            renderer.DrawRect(pos, size, backColor);
            if(text.size() == 0 && !hasFocus)
                renderer.DrawString(previewText, *font, fontSize, textPos, previewTextColor);
            else
                renderer.DrawString(text + caret, *font, fontSize, textPos, textColor);
        }

        void InputBox::OnMouseDown(Input::InputKey mouseCode)
        {

        }

        void InputBox::OnMouseUp(Input::InputKey mouseCode)
        {
            if(mouseCode == Input::InputKey::MOUSE_LEFT)
            {
                hasFocus = isInBounds;
            }
        }

        void InputBox::OnMouseMove(float x, float y)
        {
            glm::vec2 pos = parentPos + position;

            if( x > pos.x && x < pos.x + size.x &&
                y > pos.y && y < pos.y + size.y)
                isInBounds = true;
            else 
                isInBounds = false;
        }

        void InputBox::OnChar(uint32_t c)
        {
            if(hasFocus)
                utf8::append(c, std::back_inserter(text));
        }

        void InputBox::OnKeyRepeat(Input::InputKey keyCode)
        {
            if(hasFocus)
                OnKey(keyCode, true);
        }

        void InputBox::OnKeyDown(Input::InputKey keyCode)
        {
            if(hasFocus)
                OnKey(keyCode, false);
        }

        void InputBox::OnKeyUp(Input::InputKey keyCode)
        {
            if(keyCode == Input::InputKey::LEFT_CONTROL)
                ctrlDown = false;
        }

        void InputBox::OnKey(Input::InputKey keyCode, bool repeat)
        {
            if(keyCode == Input::InputKey::BACKSPACE)
            {
                if(text.size() > 0)
                {
                    auto it = text.end();
                    utf8::prior(it, text.begin());
                    text.erase(it, text.end());
                }
            }
            else if(keyCode == Input::InputKey::LEFT)
            {
                caretPosition--;
                caretPosition = std::max(0, caretPosition);
            }
            else if(keyCode == Input::InputKey::RIGHT)
            {
                caretPosition++;
                caretPosition = std::min(static_cast<int>(text.size()), caretPosition);
            }
            else if(keyCode == Input::InputKey::LEFT_CONTROL && !repeat)
            {
                ctrlDown = true;
            }
            else if(keyCode == Input::InputKey::V && ctrlDown && hasFocus)
            {
                const auto &window = GetWindow();
                std::string clipbrd = window.GetClipboard();
                text += clipbrd;
            }
        }
    }
}
