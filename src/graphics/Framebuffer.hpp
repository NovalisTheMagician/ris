#pragma once

#include "graphics/GLObject.hpp"

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "graphics/Texture.hpp"

namespace RIS::Graphics
{
    class Framebuffer : public GLObject
    {
    public:
        Framebuffer(int width, int height, TextureFormat colorFormat = TextureFormat::RGBA8, TextureFormat depthFormat = TextureFormat::DEPTH32);
        Framebuffer();
        Framebuffer(GLuint frambufId);
        ~Framebuffer();

        Framebuffer(const Framebuffer &) = delete;
        Framebuffer& operator=(const Framebuffer &) = delete;

        Framebuffer(Framebuffer &&other);
        Framebuffer& operator=(Framebuffer &&other);

        void Resize(int width, int height);
        void Bind();
        void Clear(const glm::vec4 &color, float depth);

        Texture& ColorTexture();
        Texture& DepthTexture();

    private:
        Texture colorTexture;
        Texture depthTexture;
        TextureFormat colorFormat;
        TextureFormat depthFormat;

    };
}
