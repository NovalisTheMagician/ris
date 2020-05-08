#if 0
#include "graphics/GLRenderer.hpp"

#include <glbinding/gl46core/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/VertexTypes.hpp"

#include "misc/Logger.hpp"

#include "graphics/Buffer.hpp"
#include "graphics/VertexArray.hpp"

#include "graphics/Texture.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include "RIS.hpp"

#undef DrawText

using namespace gl46core;
using namespace RIS::VertexType;

using namespace std::literals;

using std::string;

namespace RIS
{
    const int GL2DRenderer::MAX_CHARS = 512;

    GL2DRenderer::GL2DRenderer(GLRenderer &renderer)
        : renderer(renderer), highestUnusedFontId(1)
    {
    }

    void GL2DRenderer::Setup()
    {
        perFrameBuffer = Buffer::Create(perFrame, GL_DYNAMIC_STORAGE_BIT);

        perObjectBuffer = Buffer::Create(perObject, GL_DYNAMIC_STORAGE_BIT);

        uiLayout = VertexArray::Create();
        uiLayout.SetAttribFormat(0, 2, GL_FLOAT, offsetof(UIVertex, position));
        uiLayout.SetAttribFormat(1, 2, GL_FLOAT, offsetof(UIVertex, texCoords));

        std::vector<UIVertex> vertices(6);
        vertices[0] = { {0, 1}, {0, 1} };
        vertices[1] = { {0, 0}, {0, 0} };
        vertices[2] = { {1, 0}, {1, 0} };

        vertices[3] = { {0, 1}, {0, 1} };
        vertices[4] = { {1, 0}, {1, 0} };
        vertices[5] = { {1, 1}, {1, 1} };

        uiBuffer = Buffer::Create(vertices, GL_DYNAMIC_STORAGE_BIT);
        textBuffer = Buffer::Create(MAX_CHARS*6 * sizeof UIVertex, GL_DYNAMIC_STORAGE_BIT);

        uiSampler = Sampler::Create(GL_LINEAR, GL_LINEAR, 1.0f);
    }

    void GL2DRenderer::LoadShaders()
    {
        auto createShader = [this](auto &future, gl::GLenum shaderType)
        {
            auto [data, size] = future.get();
            return Shader::Create(data.get(), size, shaderType, renderer.useAmdFix);
        };

        auto &loader = GetSystems().GetLoader();

        AssetType shaderLoadType = renderer.useAmdFix ? AssetType::SHADERSRC : AssetType::SHADER;

        auto shaderUivFut = loader.LoadAsset(shaderLoadType, "uiVert");
        auto shaderUifFut = loader.LoadAsset(shaderLoadType, "uiFrag");
        auto shaderUitFut = loader.LoadAsset(shaderLoadType, "uiText");

        uiVertex = createShader(shaderUivFut, GL_VERTEX_SHADER);
        uiFragment = createShader(shaderUifFut, GL_FRAGMENT_SHADER);
        uiText = createShader(shaderUitFut, GL_FRAGMENT_SHADER);
    }

    ResourceId GL2DRenderer::LoadFont(const std::string &fontName)
    {
        if(loadedFonts.count(fontName) == 1)
            return loadedFonts.at(fontName);

        auto &loader = GetSystems().GetLoader();

        auto [data, size] = loader.LoadAsset(AssetType::FONT, fontName).get();
        std::string fontStr(reinterpret_cast<const char*>(data.get()), size);

        rapidjson::Document fontJson;
        rapidjson::ParseResult res = fontJson.Parse(fontStr.c_str()); 
        if(res.IsError())
        {
            string errorMsg = rapidjson::GetParseError_En(res.Code());
            Logger::Instance().Error("Failed to parse font ("s + fontName + "): "s + errorMsg + "(" + std::to_string(res.Offset()) + ")");
            return { 0 };
        }

        Font font = { 0 };
        font.ascender = fontJson["ascender"].GetFloat();
        font.bitmapWidth = fontJson["bitmap_width"].GetInt();
        font.bitmapHeight = fontJson["bitmap_height"].GetInt();
        font.descender = fontJson["descender"].GetFloat();
        font.height = fontJson["height"].GetFloat();
        font.maxAdvance = fontJson["max_advance"].GetFloat();
        font.name = fontJson["name"].GetString();
        font.size = fontJson["size"].GetFloat();
        font.spaceAdvance = fontJson["space_advance"].GetFloat();
        font.textureId = renderer.LoadTexture(fontName, false);

        const rapidjson::Value &glyphData = fontJson["glyph_data"];
        for (auto itr = glyphData.MemberBegin(); itr != glyphData.MemberEnd(); ++itr)
        {
            const rapidjson::Value &glyphJson = itr->value;
            Glyph glyph = { 0 };
            glyph.advanceX = glyphJson["advance_x"].GetFloat();
            glyph.bboxHeight = glyphJson["bbox_height"].GetFloat();
            glyph.bboxWidth = glyphJson["bbox_width"].GetFloat();
            glyph.bearingX = glyphJson["bearing_x"].GetFloat();
            glyph.bearingY = glyphJson["bearing_y"].GetFloat();
            glyph.charCode = glyphJson["charcode"].GetString()[0];
            glyph.s0 = glyphJson["s0"].GetFloat();
            glyph.t0 = glyphJson["t0"].GetFloat();
            glyph.s1 = glyphJson["s1"].GetFloat();
            glyph.t1 = glyphJson["t1"].GetFloat();

            const rapidjson::Value &kerningsJson = glyphJson["kernings"];
            for(auto kItr = kerningsJson.MemberBegin(); kItr != kerningsJson.MemberEnd(); ++kItr)
            {
                char c = kItr->name.GetString()[0];
                float val = kItr->value.GetFloat();
                glyph.kernings.insert({ c, val });
            }

            font.glyphs.insert({ glyph.charCode, glyph });
        }

        int id = highestUnusedFontId++;
        fonts[id] = font;
        loadedFonts.insert({fontName, id});

        return id;
    }

    void GL2DRenderer::DestroyFont(ResourceId fontId)
    {
        if(fonts.count(fontId) > 0)
        {
            fonts.erase(fontId);
            for(auto it = loadedFonts.begin(); it != loadedFonts.end(); ++it)
            {
                if(it->second == fontId)
                {
                    loadedFonts.erase(it);
                    break;   
                }
            }
        }
    }

    void GL2DRenderer::SetViewsize(int width, int height)
    {
        perFrame.viewSize = glm::vec2(static_cast<float>(width), static_cast<float>(height));
        perFrameBuffer.UpdateData(perFrame);
    }

    void GL2DRenderer::SetTexture(ResourceId textureId, int textureUnit)
    {
        if(renderer.textures.count(textureId) != 1)
            return;
        auto &texture = renderer.textures.at(textureId);
        texture.Bind(textureUnit);
    }

    void GL2DRenderer::Begin()
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        perFrameBuffer.Bind(GL_UNIFORM_BUFFER, 0);
        perObjectBuffer.Bind(GL_UNIFORM_BUFFER, 1);

        uiSampler.Bind(0);

        renderer.pipeline.SetShader(uiVertex);
        uiLayout.Bind();
    }

    void GL2DRenderer::End()
    {
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void GL2DRenderer::DrawText(const std::string &text, ResourceId fontId, const glm::vec2 &position, float size, const glm::vec4 &color)
    {
        const Font &font = fonts.at(fontId);

        perObject.position = position;
        perObject.color = color;
        perObject.size = glm::vec2(1);
        perObjectBuffer.UpdateData(perObject);

        renderer.pipeline.SetShader(uiText);
        renderer.pipeline.Use();

        float penX = 0.0f;
        float penY = 0.0f;
        float fontSize = size;
        if(fontSize == -1)
            fontSize = font.size;

        std::vector<UIVertex> vertices;
        for(int i = 0; i < text.length(); ++i)
        {
            char c = text[i];
            if(c != ' ')
            {
                if(font.glyphs.count(c) == 0)
                    continue;
            
                const Glyph &glyph = font.glyphs.at(c);
                if(i > 0)
                {
                    char kernChar = text[i-1];
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
                penX += font.spaceAdvance * size;
            }
        }
        textBuffer.UpdateData(vertices);
        renderer.textures.at(font.textureId).Bind(0);
        uiLayout.SetVertexBuffer<UIVertex>(textBuffer);

        std::size_t numVertices = vertices.size();

        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }

    void GL2DRenderer::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        perObject.position = position;
        perObject.color = color;
        perObject.size = size;
        perObjectBuffer.UpdateData(perObject);
        perObjectBuffer.Bind(GL_UNIFORM_BUFFER, 1);
        uiLayout.SetVertexBuffer<UIVertex>(uiBuffer);
        renderer.pipeline.SetShader(uiFragment);
        renderer.pipeline.Use();

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    TextMetrics GL2DRenderer::MeasureText(const std::string &text, ResourceId fontId, float size)
    {
        const Font &font = fonts.at(fontId);

        float penX = 0.0f;
        float penY = 0.0f;
        float fontSize = size;
        if(fontSize == -1)
            fontSize = font.size;
        
        float height = 0;

        for(int i = 0; i < text.length(); ++i)
        {
            char c = text[i];
            if(c != ' ')
            {
                if(font.glyphs.count(c) == 0)
                    continue;
            
                const Glyph &glyph = font.glyphs.at(c);
                if(i > 0)
                {
                    char kernChar = text[i-1];
                    if(glyph.kernings.count(kernChar) > 0)
                    {
                        float kernVal = glyph.kernings.at(kernChar);
                        penX += kernVal * fontSize;
                    }
                }

                float glyphHeight = glyph.bboxHeight * fontSize;
                float glyphAdvanceX = glyph.advanceX * fontSize;

                float h = glyphHeight;

                penX += glyphAdvanceX;

                if(height < h)
                    height = h;
            }
            else
            {
                penX += font.spaceAdvance * size;
            }
        }

        return { penX, height };
    }

    float GL2DRenderer::MaxHeightFont(ResourceId fontId, float size)
    {
        const Font &font = fonts.at(fontId);
        return font.height * size;
    }
}
#endif
