#pragma once

#include <cstdint>

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

        uint32_t numShaders;
        uint32_t shaderOffset;

        uint32_t numTextures;
        uint32_t textureOffset;

        uint32_t numTexts;
        uint32_t textOffset;

        uint32_t numUIs;
        uint32_t uiOffset;
    };

    struct ArchiveTableEntry
    {
        char name[16];
        uint32_t dataSize;
        uint32_t dataOffset;
    };
    #pragma pack(pop)

    const uint8_t ARCHIVE_FORMAT_VERSION = 1;
}

