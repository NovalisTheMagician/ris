#include "graphics/Framebuffer.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RIS::Graphics
{
    Framebuffer::Framebuffer()
        : GLObject(0)
    {
    }

    Framebuffer::Framebuffer(GLuint frambufId)
        : GLObject(frambufId), colorFormat(TextureFormat::RGBA8), depthFormat(TextureFormat::DEPTH32)
    {
    }

    Framebuffer::Framebuffer(int width, int height, TextureFormat colorFormat, TextureFormat depthFormat)
        : colorFormat(colorFormat), depthFormat(depthFormat)
    {
        glCreateFramebuffers(1, &id);
        Resize(width, height);
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &id);
    }

    Framebuffer::Framebuffer(Framebuffer &&other)
    {
        std::swap(id, other.id);
        colorTexture = std::move(other.colorTexture);
        depthTexture = std::move(other.depthTexture);
        colorFormat = other.colorFormat;
        depthFormat = other.depthFormat;
        other.id = 0;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer &&other)
    {
        std::swap(id, other.id);
        colorTexture = std::move(other.colorTexture);
        depthTexture = std::move(other.depthTexture);
        depthFormat = other.depthFormat;
        colorFormat = other.colorFormat;
        return *this;
    }

    void Framebuffer::Clear(const glm::vec4 &color, float depth)
    {
        glClearNamedFramebufferfv(id, GL_COLOR, 0, glm::value_ptr(color));
        glClearNamedFramebufferfv(id, GL_DEPTH, 0, &depth);
    }

    void Framebuffer::Resize(int width, int height)
    {
        colorTexture = Texture(colorFormat, width, height);
        glNamedFramebufferTexture(id, GL_COLOR_ATTACHMENT0, colorTexture.GetId(), 0);
        depthTexture = Texture(depthFormat, width, height);
        glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, depthTexture.GetId(), 0);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    Texture& Framebuffer::ColorTexture()
    {
        return colorTexture;
    }

    Texture& Framebuffer::DepthTexture()
    {
        return depthTexture;
    }
}
