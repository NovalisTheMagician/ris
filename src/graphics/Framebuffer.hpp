#pragma once

#include "graphics/GLObject.hpp"

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "graphics/Texture.hpp"

namespace RIS
{
    namespace Graphics
    {
        class Framebuffer : public GLObject
        {
        public:
            Framebuffer(int width, int height, GLenum textureFormat, bool useDepth);
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
            bool useDepth;
            GLenum colorFormat;

        };
    }
}
