#pragma once

#include "Button_Square.h"

class VirtualKeyboard_Key :
	public Button_Square
{
public:
	VirtualKeyboard_Key();
	VirtualKeyboard_Key(HWND hParent, int left, int top, int width, int height, int id, BOOL bPush, BOOL bControl, BOOL bFullDisplay, BOOL bSwap);
	~VirtualKeyboard_Key();

	void SetText(char *lpszValue, char **lppszDisplay, int size);
	void ShowMask(BOOL bShowMask);
	int GetIndexActive();
	void SetIndexActive(int index);
	void SwitchActive();
	int GetMaxIndex();
	char GetValue();
	char* GetName();

	BOOL IsControl();
	BOOL IsFulLDisplay();
	BOOL IsSwap();

protected:
	virtual void _OnPaintText(HDC hdc, COLORREF color);
private:
	void _Init(BOOL bControl, BOOL bPush, BOOL bFullDisplay, BOOL bSwap);

private:
	char m_lpszValue[2];
	char* m_lpszDisplay[2];
	BOOL m_bControlKey;

	BOOL m_bFullDisplay;
	int m_nIndexActive;
	int m_nMaxIndex;
	BOOL m_bShowMask;
	BOOL m_bSwap;
};