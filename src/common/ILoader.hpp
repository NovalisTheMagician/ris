#pragma once

#include <memory>
#include <string>
#include <cstddef>
#include <future>
#include <tuple>

namespace RIS
{
    enum class AssetType
    {
        TEXTURE,
        MODEL,
        SOUND,
        SHADER,
        SHADERSRC,
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
        virtual std::future<std::tuple<std::unique_ptr<std::byte[]>, std::size_t>> LoadAsset(AssetType type, const std::string &name, bool ignoreOverlays = false) = 0;
    };
}