#include "ui/Panel.hpp"

#include <algorithm>

namespace RIS
{
    namespace UI
    {
        Panel::Ptr Panel::Create()
        {
            return std::make_shared<Panel>();
        }

        void Panel::SetColor(const glm::vec4 &color)
        {
            this->color = color;
        }

        void Panel::SetImage(std::shared_ptr<Graphics::Texture> image)
        {
            backgroundImage = image;
        }

        void Panel::Add(Component::Ptr component)
        {
            if(component)
            {
                components.push_back(component);
            }
        }

        void Panel::Remove(Component::Ptr component)
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

        void Panel::RemoveAll()
        {
            components.clear();
        }

        Component::Ptr Panel::Find(const std::string &name, bool recursive)
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
                    Container::Ptr container = std::dynamic_pointer_cast<Container>(c);
                    if(container)
                    {
                        return container->Find(name, recursive);
                    }
                }
            }

            return nullptr;
        }

        void Panel::OnChar(uint32_t c)
        {
            std::for_each(components.begin(), components.end(), [c](auto component){ component->OnChar(c); });
        }

        void Panel::OnMouseMove(float x, float y)
        {
            float relX = x - position.x;
            float relY = y - position.y;
            std::for_each(components.begin(), components.end(), [relX, relY](auto component){ component->OnMouseMove(relX, relY); });
        }

        void Panel::OnMouseDown(Input::InputKey mouseCode)
        {
            std::for_each(components.begin(), components.end(), [mouseCode](auto component){ component->OnMouseDown(mouseCode); });
        }

        void Panel::OnMouseUp(Input::InputKey mouseCode)
        {
            std::for_each(components.begin(), components.end(), [mouseCode](auto component){ component->OnMouseUp(mouseCode); });
        }

        void Panel::OnMouseWheel(float x, float y)
        {
            std::for_each(components.begin(), components.end(), [x, y](auto component){ component->OnMouseWheel(x, y); });
        }

        void Panel::OnKeyDown(Input::InputKey keyCode)
        {
            std::for_each(components.begin(), components.end(), [keyCode](auto component){ component->OnKeyDown(keyCode); });
        }

        void Panel::OnKeyUp(Input::InputKey keyCode)
        {
            std::for_each(components.begin(), components.end(), [keyCode](auto component){ component->OnKeyUp(keyCode); });
        }

        void Panel::OnKeyRepeat(Input::InputKey keyCode)
        {
            std::for_each(components.begin(), components.end(), [keyCode](auto component){ component->OnKeyRepeat(keyCode); });
        }

        void Panel::Update(const Timer &timer)
        {
            std::for_each(components.begin(), components.end(), [&timer](auto component){ component->Update(timer); });
        }

        void Panel::Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition)
        {
            glm::vec2 pos = parentPosition + position;

            if(backgroundImage)
                renderer.DrawTexture(*backgroundImage, pos, size, color);
            else
                renderer.DrawRect(pos, size, color);
            
            std::for_each(components.begin(), components.end(), [&renderer, &pos](auto component){ component->Draw(renderer, pos); });
        }
    }
}
