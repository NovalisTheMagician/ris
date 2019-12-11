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
        Texture(const std::byte *data, const std::size_t &size);
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

        template<typename T>
        void SetBuffer(const Buffer<T> &buffer, gl::GLenum format);

        void Bind(gl::GLuint textureUnit);

    public:
        static Texture Create(const std::byte *data, const std::size_t &size);
        static Texture Create(gl::GLenum format, int width, int height);
        static Texture Create(const glm::vec4 color);
        static Texture Create();

    };

    template<typename T>
    void Texture::SetBuffer(const Buffer<T> &buffer, gl::GLenum format)
    {
        gl::glTextureBuffer(id, format, buffer.GetId());
    }
}
