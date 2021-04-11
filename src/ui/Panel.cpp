#include "ui/Panel.hpp"

#include <algorithm>
#include <numeric>

namespace RIS::UI
{
    Panel::Panel(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont, glm::vec2 parentSize)
        : Component(parentFramebuffer, defaultFont, parentSize), panelFramebuffer(static_cast<int>(size.x), static_cast<int>(size.y))
    {}

    Panel& Panel::SetSize(const glm::vec2 &size)
    {
        this->size = size;
        glm::ivec2 s = size;
        panelFramebuffer.Resize(s.x, s.y);
        return *this;
    }

    Panel& Panel::SetColor(const glm::vec4 &color)
    {
        this->color = color;
        return *this;
    }

    void Panel::OnChar(uint32_t c)
    {
        ForeachDispatch(components, [c](auto &comp){ comp.OnChar(c); });
    }

    void Panel::OnMouseMove(float x, float y)
    {
        float relX = x - position.x - offset.x;
        float relY = y - position.y - offset.y;

        glm::vec2 topLeft = position;
        glm::vec2 botRight = position + size;
        if(x < topLeft.x || x > botRight.x || y < topLeft.y || y > botRight.y)
        {
            relX = std::numeric_limits<float>::infinity();
            relY = std::numeric_limits<float>::infinity();
        }

        ForeachDispatch(components, [relX, relY](auto &&comp){ comp.OnMouseMove(relX, relY); });
    }

    void Panel::OnMouseDown(Input::InputKey mouseCode)
    {
        ForeachDispatch(components, [mouseCode](auto &&comp){ comp.OnMouseDown(mouseCode); });
    }

    void Panel::OnMouseUp(Input::InputKey mouseCode)
    {
        ForeachDispatch(components, [mouseCode](auto &&comp){ comp.OnMouseUp(mouseCode); });
    }

    void Panel::OnMouseWheel(float x, float y)
    {
        Component::OnMouseWheel(x, y);
        ForeachDispatch(components, [x, y](auto &&comp){ comp.OnMouseWheel(x, y); });
    }

    void Panel::OnKeyDown(Input::InputKey keyCode)
    {
        ForeachDispatch(components, [keyCode](auto &&comp){ comp.OnKeyDown(keyCode); });
    }

    void Panel::OnKeyUp(Input::InputKey keyCode)
    {
        ForeachDispatch(components, [keyCode](auto &&comp){ comp.OnKeyUp(keyCode); });
    }

    void Panel::OnKeyRepeat(Input::InputKey keyCode)
    {
        ForeachDispatch(components, [keyCode](auto &&comp){ comp.OnKeyRepeat(keyCode); });
    }

    void Panel::Reset()
    {
        Component::Reset();
        ForeachDispatch(components, [](auto &&comp){ comp.Reset(); });
    }

    void Panel::Update(const Timer &timer)
    {
        ForeachDispatch(components, [&timer](auto &&comp){ comp.Update(timer); });
    }

    void Panel::Draw(Graphics::SpriteRenderer &renderer, glm::vec2 offset)
    {
        glm::vec2 pos = GetAnchoredPosition() + offset;
        /*
        if(backgroundImage)
            renderer.DrawTexture(*backgroundImage, position, size, color);
        else
            renderer.DrawRect(position, size, color);
        */
        panelFramebuffer.Bind();
        panelFramebuffer.Clear(color, 1.0f);
        renderer.SetViewport(size.x, size.y, true);
        ForeachDispatch(components, [&renderer, this](auto &&comp){ comp.Draw(renderer, this->offset); });
        parentFramebuffer.Bind();
        renderer.SetViewport(parentSize.x, parentSize.y, true);
        renderer.DrawTexture(panelFramebuffer.ColorTexture(), pos, size);
    }

    Button& Panel::CreateButton()
    {
        Button b(panelFramebuffer, font, size);
        components.emplace_back(std::move(b));
        return std::get<Button>(components.back());
    }

    Label& Panel::CreateLabel()
    {
        Label l(panelFramebuffer, font, size);
        components.push_back(std::move(l));
        return std::get<Label>(components.back());
    }

    Image& Panel::CreateImage()
    {
        Image i(panelFramebuffer, font, size);
        components.push_back(std::move(i));
        return std::get<Image>(components.back());
    }

    Inputbox& Panel::CreateInputbox()
    {
        Inputbox ib(panelFramebuffer, font, size);
        components.push_back(std::move(ib));
        return std::get<Inputbox>(components.back());
    }

    Panel& Panel::CreatePanel()
    {
        Panel p(panelFramebuffer, font, size);
        components.push_back(std::move(p));
        return std::get<Panel>(components.back());
    }
}
