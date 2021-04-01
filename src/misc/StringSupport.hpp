#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <cwctype>
#include <codecvt>

#include <sstream>

//from:
// https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string/25829178
// https://stackoverflow.com/questions/42022182/converting-wstring-to-lower-case

namespace RIS
{
	inline std::string& ltrim(std::string &str)
	{
		auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
		str.erase(str.begin(), it2);
		return str;
	}

	inline std::string& rtrim(std::string &str)
	{
		auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
		str.erase(it1.base(), str.end());
		return str;
	}

	inline std::string& trim(std::string &str)
	{
		return ltrim(rtrim(str));
	}

	inline std::string trim(const std::string &str)
	{
		auto s = str;
		return ltrim(rtrim(s));
	}

	inline std::wstring& lowerCase(std::wstring &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), std::towlower);
		return str;
	}

	inline std::string& lowerCase(std::string &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
		return str;
	}

	inline std::wstring lowerCase(const std::wstring &str)
	{
		std::wstring newStr(str);
		std::transform(str.begin(), str.end(), newStr.begin(), std::towlower);
		return str;
	}

	inline std::string lowerCase(const std::string &str)
	{
		std::string newStr(str);
		std::transform(str.begin(), str.end(), newStr.begin(), [](unsigned char c){ return std::tolower(c); });
		return str;
	}

	inline std::vector<std::string> split(const std::string &str, const std::string &delimiter)
	{
		std::vector<std::string> tokens;
		auto start = 0U;
		auto end = str.find(delimiter);
		while (end != std::string::npos)
		{
			tokens.push_back(str.substr(start, end - start));
			start = end + delimiter.length();
			end = str.find(delimiter, start);
		}
		tokens.push_back(str.substr(start, end));
		return tokens;
	}

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif
	inline std::string wstringToString(const std::wstring &str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
	}

	inline std::wstring stringToWString(const std::string &str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
	}
#ifdef _WIN32
#pragma warning(pop)
#endif

	template<typename T>
	inline std::string toString(const T& val)
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
}
