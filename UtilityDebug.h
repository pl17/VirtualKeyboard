#ifndef _UTILITYDEBUG_H_
#define _UTILITYDEBUG_H_

#ifdef _PDBG//Use debug

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0500
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINDOWS_
#include <Windows.h>
#endif

class IOutputDebug;

//Define function
void LogCreateFileW(IOutputDebug** pIDebug, WCHAR *filename, BOOL bOverwrite=TRUE);
void LogCreateFileA(IOutputDebug** pIDebug, char *filename, BOOL bOverwrite=TRUE);

void LogCreateConsole(IOutputDebug** pIDebug);
void LogCreateView(IOutputDebug** pIDebug);

void LogShowTime(IOutputDebug* pIDebug, BOOL bVisible);
void LogClose(IOutputDebug* pIDebugp);
void LogWriteLineW(IOutputDebug* pIDebug, WCHAR *szFormatText, ...);
void LogWriteLineA(IOutputDebug* pIDebug, char *szFormatText, ...);
void LogWriteW(IOutputDebug* pIDebug, WCHAR *szFormatText, ...);
void LogWriteA(IOutputDebug* pIDebug, char *szFormatText, ...);
void LogReset(IOutputDebug* pIDebug);

//process unicode/ascii
#ifdef UNICODE
#define LogWriteLine	LogWriteLineW
#define LogWrite		LogWriteW
#define LogCreateFile	LogCreateFileW
#else
#define LogWriteLine	LogWriteLineA
#define LogWrite		LogWriteA
#define LogCreateFile	LogCreateFileA
#endif

#else//Not use debug

#define IOutputDebug void

#define LogCreateFileA(pIDebug, ...)
#define LogCreateFileW(pIDebug, ...)
#define LogCreateFile	LogCreateFileA
#define LogCreateConsole(pIDebug)
#define LogCreateView(pIDebug)

#define LogShowTime(pIDebug, bVisible)
#define LogClose(pIDebug)
#define LogWriteLineW(pIDebug, szFormatText, ...)
#define LogWriteLineA(pIDebug, szFormatText, ...)
#define LogWriteW(pIDebug, szFormatText, ...)
#define LogWriteA(pIDebug, szFormatText, ...)

#define LogWriteLine
#define LogWrite

#define LogReset(pIDebug)
#endif

#endif