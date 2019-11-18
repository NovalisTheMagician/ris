#pragma once

#include "ILoader.hpp"
#include "SystemLocator.hpp"

#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <filesystem>

#include <unordered_map>
#include <map>

namespace RIS
{
    struct LoaderException : public std::runtime_error
	{
		LoaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    struct ArchiveTableEntry;

    class BarcLoader : public ILoader
    {
    public:
        BarcLoader(const SystemLocator &systems, const std::string &assetRoot);
        ~BarcLoader();

        void AddOverlay(const std::string &overlayName) override;

        bool HasAsset(AssetType type, const std::string &name) const override;
        std::unique_ptr<std::byte[]> LoadAsset(AssetType type, const std::string &name, std::size_t &size, bool ignoreOverlays = false) override;

    private:
        void BuildTables(std::fstream &stream, int streamId);
        void PopulateTable(AssetType type, std::vector<ArchiveTableEntry> entries, std::fstream &stream, int streamId);

        std::unique_ptr<std::byte[]> LoadAssetFromFilesystem(AssetType type, const std::string &name, std::size_t &size);

    private:
        struct TableEntry
        {
            std::size_t offset;
            std::size_t size;
            int streamId;
        };

    private:
        const SystemLocator &systems;
        const std::string &assetRoot;

        std::vector<std::fstream> archiveOverlays;
        std::unordered_map<AssetType, std::unordered_map<std::string, TableEntry>> assetTable;

    };
}
