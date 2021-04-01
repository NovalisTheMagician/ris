#include "loader/ImageLoader.hpp"

#include "graphics/Image.hpp"

namespace RIS::Loader
{
    template<>
    std::shared_ptr<Graphics::Image> Load<Graphics::Image>(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        if(bytes.size() == 0)
            return nullptr;
        return std::make_shared<Graphics::Image>(bytes);
    }
}
