#include "graphics/SpriteRenderer.hpp"

#include "RIS.hpp"

#include "loader/Loader.hpp"

#include "graphics/VertexTypes.hpp"

#include <vector>

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include <utf8.h>

namespace RIS
{
    namespace Graphics
    {
        const int SpriteRenderer::MAX_STRING_LEN = 1024;

        SpriteRenderer::SpriteRenderer()
            : sampler(GL_LINEAR, GL_LINEAR, 1.0f)
            , pipeline()
            , vertexLayout()
            , viewProjectionBuffer(sizeof glm::mat4, GL_DYNAMIC_STORAGE_BIT)
            , worldBuffer(sizeof WorldBufferData, GL_DYNAMIC_STORAGE_BIT)
            , vertexSpriteBuffer(6 * sizeof VertexType::SpriteVertex, GL_DYNAMIC_STORAGE_BIT)
            , vertexTextBuffer(6 * MAX_STRING_LEN * sizeof VertexType::SpriteVertex, GL_DYNAMIC_STORAGE_BIT)
            , white({1, 1, 1, 1})
        {
            vertexLayout.SetAttribFormat(0, 2, GL_FLOAT, offsetof(VertexType::SpriteVertex, position));
            vertexLayout.SetAttribFormat(1, 2, GL_FLOAT, offsetof(VertexType::SpriteVertex, texCoords));

            auto &loader = GetLoader();

            vertexShader = loader.Load<Shader>("shaders/spriteVertex.glsl", ShaderType::VERTEX);
            fragmentSpriteShader = loader.Load<Shader>("shaders/spriteFragment.glsl", ShaderType::FRAGMENT);
            fragmentTextShader = loader.Load<Shader>("shaders/textFragment.glsl", ShaderType::FRAGMENT);

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
            viewProjectionBuffer.Bind(GL_UNIFORM_BUFFER, 0);

            pipeline.Use();
        }

        void SpriteRenderer::End()
        {
            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }

        void SpriteRenderer::DrawTexture(Texture &texture, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tint)
        {
            pipeline.SetShader(*fragmentSpriteShader);
            vertexLayout.SetVertexBuffer<VertexType::SpriteVertex>(vertexSpriteBuffer);

            texture.Bind(0);

            glm::mat4 world(1.0f);
            world = glm::translate(world, glm::vec3(position, 0.0f));
            world = glm::scale(world, glm::vec3(size, 1.0f));

            worldBuffer.UpdateData<WorldBufferData>({world, tint});
            worldBuffer.Bind(GL_UNIFORM_BUFFER, 1);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        void SpriteRenderer::DrawRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tint)
        {
            pipeline.SetShader(*fragmentSpriteShader);
            vertexLayout.SetVertexBuffer<VertexType::SpriteVertex>(vertexSpriteBuffer);

            white.Bind(0);

            glm::mat4 world(1.0f);
            world = glm::translate(world, glm::vec3(position, 0.0f));
            world = glm::scale(world, glm::vec3(size, 1.0f));

            worldBuffer.UpdateData<WorldBufferData>({world, tint});
            worldBuffer.Bind(GL_UNIFORM_BUFFER, 1);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        void SpriteRenderer::DrawString(const std::string &str, const Font &font, float size, const glm::vec2 &position, const glm::vec4 &tint)
        {
            pipeline.SetShader(*fragmentTextShader);
            vertexLayout.SetVertexBuffer<VertexType::SpriteVertex>(vertexTextBuffer);

            float penX = 0.0f;
            float penY = 0.0f;
            float fontSize = size;
            if(fontSize == -1)
                fontSize = font.GetSize();

            std::vector<VertexType::SpriteVertex> vertices;

            bool first = true;
            for(auto &it = str.begin(); it != str.end();)
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
                        uint32_t kernChar = utf8::prior(it, str.begin());
                        utf8::advance(it, 1, str.end());
                        if(glyph.kernings.count(kernChar) > 0)
                        {
                            float kernVal = glyph.kernings.at(kernChar);
                            penX += kernVal * fontSize;
                        }
                    }

                    float glyphWidth = glyph.bboxWidth * fontSize;
                    float glyphHeight = glyph.bboxHeight * fontSize;
                    float glyphBearingX = glyph.bearingX * fontSize;
                    float glyphBearingY = glyph.bearingY * fontSize;
                    float glyphAdvanceX = glyph.advanceX * fontSize;

                    float x = penX + glyphBearingX;
                    float y = penY + (font[U'H'].bearingY - glyph.bearingY) * fontSize;
                    float w = glyphWidth;
                    float h = glyphHeight;

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
            world = glm::translate(world, glm::vec3(position, 0.0f));

            worldBuffer.UpdateData<WorldBufferData>({world, tint});
            worldBuffer.Bind(GL_UNIFORM_BUFFER, 1);

            vertexTextBuffer.UpdateData(vertices);
            std::size_t numVertices = vertices.size();
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        }
    }
}
