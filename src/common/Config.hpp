#pragma once

#include <unordered_map>
#include <string>
#include <sstream>

#include <variant>

namespace RIS
{
	class Config
	{
	public:
		Config(const std::string &configPath);
		~Config();
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) = default;
		Config& operator=(Config&&) = default;

		template<typename T>
		T GetValue(const std::string &key, T defValue) const;
		template<typename T>
		T GetValue(const std::string &key, T defValue);

		template<typename T>
		void SetValue(const std::string &key, T value);

	private:
		using VariantType = std::variant<std::string, float, bool>;

		std::unordered_map<std::string, VariantType> configMap;
		std::string configPath;

		bool isDirty;

	};

	template<typename T>
	T Config::GetValue(const std::string &key, T defValue) const
	{
		if (configMap.count(key) > 0)
		{
			auto& value = configMap.at(key);
			try 
			{
				return std::get<T>(value);
			}
			catch (const std::bad_variant_access&) 
			{
			}
		}
		return defValue;
	}

	template<typename T>
	T Config::GetValue(const std::string &key, T defValue)
	{
		if (configMap.count(key) > 0)
		{
			auto& value = configMap.at(key);
			try 
			{
				return std::get<T>(value);
			}
			catch (const std::bad_variant_access&) 
			{
			}
		}
		configMap[key] = defValue;
		isDirty = true;
		return defValue;
	}

	template<>
	inline int Config::GetValue(const std::string &key, int defValue) const
	{
		if (configMap.count(key) > 0)
		{
			auto& value = configMap.at(key);
			try 
			{
				return std::get<float>(value);
			}
			catch (const std::bad_variant_access&) 
			{
			}
		}
		return defValue;
	}

	template<>
	inline int Config::GetValue(const std::string &key, int defValue)
	{
		if (configMap.count(key) > 0)
		{
			auto& value = configMap.at(key);
			try 
			{
				return std::get<float>(value);
			}
			catch (const std::bad_variant_access&) 
			{
			}
		}
		configMap[key] = static_cast<float>(defValue);
		isDirty = true;
		return defValue;
	}

	template<typename T>
	void Config::SetValue(const std::string &key, T value)
	{
		configMap[key] = value;
		isDirty = true;
	}

	template<>
	inline void Config::SetValue<int>(const std::string &key, int value)
	{
		configMap[key] = static_cast<float>(value);
		isDirty = true;
	}
}
