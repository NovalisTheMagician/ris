#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <any>
#include <optional>
#include <memory>

#include "misc/Logger.hpp"

#include <zip.hpp>

namespace RIS
{
    namespace Loader
    {
        template<typename T>
        using LoadFunc = std::function<std::shared_ptr<T>(const std::vector<std::byte>&, const std::string&, bool, std::any)>;
        template<typename T>
        using DefaultFunc = std::function<std::shared_ptr<T>()>;

        struct LoaderException : public std::runtime_error
        {
            LoaderException(std::string reason) : std::runtime_error(reason.c_str()) {}
        };

        class Loader
        {
        public:
            Loader(const std::string &assetRoot);
            ~Loader() = default;
            Loader(const Loader&) = delete;
            Loader& operator=(const Loader&) = delete;
            Loader(Loader&&) = default;
            Loader& operator=(Loader&&) = default;

            void AddOverlay(const std::string &overlayName);
            bool ContainsAsset(const std::string &name) const;

            void PostInit();

            template<typename T>
            void RegisterLoadFunction(LoadFunc<T> function, std::optional<DefaultFunc<T>> defaultFunc = {});

            template<typename T>
            std::shared_ptr<T> Load(const std::string &assetName, std::any param = {}, bool cacheAsset = true);

        private:
            std::vector<std::byte> LoadBytesFromFilesystem(const std::string &name);
            std::vector<std::byte> LoadBytes(const std::string &name);

        private:
            std::string assetRoot;
            std::vector<libzip::archive> archives;
            std::unordered_map<std::type_index, std::any> loadFuncs;
            std::unordered_map<std::type_index, std::any> defaultFuncs;

        };

        template<typename T>
        void Loader::RegisterLoadFunction(LoadFunc<T> function, std::optional<DefaultFunc<T>> defaultFunc)
        {
            loadFuncs.insert_or_assign(typeid(T), function);
            if(defaultFunc.has_value())
            {
                defaultFuncs.insert_or_assign(typeid(T), defaultFunc);
            }
        }

        template<typename T>
        std::shared_ptr<T> Loader::Load(const std::string &assetName, std::any param, bool cacheAsset)
        {
            const type_info &ti = typeid(T);
            if(loadFuncs.count(ti) == 0)
            {
                throw LoaderException(std::string("No Loader registered for type ") + ti.name());
            }

            LoadFunc<T> &func = std::any_cast<LoadFunc<T>>(loadFuncs.at(ti));

            std::vector<std::byte> bytes;
            try
            {
                bytes = LoadBytes(assetName);
            }
            catch(const LoaderException& e)
            {
                Logger::Instance().Error(e.what());
                if(defaultFuncs.count(ti) > 0)
                {
                    DefaultFunc<T> &defFunc = std::any_cast<DefaultFunc<T>>(defaultFuncs.at(ti));
                    return defFunc();
                }
                throw;
            }
            return func(bytes, assetName, cacheAsset, param);
        }
    }
}
