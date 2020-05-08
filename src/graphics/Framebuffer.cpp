#include "graphics/Framebuffer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    namespace Graphics
    {
        Framebuffer::Framebuffer()
            : GLObject(0)
        {
        }

        Framebuffer::Framebuffer(GLuint frambufId)
            : GLObject(frambufId), colorFormat(GL_SRGB8_ALPHA8)
        {
        }

        Framebuffer::Framebuffer(int width, int height, GLenum colorFormat, bool useDepth)
            : useDepth(useDepth), colorFormat(colorFormat)
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
            useDepth = other.useDepth;
            colorFormat = other.colorFormat;
            other.id = 0;
        }

        Framebuffer& Framebuffer::operator=(Framebuffer &&other)
        {
            std::swap(id, other.id);
            colorTexture = std::move(other.colorTexture);
            depthTexture = std::move(other.depthTexture);
            useDepth = other.useDepth;
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
            if(useDepth)
            {
                depthTexture = Texture(GL_DEPTH_COMPONENT32, width, height);
                glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, depthTexture.GetId(), 0);
            }
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
}
