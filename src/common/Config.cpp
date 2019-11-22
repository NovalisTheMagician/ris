#include "Config.hpp"
#include <fstream>
#include <filesystem>

#include <exception>

#include "StringSupport.hpp"

using std::string;
using std::wstring;
using std::stringstream;

using std::ifstream;
using std::ofstream;

namespace RIS
{
	Config::Config(const std::string &configPath)
		: configMap(), configPath(configPath), isDirty(false)
	{
		ifstream inputStream(configPath);
		if (!inputStream)
		{
			std::ofstream(configPath);
			return;
		}

		this->configPath = configPath;
		string line;
		while (std::getline(inputStream, line))
		{
			if (line[0] == '/' && line[1] == '/') continue;

			stringstream ss(line);
			string key, value;
			std::getline(ss, key, '=');
			std::getline(ss, value, '=');

			trim(key);
			trim(value);

			configMap[key] = value;
		}
	}

	Config::~Config()
	{
		if (isDirty)
		{
			ofstream outputStream(configPath);
			if (outputStream)
			{
				for (auto const&[key, val] : configMap)
				{
					outputStream << key << "=" << val << std::endl;
				}
			}

			configMap.clear();
		}
	}

	const string& Config::GetString(const string &key, const string &def) const
	{
		if (configMap.count(key) > 0)
		{
			return configMap.at(key);
		}
		return def;
	}

	const string& Config::GetString(const string &key, const string &def)
	{
		if (configMap.count(key) > 0)
		{
			return configMap.at(key);
		}
		configMap[key] = def;
		isDirty = true;
		return def;
	}

	int Config::GetInt(const string &key, const int &def) const
	{
		if (configMap.count(key) > 0)
		{
			stringstream ss(configMap.at(key));
			int val;
			ss >> val;
			return val;
		}
		return def;
	}

	int Config::GetInt(const string &key, const int &def)
	{
		if (configMap.count(key) > 0)
		{
			stringstream ss(configMap.at(key));
			int val;
			ss >> val;
			return val;
		}
		configMap[key] = std::to_string(def);
		isDirty = true;
		return def;
	}
}
