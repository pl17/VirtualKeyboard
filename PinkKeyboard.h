#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef _WINDOWS_

#ifdef FIX_RUNTIME
#include "FixRuntime.h"
#endif

#include <Windows.h>
#endif

void PinkVkShowA(const char *szConfigFileName=NULL, int left=0x80000000, int top=0);
void PinkVkShowW(const WCHAR *szConfigFileName=NULL, int left=0x80000000, int top=0);
void PinkVkClose();

void PinkVkGlobalInit();
void PinkVkGlobalClose();

void PinkVkSetupCommunication(HWND hWnd, BOOL bSubWin=TRUE);

#ifdef UNICODE
#define PinkVkShow PinkVkShowW
#else
#define PinkVkShow PinkVkShowA
#endif