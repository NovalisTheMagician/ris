#pragma once

#include "graphics/GLObject.hpp"

#include <glad/glad.h>

namespace RIS::Graphics
{
    enum class MinFilter
    {
        LINEAR = GL_LINEAR,
        NEAREST = GL_NEAREST,

        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,

        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST
    };

    enum class MagFilter
    {
        LINEAR = GL_LINEAR,
        NEAREST = GL_NEAREST
    };

    class Sampler : GLObject
    {
    public:
        static Sampler Bilinear(float maxAniso = 1.0f);
        static Sampler Trilinear(float maxAniso = 1.0f);
        static Sampler Nearest(float maxAniso = 1.0f);
    public:
        Sampler(MinFilter minFilter, MagFilter magFilter, float maxAniso = 1.0f);
        Sampler();
        ~Sampler();

        Sampler(const Sampler &) = delete;
        Sampler& operator=(const Sampler &) = delete;

        Sampler(Sampler &&other);
        Sampler& operator=(Sampler &&other);

        void SetMinFilter(MinFilter minFilter);
        void SetMagFilter(MagFilter magFilter);
        void SetMaxAnisotropy(float maxAniso);

        void Bind(int textureUnit) const;

    };
}
