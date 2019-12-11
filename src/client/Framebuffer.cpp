#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    Framebuffer::Framebuffer()
        : framebufferId(0)
    {
    }

    Framebuffer::Framebuffer(GLuint frambufId)
        : framebufferId(frambufId)
    {
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &framebufferId);
    }

    Framebuffer::Framebuffer(Framebuffer &&other)
    {
        framebufferId = other.framebufferId;
        colorTexture = std::move(other.colorTexture);
        depthTexture = std::move(other.depthTexture);
        other.framebufferId = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer &&other)
    {
        framebufferId = other.framebufferId;
        other.framebufferId = 0;
        colorTexture = std::move(other.colorTexture);
        depthTexture = std::move(other.depthTexture);
        return *this;
    }

    void Framebuffer::Create(int width, int height, GLenum colorFormat, bool useDepth)
    {
        glCreateFramebuffers(1, &framebufferId);
        colorTexture.Create(colorFormat, width, height);
        glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, colorTexture.GetId(), 0);
        if(useDepth)
        {
            depthTexture.Create(GL_DEPTH_COMPONENT32, width, height);
            glNamedFramebufferTexture(framebufferId, GL_DEPTH_ATTACHMENT, depthTexture.GetId(), 0);
        }
    }

    void Framebuffer::Clear(const glm::vec4 &color, float depth)
    {
        glClearNamedFramebufferfv(framebufferId, GL_COLOR, 0, glm::value_ptr(color));
        glClearNamedFramebufferfv(framebufferId, GL_DEPTH, 0, &depth);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    }

    GLuint Framebuffer::GetId() const
    {
        return framebufferId;
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
