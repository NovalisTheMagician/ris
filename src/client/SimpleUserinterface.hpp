#pragma once

#include "common/IUserinterface.hpp"
#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <string>

namespace RIS
{
    class Component
    {
    public:
        virtual ~Component() = default;

        virtual void Update() = 0;
        virtual void Draw(IRenderer &renderer) = 0;
    };
    using ComponentPtr = std::shared_ptr<Component>;

    class Container : public Component
    {
    public:
        virtual ~Container() = default;

        virtual void Add(ComponentPtr component) = 0;
        virtual void Remove(ComponentPtr component) = 0;
        virtual void RemoveAll() = 0;
    };
    using ContainerPtr = std::shared_ptr<Container>;

    class UIPanel : public Container
    {
    public:
        UIPanel(const SystemLocator &systems);
        ~UIPanel();

        void SetColor(const glm::vec4 &color);
        void SetPosition(const glm::vec2 &position);

        void Add(ComponentPtr component) override;
        void Remove(ComponentPtr component) override;
        void RemoveAll() override;

        void Update() override;
        void Draw(IRenderer &renderer) override;

    private:
        std::vector<ComponentPtr> components;
        const SystemLocator &systems;

        glm::vec4 color;
        glm::vec2 position;

    };
    using PanelPtr = std::shared_ptr<UIPanel>;

    class UIImage : public Component
    {

    };
    using ImagePtr = std::shared_ptr<UIImage>;

    class UILabel : public Component
    {
    public:
        UILabel(const SystemLocator &systems);
        ~UILabel();

        void SetFont(int font);
        void SetTextColor(const glm::vec4 &color);
        void SetVisible(bool visible);

        void Update() override;
        void Draw(IRenderer &renderer) override;

    private:
        const SystemLocator &systems;

        int font;
        glm::vec4 fontColor;
        bool isVisible;

    };
    using LabelPtr = std::shared_ptr<UILabel>;

    class UIButton : public Component
    {

    };
    using ButtonPtr = std::shared_ptr<UIButton>;

    class UITextBox : public Component
    {

    };
    using TextBoxPtr = std::shared_ptr<UITextBox>;

    class Console
    {
    public:
        Console();
        ~Console();

        void Open();
        void Close();

        void Update();
        void Draw();

    private:
        bool isOpen;
        bool isMoving;

    };

    class SimpleUserinterface : public IUserinterface
    {
    public:
        SimpleUserinterface(const SystemLocator &systems, Config &config);
        ~SimpleUserinterface();

        void InitializeRootElements() override;

        void LoadLayout(const std::string &layout) override;

        void Draw() override;
        void Update() override;

    private:
        const SystemLocator &systems;
        Config &config;

        int uiWidth, uiHeight;
        ContainerPtr rootContainer;

        int uiFramebufferId;

        // need to make some friends
        friend UIPanel;
        friend UIButton;
        friend UIImage;
        friend UILabel;
        friend UITextBox;

    };
}