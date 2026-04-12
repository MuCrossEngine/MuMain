// Nprotect.h: interface for the CNprotect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPROTECT_H__C46E8B63_57C1_4E4B_B35A_63ABE5C29D67__INCLUDED_)
#define AFX_NPROTECT_H__C46E8B63_57C1_4E4B_B35A_63ABE5C29D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define HACK_TIMER	1000
#define WINDOWMINIMIZED_TIMER	1001

#ifndef __ANDROID__

typedef BOOL(NPROTECTCHECKCRC)(char *);
extern NPROTECTCHECKCRC *g_pNprotectCheckCRC;


BOOL LaunchNprotect( void);
void KillProtect();
bool CheckTotalNprotect( void);
void CheckNpmonCrc( HWND hWnd);
BOOL FindNprotectWindow( void);
void CloseNprotect( void);

#else // Android stubs
typedef int NPROTECTCHECKCRC;
inline BOOL LaunchNprotect()        { return TRUE; }
inline void KillProtect()           {}
inline bool CheckTotalNprotect()    { return true; }
inline void CheckNpmonCrc(HWND)     {}
inline BOOL FindNprotectWindow()    { return FALSE; }
inline void CloseNprotect()         {}
#endif // __ANDROID__


#endif // !defined(AFX_NPROTECT_H__C46E8B63_57C1_4E4B_B35A_63ABE5C29D67__INCLUDED_)
