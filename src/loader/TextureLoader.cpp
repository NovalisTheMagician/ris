#include "loader/TextureLoader.hpp"

#include "graphics/Texture.hpp"

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Texture> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        return std::make_shared<Graphics::Texture>(bytes);
    }
}
