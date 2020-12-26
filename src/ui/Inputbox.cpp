#include "window/Window.hpp"
#include "RIS.hpp"

#include "ui/Inputbox.hpp"

#include <utf8.h>

#include <cmath>
#include "misc/MathHelper.hpp"

#include <string_view>

namespace RIS::UI
{
    InputBox::Ptr InputBox::Create(std::shared_ptr<Graphics::Font> defaultFont)
    {
        return std::make_shared<InputBox>(defaultFont);
    }

    InputBox::InputBox(std::shared_ptr<Graphics::Font> defaultFont)
        : font(defaultFont)
    {
        fontHeight = font->GetMaxHeight(fontSize);
    }

    void InputBox::SetPreviewText(const std::string &previewText)
    {
        this->previewText = previewText;
    }

    void InputBox::SetText(const std::string &text)
    {
        this->text = text;
        RecalcCharWidths();

        caretPosition = utf8::distance(text.begin(), text.end());
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
        fontHeight = this->font->GetMaxHeight(fontSize);
        RecalcCharWidths();
    }
    
    void InputBox::SetFontSize(float fontSize)
    {
        this->fontSize = fontSize;
        fontHeight = font->GetMaxHeight(this->fontSize);
        RecalcCharWidths();
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
        glm::vec2 pos = position + parentPosition;
        glm::vec2 textPos = pos + glm::vec2(2, size.y / 4);

        renderer.DrawRect(pos, size, backColor);
        if(text.empty() && !hasFocus)
            renderer.DrawString(previewText, *font, fontSize, textPos, previewTextColor);
        else
        {
            renderer.DrawString(text, *font, fontSize, textPos, textColor);
            if(hasFocus)
            {
                float width = 0;
                if(caretPosition > 0)
                    width = charWidths.at(caretPosition - 1);
                glm::vec2 caretPos(textPos.x + width, textPos.y - 2);
                renderer.DrawRect(caretPos, {1, fontHeight}, textColor);
            }
        }
    }

    void InputBox::OnMouseDown(Input::InputKey mouseCode)
    {

    }

    void InputBox::OnMouseUp(Input::InputKey mouseCode)
    {
        if(mouseCode == Input::InputKey::MOUSE_LEFT)
        {
            hasFocus = isInBounds;
            if(hasFocus)
                caretPosition = utf8::distance(text.begin(), text.end());
        }
    }

    void InputBox::OnMouseMove(float x, float y)
    {
        if( x > position.x && x < position.x + size.x &&
            y > position.y && y < position.y + size.y)
            isInBounds = true;
        else 
            isInBounds = false;
    }

    void InputBox::OnChar(uint32_t c)
    {
        if(hasFocus)
        {
            auto it = text.begin();
            for(std::size_t i = 0; i < caretPosition; ++i)
                utf8::next(it, text.end());

            utf8::append(c, std::inserter(text, it));

            RecalcCharWidths();

            caretPosition++;
        }
    }

    void InputBox::OnKeyRepeat(Input::InputKey keyCode)
    {
        OnKey(keyCode, true);
    }

    void InputBox::OnKeyDown(Input::InputKey keyCode)
    {
        OnKey(keyCode, false);
    }

    void InputBox::OnKeyUp(Input::InputKey keyCode)
    {
        if(keyCode == Input::InputKey::LEFT_CONTROL)
            ctrlDown = false;
    }

    void InputBox::OnKey(Input::InputKey keyCode, bool repeat)
    {
        if(keyCode == Input::InputKey::BACKSPACE && hasFocus)
        {
            if(text.size() > 0 && caretPosition > 0)
            {
                auto it = text.begin();
                for(std::size_t i = 0; i < caretPosition; ++i)
                    utf8::next(it, text.end());

                auto toDelete = it;
                utf8::prior(toDelete, text.begin());
                text.erase(toDelete, it);

                caretPosition--;
                caretPosition = std::max(0, caretPosition);

                RecalcCharWidths();
            }
        }
        else if(keyCode == Input::InputKey::DELETE && hasFocus)
        {
            std::size_t numChars = utf8::distance(text.begin(), text.end());
            if(text.size() > 0 && caretPosition < numChars)
            {
                auto it = text.begin();
                for(std::size_t i = 0; i < caretPosition + 1; ++i)
                    utf8::next(it, text.end());

                auto toDelete = it;
                utf8::prior(toDelete, text.begin());
                text.erase(toDelete, it);

                RecalcCharWidths();
            }
        }
        else if(keyCode == Input::InputKey::LEFT && hasFocus)
        {
            caretPosition--;
            caretPosition = std::max(0, caretPosition);
        }
        else if(keyCode == Input::InputKey::RIGHT && hasFocus)
        {
            caretPosition++;
            caretPosition = std::min(static_cast<int>(utf8::distance(text.begin(), text.end())), caretPosition);
        }
        else if(keyCode == Input::InputKey::LEFT_CONTROL && !repeat)
        {
            ctrlDown = true;
        }
        else if(keyCode == Input::InputKey::V && ctrlDown && hasFocus)
        {
            const auto &window = GetWindow();
            std::string clipbrd = window.GetClipboard();

            auto it = text.begin();
            for(std::size_t i = 0; i < caretPosition; ++i)
                utf8::next(it, text.end());
            
            text.insert(it, clipbrd.begin(), clipbrd.end());

            caretPosition += utf8::distance(clipbrd.begin(), clipbrd.end());

            RecalcCharWidths();
        }
        else if(keyCode == Input::InputKey::C && ctrlDown && hasFocus)
        {
            const auto &window = GetWindow();
            window.SetClipboard(text);
        }
    }

    void InputBox::RecalcCharWidths()
    {
        charWidths.clear();

        std::string_view view = text;
        std::size_t numChars = utf8::distance(view.begin(), view.end());

        auto it = view.begin();

        for(std::size_t i = 0; i < numChars; ++i)
        {
            utf8::next(it, view.end());
            std::string str(view.begin(), it);

            auto strMeasure = font->MeasureString(str, fontSize);
            charWidths.push_back(strMeasure.width);
        }
    }
}
