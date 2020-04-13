#pragma once

#include "Container.hpp"

#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include <vector>

namespace RIS
{
    class UIPanel : public Container
    {
    public:
        UIPanel(const SystemLocator &systems);
        virtual ~UIPanel();

        void SetColor(const glm::vec4 &color);
        void SetPosition(const glm::vec2 &position);
        void SetImage(int image);
        void SetSize(const glm::vec2 &size);

        void Add(ComponentPtr component) override;
        void Remove(ComponentPtr component) override;
        void RemoveAll() override;
        ComponentPtr Find(const std::string &name, bool recursive = false) override;

        void OnChar(char c) override;
        void OnMouseMove(float x, float y) override;
        void OnMouseDown(InputButton mouseCode) override;
        void OnMouseUp(InputButton mouseCode) override;

        void OnKeyDown(InputKey keyCode) override;
        void OnKeyUp(InputKey keyCode) override;

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        std::vector<ComponentPtr> components;

        glm::vec4 color;
        glm::vec2 position, size;

        int backgroundImage;

    };
    using PanelPtr = std::shared_ptr<UIPanel>;
}

#define MakePanel(x) std::make_shared<UIPanel>(x)
