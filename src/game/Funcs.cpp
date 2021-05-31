#include "game/GameLoop.hpp"

#include "RIS.hpp"
#include "ui/Console.hpp"
#include "loader/Loader.hpp"
#include "window/Window.hpp"
#include "window/Paths.hpp"

#include "misc/StringSupport.hpp"

#include <string>

#include <magic_enum.hpp>
#include <fmt/format.h>

using namespace std::literals;

namespace RIS::Game
{
    void GameLoop::RegisterFuncs()
    {
        auto &console = GetConsole();
        auto &window = GetWindow();

        auto exitFunc = [&window](const auto&){ window.Exit(0); return ""; };
        console.BindFunc("exit", exitFunc);
        console.BindFunc("quit", exitFunc);

        console.BindFunc("vsync", [&window](const std::vector<std::string> &params)
        { 
            if(params.size() == 0)
            {
                const auto &config = GetConfig();
                return std::to_string(config.GetValue("r_vsync", false));
            }
            else
            {
                try
                {
                    bool value = std::stoi(params.at(0));
                    window.SetVsync(value);
                }
                catch(const std::exception&)
                {
                    return "Invalid Value"s;
                }
            }
            return ""s;
        });
        
        console.BindFunc("clear_cache", [](const auto&)
        {
            auto &cache = Loader::GetCache();
            cache.Cleanup();
            return "";
        });

        console.BindFunc("bind", [this](const std::vector<std::string> &params)
        {
            if(params.size() != 2)
                return "Wrong number of parameters!"s;

            std::string keyName = upperCase(params.at(0));
            std::string actionName = upperCase(params.at(1));

            auto key = magic_enum::enum_cast<Input::InputKey>(keyName);
            auto action = magic_enum::enum_cast<Action>(actionName);

            if(!key)
                return fmt::format("Unkown Key {}", keyName);

            if(!action)
                return fmt::format("Unkown Action {}", actionName);

            inputMapper.Map(*key, *action);
            
            return ""s;
        });

        console.BindFunc("unbind", [this](const std::vector<std::string> &params)
        {
            if(params.size() != 1)
                return "Wrong number of parameters!"s;

            std::string actionName = upperCase(params.at(0));

            if(actionName == "ALL")
            {
                inputMapper.Clear();
                return "Cleared all bindings"s;
            }

            if(auto action = magic_enum::enum_cast<Action>(actionName))
            {
                inputMapper.Clear(*action);
                return fmt::format("cleared binding for {}", actionName);
            }
            
            return fmt::format("Unkown Action {}", actionName);
        });

        console.BindFunc("save_bindings", [this](const std::vector<std::string> &params)
        {
            auto path = Window::GetConfigPath();
            path /= Input::BINDINGS_FILE_NAME;
            inputMapper.SaveMapping(path.generic_string());
            return "Saved keybinds";
        });

        console.BindFunc("show_config_path", [](const std::vector<std::string> &params)
        {
            return Window::GetConfigPath().generic_string();
        });

        console.BindFunc("show_save_path", [](const std::vector<std::string> &params)
        {
            return Window::GetSavePath().generic_string();
        });

        console.BindFunc("show_mod_path", [](const std::vector<std::string> &params)
        {
            return Window::GetModPath().generic_string();
        });
    }
}
