#include "loader/FontLoader.hpp"

#include "graphics/Font.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <utf8.h>

#include "misc/Logger.hpp"

namespace RIS::Loader
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

            auto charcode = glyphJson["charcode"].GetString();
            auto begin = charcode;
            auto end = charcode + std::strlen(charcode);
            glyph.charCode = utf8::next(begin, end);

            const rapidjson::Value &kerningsJson = glyphJson["kernings"];
            for(auto kItr = kerningsJson.MemberBegin(); kItr != kerningsJson.MemberEnd(); ++kItr)
            {
                auto char2 = kItr->name.GetString();
                begin = char2;
                end = char2 + std::strlen(char2);
                uint32_t c = utf8::next(begin, end);
                float val = kItr->value.GetFloat();
                glyph.kernings.insert({ c, val });
            }

            glyphs.insert({ glyph.charCode, glyph });
        }

        return std::make_shared<Graphics::Font>(ascender, descender, height, maxAdvance, fontName, size, spaceAdvance, glyphs, fontTexture);
    }
}
