#include "graphics/TextRenderer.hpp"

#include "RIS.hpp"

#include "loader/Loader.hpp"

#include "graphics/VertexTypes.hpp"

#include <vector>

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace RIS
{
    namespace Graphics
    {
        const int TextRenderer::MAX_STRING_LEN = 1024;

        TextRenderer::TextRenderer()
            : sampler(GL_LINEAR, GL_LINEAR, 1.0f), pipeline(), vertexLayout(), vertexBuffer(MAX_STRING_LEN * 6 * sizeof VertexType::TextVertex, GL_DYNAMIC_STORAGE_BIT), viewProjectionbuffer(), worldBuffer(),
              vpb(glm::mat4(), GL_DYNAMIC_STORAGE_BIT), wb(glm::mat4(), GL_DYNAMIC_STORAGE_BIT)
        {
            vertexLayout.SetAttribFormat(0, 2, GL_FLOAT, offsetof(VertexType::TextVertex, position));
            vertexLayout.SetAttribFormat(1, 2, GL_FLOAT, offsetof(VertexType::TextVertex, texCoords));
            vertexLayout.SetAttribFormat(2, 4, GL_FLOAT, offsetof(VertexType::TextVertex, color));
            vertexLayout.SetVertexBuffer<VertexType::TextVertex>(vertexBuffer);

            auto &loader = GetLoader();

            vertexShader = loader.Load<Shader>("shaders/textVertex.glsl", ShaderType::VERTEX);
            fragmentShader = loader.Load<Shader>("shaders/textFragment.glsl", ShaderType::FRAGMENT);

            pipeline.SetShader(*vertexShader);
            pipeline.SetShader(*fragmentShader);
        }

        void TextRenderer::Begin(float viewWidth, float viewHeight)
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            sampler.Bind(0);

            vertexLayout.Bind();

            //viewProjectionbuffer.Update(glm::ortho(0.0f, viewWidth, viewHeight, 0.0f, -1.0f, 1.0f));
            //viewProjectionbuffer.Bind(0);

            vpb.UpdateData(glm::ortho(0.0f, viewWidth, viewHeight, 0.0f, -1.0f, 1.0f));
            vpb.Bind(GL_UNIFORM_BUFFER, 0);
        }

        void TextRenderer::End()
        {
            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }

        void TextRenderer::DrawString(const std::string &string, const Font &font, float size, const glm::vec2 &position, const glm::vec4 &color)
        {
            float penX = 0.0f;
            float penY = 0.0f;
            float fontSize = size;
            if(fontSize == -1)
                fontSize = font.GetSize();

            std::vector<VertexType::TextVertex> vertices;
            for(int i = 0; i < string.length(); ++i)
            {
                char c = string[i];
                if(c != ' ')
                {
                    if(font.HasGlyph(c) == 0)
                        continue;
                
                    const Glyph &glyph = font[c];
                    if(i > 0)
                    {
                        char kernChar = string[i-1];
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
                    float y = penY + glyphBearingY;
                    float w = glyphWidth;
                    float h = glyphHeight;

                    //flip the t due to opengl having y go up
                    float s0 = glyph.s0;
                    float t0 = glyph.t0;
                    float s1 = glyph.s1;
                    float t1 = glyph.t1;

                    vertices.push_back({ {x, y-h}, {s0, t1}, color });
                    vertices.push_back({ {x, y}, {s0, t0}, color });
                    vertices.push_back({ {x+w, y}, {s1, t0}, color });

                    vertices.push_back({ {x, y-h}, {s0, t1}, color });
                    vertices.push_back({ {x+w, y}, {s1, t0}, color });
                    vertices.push_back({ {x+w, y-h}, {s1, t1}, color });

                    penX += glyphAdvanceX;
                }
                else
                {
                    penX += font.GetSpaceAdvance() * size;
                }
            }

            font.GetTexture()->Bind(0);

            //worldBuffer.Update(glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)));
            //worldBuffer.Bind(1);
            wb.UpdateData(glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)));
            wb.Bind(GL_UNIFORM_BUFFER, 1);

            vertexBuffer.UpdateData(vertices);
            std::size_t numVertices = vertices.size();
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        }
    }
}
