#include "graphics/TextRenderer.hpp"

#include "graphics/VertexTypes.hpp"

#include <vector>

#include <glbinding/gl46core/gl.h>
using namespace gl46core;

#include "RIS.hpp"

namespace RIS
{
    namespace Graphics
    {
        const int TextRenderer::MAX_STRING_LEN = 1024;

        TextRenderer::TextRenderer()
            : sampler(GL_LINEAR, GL_LINEAR, 1.0f), pipeline(), vertexLayout(), vertexBuffer(MAX_STRING_LEN * 6 * sizeof VertexType::UIVertex, GL_DYNAMIC_STORAGE_BIT)
        {
            vertexLayout.SetAttribFormat(0, 2, GL_FLOAT, offsetof(VertexType::UIVertex, position));
            vertexLayout.SetAttribFormat(1, 2, GL_FLOAT, offsetof(VertexType::UIVertex, texCoords));

            auto &loader = GetSystems().GetLoader();

            vertexShader = loader.Load<Shader>("shaders/textVertex.spv", ShaderType::VERTEX);
            fragmentShader = loader.Load<Shader>("shaders/textFragment.spv", ShaderType::FRAGMENT);

            pipeline.SetShader(*vertexShader.get());
            pipeline.SetShader(*fragmentShader.get());
        }

        void TextRenderer::DrawString(const std::string &string, const Font &font, float size, const glm::vec2 &position)
        {
            float penX = 0.0f;
            float penY = 0.0f;
            float fontSize = size;
            if(fontSize == -1)
                fontSize = font.GetSize();

            std::vector<VertexType::UIVertex> vertices;
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

                    vertices.push_back({ {x, y-h}, {s0, t1} });
                    vertices.push_back({ {x, y}, {s0, t0} });
                    vertices.push_back({ {x+w, y}, {s1, t0} });

                    vertices.push_back({ {x, y-h}, {s0, t1} });
                    vertices.push_back({ {x+w, y}, {s1, t0} });
                    vertices.push_back({ {x+w, y-h}, {s1, t1} });

                    penX += glyphAdvanceX;
                }
                else
                {
                    penX += font.GetSpaceAdvance() * size;
                }
            }

            vertexBuffer.UpdateData(vertices);
        }
    }
}
