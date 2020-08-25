#pragma once

#include "RisExcept.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

        struct LoaderException : public RISException
        {
            LoaderException(const std::string &reason) : RISException(reason) {}
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

            std::unordered_set<std::string> GetFilesFromFolder(const std::filesystem::path &folder);

            template<typename T>
            void RegisterLoadFunction(LoadFunc<T> function, std::optional<DefaultFunc<T>> defaultFunc = {});

            template<typename T, bool DoThrow = true>
            std::shared_ptr<T> Load(const std::string &assetName, std::any param = {}, bool cacheAsset = true);

            std::vector<std::byte> LoadBytes(const std::string &name);

        private:
            std::vector<std::byte> LoadBytesFromFilesystem(const std::string &name);

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
                defaultFuncs.insert_or_assign(typeid(T), defaultFunc.value());
            }
        }

        template<typename T, bool DoThrow>
        std::shared_ptr<T> Loader::Load(const std::string &assetName, std::any param, bool cacheAsset)
        {
            const type_info &ti = typeid(T);
            if(loadFuncs.count(ti) == 0)
            {
                if constexpr (DoThrow)
                    throw LoaderException(std::string("No Loader registered for type ") + ti.name());
                else
                    return nullptr;
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
                if constexpr (DoThrow)
                    throw;
                else
                    return nullptr;
            }
            if constexpr (DoThrow)
                return func(bytes, assetName, cacheAsset, param);
            else
            {
                try
                {
                    return func(bytes, assetName, cacheAsset, param);
                }
                catch(const RISException& e)
                {
                    return nullptr;
                }
            }
        }
    }
}
