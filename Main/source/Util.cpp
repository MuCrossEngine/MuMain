#include "StdAfx.h"
#include "Util.h"
#include <Rpc.h>
#include <locale>
#include <codecvt>

#pragma comment(lib,"Rpcrt4.lib")

static BYTE bBuxCode[3] = { 0xfc, 0xcf, 0xab };
std::string path_global;

std::string GetFileNameWithExtension(const std::string& filePath)
{
	size_t slashPos = filePath.find_last_of("\\/"); // Encuentra la última barra inclinada.

	if (slashPos != std::string::npos)
	{
		return filePath.substr(slashPos + 1);
	}
	else
	{
		return filePath; // Si no se encuentra una barra inclinada, devolver toda la ruta.
	}
}

void changeExtension(std::string& FileExport)
{
	size_t Success = -1;

	if ((Success = FileExport.find(".TGA"), Success != -1))
	{
		FileExport = FileExport.replace(Success, 4, ".OZT");
		return;
	}
	if ((Success = FileExport.find(".tga"), Success != -1))
	{
		FileExport = FileExport.replace(Success, 4, ".ozt");
		return;
	}
	if ((Success = FileExport.find(".JPG"), Success != -1))
	{
		FileExport = FileExport.replace(Success, 4, ".OZJ");
		return;
	}
	if ((Success = FileExport.find(".jpg"), Success != -1))
	{
		FileExport = FileExport.replace(Success, 4, ".ozj");
		return;
	}
}

void BuxConvert(BYTE* pbyBuffer, int Size)
{
	for (int i = 0; i < Size; i++)
	{
		pbyBuffer[i] ^= bBuxCode[i % 3];
	}
}

DWORD GenerateCheckSum2(BYTE* pbyBuffer, int dwSize, WORD Key)
{
	DWORD dwKey = (DWORD)Key;
	DWORD dwResult = (DWORD)(Key << 9);

	for (int dwChecked = 0; dwChecked <= dwSize - 4; dwChecked += 4)
	{
		DWORD dwTemp;

		memcpy(&dwTemp, pbyBuffer + dwChecked, 4);

		DWORD v4 = (Key + (dwChecked >> 2)) % 2;
		switch (v4)
		{
		case 0:
			dwResult ^= dwTemp;
			break;
		case 1:
			dwResult += dwTemp;
			break;
		}

		if (!(dwChecked % 0x10))
		{
			dwResult ^= (unsigned int)(dwResult + dwKey) >> ((dwChecked >> 2) % 8 + 1);
		}
	}
	return dwResult;
}

void PackFileEncrypt(std::string filename, BYTE* pbyBuffer, int MAX_LINE, int Size)
{
	FILE* fp = fopen(filename.c_str(), "wb");

	if (fp != NULL)
	{
		DWORD MAX_BUFFER = (MAX_LINE * Size);

		BuxConvert(pbyBuffer, MAX_BUFFER);
		fwrite(pbyBuffer, MAX_BUFFER, 1u, fp);
		fclose(fp);
	}
	else
	{
		char Text[256];
		sprintf_s(Text, "%s - File not exist.", filename.c_str());
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
	}
}

void PackFileEncrypt(std::string filename, BYTE* pbyBuffer, int MAX_LINE, int Size, DWORD Key, bool WriteMax, bool CheckSum)
{
	FILE* fp = fopen(filename.c_str(), "wb");

	if (fp != NULL)
	{
		DWORD MAX_BUFFER = (MAX_LINE * Size);

		BYTE* Buffer = new BYTE[MAX_BUFFER];

		if (WriteMax == true)
		{
			fwrite(&MAX_LINE, 4u, 1u, fp);
		}

		for (int i = 0; i < MAX_LINE; ++i)
		{
			BuxConvert(pbyBuffer, Size);

			memcpy((char*)(Buffer + Size * i), pbyBuffer, Size);

			fwrite((char*)(Buffer + Size * i), Size, 1u, fp);

			pbyBuffer += Size;
		}

		if (CheckSum)
		{
			DWORD dwCheckSum = GenerateCheckSum2(Buffer, MAX_BUFFER, Key);

			fwrite(&dwCheckSum, 4u, 1u, fp);
		}

		fclose(fp);

		delete[] Buffer;
	}
	else
	{
		char Text[256];
		sprintf_s(Text, "%s - File not exist.", filename.c_str());
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
	}
}

int PackFileDecrypt(std::string filename, BYTE** pbyBuffer, int MAX_LINE, int Size, DWORD Key)
{
	FILE* fp = fopen(filename.c_str(), "rb");

	if (fp != NULL)
	{
		if (MAX_LINE == 0)
		{
			fread(&MAX_LINE, 4u, 1u, fp);
		}
		else
		{
			fseek(fp, 0, SEEK_END);
			long fileSize = (ftell(fp) - 4);
			fseek(fp, 0, SEEK_SET);

			if (fileSize > 0 && MAX_LINE != (int)(fileSize / Size))
			{
				MAX_LINE = (fileSize / Size);
			}
		}

		if (MAX_LINE > 0)
		{
			int MAX_BUFFER = MAX_LINE * Size;
			std::vector<BYTE> buffer(MAX_BUFFER);

			fread(buffer.data(), MAX_BUFFER, 1u, fp);
			DWORD dwCheckSum;
			fread(&dwCheckSum, sizeof(DWORD), 1u, fp);
			fclose(fp);

			bool success = false;

			if (Key != 0)
			{
				success = (dwCheckSum != GenerateCheckSum2(buffer.data(), MAX_BUFFER, Key));
			}

			if (success)
			{
				char Text[256];
				sprintf_s(Text, "%s - File corrupted.", filename.c_str());
				MessageBox(NULL, Text, "TxtConvertor", MB_OK);
				return 0;
			}
			else
			{
				BYTE* pSeek = buffer.data();
				(*pbyBuffer) = new BYTE[MAX_BUFFER];

				for (int i = 0; i < MAX_LINE; i++)
				{
					BuxConvert(pSeek, Size);
					memcpy((char*)((*pbyBuffer) + Size * i), pSeek, Size);
					pSeek += Size;
				}
			}
		}
		else
		{
			MessageBox(NULL, "Data Count 0", "TxtConvertor", MB_OK);
			fclose(fp);
		}
	}
	return MAX_LINE;
}

void copy_new_file(std::string filename, std::string filesave)
{
	FILE* fp = fopen(filename.c_str(), "rb");

	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		long fileSize = (ftell(fp) - 4);
		fseek(fp, 0, SEEK_SET);

		DWORD Count = 0;
		fread(&Count, sizeof(DWORD), 1, fp);

		std::vector<BYTE> Buffer(fileSize);
		fread(Buffer.data(), fileSize, 1, fp);

		fclose(fp);

		fp = fopen(filesave.c_str(), "wb");
		fwrite(Buffer.data(), fileSize, 1u, fp);
		fclose(fp);
	}
}

void GetModulePath(HMODULE hModule, char* path_module)
{
	wchar_t buffer[MAX_PATH];
	DWORD result = GetModuleFileNameW(hModule, buffer, MAX_PATH);

	if (result == 0 || result >= MAX_PATH)
	{
		DWORD error = GetLastError();
		std::cerr << "GetModuleFileNameW failed with error: " << error << std::endl;
		MessageBox(0, "Failed to get module path!", "Error", MB_OK | MB_ICONERROR);
		path_module[0] = '\0';
		return;
	}

	//std::wstring ();
	std::string fullPath = WStringToString(buffer);

	strncpy_s(path_module, MAX_PATH, fullPath.c_str(), MAX_PATH - 1);
	path_module[MAX_PATH - 1] = '\0';
}

std::string GetExecutablePath(const std::string& wstr)
{
	if (path_global.empty())
	{
		wchar_t buffer[MAX_PATH];

		GetModuleFileNameW(NULL, buffer, MAX_PATH);

		std::wstring fullPath(buffer);

		size_t pos = fullPath.find_last_of(L"\\/");

		if (pos != std::wstring::npos)
		{
			fullPath = fullPath.substr(0, pos) + L"\\";
		}

		path_global = WStringToString(fullPath);
	}
	return (path_global + wstr);
}

std::string WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

void create_hwid_system(char* ComputerHardwareId)
{
	DWORD VolumeSerialNumber;

	if (GetVolumeInformation("C:\\", 0, 0, &VolumeSerialNumber, 0, 0, 0, 0) == 0)
	{
		return;
	}

	UUID uuid;

	UuidCreateSequential(&uuid);

	SYSTEM_INFO SystemInfo;

	GetSystemInfo(&SystemInfo);

	DWORD ComputerHardwareId1 = VolumeSerialNumber ^ 0x12B586FE;

	DWORD ComputerHardwareId2 = *(DWORD*)(&uuid.Data4[2]) ^ 0x5D78A569;

	DWORD ComputerHardwareId3 = ((*(WORD*)(&uuid.Data4[6]) & 0xFFFF) | (SystemInfo.wProcessorArchitecture << 16)) ^ 0xF45BC123;

	DWORD ComputerHardwareId4 = ((SystemInfo.wProcessorLevel & 0xFFFF) | (SystemInfo.wProcessorRevision << 16)) ^ 0xB542D8E1;

	sprintf(ComputerHardwareId, "%08X-%08X-%08X-%08X", ComputerHardwareId1, ComputerHardwareId2, ComputerHardwareId3, ComputerHardwareId4);
}

void textwrite(std::string& buffer, bool line, FILE* fp)
{
	if (line) buffer += "\n";

	fwrite(buffer.data(), 1, buffer.size(), fp);
	buffer.clear();
}

float Render22(float a1, float a2)
{
	float v5 = 0.0f;
	float v2 = a2 * 0.01745f;

	float work = a1 / 100.f;

	float v6 = (float)((int)(v2 * 1000.0f * work + timeGetTime()) % (int)(6283.185546875f * work)) * 0.001f * a1;

	if (v6 != 0.f)
	{
		if (v6 >= 3.14f)
			v5 = cosf(v6);
		else
			v5 = -cosf(v6);
	}
	return (float)((v5 + 1.0f) * 0.5f);
}

float sub_11B5EE9(float arg1, float arg2, int arg3)
{
	int v6 = (int)((float)arg3 * arg1);
	int v4 = (int)((float)arg3 * arg2);
	float v7 = (v6 + rand() % (v4 - v6 + 1)) / (float)arg3;

	if (v7 < arg1)
		return arg1;
	if (v7 > arg2)
		v7 = arg2;

	return v7;
}