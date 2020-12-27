#include "graphics/Image.hpp"

#include "graphics/Texture.hpp"

#include <vector>

namespace RIS::Graphics
{
    Image::Image(const std::vector<std::byte> &data)
    {
        texture = gli::load(reinterpret_cast<const char*>(data.data()), data.size());
        if(texture.empty() || texture.format() != gli::format::FORMAT_RGBA8_UNORM_PACK8)
            return; // throw here

        width = texture.extent().x;
        height = texture.extent().y;

        pixels = static_cast<std::uint32_t*>(texture.data(0, 0, 0));
    }

    int Image::GetWidth() const
    {
        return width;
    }

    int Image::GetHeight() const
    {
        return height;
    }

    std::uint32_t Image::GetPixel(int x, int y) const
    {
        //check range
        return pixels[y * width + x];
    }

    unsigned char* Image::GetPixels()
    {
        return reinterpret_cast<unsigned char*>(pixels);
    }

    std::shared_ptr<Texture> Image::MakeTexture() const
    {
        return std::make_shared<Texture>(reinterpret_cast<const std::byte*>(pixels), width, height);
    }
}
