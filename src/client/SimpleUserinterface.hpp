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
        Component(const SystemLocator &systems) : systems(systems) {};
        virtual ~Component() = default;

        virtual void OnMouseMove(int x, int y) {};
        virtual void OnMouseDown(int mouseCode) {};
        virtual void OnMouseUp(int mouseCode) {};

        virtual void OnKeyDown(int keyCode) {};
        virtual void OnKeyUp(int keyCode) {};

        virtual void OnChar(char c) {};

        virtual void Update() = 0;
        virtual void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) = 0;

    protected:
        const SystemLocator &systems;
    };
    using ComponentPtr = std::shared_ptr<Component>;

    class Container : public Component
    {
    public:
        Container(const SystemLocator &systems) : Component(systems) {};
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
        virtual ~UIPanel();

        void SetColor(const glm::vec4 &color);
        void SetPosition(const glm::vec2 &position);
        void SetImage(int image);
        void SetSize(const glm::vec2 &size);

        void Add(ComponentPtr component) override;
        void Remove(ComponentPtr component) override;
        void RemoveAll() override;

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        std::vector<ComponentPtr> components;

        glm::vec4 color;
        glm::vec2 position, size;

        int backgroundImage;

    };
    using PanelPtr = std::shared_ptr<UIPanel>;
#define MakePanel(x) std::make_shared<UIPanel>(x)

    class UIImage : public Component
    {
    public:
        UIImage(const SystemLocator &systems);
        virtual ~UIImage();

        void SetImage(int image);
        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        int image;
        glm::vec2 position, size;

    };
    using ImagePtr = std::shared_ptr<UIImage>;
#define MakeImage(x) std::make_shared<UIImage>(x)

    class UILabel : public Component
    {
    public:
        UILabel(const SystemLocator &systems);
        virtual ~UILabel();

        void SetFont(int font, float fontSize);
        void SetTextColor(const glm::vec4 &color);
        void SetPosition(const glm::vec2 &position);
        void SetVisible(bool visible);
        void SetText(const std::string &text);

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        glm::vec2 position;
        std::string text;

        int font;
        float fontSize;
        glm::vec4 fontColor;
        bool isVisible;

    };
    using LabelPtr = std::shared_ptr<UILabel>;
#define MakeLabel(x) std::make_shared<UILabel>(x)

    class UIButton : public Component
    {
    public:
        UIButton(const SystemLocator &systems);
        virtual ~UIButton();

        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);

        void SetText(const std::string &text);
        void SetFont(int font, float size);
        void SetTextColor(const glm::vec4 &color);

        void SetColors(const glm::vec4 &normal, const glm::vec4 &hover, const glm::vec4 &down);
        void SetImages(int normal, int hover, int down);

        void SetNormalColor(const glm::vec4 &color);
        void SetNormalImage(int image);
        void SetHoverColor(const glm::vec4 &color);
        void SetHoverImage(int image);
        void SetDownColor(const glm::vec4 &color);
        void SetDownImage(int image);

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        std::string text;
        int font;
        float fontSize;

        glm::vec2 position, size;

        glm::vec4 normalColor, hoverColor, downColor;
        int normalImage, hoverImage, downImage;

    };
    using ButtonPtr = std::shared_ptr<UIButton>;
#define MakeButton(x) std::make_shared<UIButton>(x)

    class UIInputBox : public Component
    {
    public:
        UIInputBox(const SystemLocator &systems);
        virtual ~UIInputBox();

        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);

        void SetPreviewText(const std::string &previewText);
        void SetText(const std::string &text);
        void SetPreviewTextColor(const glm::vec4 &previewColor);
        void SetTextColor(const glm::vec4 &textColor);
        void SetFont(int font, float fontSize);

        std::string GetText() const;

        void Update() override;
        void Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition) override;

    private:
        std::string previewText, text;
        glm::vec2 position, size;

        glm::vec4 previewTextColor, textColor;
        int font;
        float fontSize;

    };
    using InputBoxPtr = std::shared_ptr<UIInputBox>;
#define MakeInputBox(x) std::make_shared<UIInputBox>(x)

    class Console
    {
    public:
        Console();
        ~Console();

        void Open();
        void Close();

        bool IsOpen();

        void Update();
        void Draw(I2DRenderer &renderer);

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
        friend UIInputBox;

    };
}