#pragma once

#include "VirtualKeyboard_Key.h"

enum VirtualKeyboard_Style
{
	VKSTYLE_PINK,
	VKSTYLE_GREEN,
	VKSTYLE_AQUABLUE,
	VKSTYLE_ORANGE,
	VKSTYLE_WHITE,
	VKSTYLE_CRIMSON,
	VKSTYLE_OLIVE,
};

#define WMM_VKMSG_START		(WM_USER+0x1812)
#define WMM_KEYDOWN			(WM_USER+0x1812)
#define WMM_CHAR			(WM_USER+0x1813)
#define WMM_KEYUP			(WM_USER+0x1814)
#define WMM_VKPING			(WM_USER+0x1888)
#define WMM_VKMSG_END		(WM_USER+0x1912)

#define VKPING_KEY	0x029D18

class VirtualKeyboard
{
public:
	VirtualKeyboard();
	VirtualKeyboard(int left, int top, const char *szConfigFileName);
	VirtualKeyboard(int left, int top, const WCHAR *szConfigFileName);
	~VirtualKeyboard();

	static void GlobalInit();
	static void GlobalClose();

	void SetIndexActive(int index);
	void SwitchActiveNums();
	void SwitchActiveAll();

	void SwapButton(int isrc, int ides);
	void SwapRandom();
	void ResetDice(BOOL bFullReset);
	void ResizeKeyboard();

	void SetShowMask(BOOL bMask);
	void SetStyle(VirtualKeyboard_Style style, BOOL bRedraw=FALSE);

	HWND GetWindow();

	static LRESULT CALLBACK WndEdit(HWND, UINT, WPARAM, LPARAM);
	static void Close();
private:
	//Single Instance
	static VirtualKeyboard* m_pthis;
	static HANDLE m_hevWinInit;

private:
	HWND m_hWnd;
	HWND m_hCbxStyle;
	HWND m_hWndFocus;

	RECT m_rcBox;

	HANDLE m_hThreadVk;

	HANDLE m_hThreadKey;

	WCHAR *m_szConfigFileNameW;
	char *m_szConfigFileNameA;

	VirtualKeyboard_Key **m_keys;
	int *m_keysIndex;
	VirtualKeyboard_Key *m_kShift;
	VirtualKeyboard_Key *m_kCapsLock;
	VirtualKeyboard_Key *m_kTab;
	VirtualKeyboard_Key *m_kBackspace;
	VirtualKeyboard_Key *m_kShowMask;
	VirtualKeyboard_Key *m_kDice;
	VirtualKeyboard_Key *m_kUndo;
	VirtualKeyboard_Key *m_kExpand;

	BOOL m_bTrackMouse;
	BOOL m_bShiftKeyboard;
	int m_nKeys;

	int m_nStyleIndex;

	int m_nInitLeft, m_nInitTop;

	typedef struct _VKEYBOARD
	{
		char *szValue;
		char* szDisplay[2];

		WORD wImageId;
		float fWidth;

		BOOL bPush;
		BOOL bControl;
		BOOL bFullDisplay;
		BOOL bSwap;
	}VKEYBOARD;

	typedef struct _VKB_LAYOUT
	{
		int marginleft;
		int margintop;
		int padding_inner;

		int *rows;
		int rowCount;
	} VKB_LAYOUT;

	typedef struct _VKB_STYLE
	{
		VirtualKeyboard_Style style;
		COLORREF clTextNormal, clTextHover, clTextSelected, clTextDisable;
		COLORREF clBorderNormal, clBorderHover, clBorderSelected, clBorderDisable;
	}VKB_STYLE;

	typedef struct _VKB_CONFIG
	{
		int nStyleIndex;
		BOOL bShow;
	} VKB_CONFIG;

	static float m_nMaxWidthUnit;
	static int m_nMaxHeightUnit;

	static const VKEYBOARD m_listKey[];
	static const int m_nListKey;

	static VKB_LAYOUT m_layout;

	static VKB_STYLE m_listStyle[];
	static const int m_nListStyle;

	static const int m_nMinWidth;
	static const int m_nMinHeight;

	static const TCHAR *m_szClassName;

	void _InitInstance(int left, int top, const void *szConfigFileName, int unitSize);

	//Process message
	void _OnClick(VirtualKeyboard_Key *key);
	void _OnHoverStart(VirtualKeyboard_Key *key);
	void _OnHoverEnd(VirtualKeyboard_Key *key);
	void _OnMouseLeave();
	void _OnLMouseDown(int x, int y, DWORD buttons);
	void _OnLMouseUp(int x, int y, DWORD buttons);
	void _OnMouseMove(int x, int y, DWORD buttons);
	void _OnPaint(HDC hdc);
	void _OnKeyUp(DWORD keycode);
	void _OnKeyDown(DWORD keycode);

	//Windows function
	void _InitControls();
	void _CreateKeyboardWindow();
	VirtualKeyboard_Key* _FindKeyByName(char *szName);
	BOOL _ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lparam);

	//Config
	void _SaveConfig();
	void _LoadConfig();

	//Static function
	static void _RegisterClass();
	static void _UnRegisterClass();
	static HICON m_hIconVk;

	static LRESULT CALLBACK	_WndProc(HWND, UINT, WPARAM, LPARAM);
	static DWORD _ThreadGUI(LPVOID lpParam);
	static DWORD _ThreadKey(LPVOID lpParam);
	static void _SimulatorKeyboard(VirtualKeyboard_Key *key);
	static BOOL _VkPostEncryptMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL _IsPrivateCommunication(HWND hWnd);

	//Method private communication
	static void CALLBACK _VkDecryptMessage(HWND hWnd, UINT *message, WPARAM *wParam, LPARAM *lParam);
	static BOOL _VkIsPingMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static int _VkProcessPingMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
