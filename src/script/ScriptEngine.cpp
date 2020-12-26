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

#include <squirrel.h>
#include <sqstdmath.h>
#include <sqstdstring.h>

using namespace std::literals;

#define RIS_BUF_SIZE 256

namespace RIS::Script
{
    ScriptEngine::ScriptEngine()
    {}

    void ScriptEngine::PostInit()
    {
        UI::Console &console = GetConsole();

        console.BindFunc("reload", [this](std::vector<std::string> params)
        {
            Reload();
            return "";
        });
    }

    void ScriptEngine::LoadScripts()
    {
        vm.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::utf8);

        /*
        auto &loader = GetLoader();
        auto &console = GetConsole();
        auto files = loader.GetFilesFromFolder("scripts"s);

        for(const auto &file : files)
        {
            auto bytes = loader.LoadBytes(file);
            auto source = std::string(reinterpret_cast<char*>(bytes.data()), bytes.size());

            auto result = vm.safe_script(source, sol::script_pass_on_error);
            if(!result.valid())
            {
                console.Print(result);
            }
        }
        */

        auto &window = GetWindow();
        auto &interface = GetUserinterface();
        window.RegisterScriptFunctions();
        interface.RegisterScriptFunctions();

        interface.Invalidate();
    }

    void ScriptEngine::Reload()
    {
        auto &interface = GetUserinterface();
        interface.ReleaseScriptReferences();

        vm = {};

        LoadScripts();
    }
}