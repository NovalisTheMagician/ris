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

namespace RIS
{
    namespace Script
    {
        ScriptEngine::ScriptEngine()
            : vm(new squall::VMStd())
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
            auto vmInstance = vm->handle();
            sq_setcompilererrorhandler(vmInstance, [](HSQUIRRELVM v, const SQChar *desc, const SQChar *source, SQInteger line, SQInteger column)
            {
                auto &console = GetConsole();
                console.Print("Scripterror in "s + source + " (Ln "s + std::to_string(line) + ", Col "s + std::to_string(column) + ")"s);
                console.Print(desc);
            });

            sq_setprintfunc(vmInstance, [](HSQUIRRELVM v, const SQChar *msg, ...)
            {
                char buf[RIS_BUF_SIZE];
                va_list args;
                va_start(args, msg);
                vsnprintf(buf, RIS_BUF_SIZE, msg, args);
                va_end(args);

                auto &console = GetConsole();
                console.Print(buf);
            }, [](HSQUIRRELVM v, const SQChar *msg, ...)
            {
                char buf[RIS_BUF_SIZE];
                va_list args;
                va_start(args, msg);
                vsnprintf(buf, RIS_BUF_SIZE, msg, args);
                va_end(args);

                auto &console = GetConsole();
                console.Print(buf);
                Logger::Instance().Error(buf);
            });

            //check for return value (can it even fail?)
            sqstd_register_mathlib(vmInstance);
            sqstd_register_stringlib(vmInstance);

            auto &loader = GetLoader();
            auto &console = GetConsole();
            auto files = loader.GetFilesFromFolder("scripts"s);

            for(const auto &file : files)
            {
                auto bytes = loader.LoadBytes(file);
                try
                {
                    auto source = std::string(reinterpret_cast<char*>(bytes.data()), bytes.size());
                    vm->dostring(source.c_str(), file.c_str());
                }
                catch(const squall::squirrel_error& e)
                {
                    //console.Print(e.what());
                }
            }

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
            vm.reset(new squall::VMStd());
            LoadScripts();
        }
    }
}