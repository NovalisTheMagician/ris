#include "loader/Loader.hpp"

#include <filesystem>
#include <algorithm>
#include <fstream>

#include "misc/Logger.hpp"

namespace RIS
{
    namespace Loader
    {
        Loader::Loader(const std::string &assetRoot)
            : assetRoot(assetRoot)
        {
            /*
            if(!assetRoot.empty())
                if(!std::filesystem::is_directory(assetRoot))
                    throw LoaderException("AssetRoot (" + assetRoot + ") is not a directory");
            */
        }

        void Loader::AddOverlay(const std::string &overlayName)
        {
            if(!overlayName.empty())
            {
                try
                {
                    archives.emplace_back(overlayName);
                }
                catch(const std::exception &e)
                {
                    throw LoaderException("Couldn't open " + overlayName);
                }
            }
        }

        bool Loader::ContainsAsset(const std::string &name) const
        {
            for(auto it = archives.rbegin(); it != archives.rend(); ++it)
            {
                auto &archive = *it;
                if(archive.exists(name))
                    return true;
            }
            return false;
        }

        void Loader::PostInit()
        {
            RegisterLoadFunction<std::string>([](const std::vector<std::byte> &data, const std::string &name, bool cache, std::any param)
            {
                return std::make_shared<std::string>(reinterpret_cast<const char*>(data.data()), data.size());
            }, []()
            {
                return std::make_shared<std::string>();
            });
        }

        std::vector<std::byte> Loader::LoadBytes(const std::string &name)
        {
            if(!assetRoot.empty())
            {
                try
                {
                    return LoadBytesFromFilesystem(name);
                }
                catch(const std::runtime_error &e)
                {

                }
            }

            std::filesystem::path filePath = name;
            for(auto it = archives.rbegin(); it != archives.rend(); ++it)
            {
                auto &archive = *it;
                try
                {
                    int64_t fileIndex = archive.find(filePath.generic_string(), ZIP_FL_NOCASE);
                    libzip::stat fileStat = archive.stat(fileIndex);
                    std::size_t size = static_cast<std::size_t>(fileStat.size);
                    
                    std::vector<std::byte> bytes(size);

                    libzip::file file = archive.open(fileIndex);
                    if(file.read(bytes.data(), fileStat.size) == -1)
                        throw LoaderException("Asset (" + name + ") read error");
                    return bytes;
                }
                catch(const std::runtime_error &e)
                {
                    Logger::Instance().Error(e.what());
                }
            }
            throw LoaderException("Asset (" + name + ") not found");
        }

        std::vector<std::byte> Loader::LoadBytesFromFilesystem(const std::string &name)
        {
            std::filesystem::path filePath  = assetRoot;
            filePath /= name;

            if(!std::filesystem::exists(filePath))
                throw LoaderException("Asset (" + filePath.generic_string() + ") not found");

            std::fstream stream(filePath, std::fstream::in | std::fstream::binary | std::fstream::ate);

            std::size_t size = stream.tellg();
            stream.seekg(0);

            std::vector<std::byte> bytes(size);

            stream.read(reinterpret_cast<char*>(bytes.data()), size);

            return bytes;
        }
    }
}
