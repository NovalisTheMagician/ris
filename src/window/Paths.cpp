#include "window/Paths.hpp"

#include "misc/Version.hpp"

#ifdef _WIN32
#include <Shlobj.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif 

#include <string>
#include <string_view>

namespace RIS::Window
{
    static std::string GetFSFriendlyName(std::string_view name)
    {
#ifdef _WIN32
        return std::string(name);
#else
        std::string newName(name);
        newName.erase(std::remove_if(newName.begin(), newName.end(), [](unsigned char x) { return std::isspace(x); }), newName.end());
        return newName;
#endif
    }

    static std::filesystem::path GetHomePath()
    {
#ifdef _WIN32
        char docPath[MAX_PATH];
        SHGetSpecialFolderPathA(nullptr, docPath, CSIDL_MYDOCUMENTS, false);
        std::filesystem::path path(docPath);
        return path / GetFSFriendlyName(Version::GAME_NAME);
#elif __linux__
        const char *homeDir;
        if ((homeDir = getenv("HOME")) == NULL) {
            homeDir = getpwuid(getuid())->pw_dir;
        }
        std::filesystem::path path(homeDir);
        return path / ("." + std::string(Version::GAME_NAME));
#elif __EMSCRIPTEN__
        return "";
#endif
    }

    static std::filesystem::path GetAppdataPath()
    {
#ifdef _WIN32
        char docPath[MAX_PATH];
        SHGetSpecialFolderPathA(nullptr, docPath, CSIDL_LOCAL_APPDATA, false);
        std::filesystem::path path(docPath);
        return path / Version::GAME_NAME;
#else 
        return GetHomePath();
#endif
    }

    std::filesystem::path GetConfigPath()
    {
        return GetHomePath() / "Config/";
    }

    std::filesystem::path GetSavePath()
    {
        return GetHomePath() / "Saves/";
    }

    std::filesystem::path GetModPath()
    {
        return GetHomePath() / "Mods/";
    }

    std::filesystem::path GetCachePath()
    {
        return GetAppdataPath() / "Cache/";
    }

    std::filesystem::path GetTempPath()
    {
#ifdef __EMSCRIPTEN__
        return "Temp/";
#else
        return std::filesystem::temp_directory_path();
#endif
    }

    void CreatePaths()
    {
        std::filesystem::create_directories(GetConfigPath());
        std::filesystem::create_directories(GetSavePath());
        std::filesystem::create_directories(GetModPath());
    }
}
