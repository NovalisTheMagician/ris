#include "ui/Image.hpp"

namespace RIS::UI
{
    Image::Image(Graphics::Framebuffer &parentFramebuffer, Graphics::Font::Ptr defaultFont)
        :parentFramebuffer(parentFramebuffer), font(defaultFont)
    {}

    Image& Image::SetName(const std::string &name)
    { 
        this->name = name; return *this; 
    }

    std::string Image::GetName() const 
    { 
        return name; 
    }

    Image& Image::SetAnchor(Anchor anchor) 
    { 
        this->anchor = anchor; 
        return *this; 
    }

    Anchor Image::GetAnchor() const 
    { 
        return anchor; 
    }

    Image& Image::SetPosition(const glm::vec2 &position) 
    { 
        this->position = position; 
        return *this; 
    }

    Image& Image::SetSize(const glm::vec2 &size) 
    { 
        this->size = size; 
        return *this; 
    }

    Image& Image::SetImage(std::shared_ptr<Graphics::Texture> image)
    {
        this->image = image;
        return *this;
    }

    Image& Image::SetColor(const glm::vec4 &color)
    {
        this->color = color;
        return *this;
    }

    void Image::Update(const Timer &timer)
    {

    }

    void Image::Draw(Graphics::SpriteRenderer &renderer)
    {
        if(image)
            renderer.DrawTexture(*image, position, size, {1, 1, 1, 1});
    }
}
