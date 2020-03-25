#pragma once

#include "ILoader.hpp"
#include "SystemLocator.hpp"
#include "Config.hpp"

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <tuple>
#include <filesystem>

#include <future>

#include <zip.hpp>

namespace RIS
{
    struct ZipLoaderException : public std::runtime_error
	{
		ZipLoaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class ZipLoader : public ILoader
    {
    public:
        ZipLoader(const SystemLocator &systems, Config &config, const std::string &assetRoot);
        ~ZipLoader();

        void AddOverlay(const std::string &overlayName) override;

        bool HasAsset(AssetType type, const std::string &name) const override;
        std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> LoadAsset(AssetType type, const std::string &name, bool ignoreOverlays = false) override;

    private:
        std::tuple<std::filesystem::path, std::string> GetAssetProps(AssetType type) const;
        std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> LoadAssetFromFilesystem(AssetType type, const std::string &name);

    private:
        const SystemLocator &systems;
        Config &config;

        std::string assetRoot;
        std::vector<libzip::archive> archives;

    };
}
