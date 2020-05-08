#include "graphics/Sampler.hpp"

#include <glbinding/gl46core/gl.h>

using namespace gl46core;

namespace RIS
{
    namespace Graphics
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
            std::swap(id, other.id);
        }

        Sampler& Sampler::operator=(Sampler &&other)
        {
            std::swap(id, other.id);
            return *this;
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
}
