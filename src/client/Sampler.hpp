#pragma once

#include "GLObject.hpp"

#include <glbinding/gl46core/gl.h>

namespace RIS
{
    class Sampler : GLObject
    {
    private:
        Sampler(gl::GLenum minFilter, gl::GLenum magFilter, float maxAniso);

    public:
        Sampler();
        ~Sampler();

        Sampler(const Sampler &) = delete;
        Sampler& operator=(const Sampler &) = delete;

        Sampler(Sampler &&other);
        Sampler& operator=(Sampler &&other);

        void SetMinFilter(gl::GLenum minFilter);
        void SetMagFilter(gl::GLenum magFilter);
        void SetMaxAnisotropy(float maxAniso);

        void Bind(int textureUnit);

    public:
        static Sampler Create(gl::GLenum minFilter, gl::GLenum magFilter, float maxAniso);

    };
}
