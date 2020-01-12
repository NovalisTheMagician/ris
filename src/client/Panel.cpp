#include "Panel.hpp"

#include <algorithm>

namespace RIS
{
    UIPanel::UIPanel(const SystemLocator &systems)
        : Container(systems), components(), color(0, 0, 0, 0), position(0, 0), backgroundImage(1), size(0, 0)
    {
    }

    UIPanel::~UIPanel()
    {
        components.clear();
    }

    void UIPanel::SetColor(const glm::vec4 &color)
    {
        this->color = color;
    }

    void UIPanel::SetPosition(const glm::vec2 &position)
    {
        this->position = position;
    }

    void UIPanel::SetSize(const glm::vec2 &size)
    {
        this->size = size;
    }

    void UIPanel::SetImage(int image)
    {
        backgroundImage = image;
    }

    void UIPanel::Add(ComponentPtr component)
    {
        if(component)
        {
            components.push_back(component);
        }
    }

    void UIPanel::Remove(ComponentPtr component)
    {
        if(component)
        {
            auto res = std::find(components.begin(), components.end(), component);
            if(res != components.end())
            {
                components.erase(res);
            }
        }
    }

    void UIPanel::RemoveAll()
    {
        components.clear();
    }

    ComponentPtr UIPanel::Find(const std::string &name, bool recursive)
    {
        auto found = std::find_if(components.begin(), components.end(), [&name](auto &component){ return component->GetName() == name; });
        if(found != components.end())
        {
            return *found;
        }

        if(recursive)
        {
            for(auto &c : components)
            {
                ContainerPtr container = std::dynamic_pointer_cast<Container>(c);
                if(container)
                {
                    return container->Find(name, recursive);
                }
            }
        }

        return nullptr;
    }

    void UIPanel::OnChar(char c)
    {
        std::for_each(components.begin(), components.end(), [c](auto component){ component->OnChar(c); });
    }

    void UIPanel::OnMouseMove(float x, float y)
    {
        std::for_each(components.begin(), components.end(), [x, y](auto component){ component->OnMouseMove(x, y); });
    }

    void UIPanel::OnMouseDown(InputButtons mouseCode)
    {
        std::for_each(components.begin(), components.end(), [mouseCode](auto component){ component->OnMouseDown(mouseCode); });
    }

    void UIPanel::OnMouseUp(InputButtons mouseCode)
    {
        std::for_each(components.begin(), components.end(), [mouseCode](auto component){ component->OnMouseUp(mouseCode); });
    }

    void UIPanel::OnKeyDown(InputKeys keyCode)
    {
        std::for_each(components.begin(), components.end(), [keyCode](auto component){ component->OnKeyDown(keyCode); });
    }

    void UIPanel::OnKeyUp(InputKeys keyCode)
    {
        std::for_each(components.begin(), components.end(), [keyCode](auto component){ component->OnKeyUp(keyCode); });
    }

    void UIPanel::Update()
    {
        std::for_each(components.begin(), components.end(), [](auto component){ component->Update(); });
    }

    void UIPanel::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        glm::vec2 pos = parentPosition + position;

        renderer.SetTexture(backgroundImage, 0);
        renderer.DrawQuad(pos, size, color);
        std::for_each(components.begin(), components.end(), [&renderer, &pos](auto component){ component->Draw(renderer, pos); });
    }
}
