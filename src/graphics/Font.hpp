#pragma once

#include <string>
#include <unordered_map>
#include <tuple>

#include "graphics/Texture.hpp"

#include <memory>

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
            char charCode;
            float s0, s1, t0, t1;
            std::unordered_map<char, float> kernings;
        };

        class Font
        {
        public:
            Font(float ascender, float descender, float height, float maxAdvance, const std::string &name, float size, float spaceAdvance, const std::unordered_map<char, Glyph> glyphs, std::shared_ptr<Texture> texture);
            ~Font() = default;
            Font(const Font&) = default;
            Font& operator=(const Font&) = default;
            Font(Font&&) = default;
            Font& operator=(Font&&) = default;

            TextMetrics MeasureString(const std::string &str, float fontSize = -1) const;

            std::shared_ptr<Texture> GetTexture() const;

            float GetSize() const;
            float GetMaxAdvance() const;
            float GetSpaceAdvance() const;
            float GetMaxHeight(float size) const;

            size_t NumGlyphs() const;
            bool HasGlyph(char character) const;
            const Glyph& operator[](char character) const;

        private:
            float ascender, descender;
            float height;
            float maxAdvance;
            std::string name;
            float size;
            float spaceAdvance;
            std::unordered_map<char, Glyph> glyphs;

            std::shared_ptr<Texture> texture;
        };
    }
}
