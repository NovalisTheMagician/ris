#include "graphics/Font.hpp"

namespace RIS
{
    namespace Graphics
    {
        Font::Font(float ascender, float descender, float height, float maxAdvance, const std::string &name, float size, float spaceAdvance, const std::unordered_map<char, Glyph> glyphs, std::shared_ptr<Texture> texture)
            : ascender(ascender), descender(descender), height(height), maxAdvance(maxAdvance), name(name), size(size), spaceAdvance(spaceAdvance), glyphs(glyphs), texture(texture)
        {}

        TextMetrics Font::MeasureString(const std::string &str, float fontSize) const
        {
            float penX = 0.0f;
            float penY = 0.0f;
            if(fontSize == -1)
                fontSize = size;
            
            float height = 0;

            for(int i = 0; i < str.length(); ++i)
            {
                char c = str[i];
                if(c != ' ')
                {
                    if(glyphs.count(c) == 0)
                        continue;
                
                    const Glyph &glyph = glyphs.at(c);
                    if(i > 0)
                    {
                        char kernChar = str[i-1];
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

        bool Font::HasGlyph(char character) const
        {
            return glyphs.count(character) > 0;
        }

        const Glyph& Font::operator[](char character) const
        {
            return glyphs.at(character);
        }
    }
}
