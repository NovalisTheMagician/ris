#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

using namespace gl46core;

namespace RIS
{
    Sampler::Sampler()
        : samplerId(0)
    {
    }

    Sampler::~Sampler()
    {
        glDeleteSamplers(1, &samplerId);
    }

    Sampler::Sampler(Sampler &&other)
    {
        samplerId = other.samplerId;
        other.samplerId = 0;
    }

    Sampler& Sampler::operator=(Sampler &&other)
    {
        samplerId = other.samplerId;
        other.samplerId = 0;
        return *this;
    }

    void Sampler::Create()
    {
        glGenSamplers(1, &samplerId);
    }

    void Sampler::SetMinFilter(GLenum minFilter)
    {
        glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, minFilter);
    }

    void Sampler::SetMagFilter(GLenum magFilter)
    {
        glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, magFilter);
    }

    void Sampler::SetMaxAnisotropy(float maxAniso)
    {
        glSamplerParameterfv(samplerId, GL_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    }

    void Sampler::Bind(int textureUnit)
    {
        glBindSampler(textureUnit, samplerId);
    }

    GLuint Sampler::GetId() const
    {
        return samplerId;
    }
}
