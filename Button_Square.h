#pragma once

#include "Button_Base.h"

class Button_Square :
	public Button_Base
{
public:
	Button_Square(WCHAR *lpszText, int left, int top, int width, int height, HWND hParent, int id);

	void OnHover();
	BOOL _IsInside( int x, int y );
protected:

	virtual void _OnPaintShape( HDC hdc, HBRUSH hbrBackground, HPEN hPen );
	void _Init(int left, int top, int width, int height, HWND hParent, int id);
};
