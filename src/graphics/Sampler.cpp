#include "graphics/Sampler.hpp"

#include <glad/glad.h>

#include <utility>

namespace RIS::Graphics
{
    Sampler Sampler::Bilinear(float maxAniso)
    {
        return Sampler(MinFilter::LINEAR, MagFilter::LINEAR, maxAniso);
    }

    Sampler Sampler::Trilinear(float maxAniso)
    {
        return Sampler(MinFilter::LINEAR_MIPMAP_LINEAR, MagFilter::LINEAR, maxAniso);
    }

    Sampler Sampler::Nearest(float maxAniso)
    {
        return Sampler(MinFilter::NEAREST, MagFilter::NEAREST, maxAniso);
    }

    Sampler::Sampler()
        : GLObject(0)
    {
    }

    Sampler::Sampler(MinFilter minFilter, MagFilter magFilter, float maxAniso)
    {
        glGenSamplers(1, &id);
        //glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
        //glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
        //glSamplerParameterfv(id, GL_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        SetMinFilter(minFilter);
        SetMagFilter(magFilter);
        SetMaxAnisotropy(maxAniso);
    }

    Sampler::~Sampler()
    {
        glDeleteSamplers(1, &id);
    }

    Sampler::Sampler(Sampler &&other)
    {
        std::swap(id, other.id);
    }

    Sampler& Sampler::operator=(Sampler &&other)
    {
        std::swap(id, other.id);
        return *this;
    }

    void Sampler::SetMinFilter(MinFilter minFilter)
    {
        glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter));
    }

    void Sampler::SetMagFilter(MagFilter magFilter)
    {
        glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter));
    }

    void Sampler::SetMaxAnisotropy(float maxAniso)
    {
        glSamplerParameterfv(id, GL_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    }

    void Sampler::Bind(int textureUnit) const
    {
        glBindSampler(textureUnit, id);
    }
}
