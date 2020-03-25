#include "ZipLoader.hpp"

#include <filesystem>
#include <algorithm>
#include <fstream>

namespace RIS
{
    ZipLoader::ZipLoader(const SystemLocator &systems, Config &config, const std::string &assetRoot)
        : systems(systems), config(config), assetRoot(assetRoot)
    {
        /*
        if(!assetRoot.empty())
            if(!std::filesystem::is_directory(assetRoot))
                throw ZipLoaderException("AssetRoot (" + assetRoot + ") is not a directory");
        */
    }

    ZipLoader::~ZipLoader()
    {

    }

    void ZipLoader::AddOverlay(const std::string &overlayName)
    {
        archives.emplace_back(overlayName);
    }

    bool ZipLoader::HasAsset(AssetType type, const std::string &name) const
    {
        for(auto it = archives.rbegin(); it != archives.rend(); ++it)
        {
            auto &archive = *it;
            if(archive.exists(name))
                return true;
        }
        return false;
    }

    std::tuple<std::filesystem::path, std::string> ZipLoader::GetAssetProps(AssetType type) const
    {
        std::string extension = "";
        std::filesystem::path folder = "";
        switch(type)
        {
        case AssetType::TEXTURE:    extension = ".dds";     folder = "textures"; break;
        case AssetType::MODEL:      extension = ".mdl";     folder = "models"; break;
        case AssetType::SOUND:      extension = ".wav";     folder = "sounds"; break;
        case AssetType::SHADER:     extension = ".spv";     folder = "shaders"; break;
        case AssetType::SHADERSRC:  extension = ".amd.src"; folder = "shaders"; break;
        case AssetType::TEXT:       extension = ".txt";     folder = "texts"; break;
        case AssetType::UILAYOUT:   extension = ".json";    folder = "ui"; break;
        case AssetType::FONT:       extension = ".json";    folder = "ui"; break;
        }
        return { folder, extension };
    }

    std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> ZipLoader::LoadAsset(AssetType type, const std::string &name, bool ignoreOverlays)
    {
        if(!assetRoot.empty())
        {
            try
            {
                return LoadAssetFromFilesystem(type, name);
            }
            catch(const std::runtime_error &e)
            {

            }
        }

        //if(!HasAsset(type, name)) throw LoaderException("Asset (" + name + ") not found");
        const auto& [folder, extension] = GetAssetProps(type);
        std::filesystem::path filePath = folder / (name + extension);

        return std::async(std::launch::async, [filePath, name, this]()
        {
            for(auto it = archives.rbegin(); it != archives.rend(); ++it)
            {
                auto &archive = *it;
                try
                {
                    int64_t fileIndex = archive.find(filePath.generic_string());
                    libzip::stat fileStat = archive.stat(fileIndex);
                    std::size_t size = static_cast<std::size_t>(fileStat.size);
                    
                    std::unique_ptr<std::byte[]> data(new std::byte[size]);

                    libzip::file file = archive.open(fileIndex);
                    if(file.read(data.get(), fileStat.size) == -1)
                        throw ZipLoaderException("Asset (" + name + ") read error");
                    return std::make_tuple(std::move(data), size);
                }
                catch(const std::runtime_error &e)
                {}
            }
            throw ZipLoaderException("Asset (" + name + ") not found");
        });
    }

    std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> ZipLoader::LoadAssetFromFilesystem(AssetType type, const std::string &name)
    {
        const auto& [folder, extension] = GetAssetProps(type);
        std::filesystem::path filePath = assetRoot / folder / (name + extension);

        if(!std::filesystem::exists(filePath))
            throw ZipLoaderException("Asset (" + filePath.generic_string() + ") not found");

        return std::async(std::launch::async, [filePath](){
            std::fstream stream(filePath, std::fstream::in | std::fstream::binary | std::fstream::ate);

            std::size_t size = stream.tellg();
            stream.seekg(0);

            std::unique_ptr<std::byte[]> data(new std::byte[size]);
            stream.read(reinterpret_cast<char*>(data.get()), size);

            return std::make_tuple(std::move(data), size);
        });
    }
}
