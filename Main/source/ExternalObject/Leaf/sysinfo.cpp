#include "stdafx.h"

#include "rdtsc.h"
#include "sysinfo.h"
#include "intrinsics.h"

bool leaf::GetOSInfoString(OUT std::string& osinfo)
{
	OSVERSIONINFO osi;
	ZeroMemory(&osi, sizeof(OSVERSIONINFO));
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osi) == false)
		return false;
	
	bool bGetVersion = false;
	int iBuildNumberType = 0;
	switch(osi.dwMajorVersion)
	{
	case 3:	// NT 3.51
		{
			if(osi.dwMinorVersion == 51) {
				osinfo = "Windows NT 3.51";
				bGetVersion = true;
			}
		}
		break;
	case 4:
		{
			switch(osi.dwMinorVersion)
			{
			case 0:
				{
					switch(osi.dwPlatformId)
					{
					case VER_PLATFORM_WIN32_WINDOWS:
						{
							osinfo = "Windows 95";
							iBuildNumberType = 1;
							bGetVersion = true;
							if(osi.szCSDVersion[1] == 'C' || osi.szCSDVersion[1] == 'B'){
								osinfo += " OSR2";
							}
						}
						break;
					case VER_PLATFORM_WIN32_NT:
						{
							osinfo = "Windows NT 4.0";
							bGetVersion = true;
						}
						break;
					}
				}
				break;
			case 10:
				{
					osinfo = "Windows 98";
					iBuildNumberType = 1;
					bGetVersion = true;
					if(osi.szCSDVersion[1] == 'A') {
						osinfo += " SE";
					}
				}
				break;
			case 90:
				{
					osinfo = "Windows Me";
					iBuildNumberType = 1;
					bGetVersion = true;
				}
				break;
			}
		}
		break;
	case 5:
		{
			switch(osi.dwMinorVersion)
			{
			case 0:
				{
					osinfo = "Windows 2000";
					bGetVersion = true;
					
					leaf::CRegKey RegKey;
					RegKey.SetKey(leaf::CRegKey::_HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions");
					
					std::string	ProductType;
					RegKey.ReadString("ProductType", ProductType);
					
					if(stricmp(ProductType.c_str(), "WINNT") == 0) {
						osinfo += " Professional";
					}
					else if(stricmp(ProductType.c_str(), "LANMANNT") == 0) {
						osinfo += " Server";
					}
					else if(stricmp(ProductType.c_str(), "SERVERNT") == 0) {
						osinfo += " Advanced Server";
					}
				}
				break;
			case 1:
				{
					osinfo = "Windows XP";
					bGetVersion = true;
				}
				break;
			case 2:
				{
					osinfo = "Windows 2003 family";
					bGetVersion = true;
				}
				break;
			}
		}
		break;
	}
	
	if(!bGetVersion) {
		char szOSVersion[128] = {0, };
		sprintf(szOSVersion, "Unknown %d.%d", osi.dwMajorVersion, osi.dwMinorVersion);
		osinfo = szOSVersion;
	}
	
	char szBuildNum[64] = {0, };
	switch(iBuildNumberType)
	{
	case 0:
		{
			sprintf(szBuildNum, " Build %d", osi.dwBuildNumber);
		}
		break;
	case 1:
		{
			sprintf(szBuildNum, " Build %d.%d.%d ", HIBYTE(HIWORD(osi.dwBuildNumber)), 
				LOBYTE(HIWORD(osi.dwBuildNumber)), LOWORD(osi.dwBuildNumber));
		}
		break;
	}
	osinfo += szBuildNum;
	
	if(osi.szCSDVersion[0]) {
		char szCSDVersion[128] = {0, };
		sprintf(szCSDVersion, "(%s)", osi.szCSDVersion);
		osinfo += szCSDVersion;
	}	
	return true;
}
void leaf::GetCPUInfoString(OUT std::string& cpuinfo)
{
	DWORD eaxreg, ebxreg, ecxreg, edxreg;
	
	// We read the standard CPUID level 0x00000000 which should
	// be available on every x86 processor
	__asm
	{
		mov eax, 0
		cpuid
		mov eaxreg, eax
		mov ebxreg, ebx
		mov edxreg, edx
		mov ecxreg, ecx
	}
	int iBrand = ebxreg;
	if(iBrand) {
		char szBrand[24] = {0, };
		*((DWORD*) szBrand) = ebxreg;
		*((DWORD*) (szBrand+4)) = edxreg;
		*((DWORD*) (szBrand+8)) = ecxreg;
		
		cpuinfo = szBrand;
		cpuinfo += " - ";
	}
	unsigned long ulMaxSupportedLevel, ulMaxSupportedExtendedLevel;
	ulMaxSupportedLevel = eaxreg & 0xFFFF;
	// Then we read the ext. CPUID level 0x80000000
	// ...to check the max. supportted extended CPUID level
	__asm
	{
		mov eax, 0x80000000
		cpuid
		mov eaxreg, eax
	}
	ulMaxSupportedExtendedLevel = eaxreg;
	
	// First we get the CPUID standard level 0x00000001
	__asm
	{
		mov eax, 1
		cpuid
		mov eaxreg, eax
	}
	unsigned int uiFamily   = (eaxreg >> 8) & 0xF;
	unsigned int uiModel    = (eaxreg >> 4) & 0xF;
	
	switch(iBrand)
	{
	case 0x756E6547:	// GenuineIntel
		{
			switch (uiFamily)
			{
			case 3:	// 386
				break;
			case 4:	// 486
				break;
			case 5:	// pentium
				break;
			case 6:	// pentium pro - pentium 3
				{
					switch(uiModel)
					{
					case 0:
					case 1:
					default:
						cpuinfo += "Pentium Pro";
						break;
					case 3:
					case 5:
						cpuinfo += "Pentium 2";
						break;
					case 6:
						cpuinfo += "Pentium Celeron";
						break;
					case 7:
					case 8:
					case 0xA:
					case 0xB:
						cpuinfo += "Pentium 3";
						break;
					}	//. switch(uiModel)
				}
				break;
			case 15:	// pentium 4
				{
					cpuinfo += "Pentium 4";
				}
				break;
			default:
				{
					cpuinfo += "Unknown";
				}
				break;
			}	//. switch(uiFamily)
		}
		break;
	case 0x68747541:	// AuthenticAMD
		{
			switch (uiFamily)
			{
			case 4:	// 486, 586
				{
					switch(uiModel)
					{
					case 3:
					case 7:
					case 8:
					case 9:
						cpuinfo += "AMD 486";
						break;
					case 0xE:
					case 0xF:
						cpuinfo += "AMD 586";
						break;
					default:
						cpuinfo += "AMD Unknown (486 or 586)";
						break;
					}	//. switch(uiModel)
				}
				break;
			case 5:	// K5, K6
				{
					switch(uiModel)
					{
					case 0:
					case 1:
					case 2:
					case 3:
						cpuinfo += "AMD K5 5k86";
						break;
					case 6:
					case 7:
						cpuinfo += "AMD K6";
						break;
					case 8:
						cpuinfo += "AMD K6-2";
						break;
					case 9:
						cpuinfo += "AMD K6-3";
						break;
					case 0xD:
						cpuinfo += "AMD K6-2+ or K6-3+";
						break;
					default:
						cpuinfo += "AMD Unknown (K5 or K6)";
						break;
					}	//. switch(uiModel)
				}
				break;
			case 6:	// K7 Athlon, Duron
				{
					switch(uiModel)
					{
					case 1:
					case 2:
					case 4:
					case 6:
						cpuinfo += "AMD K-7 Athlon";
						break;
					case 3:
					case 7:
						cpuinfo += "AMD K-7 Duron";
						break;
					default:
						cpuinfo += "AMD K-7 Unknown";
						break;
					}	//. switch(uiModel)
				}
				break;
			default:
				{
					cpuinfo += "AMD Unknown";
				}
			}	//. switch(uiFamily)
		}
		break;
	case 0x69727943:	// CyrixInstead
	default:
		{
			cpuinfo = "Unknown";
		}
	}	//. switch(iBrand)
	
	// ¼Óµµ
	__int64 llFreq = GetCPUFrequency( 50) / 1000000;
	char szFreq[24] = {0, };
	if(llFreq > 1000) {
		double fFreq = double(llFreq) / 1000.f;
		sprintf(szFreq, " CPU %.2fGHz", fFreq);
	}
	else {
		sprintf(szFreq, " CPU %dMHz", (int)llFreq);
	}
	cpuinfo += szFreq;
}

////////////////////////////////////////////////////////////////////////////////
/// All Platforms
////////////////////////////////////////////////////////////////////////////////

namespace embree
{
	std::string getPlatformName()
	{
#if defined(__LINUX__) && !defined(__X86_64__)
		return "Linux (32bit)";
#elif defined(__LINUX__) && defined(__X86_64__)
		return "Linux (64bit)";
#elif defined(__FREEBSD__) && !defined(__X86_64__)
		return "FreeBSD (32bit)";
#elif defined(__FREEBSD__) && defined(__X86_64__)
		return "FreeBSD (64bit)";
#elif defined(__CYGWIN__) && !defined(__X86_64__)
		return "Cygwin (32bit)";
#elif defined(__CYGWIN__) && defined(__X86_64__)
		return "Cygwin (64bit)";
#elif defined(__WIN32__) && !defined(__X86_64__)
		return "Windows (32bit)";
#elif defined(__WIN32__) && defined(__X86_64__)
		return "Windows (64bit)";
#elif defined(__MACOSX__) && !defined(__X86_64__)
		return "MacOS (32bit)";
#elif defined(__MACOSX__) && defined(__X86_64__)
		return "MacOS (64bit)";
#elif defined(__UNIX__) && !defined(__X86_64__)
		return "Unix (32bit)";
#elif defined(__UNIX__) && defined(__X86_64__)
		return "Unix (64bit)";
#else
		return "Unknown";
#endif
	}

	std::string getCPUVendor()
	{
		int cpuinfo[4];
		__cpuid(cpuinfo, 0);
		int name[4];
		name[0] = cpuinfo[1];
		name[1] = cpuinfo[3];
		name[2] = cpuinfo[2];
		name[3] = 0;
		return (char*)name;
	}

	CPUModel getCPUModel()
	{
		int out[4];
		__cpuid(out, 0);
		if (out[0] < 1) return CPU_UNKNOWN;
		__cpuid(out, 1);
		int family = ((out[0] >> 8) & 0x0F) + ((out[0] >> 20) & 0xFF);
		int model = ((out[0] >> 4) & 0x0F) | ((out[0] >> 12) & 0xF0);
		if (family != 6) return CPU_UNKNOWN;             // earlier than P6
		if (model == 0x0E) return CPU_CORE1;             // Core 1
		if (model == 0x0F) return CPU_CORE2;             // Core 2, 65 nm
		if (model == 0x16) return CPU_CORE2;             // Core 2, 65 nm Celeron
		if (model == 0x17) return CPU_CORE2;             // Core 2, 45 nm
		if (model == 0x1A) return CPU_CORE_NEHALEM;      // Core i7, Nehalem
		if (model == 0x1E) return CPU_CORE_NEHALEM;      // Core i7
		if (model == 0x1F) return CPU_CORE_NEHALEM;      // Core i7
		if (model == 0x2C) return CPU_CORE_NEHALEM;      // Core i7, Xeon
		if (model == 0x2E) return CPU_CORE_NEHALEM;      // Core i7, Xeon
		if (model == 0x2A) return CPU_CORE_SANDYBRIDGE;  // Core i7, SandyBridge
		return CPU_UNKNOWN;
	}
}

////////////////////////////////////////////////////////////////////////////////
/// Windows Platform
////////////////////////////////////////////////////////////////////////////////

#ifdef __WIN32__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace embree
{
	std::string getExecutableFileName() {
		char filename[1024];
		if (!GetModuleFileName(NULL, filename, sizeof(filename))) return std::string();
		return std::string(filename);
	}

	size_t getNumberOfLogicalThreads()
	{
#if (_WIN32_WINNT >= 0x0601)
		int groups = GetActiveProcessorGroupCount();
		int totalProcessors = 0;
		for (int i = 0; i < groups; i++)
			totalProcessors += GetActiveProcessorCount(i);
		return totalProcessors;
#else
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
#endif
	}

	int getTerminalWidth()
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (handle == INVALID_HANDLE_VALUE) return 80;
		CONSOLE_SCREEN_BUFFER_INFO info = { 0 };
		GetConsoleScreenBufferInfo(handle, &info);
		return info.dwSize.X;
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// Linux Platform
////////////////////////////////////////////////////////////////////////////////

#ifdef __LINUX__

#include <stdio.h>
#include <unistd.h>

namespace embree
{
	std::string getExecutableFileName()
	{
		char pid[32]; sprintf(pid, "/proc/%d/exe", getpid());
		char buf[1024];
		int bytes = readlink(pid, buf, sizeof(buf) - 1);
		if (bytes != -1) buf[bytes] = '\0';
		return std::string(buf);
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// MacOS Platform
////////////////////////////////////////////////////////////////////////////////

#ifdef __MACOSX__

#include <mach-o/dyld.h>

namespace embree
{
	std::string getExecutableFileName()
	{
		char buf[1024];
		uint32_t size = sizeof(buf);
		if (_NSGetExecutablePath(buf, &size) != 0) return std::string();
		return std::string(buf);
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// Unix Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(__UNIX__)

#include <unistd.h>
#include <sys/ioctl.h>

#if defined(__USE_NUMA__)
#include <numa.h>
#endif

namespace embree
{
	size_t getNumberOfLogicalThreads() {
		static int nThreads = -1;
		if (nThreads == -1)
			nThreads = sysconf(_SC_NPROCESSORS_CONF);
		return nThreads;
	}

	int getTerminalWidth()
	{
		struct winsize info;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &info) < 0) return 80;
		return info.ws_col;
	}
}
#endif