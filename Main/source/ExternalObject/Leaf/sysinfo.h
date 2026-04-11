
#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#include "regkey.h"
#include "platform.h"

#pragma warning(disable : 4786)
#include <string>

namespace leaf {

	//. Interface Declaration

	bool GetOSInfoString(OUT std::string& osinfo);		//. Get OS infomation
	void GetCPUInfoString(OUT std::string& cpuinfo);	//. Get CPU infomation (Do NOT support duel CPU.)
}

namespace embree
{
    enum CPUModel {
        CPU_UNKNOWN,
        CPU_CORE1,
        CPU_CORE2,
        CPU_CORE_NEHALEM,
        CPU_CORE_SANDYBRIDGE
    };

    /*! get the full path to the running executable */
    std::string getExecutableFileName();

    /*! return platform name */
    std::string getPlatformName();

    /*! return the name of the CPU */
    std::string getCPUVendor();

    /*! get microprocessor model */
    CPUModel getCPUModel();

    /*! return the number of logical threads of the system */
    size_t getNumberOfLogicalThreads();

    /*! returns the size of the terminal window in characters */
    int getTerminalWidth();
}
#endif // _SYSINFO_H_