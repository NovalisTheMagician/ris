#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexTypes.hpp"

#include "Buffer.hpp"
#include "VertexArray.hpp"

using namespace gl46core;
using namespace RIS::VertexType;

namespace RIS
{
    const int GL2DRenderer::MAX_CHARS = 512;

    GL2DRenderer::GL2DRenderer(GLRenderer &renderer)
        : renderer(renderer)
    {
    }

    GL2DRenderer::~GL2DRenderer()
    {

    }

    void GL2DRenderer::Setup()
    {
        perFrameBuffer = Buffer<PerFrameBuffer>::Create(perFrame, GL_DYNAMIC_DRAW);

        perObjectBuffer = Buffer<PerObjectBuffer>::Create(perObject, GL_DYNAMIC_DRAW);

        uiLayout = VertexArray::Create();
        uiLayout.SetAttribFormat(0, 2, GL_FLOAT, offsetof(UIVertex, position));
        uiLayout.SetAttribFormat(1, 2, GL_FLOAT, offsetof(UIVertex, texCoords));

        uiBuffer = Buffer<UIVertex>::Create(6, GL_DYNAMIC_DRAW);
        textBuffer = Buffer<UIVertex>::Create(MAX_CHARS*6, GL_DYNAMIC_DRAW);
    }

    void GL2DRenderer::SetViewsize(int width, int height)
    {
        perFrame.projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
        perFrameBuffer.UpdateData(perFrame);
    }

    void GL2DRenderer::SetPosition(const glm::vec2 &position)
    {
        perObject.world = glm::translate(glm::mat4(), glm::vec3(position, 0));
    }

    void GL2DRenderer::SetTexture(int textureId, int textureUnit)
    {
        if(renderer.textures.count(textureId) != 1)
            return;
        auto &texture = renderer.textures.at(textureId);
        texture.Bind(textureUnit);
    }

    void GL2DRenderer::SetColor(const glm::vec4 &color)
    {
        perObject.color = color;
    }

    void GL2DRenderer::Begin()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        perFrameBuffer.Bind(GL_UNIFORM_BUFFER, 0);
        perObjectBuffer.Bind(GL_UNIFORM_BUFFER, 1);

        renderer.uiSampler.Bind(0);

        renderer.pipeline.SetShader(renderer.uiVertex);
        uiLayout.Bind();
    }

    void GL2DRenderer::End()
    {
        glDisable(GL_BLEND);
    }

    void GL2DRenderer::DrawText(const std::string &text)
    {
        perObjectBuffer.UpdateData(perObject);
        renderer.pipeline.SetShader(renderer.uiText);
        renderer.pipeline.Use();
        uiLayout.SetVertexBuffer(textBuffer, 0);
    }

    void GL2DRenderer::DrawQuad(int width, int height)
    {
        std::vector<UIVertex> vertices;
        vertices.push_back({ {0, 0.5f},   {0, 0} });
        vertices.push_back({ {0, 0},        {0, 1} });
        vertices.push_back({ {0.5f, 0},    {1, 1} });

        vertices.push_back({ {0, 0.5f},   {0, 0} });
        vertices.push_back({ {0.5f, 0},    {1, 1} });
        vertices.push_back({ {0.5f, 0.5f}, {1, 0} });

        uiBuffer.UpdateData(vertices);

        perObjectBuffer.UpdateData(perObject);
        renderer.pipeline.SetShader(renderer.uiFragment);
        renderer.pipeline.Use();
        uiLayout.SetVertexBuffer(uiBuffer, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
