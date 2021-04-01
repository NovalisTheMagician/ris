#include "loader/TextLoader.hpp"

namespace RIS::Loader
{
    template<>
    std::shared_ptr<std::string> Load(const std::vector<std::byte> &bytes, const std::string &name, std::any param, const ResourcePack &resourcePack)
    {
        if(bytes.size() == 0)
            return nullptr;
        return std::make_shared<std::string>(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }
}
