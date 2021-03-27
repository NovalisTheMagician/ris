#pragma once

#include <string>
#include <map>
#include <vector>
#include <exception>

namespace RIS
{
	struct ArgsException : public std::exception
	{
		ArgsException(std::string reason) : reason(reason) {}
		const char * what() const noexcept override { return reason.c_str(); }
	private:
		std::string reason;
	};

	class Args
	{
	public:
		Args() = default;
		Args(int argc, char *argv[]);
		~Args() = default;

		Args(const Args&) = default;
		Args& operator=(const Args&) = default;
		Args(Args&&) = default;
		Args& operator=(Args&&) = default;

		bool IsSet(const std::string &flag) const;
		int NumParameters(const std::string &flag) const;
		const std::string& GetParameter(const std::string &flag) const;
		const std::vector<std::string>& GetParameters(const std::string &flag) const;

	private:
		std::map<std::string, std::vector<std::string>> args;

	};
}
