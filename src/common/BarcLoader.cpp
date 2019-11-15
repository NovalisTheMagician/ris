#include "BarcLoader.hpp"

#include <filesystem>

namespace RIS
{
    BarcLoader::BarcLoader(const SystemLocator &systems, const std::string &assetRoot)
        : systems(systems), assetRoot(assetRoot)
    {
        if(!assetRoot.empty())
            if(!std::filesystem::is_directory(assetRoot))
                throw LoaderException("AssetRoot not a directory");
    }

    BarcLoader::~BarcLoader()
    {

    }

    void BarcLoader::AddOverlay(const std::string &overlayName)
    {
        
    }

    bool BarcLoader::HasFile(AssetType type, const std::string &name) const
    {

    }

    std::unique_ptr<std::byte[]> BarcLoader::LoadFile(AssetType type, const std::string &name, bool ignoreOverlays = false) const
    {

    }
}
