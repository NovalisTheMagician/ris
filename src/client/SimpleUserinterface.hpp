#pragma once

#include "common/IUserinterface.hpp"
#include "common/IRenderer.hpp"
#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

#include <glm/glm.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <vector>
#include <unordered_map>
#include <string>

#include "Component.hpp"
#include "Container.hpp"
#include "Panel.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "Button.hpp"
#include "Inputbox.hpp"

#include "UIAction.hpp"

namespace RIS
{
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
        void OnChar(char character);
        void OnMouseMove(float x, float y);
        void OnMouseDown(int button);
        void OnMouseUp(int button);

        void LoadPanel(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadButton(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadLabel(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadImage(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);
        void LoadTextbox(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont);

    private:
        static const int JSON_VERSION;

    private:
        const SystemLocator &systems;
        Config &config;

        int uiWidth, uiHeight;
        ContainerPtr rootContainer;

        std::unordered_map<std::string, ContainerPtr> layouts;

        int uiFramebufferId;

        // need to make some friends
        friend UIPanel;
        friend UIButton;
        friend UIImage;
        friend UILabel;
        friend UIInputBox;

    };
}