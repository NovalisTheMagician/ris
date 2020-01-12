#include "SimpleUserinterface.hpp"

#include "common/IRenderer.hpp"
#include "common/ILoader.hpp"
#include "common/IAudio.hpp"
#include "common/IInput.hpp"

#include "common/Logger.hpp"

#undef min
#undef max
#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <algorithm>
#include <sstream>

#include "common/StringSupport.hpp"

using namespace std::literals;
using std::string;
using std::vector;

using namespace std::placeholders;

namespace RIS
{

    Console::Console(const SystemLocator &systems)
        : systems(systems), currentY(0), consoleFontSize(15.0f), isOpen(false), isMoving(false), openSpeed(1), backgroundColor(0.3f, 0.3f, 0.3f, 0.85f), fontColor(0.7f, 0.7f, 0.7f, 1),
            offsetY(7.5f)
    {

    }

    Console::~Console()
    {

    }

    void Console::InitLimits(const glm::vec2 &viewSize)
    {
        auto &r2d = systems.GetRenderer().Get2DRenderer();

        this->viewSize = viewSize;
        maxY = viewSize.y * 0.5f;
        currentY = viewSize.y;

        consoleFont = r2d.LoadFont("console");
        maxLineHeight = r2d.MaxHeightFont(consoleFont, consoleFontSize);
        maxLines = 512;//static_cast<int>(maxY / maxLineHeight);

        IWindow& wnd = systems.GetWindow();

        BindFunc("con", std::bind(&Console::SetParam, this, _1));
        BindFunc("exit", [&wnd](vector<string> params){ wnd.Exit(0); return ""; });
    }

    void Console::Open()
    {
        if(!isOpen)
        {
            isOpen = true;
            isMoving = true;
        }
    }

    void Console::Close()
    {
        if(isOpen)
        {
            isOpen = false;
            isMoving = true;
        }
    }

    void Console::Toggle()
    {
        if(isOpen)
            Close();
        else 
            Open();
    }

    void Console::Print(const std::string &msg)
    {
        while(lines.size() >= maxLines - 2)
            lines.pop_back();
        
        lines.insert(lines.begin(), msg);
    }

    void Console::BindVar(const std::string &name, const long *var)
    {
        longVars.insert_or_assign(name, var);
    }

    void Console::BindVar(const std::string &name, const float *var)
    {
        floatVars.insert_or_assign(name, var);
    }

    void Console::BindVar(const std::string &name, const std::string *var)
    {
        stringVars.insert_or_assign(name, var);
    }

    void Console::BindFunc(const std::string &name, ConsoleFunc func)
    {
        if(name == "set")
            return;
        funcVars.insert_or_assign(name, func);
    }

    bool Console::IsOpen()
    {
        return isOpen;
    }

    void Console::Update()
    {
        if(isMoving)
        {
            float dir = -1;
            if(!isOpen)
            {
                dir = -dir;
            }

            currentY += dir * openSpeed;

            if(isOpen && currentY <= maxY)
            {
                isMoving = false;
                currentY = maxY;
            }
            else if(!isOpen && currentY >= viewSize.y)
            {
                isMoving = false;
                currentY = viewSize.y;
            }
        }
    }

    void Console::Draw(I2DRenderer &renderer)
    {
        if(isOpen || isMoving)
        {
            renderer.SetTexture(1, 0);
            renderer.DrawQuad({0, currentY}, {viewSize.x, maxY}, backgroundColor);

            auto it = lines.begin();
            for(int i = 1; it != lines.end(); ++it, ++i)
            {
                const string &msg = *it;
                glm::vec2 pos = {0, (currentY + i * maxLineHeight) + offsetY};
                renderer.DrawText(msg, consoleFont, pos, consoleFontSize, fontColor);
            }
            renderer.DrawText(">" + inputLine + "_", consoleFont, {0, currentY + offsetY}, consoleFontSize, fontColor);
        }
    }

    void Console::OnChar(char c)
    {
        if(isOpen)
        {
            inputLine.insert(inputLine.end(), c);
        }
    }

    void Console::OnKeyDown(InputKeys key)
    {
        if(isOpen)
        {
            if(key == InputKeys::ENTER || key == InputKeys::KP_ENTER)
            {
                if(inputLine.empty())
                    return;

                Print(inputLine);
                if(!ProcessLine(inputLine))
                    Print("Unkown command: \"" + inputLine + "\"");
                inputLine = "";
            }
            else if(key == InputKeys::BACKSPACE)
            {
                if(inputLine.size() > 0)
                    inputLine.erase(inputLine.end() - 1);
            }
        }
    }

    bool Console::ProcessLine(const string &lineToProcess)
    {
        std::istringstream sstream(lineToProcess);
        std::vector<string> tokens{std::istream_iterator<string>{sstream},
                            std::istream_iterator<string>{}};

        string keyword = lowerCase(tokens[0]);
        tokens.erase(tokens.begin());
        if(keyword == "set")
        {
            return true;
        }
        else
        {
            if(funcVars.count(keyword) > 0)
            {
                auto func = funcVars.at(keyword);
                auto &msg = func(tokens);
                if(!msg.empty())
                    Print(msg);
                return true;
            }
        }
        return false;
    }

    string Console::SetParam(vector<string> params)
    {
        if(params.size() < 1)
            return "no param specified!";

        if(params[0] == "background")
        {
            if(params.size() < 5)
                return "not enough color data";
            
            backgroundColor.r = std::stof(params[1]);
            backgroundColor.g = std::stof(params[2]);
            backgroundColor.b = std::stof(params[3]);
            backgroundColor.a = std::stof(params[4]);
        }
        else if(params[0] == "forground")
        {
            if(params.size() < 5)
                return "not enough color data";
            
            fontColor.r = std::stof(params[1]);
            fontColor.g = std::stof(params[2]);
            fontColor.b = std::stof(params[3]);
            fontColor.a = std::stof(params[4]);
        }

        return "";
    }

// #################################

    const int SimpleUserinterface::JSON_VERSION = 1;

    SimpleUserinterface::SimpleUserinterface(const SystemLocator &systems, Config &config)
        : systems(systems), config(config), uiFramebufferId(-1), uiWidth(config.GetInt("r_width", 800)), uiHeight(config.GetInt("r_height", 600)), console(systems)
    {
        rootContainer = MakePanel(systems);
    }

    SimpleUserinterface::~SimpleUserinterface()
    {
    }

    void SimpleUserinterface::InitializeRootElements()
    {
        console.InitLimits(glm::vec2(uiWidth, uiHeight));

        console.Print("Hello World!");
        console.Print("Hello World!2");

        IRenderer &renderer = systems.GetRenderer();
        uiFramebufferId = renderer.CreateFramebuffer(uiWidth, uiHeight, true);

        IInput &input = systems.GetInput();
        input.RegisterChar("ui", std::bind(&SimpleUserinterface::OnChar, this, _1));
        input.RegisterMouse("ui", std::bind(&SimpleUserinterface::OnMouseMove, this, _1, _2));
        input.RegisterButtonDown("ui", std::bind(&SimpleUserinterface::OnMouseDown, this, _1));
        input.RegisterButtonUp("ui", std::bind(&SimpleUserinterface::OnMouseUp, this, _1));
        input.RegisterKeyDown("ui", std::bind(&SimpleUserinterface::OnKeyDown, this, _1));

        LoadLayout("main");
    }

    IConsole &SimpleUserinterface::GetConsole()
    {
        return console;
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

        console.Draw(renderer2D);

        renderer2D.End();

        renderer.Draw(uiFramebufferId);
    }

    void SimpleUserinterface::Update()
    {
        rootContainer->Update();
        console.Update();
    }

    void SimpleUserinterface::OnChar(char character)
    {
        rootContainer->OnChar(character);
        console.OnChar(character);
    }

    void SimpleUserinterface::OnMouseMove(float x, float y)
    {
        y = uiHeight - y;
        rootContainer->OnMouseMove(x, y);
    }

    void SimpleUserinterface::OnMouseDown(InputButtons button)
    {
        rootContainer->OnMouseDown(button);
    }

    void SimpleUserinterface::OnMouseUp(InputButtons button)
    {
        rootContainer->OnMouseUp(button);
    }

    void SimpleUserinterface::OnKeyDown(InputKeys key)
    {
        console.OnKeyDown(key);
    }
}
