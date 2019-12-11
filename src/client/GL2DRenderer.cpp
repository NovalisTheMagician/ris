#include "GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexTypes.hpp"

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
        perFrameBuffer.Create();
        perFrameBuffer.SetData(GL_DYNAMIC_DRAW, perFrame);

        perObjectBuffer.Create();
        perObjectBuffer.SetData(GL_DYNAMIC_DRAW, perObject);

        uiLayout.Create();
        uiLayout.SetAttribFormat(0, 2, GL_FLOAT, offsetof(UIVertex, position));
        uiLayout.SetAttribFormat(1, 2, GL_FLOAT, offsetof(UIVertex, texCoords));

        uiBuffer.Create();
        uiBuffer.Reserve(GL_DYNAMIC_DRAW, 6*sizeof(UIVertex));

        textBuffer.Create();
        textBuffer.Reserve(GL_DYNAMIC_DRAW, MAX_CHARS*6*sizeof(UIVertex));
    }

    void GL2DRenderer::SetViewsize(int width, int height)
    {
        perFrame.projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
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

        perFrameBuffer.Bind(0);
        perObjectBuffer.Bind(1);

        renderer.uiSampler.Bind(0);

        renderer.pipeline.SetShader(renderer.uiVertex, GL_VERTEX_SHADER_BIT);
        uiLayout.Bind();
    }

    void GL2DRenderer::End()
    {
        glDisable(GL_BLEND);
    }

    void GL2DRenderer::DrawText(const std::string &text)
    {
        perObjectBuffer.UpdateData(perObject);
        renderer.pipeline.SetShader(renderer.uiText, GL_FRAGMENT_SHADER_BIT);
        renderer.pipeline.Use();
        uiLayout.SetVertexBuffer(textBuffer, 0);
    }

    void GL2DRenderer::DrawQuad(int width, int height)
    {
        perObjectBuffer.UpdateData(perObject);
        renderer.pipeline.SetShader(renderer.uiFragment, GL_FRAGMENT_SHADER_BIT);
        renderer.pipeline.Use();
        uiLayout.SetVertexBuffer(uiBuffer, 0);

        /*
        { xpos,     ypos + h,   0.0, 0.0 },            
        { xpos,     ypos,       0.0, 1.0 },
        { xpos + w, ypos,       1.0, 1.0 },

        { xpos,     ypos + h,   0.0, 0.0 },
        { xpos + w, ypos,       1.0, 1.0 },
        { xpos + w, ypos + h,   1.0, 0.0 }    
        */

        std::vector<UIVertex> vertices;
        vertices.push_back({ {0, height},   {0, 0} });
        vertices.push_back({ {0, 0},        {0, 1} });
        vertices.push_back({ {width, 0},    {1, 1} });

        vertices.push_back({ {0, height},   {0, 0} });
        vertices.push_back({ {width, 0},    {1, 1} });
        vertices.push_back({ {width, height},{1, 0} });

        uiBuffer.UpdateData(vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
