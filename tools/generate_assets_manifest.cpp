#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct ManifestEntry
{
    std::string relativePath;
    std::uint32_t crc32;
    bool isZipPackage;
};

static void PrintUsage(const char* program)
{
    std::cerr
        << "Usage:\n"
        << "  " << program << " --asset-root <dir> [--output <file>] [--zip-extract <path>] [--no-zip-package]\n\n"
        << "Examples:\n"
        << "  " << program << " --asset-root /path/to/server-root\n"
        << "  " << program << " --asset-root /path/to/server-root/Data --output /path/to/server-root/Data/assets-manifest.txt\n";
}

static std::string ToLowerASCII(std::string value)
{
    for (char& c : value)
    {
        c = (char)std::tolower((unsigned char)c);
    }
    return value;
}

static bool EndsWithIgnoreCase(const std::string& value, const std::string& suffix)
{
    if (suffix.size() > value.size())
    {
        return false;
    }

    const std::size_t offset = value.size() - suffix.size();
    for (std::size_t i = 0; i < suffix.size(); ++i)
    {
        if (std::tolower((unsigned char)value[offset + i]) != std::tolower((unsigned char)suffix[i]))
        {
            return false;
        }
    }
    return true;
}

class CRC32
{
public:
    CRC32()
    {
        BuildTable();
    }

    std::uint32_t ComputeFile(const fs::path& filePath) const
    {
        std::ifstream input(filePath, std::ios::binary);
        if (!input)
        {
            throw std::runtime_error("failed to open file: " + filePath.string());
        }

        std::uint32_t crc = 0xFFFFFFFFu;
        std::array<char, 1024 * 1024> buffer{};
        while (input.good())
        {
            input.read(buffer.data(), (std::streamsize)buffer.size());
            const std::streamsize readCount = input.gcount();
            if (readCount <= 0)
            {
                continue;
            }

            const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buffer.data());
            for (std::streamsize i = 0; i < readCount; ++i)
            {
                crc = (crc >> 8) ^ table_[(crc & 0xFFu) ^ ptr[i]];
            }
        }

        return crc ^ 0xFFFFFFFFu;
    }

private:
    void BuildTable()
    {
        const std::uint32_t polynomial = 0x04C11DB7u;
        for (std::uint32_t code = 0; code <= 0xFFu; ++code)
        {
            std::uint32_t value = Reflect(code, 8) << 24;
            for (int i = 0; i < 8; ++i)
            {
                value = (value << 1) ^ ((value & (1u << 31)) ? polynomial : 0u);
            }
            table_[code] = Reflect(value, 32);
        }
    }

    static std::uint32_t Reflect(std::uint32_t value, int bits)
    {
        std::uint32_t reflected = 0;
        for (int i = 1; i <= bits; ++i)
        {
            if (value & 1u)
            {
                reflected |= (1u << (bits - i));
            }
            value >>= 1;
        }
        return reflected;
    }

    std::array<std::uint32_t, 256> table_{};
};

struct Arguments
{
    fs::path assetRoot;
    fs::path outputPath;
    std::string zipExtract = "Data";
    bool markZipAsPackage = true;
};

static bool ParseArguments(int argc, char** argv, Arguments& outArgs)
{
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "--asset-root" && (i + 1) < argc)
        {
            outArgs.assetRoot = argv[++i];
        }
        else if (arg == "--output" && (i + 1) < argc)
        {
            outArgs.outputPath = argv[++i];
        }
        else if (arg == "--zip-extract" && (i + 1) < argc)
        {
            outArgs.zipExtract = argv[++i];
        }
        else if (arg == "--no-zip-package")
        {
            outArgs.markZipAsPackage = false;
        }
        else if (arg == "--help" || arg == "-h")
        {
            PrintUsage(argv[0]);
            return false;
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << "\n";
            PrintUsage(argv[0]);
            return false;
        }
    }

    if (outArgs.assetRoot.empty())
    {
        std::cerr << "--asset-root is required\n";
        PrintUsage(argv[0]);
        return false;
    }

    return true;
}

static bool ResolveDataDirectories(const fs::path& assetRootArg, fs::path& outServerRoot, fs::path& outDataDir)
{
    fs::path absoluteRoot = fs::absolute(assetRootArg);
    if (!fs::exists(absoluteRoot) || !fs::is_directory(absoluteRoot))
    {
        std::cerr << "Asset root does not exist or is not a directory: " << absoluteRoot << "\n";
        return false;
    }

    const std::string rootNameLower = ToLowerASCII(absoluteRoot.filename().string());
    if (rootNameLower == "data")
    {
        outDataDir = absoluteRoot;
        outServerRoot = absoluteRoot.parent_path();
        return true;
    }

    fs::path dataDir = absoluteRoot / "Data";
    if (!fs::exists(dataDir) || !fs::is_directory(dataDir))
    {
        std::cerr << "Could not find Data/ under asset root: " << absoluteRoot << "\n";
        return false;
    }

    outServerRoot = absoluteRoot;
    outDataDir = dataDir;
    return true;
}

static bool ShouldSkipPath(const fs::path& filePath)
{
    const std::string name = ToLowerASCII(filePath.filename().string());
    if (name == "assets-manifest.txt")
    {
        return true;
    }
    if (EndsWithIgnoreCase(name, ".crc32"))
    {
        return true;
    }
    if (EndsWithIgnoreCase(name, ".extracted"))
    {
        return true;
    }
    return false;
}

static std::string ToManifestRelativePath(const fs::path& filePath, const fs::path& serverRoot)
{
    std::string relative = fs::relative(filePath, serverRoot).generic_string();
    while (!relative.empty() && relative[0] == '/')
    {
        relative.erase(relative.begin());
    }
    return relative;
}

static bool CollectEntries(const fs::path& serverRoot,
                           const fs::path& dataDir,
                           bool markZipAsPackage,
                           std::vector<ManifestEntry>& outEntries)
{
    outEntries.clear();
    CRC32 crc;

    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(dataDir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const fs::path filePath = entry.path();
        if (ShouldSkipPath(filePath))
        {
            continue;
        }

        ManifestEntry manifestEntry;
        manifestEntry.relativePath = ToManifestRelativePath(filePath, serverRoot);
        manifestEntry.crc32 = crc.ComputeFile(filePath);
        manifestEntry.isZipPackage = markZipAsPackage && EndsWithIgnoreCase(manifestEntry.relativePath, ".zip");
        outEntries.push_back(manifestEntry);
    }

    std::sort(outEntries.begin(), outEntries.end(), [](const ManifestEntry& a, const ManifestEntry& b) {
        return a.relativePath < b.relativePath;
    });

    return !outEntries.empty();
}

static bool WriteManifest(const fs::path& outputPath,
                          const std::vector<ManifestEntry>& entries,
                          const std::string& zipExtract)
{
    const fs::path parent = outputPath.parent_path();
    if (!parent.empty())
    {
        std::error_code ec;
        fs::create_directories(parent, ec);
        if (ec)
        {
            std::cerr << "Failed to create output directory: " << parent << " (" << ec.message() << ")\n";
            return false;
        }
    }

    std::ofstream out(outputPath, std::ios::binary | std::ios::trunc);
    if (!out)
    {
        std::cerr << "Failed to open output file: " << outputPath << "\n";
        return false;
    }

    out << "# Auto-generated by tools/generate_assets_manifest.cpp\n";
    out << "# Format: Data/path|0xCRC32[|archive=1|extract=Data]\n";

    for (const ManifestEntry& entry : entries)
    {
        std::ostringstream crcText;
        crcText << "0x" << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << entry.crc32;

        out << entry.relativePath << "|" << crcText.str();
        if (entry.isZipPackage)
        {
            out << "|archive=1";
            if (!zipExtract.empty())
            {
                out << "|extract=" << zipExtract;
            }
        }
        out << "\n";
    }

    out.flush();
    if (!out)
    {
        std::cerr << "Failed while writing manifest: " << outputPath << "\n";
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    Arguments args;
    if (!ParseArguments(argc, argv, args))
    {
        return 1;
    }

    fs::path serverRoot;
    fs::path dataDir;
    if (!ResolveDataDirectories(args.assetRoot, serverRoot, dataDir))
    {
        return 2;
    }

    fs::path outputPath = args.outputPath.empty() ? (dataDir / "assets-manifest.txt") : fs::absolute(args.outputPath);

    std::vector<ManifestEntry> entries;
    if (!CollectEntries(serverRoot, dataDir, args.markZipAsPackage, entries))
    {
        std::cerr << "No asset files found under: " << dataDir << "\n";
        return 3;
    }

    if (!WriteManifest(outputPath, entries, args.zipExtract))
    {
        return 4;
    }

    std::cout << "Manifest generated: " << outputPath << "\n";
    std::cout << "Entries: " << entries.size() << "\n";
    return 0;
}
