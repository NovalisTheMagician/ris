#include <iostream>
#include <string>
#include <filesystem>

using std::filesystem::path;

void PrintHelp(const std::string &progName)
{
    std::cout << progName << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        path progPath = argv[0];
        std::string name = progPath.filename().string();
        PrintHelp(name);
    }
    
    return 0;
}
