#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace gl46core;

namespace RIS
{
    GL2DRenderer::GL2DRenderer(GLRenderer &renderer)
        : renderer(renderer)
    {

    }

    GL2DRenderer::~GL2DRenderer()
    {

    }

    void GL2DRenderer::SetViewsize(int width, int height)
    {

    }

    void GL2DRenderer::SetPosition(const glm::vec2 &positino)
    {

    }

    void GL2DRenderer::SetTexture(int textureId, int textureUnit)
    {

    }

    void GL2DRenderer::SetColor(const glm::vec4 &color)
    {

    }

    void GL2DRenderer::Begin()
    {

    }

    void GL2DRenderer::End()
    {

    }

    void GL2DRenderer::DrawText(const std::string &text)
    {

    }

    void GL2DRenderer::DrawQuad(int width, int height)
    {
        
    }
}
