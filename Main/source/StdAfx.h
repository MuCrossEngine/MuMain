// stdafx.h : include file for standard system include files,
#pragma once

//warining
#pragma warning( disable : 4067 ) 
#pragma warning( disable : 4099 )
#pragma warning( disable : 4786 ) 
#pragma warning( disable : 4800 ) 
#pragma warning( disable : 4996 ) 
#pragma warning( disable : 4244 )
#pragma warning( disable : 4237 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4503 ) 
#pragma warning( disable : 4267 ) 
#pragma warning( disable : 4091 ) 
#pragma warning( disable : 4819 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4702 )
#pragma warning( disable : 4838 )
#pragma warning( disable : 5208 )

#pragma warning( disable : 28159 )
#pragma warning( disable : 26812 )
#pragma warning( disable : 28251 )
#pragma warning( disable : 26451 )
#pragma warning( disable : 26819 )

// Exclude rarely-used stuff from Windows headers
#ifndef __ANDROID__
#define WIN32_LEAN_AND_MEAN

#ifndef _USE_32BIT_TIME_T
	#define _USE_32BIT_TIME_T
#endif //_USE_32BIT_TIME_T

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#pragma warning( push, 3 )

#include <windows.h>

//windows
#include <WinSock2.h>
#include <mmsystem.h>
#include <shellapi.h>
#endif // __ANDROID__

//c runtime
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#ifndef __ANDROID__
#include <tchar.h>
#include <mbstring.h>
#include <conio.h>
#endif // __ANDROID__
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>

#include <string>
#include <list>
#include <map>
#include <deque>
#include <algorithm>
#include <vector>
#include <queue>

#include <thread>   // Para std::this_thread::sleep_for
#include <chrono>   // Para std::chrono::milliseconds
#include <random>

#include <cmath>
#include <fstream>
#include <unordered_map>
#ifndef __ANDROID__
#pragma warning( pop )
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

//opengl
#ifdef __ANDROID__
#include "Platform/GLFixedFunctionStubs.h"
#else
#include <gl/glew.h>
#include <gl/gl.h>
#endif // __ANDROID__

#ifndef MAIN_UPDATE
#define MAIN_UPDATE 603
#endif //_USE_32BIT_TIME_T
//patch
//winmain
#ifndef __ANDROID__
#include "Winmain.h"
#else
#include "Platform/AndroidWin32Compat.h"
#include "Platform/WinsockAndroidCompat.h"
#include "Winmain.h"
#endif
#include "Defined_Global.h"

//client
#include "_define.h"
#include "_enum.h"
#include "_types.h"
#include "_struct.h"

#include "w_WindowMessageHandler.h"
#include "_GlobalFunctions.h"

#include "imgui.h"
#ifndef __ANDROID__
#include "WINHANDLE.h"
#else
#include "WINHANDLE.h"
#include "Platform/Win32SecondaryStubs.h"
#endif

#include "_TextureIndex.h"
#include "InfoHelperFunctions.h"
#include "UIDefaultBase.h"
#include "NewUICommon.h"
#include "./Math/ZzzMathLib.h"
#include "ZzzOpenglUtil.h"
#include "Util.h"
#include "CGMCharacter.h"
#include "CGMProtect.h"
#include "CBInterface.h"
