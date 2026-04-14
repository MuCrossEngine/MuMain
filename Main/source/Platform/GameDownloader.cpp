#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// GameDownloader.cpp
// Downloads required game data on first run.
// Uses legacy GameShop FileDownloader/HTTPConnecter for transport to maximize
// original code reuse.
// ─────────────────────────────────────────────────────────────────────────────
#include "GameDownloader.h"
#include "GameAssetPath.h"
#include "AndroidEglWindow.h"
#include "Platform/AndroidWin32Compat.h"
#include "GameShop/ShopListManager/interface/FileDownloader.h"
#include "../../../Util/CCRC32.H"
#include <GLES3/gl3.h>
#include <android/log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define LOG_TAG "MUAssets"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Files that must be present for the game to run (relative to data dir)
// ─────────────────────────────────────────────────────────────────────────────
static const char* k_requiredFiles[] = {
    "Data/av-code45.pak",
    "Data/Local/Connect.msil",
    nullptr
};
static const char* k_manifestRelativePath = "Data/assets-manifest.txt";
static const std::time_t k_manifestRefreshIntervalSeconds = 6 * 60 * 60;

static std::string s_serverURL = "http://127.0.0.1/mu_assets"; // override via SetServerURL
extern bool AndroidExtractZipArchive(const char* zipPath, const char* targetDir);

namespace GameDownloader
{

void SetServerURL(const char* url) { if (url) s_serverURL = url; }
const char* GetServerURL()         { return s_serverURL.c_str(); }

struct AssetServerEndpoint
{
    std::string host;
    INTERNET_PORT port;
    std::string basePath;
};

struct AssetEntry
{
    std::string relativePath;
    bool hasExpectedCRC;
    unsigned long expectedCRC;
    bool isArchivePackage;
    std::string extractRelativePath;
};

// ── Simple progress bar rendered via GLES3 ───────────────────────────────────
static void RenderProgress(AndroidEglWindow* win, float progress, const char* label)
{
    if (!win) return;
    glViewport(0, 0, win->GetWidth(), win->GetHeight());
    glClearColor(0.05f, 0.05f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: draw actual progress bar with text using AndroidTextRenderer
    // For now just clear + swap (shows a dark blue screen during download)
    (void)progress; (void)label;

    win->SwapBuffers();
}

static bool ParseAssetServerURL(const std::string& rawUrl, AssetServerEndpoint& outEndpoint)
{
    outEndpoint.host.clear();
    outEndpoint.port = INTERNET_DEFAULT_HTTP_PORT;
    outEndpoint.basePath = "/";

    if (rawUrl.empty())
    {
        return false;
    }

    std::string url = rawUrl;
    const std::size_t schemePos = url.find("://");
    if (schemePos != std::string::npos)
    {
        std::string scheme = url.substr(0, schemePos);
        std::transform(scheme.begin(), scheme.end(), scheme.begin(), [](unsigned char c) { return (char)std::tolower(c); });
        if (scheme != "http")
        {
            LOGE("Unsupported asset server scheme: %s", scheme.c_str());
            return false;
        }
        url = url.substr(schemePos + 3);
    }

    if (url.empty())
    {
        return false;
    }

    const std::size_t pathPos = url.find('/');
    std::string hostPort = (pathPos == std::string::npos) ? url : url.substr(0, pathPos);
    std::string basePath = (pathPos == std::string::npos) ? "/" : url.substr(pathPos);

    if (hostPort.empty())
    {
        return false;
    }

    const std::size_t portSep = hostPort.rfind(':');
    if (portSep != std::string::npos)
    {
        const std::string host = hostPort.substr(0, portSep);
        const std::string portText = hostPort.substr(portSep + 1);
        if (host.empty() || portText.empty())
        {
            return false;
        }

        const long parsedPort = std::strtol(portText.c_str(), nullptr, 10);
        if (parsedPort <= 0 || parsedPort > 65535)
        {
            LOGE("Invalid asset server port in URL: %s", rawUrl.c_str());
            return false;
        }

        outEndpoint.host = host;
        outEndpoint.port = (INTERNET_PORT)parsedPort;
    }
    else
    {
        outEndpoint.host = hostPort;
    }

    while (basePath.size() > 1 && basePath.back() == '/')
    {
        basePath.pop_back();
    }
    outEndpoint.basePath = basePath.empty() ? "/" : basePath;
    return true;
}

static bool EnsureDirectoryTree(const std::string& directoryPath)
{
    if (directoryPath.empty())
    {
        return true;
    }

    std::string current;
    std::size_t cursor = 0;
    if (directoryPath[0] == '/')
    {
        current = "/";
        cursor = 1;
    }

    while (cursor <= directoryPath.size())
    {
        const std::size_t slashPos = directoryPath.find('/', cursor);
        const std::size_t partEnd = (slashPos == std::string::npos) ? directoryPath.size() : slashPos;
        const std::string part = directoryPath.substr(cursor, partEnd - cursor);

        if (!part.empty())
        {
            if (!current.empty() && current.back() != '/')
            {
                current.push_back('/');
            }
            current += part;

            if (mkdir(current.c_str(), 0755) != 0 && errno != EEXIST)
            {
                LOGE("Failed to create directory: %s (errno=%d)", current.c_str(), errno);
                return false;
            }
        }

        if (slashPos == std::string::npos)
        {
            break;
        }
        cursor = slashPos + 1;
    }

    return true;
}

static std::string BuildRemoteFilePath(const AssetServerEndpoint& endpoint, const char* relativePath)
{
    std::string rel = relativePath ? relativePath : "";
    while (!rel.empty() && rel[0] == '/')
    {
        rel.erase(rel.begin());
    }

    std::string remotePath = endpoint.basePath.empty() ? "/" : endpoint.basePath;
    if (!remotePath.empty() && remotePath.back() != '/')
    {
        remotePath.push_back('/');
    }
    remotePath += rel;
    return remotePath;
}

static void BuildFallbackRequiredFiles(std::vector<AssetEntry>& outFiles)
{
    outFiles.clear();
    for (int i = 0; k_requiredFiles[i]; ++i)
    {
        AssetEntry entry;
        entry.relativePath = k_requiredFiles[i];
        entry.hasExpectedCRC = false;
        entry.expectedCRC = 0;
        entry.isArchivePackage = false;
        entry.extractRelativePath.clear();
        outFiles.push_back(entry);
    }
}

static bool DownloadLegacyFile(const AssetServerEndpoint& endpoint,
                               const std::string& remotePath,
                               const std::string& localPath,
                               const std::string& fileName,
                               IDownloaderStateEvent* stateEvent,
                               bool overwrite,
                               WZResult* outResult)
{
    DownloadServerInfo serverInfo;
    serverInfo.SetPassiveMode(FALSE);
    serverInfo.SetOverWrite(overwrite ? TRUE : FALSE);
    serverInfo.SetDownloaderType(HTTP);
    serverInfo.SetConnectTimeout(0);      // synchronous path on Android
    serverInfo.SetReadBufferSize(64 * 1024);
    serverInfo.SetServerInfo((TCHAR*)endpoint.host.c_str(),
                             endpoint.port,
                             (TCHAR*)"",
                             (TCHAR*)"");

    DownloadFileInfo fileInfo;
    fileInfo.SetFilePath((TCHAR*)fileName.c_str(),
                         (TCHAR*)localPath.c_str(),
                         (TCHAR*)remotePath.c_str(),
                         nullptr);

    FileDownloader downloader(stateEvent, &serverInfo, &fileInfo);
    WZResult result = downloader.DownloadFile();
    if (outResult)
    {
        *outResult = result;
    }

    return result.IsSuccess();
}

static bool ReadSmallTextFile(const std::string& path, std::string& outText)
{
    outText.clear();

    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp)
    {
        return false;
    }

    char buffer[128];
    while (!feof(fp))
    {
        size_t count = fread(buffer, 1, sizeof(buffer), fp);
        if (count > 0)
        {
            outText.append(buffer, count);
        }
    }
    fclose(fp);

    return !outText.empty();
}

static std::string TrimSpaces(std::string value)
{
    std::string::size_type begin = 0;
    while (begin < value.size() && std::isspace((unsigned char)value[begin]))
    {
        ++begin;
    }

    std::string::size_type end = value.size();
    while (end > begin && std::isspace((unsigned char)value[end - 1]))
    {
        --end;
    }

    return value.substr(begin, end - begin);
}

static std::string NormalizeRelativePath(std::string path)
{
    path = TrimSpaces(path);
    std::replace(path.begin(), path.end(), '\\', '/');

    while (!path.empty() && (path[0] == '/' || path[0] == '.'))
    {
        if (path.rfind("./", 0) == 0)
        {
            path.erase(0, 2);
            continue;
        }
        if (path[0] == '/')
        {
            path.erase(path.begin());
            continue;
        }
        break;
    }

    while (!path.empty() && path.back() == '/')
    {
        path.pop_back();
    }

    return path;
}

static bool ParseCRC32Text(const std::string& rawText, unsigned long& outCRC);

static std::string ToLowerASCII(std::string value)
{
    for (char& c : value)
    {
        c = (char)std::tolower((unsigned char)c);
    }
    return value;
}

static bool StartsWithIgnoreCase(const std::string& value, const char* prefix)
{
    if (!prefix)
    {
        return false;
    }

    std::size_t prefixLen = std::strlen(prefix);
    if (value.size() < prefixLen)
    {
        return false;
    }

    for (std::size_t i = 0; i < prefixLen; ++i)
    {
        if (std::tolower((unsigned char)value[i]) != std::tolower((unsigned char)prefix[i]))
        {
            return false;
        }
    }
    return true;
}

static std::string TrimOptionalQuotes(std::string value)
{
    value = TrimSpaces(value);
    if (value.size() >= 2)
    {
        const char first = value.front();
        const char last = value.back();
        if ((first == '"' && last == '"') || (first == '\'' && last == '\''))
        {
            value = value.substr(1, value.size() - 2);
        }
    }
    return TrimSpaces(value);
}

static std::string NormalizeExtractRelativePath(std::string path)
{
    path = TrimOptionalQuotes(path);
    path = NormalizeRelativePath(path);
    if (path == ".")
    {
        path.clear();
    }
    return path;
}

static bool ParseManifestBoolToken(const std::string& tokenValue)
{
    std::string value = ToLowerASCII(TrimSpaces(tokenValue));
    if (value.empty())
    {
        return false;
    }

    return (value == "1" || value == "true" || value == "yes" ||
            value == "on" || value == "zip" || value == "archive");
}

static bool HasZipExtension(const std::string& relativePath)
{
    std::string lower = ToLowerASCII(relativePath);
    return (lower.size() >= 4 && lower.compare(lower.size() - 4, 4, ".zip") == 0);
}

static void SplitManifestTokens(const std::string& line, std::vector<std::string>& outTokens)
{
    outTokens.clear();

    char delimiter = '\0';
    if (line.find('|') != std::string::npos)
    {
        delimiter = '|';
    }
    else if (line.find(',') != std::string::npos)
    {
        delimiter = ',';
    }
    else if (line.find('\t') != std::string::npos)
    {
        delimiter = '\t';
    }

    if (delimiter != '\0')
    {
        std::size_t cursor = 0;
        while (cursor <= line.size())
        {
            std::size_t splitPos = line.find(delimiter, cursor);
            std::string token = (splitPos == std::string::npos)
                ? line.substr(cursor)
                : line.substr(cursor, splitPos - cursor);
            token = TrimSpaces(token);
            if (!token.empty())
            {
                outTokens.push_back(token);
            }

            if (splitPos == std::string::npos)
            {
                break;
            }
            cursor = splitPos + 1;
        }
        return;
    }

    std::size_t cursor = 0;
    while (cursor < line.size())
    {
        while (cursor < line.size() && std::isspace((unsigned char)line[cursor]))
        {
            ++cursor;
        }
        if (cursor >= line.size())
        {
            break;
        }

        std::size_t end = cursor;
        while (end < line.size() && !std::isspace((unsigned char)line[end]))
        {
            ++end;
        }

        std::string token = TrimSpaces(line.substr(cursor, end - cursor));
        if (!token.empty())
        {
            outTokens.push_back(token);
        }
        cursor = end;
    }
}

static bool IsLikelyUnkeyedCRCToken(const std::string& token)
{
    if (token.rfind("0x", 0) == 0 || token.rfind("0X", 0) == 0)
    {
        return true;
    }

    bool allHex = !token.empty();
    bool hasHexAlpha = false;
    for (char c : token)
    {
        if (!std::isxdigit((unsigned char)c))
        {
            allHex = false;
            break;
        }
        if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
        {
            hasHexAlpha = true;
        }
    }

    if (allHex && hasHexAlpha)
    {
        return true;
    }

    return allHex && token.size() == 8;
}

static bool TryParseManifestCRCToken(const std::string& token, bool& outIsCRC, unsigned long& outCRC)
{
    outIsCRC = false;

    std::string text = TrimSpaces(token);
    if (text.empty())
    {
        return true;
    }

    const std::size_t eqPos = text.find('=');
    if (eqPos != std::string::npos)
    {
        std::string key = ToLowerASCII(TrimSpaces(text.substr(0, eqPos)));
        if (key == "crc" || key == "crc32" || key == "checksum")
        {
            std::string value = TrimSpaces(text.substr(eqPos + 1));
            if (value.empty() || !ParseCRC32Text(value, outCRC))
            {
                return false;
            }
            outIsCRC = true;
        }
        return true;
    }

    if (!IsLikelyUnkeyedCRCToken(text))
    {
        return true;
    }

    if (!ParseCRC32Text(text, outCRC))
    {
        return false;
    }

    outIsCRC = true;
    return true;
}

static bool TryParseManifestPathToken(const std::string& token, std::string& outPath)
{
    std::string text = TrimSpaces(token);
    if (text.empty())
    {
        return false;
    }

    std::string lower = ToLowerASCII(text);
    const std::size_t eqPos = text.find('=');
    if (eqPos != std::string::npos)
    {
        const std::string key = ToLowerASCII(TrimSpaces(text.substr(0, eqPos)));
        if (key == "path" || key == "file" || key == "name")
        {
            text = text.substr(eqPos + 1);
            lower = ToLowerASCII(text);
        }
        else if (key == "crc" || key == "crc32" || key == "checksum" ||
                 key == "size" || key == "bytes" || key == "length" ||
                 key == "archive" || key == "bundle" || key == "package" || key == "zip" ||
                 key == "extract" || key == "extract_to" || key == "extractto" ||
                 key == "target" || key == "destination" || key == "dest")
        {
            return false;
        }
    }
    else if (StartsWithIgnoreCase(lower, "crc=") || StartsWithIgnoreCase(lower, "crc32="))
    {
        return false;
    }

    text = TrimOptionalQuotes(text);
    text = NormalizeRelativePath(text);
    if (text.empty())
    {
        return false;
    }

    unsigned long numericValue = 0;
    if (ParseCRC32Text(text, numericValue) &&
        text.find('/') == std::string::npos &&
        text.find('.') == std::string::npos)
    {
        return false;
    }

    outPath = text;
    return true;
}

static std::string SelectBestManifestPath(const std::vector<std::string>& candidates)
{
    if (candidates.empty())
    {
        return "";
    }

    for (const std::string& value : candidates)
    {
        if (value.find('/') != std::string::npos)
        {
            return value;
        }
    }

    for (const std::string& value : candidates)
    {
        if (value.find('.') != std::string::npos)
        {
            return value;
        }
    }

    return candidates.front();
}

static bool ParseManifestText(const std::string& manifestText, std::vector<AssetEntry>& outFiles)
{
    outFiles.clear();

    std::size_t cursor = 0;
    while (cursor <= manifestText.size())
    {
        std::size_t lineEnd = manifestText.find('\n', cursor);
        std::string line = (lineEnd == std::string::npos)
            ? manifestText.substr(cursor)
            : manifestText.substr(cursor, lineEnd - cursor);

        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        line = TrimSpaces(line);

        if (!line.empty() && line[0] != '#' && line[0] != ';')
        {
            std::vector<std::string> tokens;
            SplitManifestTokens(line, tokens);

            bool hasExpectedCRC = false;
            unsigned long expectedCRC = 0;
            bool isArchivePackage = false;
            std::string extractRelativePath;
            std::vector<std::string> pathCandidates;

            for (const std::string& token : tokens)
            {
                if (token.empty())
                {
                    continue;
                }

                if (token[0] == '#' || token[0] == ';')
                {
                    break;
                }

                const std::size_t tokenEqPos = token.find('=');
                if (tokenEqPos != std::string::npos)
                {
                    const std::string key = ToLowerASCII(TrimSpaces(token.substr(0, tokenEqPos)));
                    const std::string value = TrimSpaces(token.substr(tokenEqPos + 1));
                    if (key == "extract" || key == "extract_to" || key == "extractto" ||
                        key == "target" || key == "destination" || key == "dest")
                    {
                        extractRelativePath = NormalizeExtractRelativePath(value);
                        continue;
                    }

                    if (key == "archive" || key == "bundle" || key == "package" || key == "zip")
                    {
                        if (ParseManifestBoolToken(value))
                        {
                            isArchivePackage = true;
                        }
                        continue;
                    }
                }

                bool isCRC = false;
                unsigned long parsedCRC = 0;
                if (!TryParseManifestCRCToken(token, isCRC, parsedCRC))
                {
                    LOGE("Manifest CRC parse error token=%s line=%s", token.c_str(), line.c_str());
                    return false;
                }

                if (isCRC)
                {
                    if (!hasExpectedCRC)
                    {
                        hasExpectedCRC = true;
                        expectedCRC = parsedCRC;
                    }
                    continue;
                }

                std::string pathToken;
                if (TryParseManifestPathToken(token, pathToken))
                {
                    pathCandidates.push_back(pathToken);
                }
            }

            std::string relativePath = SelectBestManifestPath(pathCandidates);
            if (!relativePath.empty())
            {
                if (HasZipExtension(relativePath))
                {
                    isArchivePackage = true;
                }

                bool merged = false;
                for (AssetEntry& existing : outFiles)
                {
                    if (existing.relativePath == relativePath)
                    {
                        if (!existing.hasExpectedCRC && hasExpectedCRC)
                        {
                            existing.hasExpectedCRC = true;
                            existing.expectedCRC = expectedCRC;
                        }
                        if (!existing.isArchivePackage && isArchivePackage)
                        {
                            existing.isArchivePackage = true;
                        }
                        if (existing.extractRelativePath.empty() && !extractRelativePath.empty())
                        {
                            existing.extractRelativePath = extractRelativePath;
                        }
                        merged = true;
                        break;
                    }
                }

                if (!merged)
                {
                    AssetEntry entry;
                    entry.relativePath = relativePath;
                    entry.hasExpectedCRC = hasExpectedCRC;
                    entry.expectedCRC = expectedCRC;
                    entry.isArchivePackage = isArchivePackage;
                    entry.extractRelativePath = extractRelativePath;
                    outFiles.push_back(entry);
                }
            }
        }

        if (lineEnd == std::string::npos)
        {
            break;
        }
        cursor = lineEnd + 1;
    }

    return !outFiles.empty();
}

static bool LoadManifestFile(const std::string& manifestLocalPath, std::vector<AssetEntry>& outFiles)
{
    std::string text;
    if (!ReadSmallTextFile(manifestLocalPath, text))
    {
        return false;
    }

    if (!ParseManifestText(text, outFiles))
    {
        LOGE("Invalid or empty manifest file: %s", manifestLocalPath.c_str());
        return false;
    }

    return true;
}

static bool LoadRequiredFilesFromLocalManifest(std::vector<AssetEntry>& outFiles)
{
    const std::string localManifest = GameAssetPath::Resolve(k_manifestRelativePath);
    return LoadManifestFile(localManifest, outFiles);
}

static bool DownloadRequiredFilesManifest(const AssetServerEndpoint& endpoint, std::vector<AssetEntry>& outFiles)
{
    const std::string localManifest = GameAssetPath::Resolve(k_manifestRelativePath);
    const std::string tempManifest = localManifest + ".tmp";
    const std::size_t sep = localManifest.find_last_of('/');
    if (sep != std::string::npos && !EnsureDirectoryTree(localManifest.substr(0, sep)))
    {
        return false;
    }

    const std::string remoteManifest = BuildRemoteFilePath(endpoint, k_manifestRelativePath);
    WZResult result;
    if (!DownloadLegacyFile(endpoint, remoteManifest, tempManifest, "assets-manifest.txt", nullptr, true, &result))
    {
        LOGI("Remote manifest unavailable (fallback to default list): %s", result.GetErrorMessage());
        return false;
    }

    if (!LoadManifestFile(tempManifest, outFiles))
    {
        LOGE("Manifest download succeeded but parse failed: %s", tempManifest.c_str());
        DeleteFile(tempManifest.c_str());
        return false;
    }

    DeleteFile(localManifest.c_str());
    if (std::rename(tempManifest.c_str(), localManifest.c_str()) != 0)
    {
        LOGE("Failed to move manifest temp file into place: %s -> %s",
             tempManifest.c_str(), localManifest.c_str());
        DeleteFile(tempManifest.c_str());
        return false;
    }

    return true;
}

static bool ParseCRC32Text(const std::string& rawText, unsigned long& outCRC)
{
    std::string text = TrimSpaces(rawText);
    if (text.empty())
    {
        return false;
    }

    const std::size_t eqPos = text.find('=');
    if (eqPos != std::string::npos)
    {
        text = text.substr(eqPos + 1);
    }
    text = TrimSpaces(text);
    if (text.empty())
    {
        return false;
    }

    bool hasHexAlpha = false;
    for (char c : text)
    {
        if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
        {
            hasHexAlpha = true;
            break;
        }
    }

    int base = 10;
    if (text.rfind("0x", 0) == 0 || text.rfind("0X", 0) == 0 || hasHexAlpha)
    {
        base = 16;
    }

    char* endPtr = nullptr;
    unsigned long parsed = std::strtoul(text.c_str(), &endPtr, base);
    if (!endPtr || *endPtr != '\0')
    {
        return false;
    }

    outCRC = parsed;
    return true;
}

static bool ComputeFileCRC(const std::string& localPath, unsigned long& outCRC)
{
    CCRC32 crc32;
    return crc32.FileCRC(localPath.c_str(), &outCRC, 1024 * 1024);
}

static bool ValidateLocalFileCRCExpected(const std::string& localPath,
                                         const std::string& label,
                                         unsigned long expectedCRC,
                                         bool deleteOnMismatch)
{
    unsigned long actualCRC = 0;
    if (!ComputeFileCRC(localPath, actualCRC))
    {
        LOGE("CRC computation failed for %s", localPath.c_str());
        return false;
    }

    if (actualCRC != expectedCRC)
    {
        LOGE("CRC mismatch for %s expected=0x%08lX actual=0x%08lX",
             label.c_str(), expectedCRC, actualCRC);
        if (deleteOnMismatch)
        {
            DeleteFile(localPath.c_str());
        }
        return false;
    }

    LOGI("CRC valid for %s (0x%08lX)", label.c_str(), actualCRC);
    return true;
}

static bool ValidateDownloadedFileCRC(const AssetServerEndpoint& endpoint,
                                      const std::string& remotePath,
                                      const std::string& localPath,
                                      const std::string& label,
                                      bool hasExpectedCRC,
                                      unsigned long expectedCRC)
{
    if (hasExpectedCRC)
    {
        return ValidateLocalFileCRCExpected(localPath, label, expectedCRC, true);
    }

    std::string crcRemotePath = remotePath + ".crc32";
    std::string crcLocalPath = localPath + ".crc32";
    std::string crcFileName = label + ".crc32";

    WZResult crcDownloadResult;
    if (!DownloadLegacyFile(endpoint, crcRemotePath, crcLocalPath, crcFileName, nullptr, true, &crcDownloadResult))
    {
        LOGI("CRC sidecar not available for %s (optional): %s",
             label.c_str(), crcDownloadResult.GetErrorMessage());
        return true;
    }

    std::string crcText;
    if (!ReadSmallTextFile(crcLocalPath, crcText))
    {
        LOGE("CRC sidecar unreadable for %s: %s", label.c_str(), crcLocalPath.c_str());
        return false;
    }

    unsigned long sidecarExpectedCRC = 0;
    if (!ParseCRC32Text(crcText, sidecarExpectedCRC))
    {
        LOGE("CRC sidecar format invalid for %s: %s", label.c_str(), crcText.c_str());
        return false;
    }
    return ValidateLocalFileCRCExpected(localPath, label, sidecarExpectedCRC, true);
}

static std::string BuildArchiveExtractMarkerPath(const std::string& archiveLocalPath)
{
    return archiveLocalPath + ".extracted";
}

static bool WriteSmallTextFile(const std::string& path, const std::string& text)
{
    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp)
    {
        return false;
    }

    const size_t written = fwrite(text.data(), 1, text.size(), fp);
    fclose(fp);
    return (written == text.size());
}

static bool ResolveArchiveExtractTarget(const AssetEntry& entry, std::string& outTargetDir)
{
    outTargetDir = GameAssetPath::GetBase();
    if (outTargetDir.empty())
    {
        return false;
    }

    if (!entry.extractRelativePath.empty())
    {
        outTargetDir = GameAssetPath::Resolve(entry.extractRelativePath.c_str());
    }

    return EnsureDirectoryTree(outTargetDir);
}

static bool HasArchiveExtractMarker(const std::string& archiveLocalPath)
{
    const std::string markerPath = BuildArchiveExtractMarkerPath(archiveLocalPath);
    struct stat st;
    return (stat(markerPath.c_str(), &st) == 0 && st.st_size > 0);
}

static void DeleteArchiveExtractMarker(const std::string& archiveLocalPath)
{
    DeleteFile(BuildArchiveExtractMarkerPath(archiveLocalPath).c_str());
}

static bool ExtractArchivePackage(const AssetEntry& entry, const std::string& archiveLocalPath)
{
    if (!entry.isArchivePackage)
    {
        return true;
    }

    if (!HasZipExtension(entry.relativePath))
    {
        LOGE("Unsupported package extension for extraction: %s", entry.relativePath.c_str());
        return false;
    }

    std::string targetDir;
    if (!ResolveArchiveExtractTarget(entry, targetDir))
    {
        LOGE("Failed to resolve extract target dir for: %s", entry.relativePath.c_str());
        return false;
    }

    LOGI("Extracting package zip=%s target=%s",
         archiveLocalPath.c_str(), targetDir.c_str());

    if (!AndroidExtractZipArchive(archiveLocalPath.c_str(), targetDir.c_str()))
    {
        LOGE("Archive extraction failed: %s", entry.relativePath.c_str());
        return false;
    }

    const std::string markerPath = BuildArchiveExtractMarkerPath(archiveLocalPath);
    if (!WriteSmallTextFile(markerPath, "ok\n"))
    {
        LOGE("Failed to write archive extraction marker: %s", markerPath.c_str());
        return false;
    }

    return true;
}

static bool IsManifestRefreshRequired(const std::string& manifestLocalPath)
{
    struct stat st;
    if (stat(manifestLocalPath.c_str(), &st) != 0 || st.st_mtime <= 0)
    {
        return false;
    }

    const std::time_t now = std::time(nullptr);
    if (now <= 0 || now < st.st_mtime)
    {
        return false;
    }

    const std::time_t ageSeconds = now - st.st_mtime;
    return ageSeconds >= k_manifestRefreshIntervalSeconds;
}

bool IsDataReady()
{
    std::vector<AssetEntry> requiredFiles;
    const bool hasLocalManifest = LoadRequiredFilesFromLocalManifest(requiredFiles);
    if (!hasLocalManifest)
    {
        BuildFallbackRequiredFiles(requiredFiles);
    }
    else
    {
        const std::string localManifest = GameAssetPath::Resolve(k_manifestRelativePath);
        if (IsManifestRefreshRequired(localManifest))
        {
            LOGI("Manifest refresh interval exceeded; forcing downloader check");
            return false;
        }
    }

    for (const AssetEntry& entry : requiredFiles)
    {
        std::string full = GameAssetPath::Resolve(entry.relativePath.c_str());
        struct stat st;
        if (stat(full.c_str(), &st) != 0 || st.st_size == 0)
        {
            LOGI("Missing: %s", full.c_str());
            return false;
        }

        if (entry.isArchivePackage && !HasArchiveExtractMarker(full))
        {
            LOGI("Archive extraction marker missing: %s", full.c_str());
            return false;
        }
    }
    return true;
}

class AndroidDownloadStateEvent final : public IDownloaderStateEvent
{
public:
    AndroidDownloadStateEvent(AndroidEglWindow* window, int fileIndex, int totalFiles, const char* label)
        : m_window(window),
          m_fileIndex(fileIndex),
          m_totalFiles(totalFiles > 0 ? totalFiles : 1),
          m_fileLength(0),
          m_label(label ? label : "")
    {
    }

    void OnStartedDownloadFile(TCHAR*, ULONGLONG fileLength) override
    {
        m_fileLength = fileLength;
        Render(0.0f);
    }

    void OnProgressDownloadFile(TCHAR*, ULONGLONG downloadedLength) override
    {
        if (m_fileLength == 0)
        {
            return;
        }

        float fileProgress = (float)((double)downloadedLength / (double)m_fileLength);
        if (fileProgress < 0.0f) fileProgress = 0.0f;
        if (fileProgress > 1.0f) fileProgress = 1.0f;
        Render(fileProgress);
    }

    void OnCompletedDownloadFile(TCHAR*, WZResult) override
    {
        Render(1.0f);
    }

private:
    void Render(float fileProgress)
    {
        const float overall = ((float)m_fileIndex + fileProgress) / (float)m_totalFiles;
        RenderProgress(m_window, overall, m_label.c_str());
    }

    AndroidEglWindow* m_window;
    int m_fileIndex;
    int m_totalFiles;
    ULONGLONG m_fileLength;
    std::string m_label;
};

static bool DownloadFile(const AssetServerEndpoint& endpoint,
                         const AssetEntry& entry,
                         const std::string& destPath,
                         AndroidEglWindow* eglWindow,
                         int fileIndex,
                         int totalFiles)
{
    const char* relativePath = entry.relativePath.c_str();
    if (!relativePath || !relativePath[0])
    {
        return false;
    }

    const std::size_t sep = destPath.find_last_of('/');
    if (sep != std::string::npos && !EnsureDirectoryTree(destPath.substr(0, sep)))
    {
        return false;
    }

    std::string fileName = relativePath;
    const std::size_t nameSep = fileName.find_last_of('/');
    if (nameSep != std::string::npos)
    {
        fileName = fileName.substr(nameSep + 1);
    }
    if (fileName.empty())
    {
        fileName = "asset.bin";
    }

    const std::string remotePath = BuildRemoteFilePath(endpoint, relativePath);
    LOGI("Downloading host=%s port=%u remote=%s local=%s",
         endpoint.host.c_str(),
         (unsigned)endpoint.port,
         remotePath.c_str(),
         destPath.c_str());

    AndroidDownloadStateEvent stateEvent(eglWindow, fileIndex, totalFiles, relativePath);
    WZResult result;
    if (!DownloadLegacyFile(endpoint, remotePath, destPath, fileName, &stateEvent, true, &result))
    {
        LOGE("Download failed: file=%s code=0x%08X winErr=%u msg=%s",
             relativePath,
             (unsigned)result.GetErrorCode(),
             (unsigned)result.GetWindowErrorCode(),
             result.GetErrorMessage());
        return false;
    }

    struct stat st;
    if (stat(destPath.c_str(), &st) != 0 || st.st_size <= 0)
    {
        LOGE("Downloaded file validation failed: %s", destPath.c_str());
        return false;
    }

    if (!ValidateDownloadedFileCRC(endpoint, remotePath, destPath, fileName, entry.hasExpectedCRC, entry.expectedCRC))
    {
        LOGE("CRC validation failed for: %s", relativePath);
        return false;
    }

    return true;
}

bool DownloadAll(AndroidEglWindow* eglWindow)
{
    AssetServerEndpoint endpoint;
    if (!ParseAssetServerURL(s_serverURL, endpoint))
    {
        LOGE("Invalid asset server URL: %s", s_serverURL.c_str());
        return false;
    }

    LOGI("Starting asset download host=%s port=%u base=%s",
         endpoint.host.c_str(),
         (unsigned)endpoint.port,
         endpoint.basePath.c_str());

    std::vector<AssetEntry> requiredFiles;
    BuildFallbackRequiredFiles(requiredFiles);

    std::vector<AssetEntry> manifestFiles;
    if (DownloadRequiredFilesManifest(endpoint, manifestFiles))
    {
        requiredFiles.swap(manifestFiles);
        LOGI("Using remote manifest with %zu assets", requiredFiles.size());
    }
    else if (LoadRequiredFilesFromLocalManifest(manifestFiles))
    {
        requiredFiles.swap(manifestFiles);
        LOGI("Using local manifest with %zu assets", requiredFiles.size());
    }
    else
    {
        LOGI("Using fallback required list with %zu assets", requiredFiles.size());
    }

    if (requiredFiles.empty())
    {
        LOGE("Required file list is empty");
        return false;
    }

    const int total = (int)requiredFiles.size();
    int current = 0;

    for (const AssetEntry& entry : requiredFiles)
    {
        std::string full = GameAssetPath::Resolve(entry.relativePath.c_str());

        RenderProgress(eglWindow, (float)current / total, entry.relativePath.c_str());

        struct stat st;
        if (stat(full.c_str(), &st) == 0 && st.st_size > 0)
        {
            bool localFileValid = true;
            if (entry.hasExpectedCRC)
            {
                if (ValidateLocalFileCRCExpected(full, entry.relativePath, entry.expectedCRC, false))
                {
                    localFileValid = true;
                }
                else
                {
                    localFileValid = false;
                    LOGI("Existing file CRC invalid, redownloading: %s", entry.relativePath.c_str());
                    DeleteFile(full.c_str());
                    if (entry.isArchivePackage)
                    {
                        DeleteArchiveExtractMarker(full);
                    }
                }
            }
            else
            {
                localFileValid = true;
            }

            if (localFileValid)
            {
                if (entry.isArchivePackage)
                {
                    if (!HasArchiveExtractMarker(full))
                    {
                        LOGI("Archive already downloaded but marker is missing, extracting: %s",
                             entry.relativePath.c_str());
                        if (!ExtractArchivePackage(entry, full))
                        {
                            LOGE("Failed to extract package: %s", entry.relativePath.c_str());
                            return false;
                        }
                    }

                    LOGI("Already have valid package: %s", entry.relativePath.c_str());
                }
                else
                {
                    LOGI("Already have: %s", entry.relativePath.c_str());
                }
                current++;
                continue;
            }
        }

        if (!DownloadFile(endpoint, entry, full, eglWindow, current, total))
        {
            LOGE("Failed to download: %s", entry.relativePath.c_str());
            return false;
        }

        if (entry.isArchivePackage)
        {
            DeleteArchiveExtractMarker(full);
            if (!ExtractArchivePackage(entry, full))
            {
                LOGE("Failed to extract package after download: %s", entry.relativePath.c_str());
                return false;
            }
        }
        current++;
    }

    RenderProgress(eglWindow, 1.f, "Done");
    LOGI("All assets ready");
    return true;
}

} // namespace GameDownloader

#endif // __ANDROID__
