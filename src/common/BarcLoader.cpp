#include "BarcLoader.hpp"

#include <filesystem>

#include <utility>

namespace RIS
{
    #pragma pack(push, 1)
    struct Header
    {
        uint8_t magic[4]; // needs to be MEOW
        uint8_t version;
    };

    struct TOC
    {
        uint32_t totalItems;

        uint32_t numMetas;
        uint32_t metaOffset;

        uint32_t numModels;
        uint32_t modelOffset;

        uint32_t numSounds;
        uint32_t soundOffset;

        uint32_t numTextures;
        uint32_t textureOffset;

        uint32_t numTexts;
        uint32_t textOffset;
    };

    struct ArchiveTableEntry
    {
        char name[16];
        uint32_t dataSize;
        uint32_t dataOffset;
    };
    #pragma pack(pop)

    const int VERSION = 1;

    BarcLoader::BarcLoader(const SystemLocator &systems, const std::string &assetRoot)
        : systems(systems), assetRoot(assetRoot), archiveOverlays()
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

        if(header.version != VERSION)
            throw LoaderException("Unsupported archive version");

        stream.read(reinterpret_cast<char*>(&toc), sizeof toc);

        std::vector<ArchiveTableEntry> textureEntries(toc.numTextures);
        stream.seekg(toc.textureOffset);
        stream.read(reinterpret_cast<char*>(&textureEntries[0]), toc.numTextures * sizeof textureEntries[0]);

        PopulateTable(AssetType::TEXTURE, textureEntries, stream, streamId);
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
        return assetTable.at(type).count(name) > 0;
    }

    std::unique_ptr<std::byte[]> BarcLoader::LoadAsset(AssetType type, const std::string &name, std::size_t &size, bool ignoreOverlays)
    {
        bool foundInArchive = HasAsset(type, name);
        if(!foundInArchive)
        {
            if(assetRoot.empty())
                throw LoaderException("Asset (" + name + ") not found");
            return LoadAssetFromFilesystem(type, name, size);
        }

        std::fstream &stream = archiveOverlays[assetTable[type][name].streamId];

        //TODO

        return nullptr;
    }

    std::unique_ptr<std::byte[]> BarcLoader::LoadAssetFromFilesystem(AssetType type, const std::string &name, std::size_t &size)
    {
        return nullptr;
    }
}
