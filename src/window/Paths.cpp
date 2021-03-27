#include "window/Paths.hpp"

#include "misc/Version.hpp"

#ifdef _WIN32
#include <Shlobj.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif 

namespace RIS::Window
{
    static std::filesystem::path GetHomePath()
    {
#ifdef _WIN32
        char docPath[MAX_PATH];
        SHGetSpecialFolderPathA(nullptr, docPath, CSIDL_MYDOCUMENTS, false);
        std::filesystem::path path(docPath);
        return path / Version::GAME_NAME;
#elif __linux__
        const char *homeDir;
        if ((homeDir = getenv("HOME")) == NULL) {
            homeDir = getpwuid(getuid())->pw_dir;
        }
        std::filesystem::path path(homeDir);
        return path / ("." + Version::GAME_NAME);
#elif __EMSCRIPTEN__
        return "";
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

    std::filesystem::path GetTempPath()
    {
#ifdef __EMSCRIPTEN__
        return "Temp";
#else
        return std::filesystem::temp_directory_path();
#endif
    }
}
