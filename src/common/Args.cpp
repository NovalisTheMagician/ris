#include "Args.hpp"

#include <sstream>
#include "StringSupport.hpp"

#include <exception>

using std::string;
using std::map;
using std::vector;
using std::stringstream;

namespace RIS
{
	using namespace std::string_literals;
	
	Args::Args(int argc, char *argv[])
		: args()
	{
		int index = 1;
		string flag;
		vector<string> parameters;

		if(argc <= 1)
			return;

		flag = argv[index];

		flag = lowerCase(flag);
		if (flag[0] != '-')
			return;

		while (++index < argc)
		{
			string parm = argv[index];
			if (parm.empty())
			{
				continue;
			}
			if (parm[0] != '-')
			{
				parameters.push_back(parm);
			}
			else
			{
				args[flag] = parameters;
				flag = parm;
				flag = lowerCase(flag);
				parameters.clear();
			}
		}
		args[flag] = parameters;
	}

	Args::~Args()
	{
		args.clear();
	}

	bool Args::IsSet(const std::string &flag) const
	{
		return args.count(flag) > 0;
	}

	int Args::NumParameters(const std::string &flag) const
	{
		if (IsSet(flag))
		{
			return static_cast<int>(args.at(flag).size());
		}
		throw ArgsException("Argument not present: "s + flag);
	}

	const std::string& Args::GetParameter(const std::string &flag) const
	{
		if (NumParameters(flag) > 0)
		{
			return args.at(flag)[0];
		}
		throw ArgsException("Argument not present: "s + flag);
	}

	const std::vector<std::string>& Args::GetParameters(const std::string &flag) const
	{
		if (IsSet(flag))
		{
			return args.at(flag);
		}
		throw ArgsException("Argument not present: "s + flag);
	}
}
