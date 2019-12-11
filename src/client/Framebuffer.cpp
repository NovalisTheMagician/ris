#include "Framebuffer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    Framebuffer::Framebuffer()
        : GLObject(0)
    {
    }

    Framebuffer::Framebuffer(GLuint frambufId)
        : GLObject(frambufId)
    {
    }

    Framebuffer::Framebuffer(int width, int height, GLenum colorFormat, bool useDepth)
        : useDepth(useDepth)
    {
        glCreateFramebuffers(1, &id);
        colorTexture = Texture::Create(colorFormat, width, height);
        glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0, colorTexture.GetId(), 0);
        if(useDepth)
        {
            depthTexture = Texture::Create(GL_DEPTH_COMPONENT32, width, height);
            glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, depthTexture.GetId(), 0);
        }
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &id);
    }

    Framebuffer::Framebuffer(Framebuffer &&other)
    {
        id = other.id;
        colorTexture = std::move(other.colorTexture);
        depthTexture = std::move(other.depthTexture);
        useDepth = other.useDepth;
        other.id = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer &&other)
    {
        id = other.id;
        colorTexture = std::move(other.colorTexture);
        depthTexture = std::move(other.depthTexture);
        useDepth = other.useDepth;
        other.id = 0;
        return *this;
    }

    Framebuffer Framebuffer::Create(int width, int height, GLenum colorFormat, bool useDepth)
    {
        return Framebuffer(width, height, colorFormat, useDepth);
    }

    void Framebuffer::Clear(const glm::vec4 &color, float depth)
    {
        glClearNamedFramebufferfv(id, GL_COLOR, 0, glm::value_ptr(color));
        glClearNamedFramebufferfv(id, GL_DEPTH, 0, &depth);
    }

    void Framebuffer::Resize(int width, int height)
    {
        // TODO
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    Texture& Framebuffer::GetColorTexture()
    {
        return colorTexture;
    }

    Texture& Framebuffer::GetDepthTexture()
    {
        return depthTexture;
    }
}
