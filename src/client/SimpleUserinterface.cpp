#include "SimpleUserinterface.hpp"

#include "common/IRenderer.hpp"
#include "common/ILoader.hpp"
#include "common/IAudio.hpp"
#include "common/IInput.hpp"

#include "common/Logger.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <algorithm>

using namespace std::literals;
using std::string;

using namespace std::placeholders;

namespace RIS
{
    const int SimpleUserinterface::JSON_VERSION = 1;

    SimpleUserinterface::SimpleUserinterface(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), uiFramebufferId(-1)
    {
        rootContainer = MakePanel(systems);
    }

    SimpleUserinterface::~SimpleUserinterface()
    {
    }

    void SimpleUserinterface::InitializeRootElements()
    {
        IRenderer &renderer = systems.GetRenderer();
        uiWidth = config.GetInt("r_width", 800);
        uiHeight = config.GetInt("r_height", 600);
        uiFramebufferId = renderer.CreateFramebuffer(uiWidth, uiHeight, true);

        IInput &input = systems.GetInput();
        input.RegisterChar("ui", std::bind(&SimpleUserinterface::OnChar, this, _1));
        input.RegisterMouse("ui", std::bind(&SimpleUserinterface::OnMouseMove, this, _1, _2));
        input.RegisterButtonDown("ui", std::bind(&SimpleUserinterface::OnMouseDown, this, _1));
        input.RegisterButtonUp("ui", std::bind(&SimpleUserinterface::OnMouseUp, this, _1));

        LoadLayout("main");
    }

    void SimpleUserinterface::LoadPanel(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont)
    {
        PanelPtr panel = MakePanel(systems);
        std::string name = jsonValue["name"].GetString();
        panel->SetName(name);

        auto value = jsonValue.FindMember("position");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 position = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            panel->SetPosition(position);
        }
        
        value = jsonValue.FindMember("size");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 size = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            panel->SetSize(size);
        }
        
        value = jsonValue.FindMember("color");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec4 color = { value->value[0].GetFloat(), value->value[1].GetFloat(), value->value[2].GetFloat(), value->value[3].GetFloat() };
            panel->SetColor(color);
        }

        auto componentsIt = jsonValue.FindMember("components");
        if(componentsIt != jsonValue.MemberEnd())
        {
            for(auto &component : componentsIt->value.GetArray())
            {
                std::string type = component["type"].GetString();
                if(type == "panel")
                {
                    LoadPanel(component, panel, defaultFont);
                }
                else if(type == "label")
                {
                    LoadLabel(component, panel, defaultFont);
                }
                else if(type == "button")
                {
                    LoadButton(component, panel, defaultFont);
                }
                else if(type == "image")
                {
                    LoadImage(component, panel, defaultFont);
                }
                else if(type == "textbox")
                {
                    LoadTextbox(component, panel, defaultFont);
                }
            }
        }

        parentContainer->Add(panel);
    }

    void SimpleUserinterface::LoadButton(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont)
    {
        ButtonPtr button = MakeButton(systems);

        string name = jsonValue["name"].GetString();
        button->SetName(name);

        auto value = jsonValue.FindMember("position");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 position = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            button->SetPosition(position);
        }
        
        value = jsonValue.FindMember("size");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 size = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            button->SetSize(size);
        }

        int font = defaultFont;
        float fontSize = -1;

        value = jsonValue.FindMember("font_size");
        if(value != jsonValue.MemberEnd())
        {
            fontSize = value->value.GetFloat();
        }

        value = jsonValue.FindMember("font");
        if(value != jsonValue.MemberEnd())
        {
            font = systems.GetRenderer().Get2DRenderer().LoadFont(value->value.GetString());
        }
        button->SetFont(font, fontSize);

        value = jsonValue.FindMember("text");
        if(value != jsonValue.MemberEnd())
        {
            string text = value->value.GetString();
            button->SetText(text);
        }

        UIAction action = UIAction::Nop;
        string actionParam1 = "";
        string actionParam2 = "";

        value = jsonValue.FindMember("action");
        if(value != jsonValue.MemberEnd())
        {
            action = static_cast<UIAction>(value->value.GetUint());
        }
        value = jsonValue.FindMember("act_param1");
        if(value != jsonValue.MemberEnd())
        {
            actionParam1 = value->value.GetString();
        }
        value = jsonValue.FindMember("act_param2");
        if(value != jsonValue.MemberEnd())
        {
            actionParam2 = value->value.GetString();
        }
        button->SetAction(action, actionParam1, actionParam2);

        value = jsonValue.FindMember("normal_color");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec4 color = { value->value[0].GetFloat(), value->value[1].GetFloat(), value->value[2].GetFloat(), value->value[3].GetFloat() };
            button->SetNormalColor(color);
        }

        value = jsonValue.FindMember("hover_color");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec4 color = { value->value[0].GetFloat(), value->value[1].GetFloat(), value->value[2].GetFloat(), value->value[3].GetFloat() };
            button->SetHoverColor(color);
        }

        value = jsonValue.FindMember("down_color");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec4 color = { value->value[0].GetFloat(), value->value[1].GetFloat(), value->value[2].GetFloat(), value->value[3].GetFloat() };
            button->SetDownColor(color);
        }

        value = jsonValue.FindMember("normal_image");
        if(value != jsonValue.MemberEnd())
        {
            string imageName = value->value.GetString();
            int image = systems.GetRenderer().LoadTexture(imageName);
            button->SetNormalImage(image);
        }

        value = jsonValue.FindMember("hover_image");
        if(value != jsonValue.MemberEnd())
        {
            string imageName = value->value.GetString();
            int image = systems.GetRenderer().LoadTexture(imageName);
            button->SetHoverImage(image);
        }

        value = jsonValue.FindMember("down_image");
        if(value != jsonValue.MemberEnd())
        {
            string imageName = value->value.GetString();
            int image = systems.GetRenderer().LoadTexture(imageName);
            button->SetDownImage(image);
        }

        parentContainer->Add(button);
    }
    
    void SimpleUserinterface::LoadLabel(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont)
    {
        LabelPtr label = MakeLabel(systems);

        string name = jsonValue["name"].GetString();
        label->SetName(name);

        auto value = jsonValue.FindMember("position");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 position = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            label->SetPosition(position);
        }
        
        int font = defaultFont;
        float fontSize = -1;

        value = jsonValue.FindMember("font_size");
        if(value != jsonValue.MemberEnd())
        {
            fontSize = value->value.GetFloat();
        }

        value = jsonValue.FindMember("font");
        if(value != jsonValue.MemberEnd())
        {
            font = systems.GetRenderer().Get2DRenderer().LoadFont(value->value.GetString());
        }
        label->SetFont(font, fontSize);

        value = jsonValue.FindMember("text");
        if(value != jsonValue.MemberEnd())
        {
            string text = value->value.GetString();
            label->SetText(text);
        }

        value = jsonValue.FindMember("text_color");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec4 textColor = { value->value[0].GetFloat(), value->value[1].GetFloat(), value->value[2].GetFloat(), value->value[3].GetFloat() };
            label->SetTextColor(textColor);
        }

        parentContainer->Add(label);
    }
    
    void SimpleUserinterface::LoadImage(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont)
    {
        ImagePtr image = MakeImage(systems);

        string name = jsonValue["name"].GetString();
        image->SetName(name);

        auto value = jsonValue.FindMember("position");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 position = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            image->SetPosition(position);
        }
        
        value = jsonValue.FindMember("size");
        if(value != jsonValue.MemberEnd())
        {
            glm::vec2 size = { value->value[0].GetFloat(), value->value[1].GetFloat() };
            image->SetSize(size);
        }

        value = jsonValue.FindMember("image");
        if(value != jsonValue.MemberEnd())
        {
            string imageName = value->value.GetString();
            int imageId = systems.GetRenderer().LoadTexture(imageName);
            image->SetImage(imageId);
        }

        parentContainer->Add(image);
    }
    
    void SimpleUserinterface::LoadTextbox(const rapidjson::Value &jsonValue, const ContainerPtr &parentContainer, int defaultFont)
    {
        InputBoxPtr textBox = MakeInputBox(systems);

        string name = jsonValue["name"].GetString();
        textBox->SetName(name);
    }

    void SimpleUserinterface::LoadLayout(const std::string &layout)
    {
        if(layouts.count(layout) > 0)
        {
            rootContainer = layouts.at(layout);
            return;
        }

        ILoader &loader = systems.GetLoader();
        I2DRenderer &renderer = systems.GetRenderer().Get2DRenderer();
        try
        {
            std::size_t size;
            auto data = loader.LoadAsset(AssetType::UILAYOUT, layout, size);
            string layoutStr(reinterpret_cast<const char*>(data.get()), size);

            rapidjson::Document layoutJson;
            rapidjson::ParseResult res = layoutJson.Parse(layoutStr.c_str()); 
            if(res.IsError())
            {
                string errorMsg = rapidjson::GetParseError_En(res.Code());
                Logger::Instance().Error("Failed to parse layout ("s + layout + "): "s + errorMsg + "(" + std::to_string(res.Offset()) + ")");
                return;
            }

            ContainerPtr container = MakePanel(systems);

            if(!layoutJson.HasMember("version"))
            {
                Logger::Instance().Error("Failed to load layout ("s + layout + "): invalid schema");
                return;
            }

            int version = layoutJson["version"].GetInt();
            if(version != JSON_VERSION)
            {
                Logger::Instance().Error("Failed to load layout ("s + layout + "): version mismatch");
                return;
            }

            auto nameVal = layoutJson.FindMember("name");
            if(nameVal != layoutJson.MemberEnd())
            {
                container->SetName(nameVal->value.GetString());
            }

            int defaultFont = 1;
            auto fontVal = layoutJson.FindMember("font");
            if(fontVal != layoutJson.MemberEnd())
            {
                defaultFont = renderer.LoadFont(fontVal->value.GetString());
            }

            auto componentsIt = layoutJson.FindMember("components");
            if(componentsIt != layoutJson.MemberEnd())
            {
                for(auto &component : componentsIt->value.GetArray())
                {
                    std::string type = component["type"].GetString();
                    if(type == "panel")
                    {
                        LoadPanel(component, container, defaultFont);
                    }
                    else if(type == "label")
                    {
                        LoadLabel(component, container, defaultFont);
                    }
                    else if(type == "button")
                    {
                        LoadButton(component, container, defaultFont);
                    }
                    else if(type == "image")
                    {
                        LoadImage(component, container, defaultFont);
                    }
                    else if(type == "textbox")
                    {
                        LoadTextbox(component, container, defaultFont);
                    }
                }
            }

            layouts.insert({ layout, container });
            rootContainer = container;
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
    }

    void SimpleUserinterface::OnMouseMove(float x, float y)
    {
        y = uiHeight - y;
        rootContainer->OnMouseMove(x, y);
    }

    void SimpleUserinterface::OnMouseDown(int button)
    {
        rootContainer->OnMouseDown(button);
    }

    void SimpleUserinterface::OnMouseUp(int button)
    {
        rootContainer->OnMouseUp(button);
    }
}
