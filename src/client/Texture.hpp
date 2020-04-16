#pragma once

#include "GLObject.hpp"

#include <glm/glm.hpp>

#include <glbinding/gl46core/gl.h>

#include "Buffer.hpp"

namespace RIS
{
    class Texture : public GLObject
    {
    private:
        Texture(const std::byte *data, const std::size_t &size, bool flip = true);
        Texture(gl::GLenum format, int width, int height);
        Texture(const glm::vec4 color);
        Texture(gl::GLenum type);

    public:
        Texture();
        ~Texture();

        Texture(const Texture &) = delete;
        Texture& operator=(const Texture &) = delete;

        Texture(Texture &&other);
        Texture& operator=(Texture &&other);

        void SetBuffer(const Buffer &buffer, gl::GLenum format);

        void Bind(gl::GLuint textureUnit);

    public:
        static Texture Create(const std::byte *data, const std::size_t &size, bool flip = true);
        static Texture Create(gl::GLenum format, int width, int height);
        static Texture Create(const glm::vec4 color);
        static Texture Create();

    };
}
