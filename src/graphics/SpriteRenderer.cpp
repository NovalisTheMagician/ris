#include "graphics/SpriteRenderer.hpp"

#include "RIS.hpp"

#include "loader/Loader.hpp"

#include "graphics/VertexTypes.hpp"

#include <vector>

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include <utf8.h>

namespace RIS::Graphics
{
    SpriteRenderer::SpriteRenderer(const Loader::ResourcePack &resourcePack)
        : vertexSpriteBuffer(6 * sizeof(VertexType::SpriteVertex))
        , vertexTextBuffer(6 * MAX_STRING_LEN * sizeof(VertexType::SpriteVertex))
        , sampler(MinFilter::LINEAR, MagFilter::LINEAR)
        , pipeline()
        , vertexLayout(VertexType::SpriteVertexFormat)
        , viewProjectionBuffer(sizeof(glm::mat4))
        , worldBuffer(sizeof(WorldBufferData))
        , white({1, 1, 1, 1})
    {
        vertexShader = Loader::Load<Shader>("shaders/spriteVertex.glsl", resourcePack, ShaderType::VERTEX);
        fragmentSpriteShader = Loader::Load<Shader>("shaders/spriteFragment.glsl", resourcePack, ShaderType::FRAGMENT);
        fragmentTextShader = Loader::Load<Shader>("shaders/textFragment.glsl", resourcePack, ShaderType::FRAGMENT);

        pipeline.SetShader(*vertexShader);

        std::vector<VertexType::SpriteVertex> vertices(6);
        vertices[0] = { {0, 1}, {0, 1} };
        vertices[1] = { {1, 0}, {1, 0} };
        vertices[2] = { {0, 0}, {0, 0} };

        vertices[3] = { {0, 1}, {0, 1} };
        vertices[4] = { {1, 1}, {1, 1} };
        vertices[5] = { {1, 0}, {1, 0} };

        vertexSpriteBuffer.UpdateData(vertices);
    }

    void SpriteRenderer::Begin(float viewWidth, float viewHeight)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        sampler.Bind(0);

        vertexLayout.Bind();

        viewProjectionBuffer.UpdateData(glm::ortho(0.0f, viewWidth, viewHeight, 0.0f, -1.0f, 1.0f));
        viewProjectionBuffer.Bind(0);

        pipeline.Use();
    }

    void SpriteRenderer::End()
    {
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void SpriteRenderer::DrawTexture(const Texture &texture, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tint)
    {
        pipeline.SetShader(*fragmentSpriteShader);
        vertexLayout.SetVertexBuffer<VertexType::SpriteVertex>(vertexSpriteBuffer);

        texture.Bind(0);

        glm::mat4 world(1.0f);
        world = glm::translate(world, glm::vec3(glm::round(position), 0.0f));
        world = glm::scale(world, glm::vec3(glm::round(size), 1.0f));

        worldBuffer.UpdateData<WorldBufferData>({world, tint});
        worldBuffer.Bind(1);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void SpriteRenderer::DrawRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tint)
    {
        pipeline.SetShader(*fragmentSpriteShader);
        vertexLayout.SetVertexBuffer<VertexType::SpriteVertex>(vertexSpriteBuffer);

        white.Bind(0);

        glm::mat4 world(1.0f);
        world = glm::translate(world, glm::vec3(glm::round(position), 0.0f));
        world = glm::scale(world, glm::vec3(glm::round(size), 1.0f));

        worldBuffer.UpdateData<WorldBufferData>({world, tint});
        worldBuffer.Bind(1);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void SpriteRenderer::DrawString(const std::string_view str, const Font &font, float size, const glm::vec2 &position, const glm::vec4 &tint)
    {
        pipeline.SetShader(*fragmentTextShader);
        vertexLayout.SetVertexBuffer<VertexType::SpriteVertex>(vertexTextBuffer);

        float penX = 0.0f;
        float penY = 0.0f;
        float fontSize = size;
        if(fontSize == -1)
            fontSize = font.GetSize();

        std::vector<VertexType::SpriteVertex> vertices;
        vertices.reserve(MAX_STRING_LEN * 6);

        bool first = true;
        for(auto it = str.begin(); it != str.end();)
        {
            uint32_t c = utf8::next(it, str.end());
            if(c != U' ')
            {
                if(font.HasGlyph(c) == 0)
                    //continue;
                    c = U'?';
            
                const Glyph &glyph = font[c];
                if(!first)
                {
                    auto tmpIt = it;
                    uint32_t kernChar = utf8::prior(tmpIt, str.begin());
                    if(glyph.kernings.count(kernChar) > 0)
                    {
                        float kernVal = glyph.kernings.at(kernChar);
                        penX += kernVal * fontSize;
                    }
                }

                float glyphWidth = glyph.bboxWidth * fontSize;
                float glyphHeight = glyph.bboxHeight * fontSize;
                float glyphBearingX = glyph.bearingX * fontSize;
                //float glyphBearingY = glyph.bearingY * fontSize;
                float glyphAdvanceX = glyph.advanceX * fontSize;

                float x = std::round(penX + glyphBearingX);
                float y = std::round(penY + (font[U'H'].bearingY - glyph.bearingY) * fontSize);
                float w = std::round(glyphWidth);
                float h = std::round(glyphHeight);

                float s0 = glyph.s0;
                float t0 = glyph.t0;
                float s1 = glyph.s1;
                float t1 = glyph.t1;

                vertices.push_back({ {x, y+h}, {s0, 1-t1} });
                vertices.push_back({ {x+w, y}, {s1, 1-t0} });
                vertices.push_back({ {x, y}, {s0, 1-t0} });

                vertices.push_back({ {x, y+h}, {s0, 1-t1} });
                vertices.push_back({ {x+w, y+h}, {s1, 1-t1} });
                vertices.push_back({ {x+w, y}, {s1, 1-t0} });

                penX += glyphAdvanceX;
            }
            else
            {
                penX += font.GetSpaceAdvance() * size;
            }
        }

        font.GetTexture()->Bind(0);

        glm::mat4 world(1.0f);
        world = glm::translate(world, glm::vec3(glm::round(position), 0.0f));

        worldBuffer.UpdateData<WorldBufferData>({world, tint});
        worldBuffer.Bind(1);

        vertexTextBuffer.UpdateData(vertices);
        std::size_t numVertices = vertices.size();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(numVertices));
    }
}
