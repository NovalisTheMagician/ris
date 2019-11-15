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
    };

    class ILoader
    {
    public:
        virtual ~ILoader() = default;

        virtual void AddOverlay(const std::string &overlayName);

        virtual bool HasFile(AssetType type, const std::string &name) const;
        virtual std::unique_ptr<std::byte[]> LoadFile(AssetType type, const std::string &name, bool ignoreOverlays = false) const;
    };
}