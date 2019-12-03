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
        : systems(systems), components()
    {
    }

    UIPanel::~UIPanel()
    {
        components.clear();
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
        std::for_each(components.begin(), components.end(), [](auto component){ component.Update(); });
    }

    void UIPanel::Draw(IRenderer &renderer)
    {
        std::for_each(components.begin(), components.end(), [](auto component){ component.Draw(renderer); });
    }

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
            if(layoutJson.Parse(reinterpret_cast<const char*>(data.get())).HasParseError())
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
        renderer.Clear(uiFramebufferId, glm::vec4(0, 0, 0, 0));

        //begin 2d rendering
        //bind specific framebuffer

        rootContainer->Draw(renderer);

        //end 2d rendering

    }

    void SimpleUserinterface::Update()
    {
        rootContainer->Update();
    }
}
