#pragma once

#include <vector>
#include <cstdint>

#include <gli/gli.hpp>

namespace RIS
{
    namespace Graphics
    {
        class Image
        {
        public:
            Image(const std::vector<std::byte> &data);

            int GetWidth() const;
            int GetHeight() const;
            std::uint32_t GetPixel(int x, int y) const;

            unsigned char* GetPixels();

        private:
            int width, height;
            std::uint32_t *pixels;
            gli::texture texture;

        };
    }
}