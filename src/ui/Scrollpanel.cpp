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
                                        .UseMouseScrolling(true);

        parentPanel.CreateButton()
                    .SetName("scroll_up_button")
                    .SetSize({scrollAreaWidth, scrollAreaWidth})
                    .SetPosition({0, 0})
                    .SetText("/\\")
                    .SetFontSize(10.0f)
                    .SetAnchor(Anchor::TopRight)
                    .SetNormalTexture(buttonTextures.upNormal)
                    .SetHoverTexture(buttonTextures.upHover)
                    .SetDownTexture(buttonTextures.upClick)
                    .SetCallback([&contentPanel](Button&)
                    {
                        glm::vec2 offset = contentPanel.GetOffset();
                        offset.y += contentPanel.GetOffsetStep().y;
                        contentPanel.SetOffset(offset);
                    });

        parentPanel.CreateButton()
                    .SetName("scroll_down_button")
                    .SetSize({scrollAreaWidth, scrollAreaWidth})
                    .SetPosition({0, 0})
                    .SetText("\\/")
                    .SetFontSize(10.0f)
                    .SetAnchor(Anchor::BottomRight)
                    .SetNormalTexture(buttonTextures.downNormal)
                    .SetHoverTexture(buttonTextures.downHover)
                    .SetDownTexture(buttonTextures.downClick)
                    .SetCallback([&contentPanel](Button&)
                    {
                        glm::vec2 offset = contentPanel.GetOffset();
                        offset.y -= contentPanel.GetOffsetStep().y;
                        contentPanel.SetOffset(offset);
                    });

        return contentPanel;
    }
}
