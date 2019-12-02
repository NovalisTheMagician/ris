#pragma once

#include "common/IUserinterface.hpp"
#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <vector>

namespace RIS
{
    class Component
    {
    public:
        virtual ~Component() = default;

        virtual void Update() = 0;
        virtual void Draw() = 0;
    };
    using ComponentPtr = std::shared_ptr<Component>;

    class Container : public Component
    {
    public:
        virtual ~Container() = default;

        virtual void Add(ComponentPtr component) = 0;
        virtual void Remove(ComponentPtr component) = 0;
    };
    using ContainerPtr = std::shared_ptr<Container>;

    class UIPanel : public Container
    {
    public:
        UIPanel();
        ~UIPanel();

        void Add(ComponentPtr component) override;
        void Remove(ComponentPtr component) override;

        void Update() override;
        void Draw() override;

    private:
        std::vector<ComponentPtr> components;

    };
    using PanelPtr = std::shared_ptr<UIPanel>;

    class UIImage : public Component
    {

    };
    using ImagePtr = std::shared_ptr<UIImage>;

    class UILabel : public Component
    {

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

    class SimpleUserinterface : public IUserinterface
    {
    public:
        SimpleUserinterface(const SystemLocator &systems, Config &config);
        ~SimpleUserinterface();

        void Draw() override;
        void Update() override;

    private:
        const SystemLocator &systems;
        Config &config;

        int uiWidth, uiHeight;
        ContainerPtr rootContainer;

    };
}