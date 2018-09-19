#include "PinkKeyboard.h"
#include "VirtualKeyboard.h"
#include "UtilityDebug.h"

extern IOutputDebug *dbgConsole;

void PinkVkShowA(const char *szConfigFileName, int left, int top)
{
	new VirtualKeyboard(left, top, szConfigFileName);
}

void PinkVkShowW(const WCHAR *szConfigFileName, int left, int top)
{
	new VirtualKeyboard(left, top, szConfigFileName);
}

void PinkVkClose()
{
	VirtualKeyboard::Close();
}

static void SetupCommunication(HWND hWnd)
{
	ULONG org;
	char szClassName[255];

	GetClassNameA(hWnd, szClassName, 255);
	if (_stricmp(szClassName, "edit")==0)
	{
		org=GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)org);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)VirtualKeyboard::WndEdit);
	}
}

static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	SetupCommunication(hWnd);
	return TRUE;
}

void PinkVkSetupCommunication(HWND hWnd, BOOL bSubWin/* =TRUE */)
{
	SetupCommunication(hWnd);
	if (bSubWin)
		EnumChildWindows(hWnd, EnumChildProc, NULL);
}

void PinkVkGlobalInit()
{
	VirtualKeyboard::GlobalInit();
}

void PinkVkGlobalClose()
{
	VirtualKeyboard::Close();
	VirtualKeyboard::GlobalClose();
}
