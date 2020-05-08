#pragma once

#include "graphics/GLObject.hpp"

#include <glm/glm.hpp>

#include <glbinding/gl46core/gl.h>

#include "graphics/Texture.hpp"

namespace RIS
{
    namespace Graphics
    {
        class Framebuffer : public GLObject
        {
        public:
            Framebuffer(int width, int height, gl::GLenum textureFormat, bool useDepth);
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

        private:
            Texture colorTexture;
            Texture depthTexture;
            bool useDepth;
            gl::GLenum colorFormat;

        };
    }
}
