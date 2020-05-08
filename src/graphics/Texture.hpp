#pragma once

#include "graphics/GLObject.hpp"

#include <glm/glm.hpp>

#include <glbinding/gl46core/gl.h>

#include "graphics/Buffer.hpp"

namespace RIS
{
    namespace Graphics
    {
        class Texture : public GLObject
        {
        public:
            Texture();
            Texture(const std::vector<std::byte> &data, bool flip = true);
            Texture(gl::GLenum format, int width, int height);
            Texture(const glm::vec4 color);
            Texture(gl::GLenum type);
            ~Texture();

            Texture(const Texture &) = delete;
            Texture& operator=(const Texture &) = delete;

            Texture(Texture &&other);
            Texture& operator=(Texture &&other);

            void SetBuffer(const Buffer &buffer, gl::GLenum format);

            void Bind(gl::GLuint textureUnit);

        };
    }
}
