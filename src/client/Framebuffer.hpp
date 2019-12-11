#pragma once

#include "GLObject.hpp"

#include <glm/glm.hpp>

#include <glbinding/gl46core/gl.h>

#include "Texture.hpp"

namespace RIS
{
    class Framebuffer : public GLObject
    {
    private:
        Framebuffer(int width, int height, gl::GLenum textureFormat, bool useDepth);

    public:
        Framebuffer();
        Framebuffer(gl::GLuint frambufId);
        ~Framebuffer();

        Framebuffer(const Framebuffer &) = delete;
        Framebuffer& operator=(const Framebuffer &) = delete;

        Framebuffer(Framebuffer &&other);
        Framebuffer& operator=(Framebuffer &&other);

        void Resize(int width, int height);
        void Bind();
        void Clear(const glm::vec4 &color, float depth);

        Texture& GetColorTexture();
        Texture& GetDepthTexture();

    public:
        static Framebuffer Create(int width, int height, gl::GLenum textureFormat, bool useDepth);

    private:
        Texture colorTexture;
        Texture depthTexture;
        bool useDepth;

    };
}
