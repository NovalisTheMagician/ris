#pragma once

#include <memory>
#include <string>
#include <cstddef>

namespace RIS
{
    enum class AssetType
    {
        TEXTURE,
        MODEL,
        SOUND,
        SHADER,
        TEXT,
        UILAYOUT,
        FONT,
    };

    class ILoader
    {
    public:
        virtual ~ILoader() = default;

        virtual void AddOverlay(const std::string &overlayName) = 0;

        virtual bool HasAsset(AssetType type, const std::string &name) const = 0;
        virtual std::unique_ptr<std::byte[]> LoadAsset(AssetType type, const std::string &name, std::size_t &size, bool ignoreOverlays = false) = 0;
    };
}