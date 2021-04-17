#include "ui/Scrollpanel.hpp"

namespace RIS::UI
{
    Panel& MakeScrollable(Panel& parentPanel, glm::vec2 scrollSize, const ScrollButtons &buttonTextures, glm::vec2 scrollStep, float scrollAreaWidth)
    {
        glm::vec2 parentSize = parentPanel.GetSize();
        Panel& contentPanel = parentPanel.CreatePanel()
                                        .SetName("content_panel")
                                        .SetPosition({0, 0})
                                        .SetSize({parentSize.x - scrollAreaWidth, parentSize.y})
                                        .SetAnchor(Anchor::TopLeft)
                                        .SetMaxOffset(scrollSize)
                                        .SetOffsetStep(scrollStep)
                                        .UseMouseScrolling(true)
                                        .SetFont(parentPanel.GetFont())
                                        .SetFontSize(parentPanel.GetFontSize());

        auto &upBtn = parentPanel.CreateButton()
                    .SetName("scroll_up_button")
                    .SetSize({scrollAreaWidth, scrollAreaWidth})
                    .SetPosition({0, 0})
                    .SetText("/\\")
                    .SetFontSize(10.0f)
                    .SetAnchor(Anchor::TopRight)
                    .SetTextures(buttonTextures.up)
                    .SetContinuous(true)
                    .SetCallback([&contentPanel](Button&)
                    {
                        glm::vec2 offset = contentPanel.GetOffset();
                        offset.y += contentPanel.GetOffsetStep().y;
                        contentPanel.SetOffset(offset);
                    });

        auto &downBtn = parentPanel.CreateButton()
                    .SetName("scroll_down_button")
                    .SetSize({scrollAreaWidth, scrollAreaWidth})
                    .SetPosition({0, 0})
                    .SetText("\\/")
                    .SetFontSize(10.0f)
                    .SetAnchor(Anchor::BottomRight)
                    .SetTextures(buttonTextures.down)
                    .SetContinuous(true)
                    .SetCallback([&contentPanel](Button&)
                    {
                        glm::vec2 offset = contentPanel.GetOffset();
                        offset.y -= contentPanel.GetOffsetStep().y;
                        contentPanel.SetOffset(offset);
                    });

        if(buttonTextures.up.normal)
        {
            upBtn.SetText("");
            downBtn.SetText("");
        }

        return contentPanel;
    }
}
