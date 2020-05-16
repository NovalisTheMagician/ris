#pragma once

#include "ui/Container.hpp"

#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include <vector>

namespace RIS
{
    namespace UI
    {
        class Panel : public Container
        {
        public:
            Panel() = default;
            virtual ~Panel() = default;
            Panel(const Panel&) = default;
            Panel& operator=(const Panel&) = default;
            Panel(Panel&&) = default;
            Panel& operator=(Panel&&) = default;

            void SetColor(const glm::vec4 &color);
            void SetPosition(const glm::vec2 &position);
            void SetImage(std::shared_ptr<Graphics::Texture> image);
            void SetSize(const glm::vec2 &size);

            void Add(ComponentPtr component) override;
            void Remove(ComponentPtr component) override;
            void RemoveAll() override;
            ComponentPtr Find(const std::string &name, bool recursive = false) override;

            void OnChar(char c) override;
            void OnMouseMove(float x, float y) override;
            void OnMouseDown(Input::InputButton mouseCode) override;
            void OnMouseUp(Input::InputButton mouseCode) override;

            void OnKeyDown(Input::InputKey keyCode) override;
            void OnKeyUp(Input::InputKey keyCode) override;

            void Update() override;
            void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

        private:
            std::vector<ComponentPtr> components;

            glm::vec4 color;
            glm::vec2 position, size;

            std::shared_ptr<Graphics::Texture> backgroundImage;

        };
        using PanelPtr = std::shared_ptr<Panel>;
    }
}
