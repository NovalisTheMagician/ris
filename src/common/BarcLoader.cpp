#include "BarcLoader.hpp"

#include <filesystem>

#include <utility>

#include "ArchiveFormat.hpp"

namespace RIS
{
    BarcLoader::BarcLoader(const SystemLocator &systems, Config &config, const std::string &assetRoot)
        : systems(systems), config(config), assetRoot(assetRoot), archiveOverlays(), assetTable()
    {
        if(!assetRoot.empty())
            if(!std::filesystem::is_directory(assetRoot))
                throw LoaderException("AssetRoot (" + assetRoot + ") is not a directory");
    }

    BarcLoader::~BarcLoader()
    {

    }

    void BarcLoader::BuildTables(std::fstream &stream, int streamId)
    {
        Header header = { 0 };
        TOC toc = { 0 };

        stream.read(reinterpret_cast<char*>(&header), sizeof header);

        if(std::memcmp(header.magic, "MEOW", 4) != 0)
            throw LoaderException("Unsupported archive type");

        if(header.version != ARCHIVE_FORMAT_VERSION)
            throw LoaderException("Unsupported archive version");

        stream.read(reinterpret_cast<char*>(&toc), sizeof toc);

        //TODO implement the other asset types

        {
            std::vector<ArchiveTableEntry> entries(toc.numModels);
            stream.seekg(toc.modelOffset);
            stream.read(reinterpret_cast<char*>(&entries[0]), toc.numModels * sizeof entries[0]);
            PopulateTable(AssetType::MODEL, entries, stream, streamId);
        }
        
        {
            std::vector<ArchiveTableEntry> entries(toc.numTextures);
            stream.seekg(toc.textureOffset);
            stream.read(reinterpret_cast<char*>(&entries[0]), toc.numTextures * sizeof entries[0]);
            PopulateTable(AssetType::TEXTURE, entries, stream, streamId);
        }

        {
            std::vector<ArchiveTableEntry> entries(toc.numSounds);
            stream.seekg(toc.soundOffset);
            stream.read(reinterpret_cast<char*>(&entries[0]), toc.numSounds * sizeof entries[0]);
            PopulateTable(AssetType::SOUND, entries, stream, streamId);
        }

        {
            std::vector<ArchiveTableEntry> entries(toc.numTexts);
            stream.seekg(toc.textOffset);
            stream.read(reinterpret_cast<char*>(&entries[0]), toc.numTexts * sizeof entries[0]);
            PopulateTable(AssetType::TEXT, entries, stream, streamId);
        }

        {
            std::vector<ArchiveTableEntry> entries(toc.numShaders);
            stream.seekg(toc.shaderOffset);
            stream.read(reinterpret_cast<char*>(&entries[0]), toc.numShaders * sizeof entries[0]);
            PopulateTable(AssetType::SHADER, entries, stream, streamId);
        }

        {
            std::vector<ArchiveTableEntry> entries(toc.numUIs);
            stream.seekg(toc.uiOffset);
            stream.read(reinterpret_cast<char*>(&entries[0]), toc.numUIs * sizeof entries[0]);
            PopulateTable(AssetType::UILAYOUT, entries, stream, streamId);
        }
    }

    void BarcLoader::PopulateTable(AssetType type, std::vector<ArchiveTableEntry> entries, std::fstream &stream, int streamId)
    {
        auto &at = assetTable[type];
        for(const auto& archiveEntry : entries)
        {
            std::string name(archiveEntry.name);
            auto &entry = at[name];
            entry.offset = archiveEntry.dataOffset;
            entry.size = archiveEntry.dataSize;
            entry.streamId = streamId;
        }
    }

    void BarcLoader::AddOverlay(const std::string &overlayName)
    {
        std::fstream stream(overlayName, std::fstream::in | std::fstream::binary);
        if(!stream)
            throw LoaderException("Couldn't load " + overlayName);

        archiveOverlays.push_back(std::move(stream));
        BuildTables(archiveOverlays.back(), archiveOverlays.size() - 1);
    }

    bool BarcLoader::HasAsset(AssetType type, const std::string &name) const
    {
        if(assetTable.count(type) > 0 && assetTable.at(type).count(name) > 0)
            return true;
        return false;
    }

    std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> BarcLoader::LoadAsset(AssetType type, const std::string &name, bool ignoreOverlays)
    {
        bool foundInArchive = HasAsset(type, name);
        if(!foundInArchive)
        {
            if(assetRoot.empty())
                throw LoaderException("Asset (" + name + ") not found");
            return LoadAssetFromFilesystem(type, name);
        }

        return std::async(std::launch::async, [this, type, name]()
        {
            auto &assetEntry = assetTable[type][name];
            std::fstream &stream = archiveOverlays[assetEntry.streamId];
            std::size_t size = assetEntry.size;
            std::unique_ptr<std::byte[]> data(new std::byte[size]);
            stream.seekg(assetEntry.offset);
            stream.read(reinterpret_cast<char*>(data.get()), size);

            return std::make_tuple(std::move(data), size);
        });
    }

    std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> BarcLoader::LoadAssetFromFilesystem(AssetType type, const std::string &name)
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
        std::filesystem::path filePath = assetRoot / folder / (name + extension);

        if(!std::filesystem::exists(filePath))
            throw LoaderException("Asset (" + filePath.generic_string() + ") not found");

        return std::async(std::launch::async, [filePath]()
        {
            std::fstream stream(filePath, std::fstream::in | std::fstream::binary | std::fstream::ate);

            std::size_t size = stream.tellg();
            stream.seekg(0);

            std::unique_ptr<std::byte[]> data(new std::byte[size]);
            stream.read(reinterpret_cast<char*>(data.get()), size);

            return std::make_tuple(std::move(data), size);
        });
    }
}
