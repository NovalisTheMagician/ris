#include "SimpleUserinterface.hpp"

#include "common/IRenderer.hpp"
#include "common/ILoader.hpp"
#include "common/IAudio.hpp"

#include "common/Logger.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <algorithm>

using namespace std::literals;
using std::string;

namespace RIS
{
    UIPanel::UIPanel(const SystemLocator &systems)
        : systems(systems), components(), color(0, 0, 0, 0), position(0, 0), backTexture(1), size(0, 0)
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

    void UIPanel::SetSize(const glm::vec2 &size)
    {
        this->size = size;
    }

    void UIPanel::SetTexture(int texture)
    {
        backTexture = texture;
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

    void UIPanel::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        glm::vec2 pos = parentPosition + position;

        renderer.SetTexture(backTexture, 0);
        renderer.DrawQuad(pos, size, color);
        std::for_each(components.begin(), components.end(), [&renderer, &pos](auto component){ component->Draw(renderer, pos); });
    }

// UILabel

    UILabel::UILabel(const SystemLocator &systems)
        : systems(systems), fontColor(1, 1, 1, 1), isVisible(true), fontSize(-1), font(0)
    {

    }

    UILabel::~UILabel()
    {

    }

    void UILabel::SetFont(int font, float fontSize)
    {
        this->font = font;
        this->fontSize = fontSize;
    }

    void UILabel::SetTextColor(const glm::vec4 &color)
    {
        fontColor = color;
    }

    void UILabel::SetVisible(bool visible)
    {
        isVisible = visible;
    }

    void UILabel::SetPosition(const glm::vec2 &position)
    {
        this->position = position;
    }

    void UILabel::SetText(const string &text)
    {
        this->text = text;
    }

    void UILabel::Update()
    {

    }

    void UILabel::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        renderer.DrawText(text, font, parentPosition + position, fontSize, fontColor);
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

    int meow;
    int immortalFont;

    void SimpleUserinterface::InitializeRootElements()
    {
        IRenderer &renderer = systems.GetRenderer();
        uiWidth = config.GetInt("r_width", 800);
        uiHeight = config.GetInt("r_height", 600);
        uiFramebufferId = renderer.CreateFramebuffer(uiWidth, uiHeight, true);

        meow = renderer.LoadTexture("meow");
        immortalFont = renderer.Get2DRenderer().LoadFont("IMMORTAL");

        PanelPtr panel = std::make_shared<UIPanel>(systems);
        panel->SetPosition({ 100, 100 });
        panel->SetSize({ 200, 300 });
        panel->SetColor({ 0.1f, 0.7f, 0.2f, 0.7f });
        panel->SetTexture(1);

        auto metrics = renderer.Get2DRenderer().MeasureText("Start", immortalFont, 30);
        float xPos = 100 - metrics.width / 2;
        float yPos = 300 - metrics.height;

        LabelPtr labelStart = std::make_shared<UILabel>(systems);
        labelStart->SetText("Start");
        labelStart->SetTextColor({ 0, 0, 0, 1 });
        labelStart->SetFont(immortalFont, 30);
        labelStart->SetPosition({ xPos, yPos });
        labelStart->SetVisible(true);

        metrics = renderer.Get2DRenderer().MeasureText("Quit", immortalFont, 30);
        xPos = 100 - metrics.width / 2;
        yPos = yPos - metrics.height;

        LabelPtr labelQuit = std::make_shared<UILabel>(systems);
        labelQuit->SetText("Quit");
        labelQuit->SetTextColor({ 0, 0, 0, 1 });
        labelQuit->SetFont(immortalFont, 30);
        labelQuit->SetPosition({ xPos, yPos });
        labelQuit->SetVisible(true);

        panel->Add(labelStart);
        panel->Add(labelQuit);

        rootContainer->Add(panel);
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

        // for intel gpus: bind the framebuffer first before clearing it
        renderer.SetFramebuffer(uiFramebufferId);
        renderer.Clear(uiFramebufferId, glm::vec4(0, 0, 0, 0));

        renderer2D.Begin();

        rootContainer->Draw(renderer2D, glm::vec2());

        renderer2D.End();

        renderer.Draw(uiFramebufferId);
    }

    void SimpleUserinterface::Update()
    {
        rootContainer->Update();
    }
}
