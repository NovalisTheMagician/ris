#include "SimpleUserinterface.hpp"

#include "common/IRenderer.hpp"
#include "common/ILoader.hpp"
#include "common/IAudio.hpp"

#include "common/Logger.hpp"

#include <rapidjson/document.h>

#include <algorithm>

using namespace std::literals;

namespace RIS
{
    UIPanel::UIPanel(const SystemLocator &systems)
        : systems(systems), components(), color(0, 0, 0, 0), position(0, 0)
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

    void UIPanel::Update()
    {
        std::for_each(components.begin(), components.end(), [](auto component){ component->Update(); });
    }

    void UIPanel::Draw(IRenderer &renderer)
    {
        std::for_each(components.begin(), components.end(), [&renderer](auto component){ component->Draw(renderer); });
    }

// UILabel

    UILabel::UILabel(const SystemLocator &systems)
        : systems(systems), fontColor(1, 1, 1, 1), isVisible(true)
    {

    }

    UILabel::~UILabel()
    {

    }

    void UILabel::SetFont(Font font)
    {
        this->font = font;
    }

    void UILabel::SetTextColor(const glm::vec4 &color)
    {
        fontColor = color;
    }

    void UILabel::SetVisible(bool visible)
    {
        isVisible = visible;
    }

    void UILabel::Update()
    {

    }

    void UILabel::Draw(IRenderer &renderer)
    {
        
    }

// Simpleuserinterface

    SimpleUserinterface::SimpleUserinterface(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), uiFramebufferId(-1)
    {
        rootContainer = std::make_shared<UIPanel>(systems);
    }

    SimpleUserinterface::~SimpleUserinterface()
    {

    }

    void SimpleUserinterface::InitializeRootElements()
    {
        IRenderer &renderer = systems.GetRenderer();
        uiWidth = config.GetInt("r_width", 800);
        uiHeight = config.GetInt("r_height", 600);
        uiFramebufferId = renderer.CreateFramebuffer(uiWidth, uiHeight, false);
    }

    void SimpleUserinterface::LoadLayout(const std::string &layout)
    {
        rootContainer->RemoveAll();

        ILoader &loader = systems.GetLoader();
        try
        {
            std::size_t size;
            auto data = loader.LoadAsset(AssetType::UILAYOUT, layout, size);

            rapidjson::Document layoutJson;
            if(layoutJson.ParseInsitu(reinterpret_cast<char*>(data.get())).HasParseError())
            {
                Logger::Instance().Error("Failed to parse layout ("s + layout + "): "s + std::to_string(layoutJson.GetParseError()));
                return;
            }

            for(auto it = layoutJson.MemberBegin(); it != layoutJson.MemberEnd(); ++it)
            {
                // TODO
            }
        }
        catch(const std::exception& e)
        {
            Logger::Instance().Error("Failed to load layout ("s + layout + "): "s + e.what());
        }
    }

    void SimpleUserinterface::Draw()
    {
        IRenderer &renderer = systems.GetRenderer();
        I2DRenderer &renderer2D = renderer.Get2DRenderer();
        renderer.Clear(uiFramebufferId, glm::vec4(0, 1, 0, 1));
        renderer.SetFramebuffer(uiFramebufferId);

        renderer2D.Begin();

        rootContainer->Draw(renderer);

        renderer2D.SetTexture(1, 0);
        renderer2D.SetColor(glm::vec4(0, 1, 0, 1));
        renderer2D.SetPosition(glm::vec2(100, 100));
        renderer2D.DrawQuad(100, 100);

        renderer2D.End();

        renderer.Draw(uiFramebufferId);

    }

    void SimpleUserinterface::Update()
    {
        rootContainer->Update();
    }
}
