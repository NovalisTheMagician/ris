#pragma once

#include "graphics/GLObject.hpp"

#include <glbinding/gl46core/gl.h>

namespace RIS
{
    namespace Graphics
    {
        class Sampler : GLObject
        {
        public:
            Sampler(gl::GLenum minFilter, gl::GLenum magFilter, float maxAniso);
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

        };
    }
}
