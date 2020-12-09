#include "graphics/Font.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <utf8.h>

#include "misc/Logger.hpp"

namespace RIS
{
    namespace Graphics
    {
        Font::Font(float ascender, float descender, float height, float maxAdvance, const std::string &name, float size, float spaceAdvance, const std::unordered_map<uint32_t, Glyph> glyphs, std::shared_ptr<Texture> texture)
            : ascender(ascender), descender(descender), height(height), maxAdvance(maxAdvance), name(name), size(size), spaceAdvance(spaceAdvance), glyphs(glyphs), texture(texture)
        {}

        TextMetrics Font::MeasureString(const std::string_view str, float fontSize) const
        {
            float penX = 0.0f;
            float penY = 0.0f;
            if(fontSize == -1)
                fontSize = size;
            
            float height = 0;

            bool first = true;
            for(auto &it = str.begin(); it != str.end();)
            {
                uint32_t c = utf8::next(it, str.end());
                if(c != U' ')
                {
                    if(glyphs.count(c) == 0)
                        continue;
                
                    const Glyph &glyph = glyphs.at(c);
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

                    float glyphHeight = glyph.bboxHeight * fontSize;
                    float glyphAdvanceX = glyph.advanceX * fontSize;

                    float h = glyphHeight;

                    penX += glyphAdvanceX;

                    if(height < h)
                        height = h;
                }
                else
                {
                    penX += spaceAdvance * fontSize;
                }
                first = false;
            }
            return { penX, height };
        }

        std::shared_ptr<Texture> Font::GetTexture() const
        {
            return texture;
        }

        float Font::GetSize() const
        {
            return size;
        }

        float Font::GetMaxAdvance() const
        {
            return maxAdvance;
        }

        float Font::GetSpaceAdvance() const
        {
            return spaceAdvance;
        }

        float Font::GetMaxHeight(float size) const
        {
            return height * size;
        }

        size_t Font::NumGlyphs() const
        {
            return glyphs.size();
        }

        bool Font::HasGlyph(uint32_t character) const
        {
            return glyphs.count(character) > 0;
        }

        const Glyph& Font::operator[](uint32_t character) const
        {
            return glyphs.at(character);
        }
    }

    namespace Loader
    {
        template<>
        std::shared_ptr<Graphics::Font> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
        {
            using namespace std::literals::string_literals;

            std::string fontStr(reinterpret_cast<const char*>(bytes.data()), bytes.size());

            rapidjson::Document fontJson;
            rapidjson::ParseResult res = fontJson.Parse(fontStr.c_str()); 
            if(res.IsError())
            {
                std::string errorMsg = rapidjson::GetParseError_En(res.Code());
                Logger::Instance().Error("Failed to parse font ("s + name + "): "s + errorMsg + "(" + std::to_string(res.Offset()) + ")");
                return nullptr;
            }

            float ascender = fontJson["ascender"].GetFloat();
            float bitmapWidth = fontJson["bitmap_width"].GetInt();
            float bitmapHeight = fontJson["bitmap_height"].GetInt();
            float descender = fontJson["descender"].GetFloat();
            float height = fontJson["height"].GetFloat();
            float maxAdvance = fontJson["max_advance"].GetFloat();
            std::string fontName = fontJson["name"].GetString();
            float size = fontJson["size"].GetFloat();
            float spaceAdvance = fontJson["space_advance"].GetFloat();

            std::string fontTexturePath = "fonts/" + fontName + ".dds";
            auto fontTexture = Load<Graphics::Texture>(resourcePack.Read(fontTexturePath), fontTexturePath, {}, resourcePack);

            std::unordered_map<uint32_t, Graphics::Glyph> glyphs;

            const rapidjson::Value &glyphData = fontJson["glyph_data"];
            for (auto itr = glyphData.MemberBegin(); itr != glyphData.MemberEnd(); ++itr)
            {
                const rapidjson::Value &glyphJson = itr->value;
                Graphics::Glyph glyph = { 0 };
                glyph.advanceX = glyphJson["advance_x"].GetFloat();
                glyph.bboxHeight = glyphJson["bbox_height"].GetFloat();
                glyph.bboxWidth = glyphJson["bbox_width"].GetFloat();
                glyph.bearingX = glyphJson["bearing_x"].GetFloat();
                glyph.bearingY = glyphJson["bearing_y"].GetFloat();
                glyph.s0 = glyphJson["s0"].GetFloat();
                glyph.t0 = glyphJson["t0"].GetFloat();
                glyph.s1 = glyphJson["s1"].GetFloat();
                glyph.t1 = glyphJson["t1"].GetFloat();

                auto c = glyphJson["charcode"].GetString();
                auto cc = std::string(c);
                glyph.charCode = utf8::next(cc.begin(), cc.end());

                const rapidjson::Value &kerningsJson = glyphJson["kernings"];
                for(auto kItr = kerningsJson.MemberBegin(); kItr != kerningsJson.MemberEnd(); ++kItr)
                {
                    auto cc = std::string(kItr->name.GetString());
                    uint32_t c = utf8::next(cc.begin(), cc.end());
                    float val = kItr->value.GetFloat();
                    glyph.kernings.insert({ c, val });
                }

                glyphs.insert({ glyph.charCode, glyph });
            }

            return std::make_shared<Graphics::Font>(ascender, descender, height, maxAdvance, fontName, size, spaceAdvance, glyphs, fontTexture);
        }
    }
}
