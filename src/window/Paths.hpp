#pragma once

#include <filesystem>

namespace RIS::Window
{
    std::filesystem::path GetConfigPath();
    std::filesystem::path GetSavePath();
    std::filesystem::path GetModPath();
    std::filesystem::path GetTempPath();
}
