#pragma once

#include <map>
#include <string>
#include <sstream>

namespace RIS
{
	class Config
	{
	public:
		Config(const std::wstring &configPath);
		~Config();
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;

		const std::string& GetString(const std::string &key, const std::string &def) const;
		const std::string& GetString(const std::string &key, const std::string &def);
		int GetInt(const std::string &key, const int &def) const;
		int GetInt(const std::string &key, const int &def);

		template<typename T>
		void SetValue(const std::string &key, const T &value);

	private:
		std::map<std::string, std::string> configMap;
		std::wstring configPath;

		bool isDirty;

	};

	template<typename T>
	void Config::SetValue(const std::string &key, const T &value)
	{
		std::stringstream sstream;
		sstream << value;
		configMap[key] = sstream.str();
		isDirty = true;
	}
}
