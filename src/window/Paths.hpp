#pragma once

#include <filesystem>

namespace RIS::Window
{
    void CreatePaths();

    std::filesystem::path GetConfigPath();
    std::filesystem::path GetSavePath();
    std::filesystem::path GetModPath();
    std::filesystem::path GetTempPath();
    std::filesystem::path GetCachePath();
}
