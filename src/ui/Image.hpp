#pragma once

#include "ui/Component.hpp"

#include "graphics/Texture.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace RIS::UI
{
    class Image
    {
    public:
        Image(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont);
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;

        Image& SetName(const std::string &name);
        std::string GetName() const;
        Image& SetAnchor(Anchor anchor);
        Anchor GetAnchor() const;
        Image& SetPosition(const glm::vec2 &position);
        Image& SetSize(const glm::vec2 &size);
        Image& SetImage(std::shared_ptr<Graphics::Texture> image);
        Image& SetColor(const glm::vec4 &color);

        void Update(const Timer &timer);
        void Draw(Graphics::SpriteRenderer &renderer);

        void OnMouseMove(float x, float y) {};
        void OnMouseDown(Input::InputKey mouseCode) {};
        void OnMouseUp(Input::InputKey mouseCode) {};
        void OnMouseWheel(float x, float y) {};

        void OnKeyDown(Input::InputKey keyCode) {};
        void OnKeyUp(Input::InputKey keyCode) {};
        void OnKeyRepeat(Input::InputKey keyCode) {};

        void OnChar(uint32_t c) {};

    private:
        std::string name;
        glm::vec2 position;
        glm::vec2 size = glm::vec2(64, 32);
        Anchor anchor = Anchor::TopLeft;
        Graphics::Framebuffer &parentFramebuffer;
        Graphics::Font::Ptr font;

        std::shared_ptr<Graphics::Texture> image;
        glm::vec4 color = glm::vec4(1, 1, 1, 1);

    };
}
