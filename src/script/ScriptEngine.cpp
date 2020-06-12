#include "window/Window.hpp"
#include "ui/Userinterface.hpp"
#include "script/ScriptEngine.hpp"

#include "RIS.hpp"

#include "misc/Version.hpp"

#include "ui/Console.hpp"

#include "loader/Loader.hpp"

#include "misc/Logger.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <unordered_set>
#include <filesystem>
#include <fstream>

#include <iostream>

using namespace std::literals;

namespace RIS
{
    namespace Script
    {
        ScriptEngine::ScriptEngine()
            : state(tcc_new())
        {}

        ScriptEngine::~ScriptEngine()
        {
            tcc_delete(state);
        }

        void ScriptEngine::PostInit()
        {
            UI::Console &console = GetConsole();

            console.BindFunc("reload", [this](std::vector<std::string> params)
            {
                Reload();
                return "";
            });

            auto &loader = GetLoader();
        }

        void ScriptEngine::LoadScripts()
        {
            auto &loader = GetLoader();
            auto files = loader.GetFilesFromFolder(std::string("scripts"));
            auto tmpPath = std::filesystem::temp_directory_path();
            tmpPath = tmpPath / Version::GAME_NAME;
            auto tmpScriptsPath = tmpPath / "scripts";

            if(std::filesystem::exists(tmpScriptsPath))
                std::filesystem::remove_all(tmpScriptsPath);

            if(!std::filesystem::exists(tmpScriptsPath))
            {
                if(!std::filesystem::exists(tmpPath))
                    std::filesystem::create_directory(tmpPath);
                std::filesystem::create_directory(tmpScriptsPath);
            }

            //do defines here

            tcc_add_sysinclude_path(state, tmpScriptsPath.generic_string().c_str());
            tcc_add_include_path(state, tmpScriptsPath.generic_string().c_str());
            tcc_add_library_path(state, tmpScriptsPath.generic_string().c_str());
            tcc_set_output_type(state, TCC_OUTPUT_MEMORY);

            tcc_set_error_func(state, nullptr, [](void* err_opaque, const char* msg)
            {
                auto &console = GetConsole();
                console.Print(msg);
            });

            //extract the static libraries and headers into tmp
            for(const auto &file : files)
            {
                std::filesystem::path filePath(file);
                std::string extension = filePath.extension().generic_string();
                if(extension == ".h" || extension == ".a")
                {
                    auto fileContents = loader.LoadBytes(file);
                    std::fstream stream(tmpPath / file, std::fstream::out | std::fstream::binary | std::fstream::trunc);
                    if (!stream) {
                        std::cerr << "file open failed: " << std::strerror(errno) << "\n";
                        continue;
                    }
                    stream.write(reinterpret_cast<char*>(fileContents.data()), fileContents.size());
                }
            }

            //compile the source scripts
            for(const auto &file : files)
            {
                std::filesystem::path filePath(file);
                if(filePath.extension().generic_string() == ".c")
                {
                    auto fileContents = loader.Load<std::string>(file);
                    tcc_compile_string(state, (*fileContents).c_str());
                }
            }

            //do external funcs here
            auto &window = GetWindow();
            auto &interface = GetUserinterface();
            window.RegisterScriptFunctions();
            interface.RegisterScriptFunctions();

            if(tcc_relocate(state, TCC_RELOCATE_AUTO) != -1)
            {
                tcc_list_symbols(state, &symbols, [](void *ctx, const char *name, const void *val)
                {
                    std::unordered_map<std::string, void*> *symbols = static_cast<std::unordered_map<std::string, void*>*>(ctx);
                    if(name[0] != '_') // only add symbols which are not library symbols
                    {
                        symbols->insert_or_assign(name, const_cast<void*>(val));
                    }
                });

                interface.Invalidate();
            }
        }

        void ScriptEngine::Reload()
        {
            tcc_delete(state);
            state = tcc_new();

            symbols.clear();

            LoadScripts();
        }
    }
}