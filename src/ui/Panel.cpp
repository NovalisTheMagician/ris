#include "ui/Panel.hpp"

#include <algorithm>

#include "ui/Button.hpp"
#include "ui/Image.hpp"
#include "ui/Inputbox.hpp"
#include "ui/Label.hpp"

namespace RIS::UI
{
    Panel::Panel(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, glm::vec2 parentSize)
        : parentFramebuffer(parentFramebuffer), font(defaultFont), panelFramebuffer(static_cast<int>(size.x), static_cast<int>(size.y)), parentSize(parentSize)
    {}

    Panel& Panel::SetName(const std::string &name)
    { 
        this->name = name; return *this; 
    }

    std::string Panel::GetName() const 
    { 
        return name; 
    }

    Panel& Panel::SetAnchor(Anchor anchor) 
    { 
        this->anchor = anchor; 
        return *this; 
    }

    Anchor Panel::GetAnchor() const 
    { 
        return anchor; 
    }

    Panel& Panel::SetPosition(const glm::vec2 &position) 
    { 
        this->position = position; 
        return *this; 
    }

    Panel& Panel::SetSize(const glm::vec2 &size) 
    { 
        this->size = size; 
        panelFramebuffer.Resize(static_cast<int>(size.x), static_cast<int>(size.y));
        return *this; 
    }

    Panel& Panel::SetColor(const glm::vec4 &color)
    {
        this->color = color;
        return *this;
    }

    void Panel::OnChar(uint32_t c)
    {
        std::for_each(components.begin(), components.end(), [c](auto &component){ std::visit([c](auto &&comp){ comp.OnChar(c); }, component); });
    }

    void Panel::OnMouseMove(float x, float y)
    {
        float relX = x - position.x;
        float relY = y - position.y;
        std::for_each(components.begin(), components.end(), [relX, relY](auto &component){  std::visit([relX, relY](auto &&comp){ comp.OnMouseMove(relX, relY); }, component); });
    }

    void Panel::OnMouseDown(Input::InputKey mouseCode)
    {
        std::for_each(components.begin(), components.end(), [mouseCode](auto &component){ std::visit([mouseCode](auto &&comp){ comp.OnMouseDown(mouseCode); }, component); });
    }

    void Panel::OnMouseUp(Input::InputKey mouseCode)
    {
        std::for_each(components.begin(), components.end(), [mouseCode](auto &component){ std::visit([mouseCode](auto &&comp){ comp.OnMouseUp(mouseCode); }, component); });
    }

    void Panel::OnMouseWheel(float x, float y)
    {
        std::for_each(components.begin(), components.end(), [x, y](auto &component){ std::visit([x, y](auto &&comp){ comp.OnMouseWheel(x, y); }, component); });
    }

    void Panel::OnKeyDown(Input::InputKey keyCode)
    {
        std::for_each(components.begin(), components.end(), [keyCode](auto &component){ std::visit([keyCode](auto &&comp){ comp.OnKeyDown(keyCode); }, component); });
    }

    void Panel::OnKeyUp(Input::InputKey keyCode)
    {
        std::for_each(components.begin(), components.end(), [keyCode](auto &component){ std::visit([keyCode](auto &&comp){ comp.OnKeyUp(keyCode); }, component); });
    }

    void Panel::OnKeyRepeat(Input::InputKey keyCode)
    {
        std::for_each(components.begin(), components.end(), [keyCode](auto &component){ std::visit([keyCode](auto &&comp){ comp.OnKeyRepeat(keyCode); }, component); });
    }

    void Panel::Update(const Timer &timer)
    {
        std::for_each(components.begin(), components.end(), [&timer](auto &component){ std::visit([&timer](auto &&comp){ comp.Update(timer); }, component); });
    }

    void Panel::Draw(Graphics::SpriteRenderer &renderer)
    {
        /*
        if(backgroundImage)
            renderer.DrawTexture(*backgroundImage, position, size, color);
        else
            renderer.DrawRect(position, size, color);
        */
        panelFramebuffer.Bind();
        panelFramebuffer.Clear({0, 1.0f, 0, 1.0f}, 1.0f);
        renderer.SetViewport(size.x, size.y, true);
        std::for_each(components.begin(), components.end(), [&renderer](auto &component){ std::visit([&renderer](auto &&comp){ comp.Draw(renderer); }, component); });
        parentFramebuffer.Bind();
        renderer.SetViewport(parentSize.x, parentSize.y);
        renderer.DrawTexture(panelFramebuffer.ColorTexture(), position, size);
    }

    Button& Panel::CreateButton()
    {
        Button b(panelFramebuffer, font);
        components.push_back(std::move(b));
        return std::get<Button>(components.back());
    }

    Label& Panel::CreateLabel()
    {
        Label l(panelFramebuffer, font);
        components.push_back(std::move(l));
        return std::get<Label>(components.back());
    }

    Image& Panel::CreateImage()
    {
        Image i(panelFramebuffer, font);
        components.push_back(std::move(i));
        return std::get<Image>(components.back());
    }

    Inputbox& Panel::CreateInputbox()
    {
        Inputbox ib(panelFramebuffer, font);
        components.push_back(std::move(ib));
        return std::get<Inputbox>(components.back());
    }
}
