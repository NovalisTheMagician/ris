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
            using Ptr = std::shared_ptr<Panel>;
            static Ptr Create();

            Panel() = default;
            virtual ~Panel() = default;
            Panel(const Panel&) = default;
            Panel& operator=(const Panel&) = default;
            Panel(Panel&&) = default;
            Panel& operator=(Panel&&) = default;

            void SetColor(const glm::vec4 &color);
            void SetImage(std::shared_ptr<Graphics::Texture> image);

            void Add(Component::Ptr component) override;
            void Remove(Component::Ptr component) override;
            void RemoveAll() override;
            Component::Ptr Find(const std::string &name, bool recursive = false) override;

            void OnChar(char c) override;
            void OnMouseMove(float x, float y) override;
            void OnMouseDown(Input::InputKey mouseCode) override;
            void OnMouseUp(Input::InputKey mouseCode) override;
            void OnMouseWheel(float x, float y) override;

            void OnKeyDown(Input::InputKey keyCode) override;
            void OnKeyUp(Input::InputKey keyCode) override;

            void Update() override;
            void Draw(Graphics::SpriteRenderer &renderer, const glm::vec2 &parentPosition) override;

        private:
            std::vector<Component::Ptr> components;

            glm::vec4 color;

            std::shared_ptr<Graphics::Texture> backgroundImage;

        };
    }
}
