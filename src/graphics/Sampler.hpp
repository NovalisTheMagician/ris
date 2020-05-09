#pragma once

#include "graphics/GLObject.hpp"

#include <glad/glad.h>

namespace RIS
{
    namespace Graphics
    {
        class Sampler : GLObject
        {
        public:
            Sampler(GLenum minFilter, GLenum magFilter, float maxAniso);
            Sampler();
            ~Sampler();

            Sampler(const Sampler &) = delete;
            Sampler& operator=(const Sampler &) = delete;

            Sampler(Sampler &&other);
            Sampler& operator=(Sampler &&other);

            void SetMinFilter(GLenum minFilter);
            void SetMagFilter(GLenum magFilter);
            void SetMaxAnisotropy(float maxAniso);

            void Bind(int textureUnit);

        };
    }
}
