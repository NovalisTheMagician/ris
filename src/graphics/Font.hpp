#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "graphics/Texture.hpp"
#include "loader/LoadFunc.hpp"

namespace RIS
{
    namespace Graphics
    {
        struct TextMetrics
        {
            float width;
            float height;
        };

        struct Glyph
        {
            float advanceX;
            float bboxWidth, bboxHeight;
            float bearingX, bearingY;
            uint32_t charCode;
            float s0, s1, t0, t1;
            std::unordered_map<uint32_t, float> kernings;
        };

        class Font
        {
        public:
            using Ptr = std::shared_ptr<Font>;

            Font(float ascender, float descender, float height, float maxAdvance, const std::string &name, float size, float spaceAdvance, const std::unordered_map<uint32_t, Glyph> glyphs, std::shared_ptr<Texture> texture);
            ~Font() = default;
            Font(const Font&) = default;
            Font& operator=(const Font&) = default;
            Font(Font&&) = default;
            Font& operator=(Font&&) = default;

            TextMetrics MeasureString(const std::string_view str, float fontSize = -1) const;

            std::shared_ptr<Texture> GetTexture() const;

            float GetSize() const;
            float GetMaxAdvance() const;
            float GetSpaceAdvance() const;
            float GetMaxHeight(float size) const;

            size_t NumGlyphs() const;
            bool HasGlyph(uint32_t character) const;
            const Glyph& operator[](uint32_t character) const;

        private:
            float ascender, descender;
            float height;
            float maxAdvance;
            std::string name;
            float size;
            float spaceAdvance;
            std::unordered_map<uint32_t, Glyph> glyphs;

            std::shared_ptr<Texture> texture;
        };
    }

    namespace Loader
    {
        template<>
        std::shared_ptr<Graphics::Font> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack);
    }
}
