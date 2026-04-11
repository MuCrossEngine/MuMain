#pragma once

class ConnectVersionHex
{
public:
	ConnectVersionHex();
	virtual ~ConnectVersionHex();

	static ConnectVersionHex* Instance();

	void Initialize();
	void OpenUpdater();
	void execute_launcher();
	void SetClientVersion(int version);
	int GetClientVersion();
	bool GetUpdateVersion();
	void SetUpdateVersion(bool bEnable);
	void write_received_file();
	void write_data_current(WORD fileVersion, DWORD offset, DWORD chunkSize, DWORD totalSize, BYTE* data);
	void data_end();
	void RenderInterface(HDC hDC);
	void download_information(WORD fileVersion, DWORD chunkSize, DWORD totalSize, float bProgress);

private:
	int ClientVersion;
	bool CheckUpdateVersion;
	bool OpenLauncher;

	WORD FileVersion;
	DWORD ChunkSize;
	DWORD TotalSize;
	float Percentage;
};

#define GMConnectHex			(ConnectVersionHex::Instance())