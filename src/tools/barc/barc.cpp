#include <iostream>
#include <string>
#include <filesystem>
#include <cstdint>
#include <fstream>
#include <vector>
#include <array>

#include "ArchiveFormat.hpp"

using namespace std::string_literals;
using std::filesystem::path;
using std::filesystem::recursive_directory_iterator;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

const uint8_t MAGIC_ID[4] = { 'M', 'E', 'O', 'W' };
const std::array<std::string, 4> SUPPORTED_EXTENSIONS = 
{ 
    ".dds"s,
    ".txt"s,
    ".wav"s,
    ".mdl"s,
};

void PrintHelp(const std::string &progName)
{
    std::cout << "Binary ARChiver V" << static_cast<int>(RIS::ARCHIVE_FORMAT_VERSION) << std::endl;
    std::cout << progName << " [OPTIONS] < root asset directory >" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "\t-p              packs the directory into an binary achive"  << std::endl;
    std::cout << "\t-u              unpacks the given binary archive into the directory (needs -f)" << std::endl;
    std::cout << "\t-f < filename > specifies a binary archive" << std::endl;
    std::cout << "\t-h              prints this help text" << std::endl;
}

bool g_pack = true;
std::string g_barcFile;
std::string g_rootDir;

bool CheckArguments(int argc, char *argv[])
{
    for(int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if(arg[0] == '-')
        {
            switch(arg[1])
            {
                case 'p': g_pack = true; break;
                case 'u': g_pack = false; break;
                case 'f': g_barcFile = argv[++i]; break;
                case 'h': 
                default: return false;
            }
        }
        else
        {
            g_rootDir = arg;
        }
    }
    return true;
}

bool IsValidFile(const path &file)
{
    std::string ext = file.extension().string();
    auto begin = SUPPORTED_EXTENSIONS.begin();
    auto end = SUPPORTED_EXTENSIONS.end();
    return std::any_of(begin, end, [&ext](auto extension){ return ext == extension; });
}

uint32_t CountFiles(const path &rootDir)
{
    uint32_t count = 0;
    for(auto &elem : recursive_directory_iterator(rootDir))
    {
        if(elem.is_regular_file() && IsValidFile(elem.path()))
        {
            count++;
        }
    }
    return count;
}

uint32_t CountFiles(const path &rootDir, const std::string &ext)
{
    uint32_t count = 0;
    std::error_code ec;
    for(auto &elem : recursive_directory_iterator(rootDir, ec))
    {
        if(elem.is_regular_file() && elem.path().extension().string() == ext)
        {
            count++;
        }
    }
    return count;
}

std::byte *ReadFile(const path &file, std::size_t &size)
{
    std::fstream f(file, std::fstream::in | std::fstream::binary | std::fstream::ate);
    if(!f)
        return nullptr;

    size = f.tellg();
    f.seekg(0);
    std::byte *data = new std::byte[size];
    f.read(reinterpret_cast<char*>(data), size);
    return data;
}

bool Pack(const std::string &rootDir, const std::string &archiveName)
{
    std::cout << "Packing " << rootDir << " into " << archiveName << "..." << std::endl;

    path root = rootDir;
    path archive = archiveName;

    uint32_t headerSize = sizeof(RIS::Header);
    uint32_t tocSize = sizeof(RIS::TOC);

    std::fstream archiveFile(archive, std::fstream::out | std::fstream::binary);
    if(!archiveFile)
    {
        std::cout << "Something went wrong" << std::endl;
        return false;
    }

    RIS::Header header = { 0 };
    std::memcpy(header.magic, MAGIC_ID, sizeof MAGIC_ID);
    header.version = RIS::ARCHIVE_FORMAT_VERSION;

    RIS::TOC toc = { 0 };
    toc.totalItems = CountFiles(root);

    toc.numTextures = CountFiles(root / "textures"s, ".dds"s);
    toc.numMetas = 0;
    toc.numModels = CountFiles(root / "models"s, ".mdl"s);
    toc.numSounds = CountFiles(root / "sounds"s, ".wav"s);
    toc.numTexts = CountFiles(root / "texts"s, ".txt"s);

    std::cout << "Writing file..." << std::endl;

    archiveFile.write(reinterpret_cast<const char*>(&header), headerSize);
    archiveFile.write(reinterpret_cast<const char*>(&toc), tocSize);

    archiveFile.close();

    std::cout << "Done!" << std::endl;

    return true;
}

bool Unpack(const std::string &rootDir, const std::string &archiveName)
{
    return false;
}

int main(int argc, char *argv[])
{
    path progPath = argv[0];
    std::string progName = progPath.filename().string();

    if(argc <= 1)
    {
        PrintHelp(progName);
        return 1;
    }

    if(!CheckArguments(argc, argv))
    {
        PrintHelp(progName);
        return 1;
    }
    
    if(!g_pack && g_barcFile.empty())
    {
        std::cout << "Missing binary archive file to extract from" << std::endl;
        PrintHelp(progName);
        return 1;
    }

    if(g_barcFile.empty())
    {
        path directory = g_rootDir;
        g_barcFile = directory.filename().string() + ".meow"s;
    }

    if(g_pack)
    {
        return !Pack(g_rootDir, g_barcFile);
    }
    else
    {
        return !Unpack(g_rootDir, g_barcFile);
    }
}
