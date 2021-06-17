#include "ui/TabbedPanel.hpp"

#include <string>

#include <fmt/format.h>

#include "ui/Button.hpp"

namespace RIS::UI
{
    std::vector<std::reference_wrapper<Panel>> MakeTabbable(Panel& parentPanel, const std::vector<std::string_view> &names, const glm::vec2 &buttonSize, float margin)
    {
        size_t size = names.size();

        std::vector<std::reference_wrapper<Button>> buttons;
        std::vector<std::reference_wrapper<Panel>> contentPanels;

        glm::vec2 panelSize = parentPanel.GetSize();
        panelSize.y -= buttonSize.y;

        for(size_t i = 0; i < size; ++i)
        {
            Button &button = parentPanel.CreateButton()
                                        .SetName(fmt::format("tabbutton_{}", i))
                                        .SetData(i)
                                        .SetText(std::string(names[i]))
                                        .SetToggleMode(true)
                                        .SetSize(buttonSize)
                                        .SetFont(parentPanel.GetFont())
                                        .SetFontSize(parentPanel.GetFontSize())
                                        .SetPosition({i * (buttonSize.x + margin) + margin, 0});
            
            Panel &panel = parentPanel.CreatePanel()
                                        .SetName(fmt::format("tabcontent_{}", i))
                                        .SetData(i)
                                        .SetSize(panelSize)
                                        .SetFont(parentPanel.GetFont())
                                        .SetFontSize(parentPanel.GetFontSize())
                                        .SetPosition({0, buttonSize.y})
                                        .SetVisible(false);

            if(i == 0)
            {
                button.SetToggle(true);
                panel.SetVisible(true);
            }

            buttons.push_back(button);
            contentPanels.push_back(panel);
        }

        for(size_t i = 0; i < size; ++i)
        {
            buttons[i].get().SetCallback([contentPanels, buttons, size](Button &button)
            {
                size_t index = std::any_cast<size_t>(button.GetData());

                for(size_t i = 0; i < size; ++i)
                {
                    buttons[i].get().SetToggle(false);
                    contentPanels[i].get().SetVisible(false);
                }

                buttons[index].get().SetToggle(true);
                contentPanels[index].get().SetVisible(true);
            });
        }

        return contentPanels;
    }
}
