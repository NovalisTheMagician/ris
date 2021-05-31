#include "game/GameLoop.hpp"

#include "RIS.hpp"
#include "ui/Userinterface.hpp"
#include "ui/Scrollpanel.hpp"
#include "ui/TabbedPanel.hpp"

#include "graphics/Font.hpp"
#include "graphics/Texture.hpp"

#include "window/Window.hpp"

#include "loader/Loaders.hpp"

#include "misc/Version.hpp"

#include <fmt/format.h>

namespace RIS::Game
{
    void GameLoop::InitMenus()
    {
        std::string version = fmt::format("V {}.{}", std::to_string(Version::MAJOR), std::to_string(Version::MINOR));
        auto &ui = GetUserinterface();

        GetConsole().Print(version);

        auto font = Loader::Load<Graphics::Font>("fonts/IMMORTAL.json", resourcePack);

        auto &rootPanel = ui.CreateMenu("mainMenu").SetSize({90, 300}).SetPosition({24, 24}).SetColor(Graphics::Colors::Cyan);
        auto &btn1 = rootPanel.CreateButton()
                                .SetCallback([](UI::Button&){ GetConsole().Print("test"); })
                                .SetText("Play")
                                .SetSize({74, 24})
                                .SetPosition({0, 0})
                                .SetFontSize(16)
                                .SetFont(font)
                                .SetName("btn_play");
        
        auto &btn2 = rootPanel.CreateButton()
                                .SetCallback([&ui](UI::Button&){ ui.PushMenu("optionsMenu"); })
                                .SetText("Options")
                                .SetSize({74, 24})
                                .SetPosition({0, 36})
                                .SetFontSize(16)
                                .SetFont(font);
        
        auto &btn3 = rootPanel.CreateButton()
                                .SetCallback([](UI::Button&){ GetWindow().Exit(0); })
                                .SetText("Quit")
                                .SetSize({74, 24})
                                .SetPosition({0, 36 * 2})
                                .SetFontSize(16)
                                .SetFont(font);

        if(auto b = rootPanel.GetComponent<UI::Button>("btn_play"))
        {
            UI::Button &button = *b;
            GetConsole().Print(button.GetText());
        }

        auto &optionsPanel = ui.CreateMenu("optionsMenu").SetSize({300, 300}).SetColor(Graphics::Colors::Red).SetAnchor(UI::Anchor::Center).SetFont(font);
        auto panels = UI::MakeTabbable(optionsPanel, {"Graphics", "Sound", "Input"}, {74, 24});

        panels[0].get().SetColor(Graphics::Colors::Yellow);
        panels[1].get().SetColor(Graphics::Colors::Magenta);
        panels[2].get().SetColor(Graphics::Colors::Green);

        UI::ScrollButtons sb;
        sb.up.normal = Loader::Load<Graphics::Texture>("ui/scroll_arrow_up.dds", resourcePack);
        sb.up.hover = Loader::Load<Graphics::Texture>("ui/scroll_arrow_up_hover.dds", resourcePack);
        sb.up.click = Loader::Load<Graphics::Texture>("ui/scroll_arrow_up_click.dds", resourcePack);

        sb.down.normal = Loader::Load<Graphics::Texture>("ui/scroll_arrow_down.dds", resourcePack);
        sb.down.hover = Loader::Load<Graphics::Texture>("ui/scroll_arrow_down_hover.dds", resourcePack);
        sb.down.click = Loader::Load<Graphics::Texture>("ui/scroll_arrow_down_click.dds", resourcePack);

        auto &scrollPanel = UI::MakeScrollable(panels[0], {0, 60}, sb, {36, 36}, 24);
        auto &testbtn = scrollPanel.CreateButton()
                                .SetCallback([](UI::Button&){ GetConsole().Print("options test"); })
                                .SetText("Test")
                                .SetSize({74, 24})
                                .SetPosition({0, 0})
                                .SetFontSize(16)
                                .SetFont(font);
        
        auto &backbtn = scrollPanel.CreateButton()
                                .SetCallback([&ui](UI::Button&){ ui.PopMenu(); })
                                .SetText("Back")
                                .SetSize({74, 24})
                                .SetPosition({0, 36})
                                .SetFontSize(16)
                                .SetFont(font);

        for(int i = 0; i < 10; ++i)
        {
            scrollPanel.CreateButton().SetText(fmt::format("Test {}", i)).SetSize({74, 24}).SetPosition({0, (i+2) * 36}).SetToggleMode(true).SetFontSize(16).SetFont(font).SetActive(i % 2);
        }
        
        //ui.PushMenu("mainMenu");
    }
}
