#include "loader/TextureLoader.hpp"

#include "graphics/Texture.hpp"

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Texture> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        if(bytes.size() == 0)
            return nullptr;
        
        if(bool *flip = std::any_cast<bool>(&param))
            return std::make_shared<Graphics::Texture>(bytes, flip);
        return std::make_shared<Graphics::Texture>(bytes);
    }
}
