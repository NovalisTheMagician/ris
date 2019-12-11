#include "Sampler.hpp"

#include <glbinding/gl46core/gl.h>

using namespace gl46core;

namespace RIS
{
    Sampler::Sampler()
        : GLObject(0)
    {
    }

    Sampler::Sampler(gl::GLenum minFilter, gl::GLenum magFilter, float maxAniso)
    {
        glGenSamplers(1, &id);
        glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
        glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter);
        glSamplerParameterfv(id, GL_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    }

    Sampler::~Sampler()
    {
        glDeleteSamplers(1, &id);
    }

    Sampler::Sampler(Sampler &&other)
    {
        id = other.id;
        other.id = 0;
    }

    Sampler& Sampler::operator=(Sampler &&other)
    {
        id = other.id;
        other.id = 0;
        return *this;
    }

    Sampler Sampler::Create(gl::GLenum minFilter, gl::GLenum maxFilter, float maxAniso)
    {
        return Sampler(minFilter, maxFilter, maxAniso);
    }

    void Sampler::SetMinFilter(GLenum minFilter)
    {
        glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
    }

    void Sampler::SetMagFilter(GLenum magFilter)
    {
        glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter);
    }

    void Sampler::SetMaxAnisotropy(float maxAniso)
    {
        glSamplerParameterfv(id, GL_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    }

    void Sampler::Bind(int textureUnit)
    {
        glBindSampler(textureUnit, id);
    }
}
