#include "graphics/Image.hpp"

namespace RIS
{
    namespace Graphics
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
    }

    namespace Loader
    {
        template<>
        std::shared_ptr<Graphics::Image> Load<Graphics::Image>(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
        {
            return std::make_shared<Graphics::Image>(bytes);
        }
    }
}
