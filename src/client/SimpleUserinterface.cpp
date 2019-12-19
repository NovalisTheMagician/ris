#include "SimpleUserinterface.hpp"

#include "common/IRenderer.hpp"
#include "common/ILoader.hpp"
#include "common/IAudio.hpp"

#include "common/Logger.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <algorithm>

#include <iostream>

using namespace std::literals;
using std::string;

using namespace std::placeholders;

namespace RIS
{
    UIPanel::UIPanel(const SystemLocator &systems)
        : Container(systems), components(), color(0, 0, 0, 0), position(0, 0), backgroundImage(1), size(0, 0)
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

    void UIPanel::SetImage(int image)
    {
        backgroundImage = image;
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

    void UIPanel::OnChar(char c)
    {
        std::for_each(components.begin(), components.end(), [c](auto component){ component->OnChar(c); });
    }

    void UIPanel::OnMouseMove(float x, float y)
    {
        std::for_each(components.begin(), components.end(), [x, y](auto component){ component->OnMouseMove(x, y); });
    }

    void UIPanel::Update()
    {
        std::for_each(components.begin(), components.end(), [](auto component){ component->Update(); });
    }

    void UIPanel::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        glm::vec2 pos = parentPosition + position;

        renderer.SetTexture(backgroundImage, 0);
        renderer.DrawQuad(pos, size, color);
        std::for_each(components.begin(), components.end(), [&renderer, &pos](auto component){ component->Draw(renderer, pos); });
    }

// UILabel

    UILabel::UILabel(const SystemLocator &systems)
        : Component(systems), fontColor(1, 1, 1, 1), isVisible(true), fontSize(-1), font(0)
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

// UIImage

    UIImage::UIImage(const SystemLocator &systems)
        : Component(systems), image(1)
    {
    }

    UIImage::~UIImage()
    {
    }

    void UIImage::SetImage(int image)
    {
        this->image = image;
    }

    void UIImage::SetPosition(const glm::vec2 &position)
    {
        this->position = position;
    }

    void UIImage::SetSize(const glm::vec2 &size)
    {
        this->size = size;
    }

    void UIImage::Update()
    {

    }

    void UIImage::Draw(I2DRenderer &renderer, const glm::vec2 &parentPosition)
    {
        glm::vec2 pos = parentPosition + position;
        renderer.SetTexture(image, 0);
        renderer.DrawQuad(position, size, {1, 1, 1, 1});
    }

// Simpleuserinterface

    SimpleUserinterface::SimpleUserinterface(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), uiFramebufferId(-1)
    {
        rootContainer = MakePanel(systems);
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

        IInput &input = systems.GetInput();
        input.RegisterChar("ui", std::bind(&SimpleUserinterface::OnChar, this, _1));

        meow = renderer.LoadTexture("meow");
        immortalFont = renderer.Get2DRenderer().LoadFont("IMMORTAL");

        PanelPtr panel = MakePanel(systems);
        panel->SetPosition({ uiWidth/2 - 100, uiHeight/2 - 150 });
        panel->SetSize({ 200, 300 });
        panel->SetColor({ 0.1f, 0.7f, 0.2f, 0.7f });
        panel->SetImage(1);

        auto metrics = renderer.Get2DRenderer().MeasureText("Start", immortalFont, 30);
        float xPos = 100 - metrics.width / 2;
        float yPos = 300 - metrics.height;

        LabelPtr labelStart = MakeLabel(systems);
        labelStart->SetText("Start");
        labelStart->SetTextColor({ 0, 0, 0, 1 });
        labelStart->SetFont(immortalFont, 30);
        labelStart->SetPosition({ xPos, yPos });
        labelStart->SetVisible(true);

        metrics = renderer.Get2DRenderer().MeasureText("Quit", immortalFont, 30);
        xPos = 100 - metrics.width / 2;
        yPos = yPos - metrics.height;

        LabelPtr labelQuit = MakeLabel(systems);
        labelQuit->SetText("Quit");
        labelQuit->SetTextColor({ 0, 0, 0, 1 });
        labelQuit->SetFont(immortalFont, 30);
        labelQuit->SetPosition({ xPos, yPos });
        labelQuit->SetVisible(true);

        ImagePtr image = MakeImage(systems);
        image->SetPosition({ 0, uiHeight-256 });
        image->SetSize({ 256, 256 });
        image->SetImage(meow);

        panel->Add(labelStart);
        panel->Add(labelQuit);

        rootContainer->Add(panel);
        rootContainer->Add(image);
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

    void SimpleUserinterface::OnChar(char character)
    {
        rootContainer->OnChar(character);
        std::cout << character;
    }

    void SimpleUserinterface::OnMouseMove(float x, float y)
    {
        
    }
}
