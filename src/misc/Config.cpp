#include "misc/Config.hpp"
#include <fstream>
#include <filesystem>

#include <exception>

#include "misc/StringSupport.hpp"

namespace RIS
{
	Config::Config()
		: configPath("config.txt")
	{}

	Config::Config(const std::string &configPath)
		: configPath(configPath)
	{
		std::ifstream inputStream(configPath);
		if (!inputStream)
		{
			std::ofstream(configPath);
			return;
		}

		this->configPath = configPath;
		std::string line;
		while (std::getline(inputStream, line))
		{
			if (line[0] == '/' && line[1] == '/') continue;

			std::stringstream ss(line);
			std::string key, value;
			std::getline(ss, key, '=');
			std::getline(ss, value, '=');

			trim(key);
			trim(value);

			//is it a number?
			try
			{
				float val = std::stof(value);
				configMap[key] = val;
				continue;
			}
			catch(const std::invalid_argument& e)
			{}

			//is it a bool?
			try
			{
				bool val;
				std::stringstream ss(value);
				ss >> std::boolalpha >> val;
				configMap[key] = val;
				continue;
			}
			catch(const std::exception& e)
			{}
			
			// it's a string!
			configMap[key] = value;
		}
	}

	Config::~Config()
	{
		if (isDirty)
		{
			std::ofstream outputStream(configPath);
			if (outputStream)
			{
				for (auto const&[key, val] : configMap)
				{
					auto &k = key;
					std::visit([&](auto&& arg)
					{
						outputStream << k << "=" << std::boolalpha << arg << std::endl;
					}, val);
				}
			}

			configMap.clear();
		}
	}
}
