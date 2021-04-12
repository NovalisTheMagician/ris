#include "window/Window.hpp"
#include "RIS.hpp"

#include "ui/Inputbox.hpp"

#include <utf8.h>

#include <cmath>
#include "misc/MathHelper.hpp"

#include <string_view>

namespace RIS::UI
{
    Inputbox::Inputbox(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, const glm::vec2 &parentSize)
        : Component(parentFramebuffer, defaultFont, parentSize)
    {
        fontHeight = font->GetMaxHeight(fontSize);
    }

    Inputbox& Inputbox::SetPreviewText(const std::string &previewText)
    {
        this->previewText = previewText;
        return *this;
    }

    Inputbox& Inputbox::SetText(const std::string &text)
    {
        this->text = text;
        RecalcCharWidths();

        caretPosition = static_cast<int>(utf8::distance(text.begin(), text.end()));
        return *this;
    }
    
    Inputbox& Inputbox::SetPreviewTextColor(const glm::vec4 &previewColor)
    {
        this->previewTextColor = previewColor;
        return *this;
    }
    
    Inputbox& Inputbox::SetTextColor(const glm::vec4 &textColor)
    {
        this->textColor = textColor;
        return *this;
    }
    
    Inputbox& Inputbox::SetFont(std::shared_ptr<Graphics::Font> font)
    {
        this->font = font;
        fontHeight = this->font->GetMaxHeight(fontSize);
        RecalcCharWidths();
        return *this;
    }
    
    Inputbox& Inputbox::SetFontSize(float fontSize)
    {
        this->fontSize = fontSize;
        fontHeight = font->GetMaxHeight(this->fontSize);
        RecalcCharWidths();
        return *this;
    }
    
    const std::string& Inputbox::GetText() const
    {
        return text;
    }

    void Inputbox::Update(const Timer &timer)
    {

    }

    void Inputbox::Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset)
    {
        if(!visible) return;

        glm::vec2 pos = GetAnchoredPosition() + offset;
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

    void Inputbox::OnMouseDown(Input::InputKey mouseCode)
    {
        if(!visible) return;
    }

    void Inputbox::OnMouseUp(Input::InputKey mouseCode)
    {
        if(!visible) return;
        if(mouseCode == Input::InputKey::MOUSE_LEFT)
        {
            hasFocus = isInBounds;
            if(hasFocus)
                caretPosition = static_cast<int>(utf8::distance(text.begin(), text.end()));
        }
    }

    void Inputbox::OnMouseMove(float x, float y)
    {
        if(!visible) return;
        glm::vec2 pos = GetAnchoredPosition();
        if( x > pos.x && x < pos.x + size.x &&
            y > pos.y && y < pos.y + size.y)
            isInBounds = true;
        else 
            isInBounds = false;
    }

    void Inputbox::OnChar(uint32_t c)
    {
        if(!visible) return;
        if(hasFocus)
        {
            auto it = text.begin();
            for(int i = 0; i < caretPosition; ++i)
                utf8::next(it, text.end());

            utf8::append(c, std::inserter(text, it));

            RecalcCharWidths();

            caretPosition++;
        }
    }

    void Inputbox::OnKeyRepeat(Input::InputKey keyCode)
    {
        OnKey(keyCode, true);
    }

    void Inputbox::OnKeyDown(Input::InputKey keyCode)
    {
        OnKey(keyCode, false);
    }

    void Inputbox::OnKeyUp(Input::InputKey keyCode)
    {
        if(!visible) return;
        if(keyCode == Input::InputKey::LEFT_CONTROL)
            ctrlDown = false;
    }

    void Inputbox::OnKey(Input::InputKey keyCode, bool repeat)
    {
        if(!visible) return;
        if(keyCode == Input::InputKey::BACKSPACE && hasFocus)
        {
            if(text.size() > 0 && caretPosition > 0)
            {
                auto it = text.begin();
                for(int i = 0; i < caretPosition; ++i)
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
            int numChars = static_cast<int>(utf8::distance(text.begin(), text.end()));
            if(text.size() > 0 && caretPosition < numChars)
            {
                auto it = text.begin();
                for(int i = 0; i < caretPosition + 1; ++i)
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
            for(int i = 0; i < caretPosition; ++i)
                utf8::next(it, text.end());
            
            text.insert(it, clipbrd.begin(), clipbrd.end());

            caretPosition += static_cast<int>(utf8::distance(clipbrd.begin(), clipbrd.end()));

            RecalcCharWidths();
        }
        else if(keyCode == Input::InputKey::C && ctrlDown && hasFocus)
        {
            const auto &window = GetWindow();
            window.SetClipboard(text);
        }
    }

    void Inputbox::RecalcCharWidths()
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
