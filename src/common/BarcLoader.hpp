#pragma once

#include "ILoader.hpp"
#include "SystemLocator.hpp"

#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>

namespace RIS
{
    struct LoaderException : public std::runtime_error
	{
		LoaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
	};

    class BarcLoader : public ILoader
    {
    public:
        BarcLoader(const SystemLocator &systems, const std::string &assetRoot);
        ~BarcLoader();

        void AddOverlay(const std::string &overlayName) override;

        bool HasFile(AssetType type, const std::string &name) const override;
        std::unique_ptr<std::byte[]> LoadFile(AssetType type, const std::string &name, bool ignoreOverlays = false) const override;

    private:
        const SystemLocator &systems;
        const std::string &assetRoot;

        std::vector<std::fstream> archiveOverlays;

    };
}
