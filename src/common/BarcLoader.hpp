#pragma once

#include "ILoader.hpp"
#include "SystemLocator.hpp"

#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <filesystem>
#include <future>
#include <unordered_map>
#include <map>

#include "common/SystemLocator.hpp"
#include "common/Config.hpp"

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
        BarcLoader(const SystemLocator &systems, Config &config, const std::string &assetRoot);
        ~BarcLoader();

        void AddOverlay(const std::string &overlayName) override;

        bool HasAsset(AssetType type, const std::string &name) const override;
        std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> LoadAsset(AssetType type, const std::string &name, bool ignoreOverlays = false) override;

    private:
        void BuildTables(std::fstream &stream, int streamId);
        void PopulateTable(AssetType type, std::vector<ArchiveTableEntry> entries, std::fstream &stream, int streamId);

        std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> LoadAssetFromFilesystem(AssetType type, const std::string &name);

    private:
        struct TableEntry
        {
            std::size_t offset;
            std::size_t size;
            int streamId;
        };

    private:
        const SystemLocator &systems;
        Config &config;
        const std::string &assetRoot;

        std::vector<std::fstream> archiveOverlays;
        std::unordered_map<AssetType, std::unordered_map<std::string, TableEntry>> assetTable;

    };
}
