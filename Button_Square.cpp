#include "Button_Square.h"


BOOL Button_Square::_IsInside( int x, int y )
{
	BOOL brs;

	brs=FALSE;

	if (this->m_rcBox.left<x && x<this->m_rcBox.right
		&& this->m_rcBox.top<y && y<this->m_rcBox.bottom)
		brs=TRUE;

	return brs;
}

void Button_Square::OnHover()
{
	
}

Button_Square::Button_Square( WCHAR *lpszText, int left, int top, int width, int height, HWND hParent, int id )
:Button_Base(hParent)
{
	this->_Init(left, top, width, height, hParent, id);
	this->SetTextW(lpszText);
}

void Button_Square::_Init( int left, int top, int width, int height, HWND hParent, int id )
{
 	this->m_rcBox.left=left;
 	this->m_rcBox.top=top;
 	this->m_rcBox.right=left+width;
 	this->m_rcBox.bottom=top+height;
	this->m_nBoxWidth=width;
	this->m_nBoxHeight=height;

	this->m_hParent=hParent;
	this->m_bPush=TRUE;

	this->_RecalcRectContent();
}

void Button_Square::_OnPaintShape( HDC hdc, HBRUSH hbrBackground, HPEN hPen )
{
	HPEN hPenOld;
	HBRUSH hBrushOld;

	//if (hbrBackground)
	hBrushOld=(HBRUSH)SelectObject(hdc, hbrBackground);

	//if (hPen)
	hPenOld=(HPEN)SelectObject(hdc, hPen);	
	Rectangle(hdc, this->m_rcBox.left, this->m_rcBox.top, this->m_rcBox.right, this->m_rcBox.bottom);

	SelectObject(hdc, hBrushOld);
	SelectObject(hdc, hPenOld);
}
