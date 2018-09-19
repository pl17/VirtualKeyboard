#pragma once

#include "IButton.h"
#include <ObjIdl.h>
#include <GdiPlus.h>

#pragma comment(lib, "GdiPlus.lib")

using namespace Gdiplus;

class Button_Base :
	public IButton
{
public:
	Button_Base(HWND hParent);
	~Button_Base();

	void OnPaint(HDC hdc);
	
	void OnClick();
	void OnHoverStart();
	void OnHoverEnd();
	void OnMouseMove( int x, int y, DWORD buttons );
	void OnLMouseDown( int x, int y, DWORD buttons );
	void OnLMouseUp( int x, int y, DWORD buttons );
	void OnMouseLeaveParent();

	void SetColor(COLORREF clNormal, COLORREF clHover, COLORREF clSelected, COLORREF clDisable);
	void SetBackground(HBRUSH hbrNormal, HBRUSH hbrHover, HBRUSH hbrSelected, HBRUSH hbrDisable);
	virtual void SetTextA( char *lpszText );
	virtual void SetTextW( WCHAR *lpszText );

	void SetImageFromResource( HMODULE hModule, TCHAR *szType, DWORD id );
	void SetImageFromFile( TCHAR *szFileName );
	void SetImageFromMemory(void *mem, DWORD dwSize);

	void SetPadding(int padleft, int padtop, int padright, int padbottom);
	void SetBorder(int wNormal, COLORREF clNormal, int wHover, COLORREF clHover, int wSelected, COLORREF clSelected, int wDisable, COLORREF clDisable);
	void SetPosition( int left, int top );
	void SetPosition( int left, int top, int width, int height );
	void GetPosition(RECT *rcPos);
	void SetSize( int width, int height );
	void SetPush(BOOL bPush);
	int GetWidth();
	int GetHeight();
	void Click();
	virtual void Invalidate(BOOL bErase=FALSE);
	void SetVisible( BOOL bVisible );
	void SetForceState(BOOL bForce);
	BOOL IsForceState();

	BOOL IsVisible();
	BOOL IsSelected();
	BOOL IsPushStyle();
	BOOL IsHover();
protected:
	//Box
	RECT m_rcBox;
	RECT m_rcPadding;
	RECT m_rcContent;
	int m_nBoxWidth;
	int m_nBoxHeight;

	//Border
	int m_nBorderWidthNormal;
	HPEN m_hpBorderNormal;
	COLORREF m_clBorderNormal;

	int m_nBorderWidthHover;
	HPEN m_hpBorderHover;
	COLORREF m_clBorderHover;
	
	int m_nBorderWidthSelected;
	HPEN m_hpBorderSelected;
	COLORREF m_clBorderSelected;

	int m_nBorderWidthDisable;
	HPEN m_hpBorderDisable;
	COLORREF m_clBorderDisable;

	BOOL m_bIsHover;
	BOOL m_bIsSelected;
	BOOL m_bIsMouseDown;
	BOOL m_bVisible;
	BOOL m_bIsDisable;
	BOOL m_bPush;
	BOOL m_bForceState;

	COLORREF m_clTextNormal;
	HBRUSH m_hbrBkNormal;
	COLORREF m_clTextHover;
	HBRUSH m_hbrBkHover;
	COLORREF m_clTextDisable;
	HBRUSH m_hbrBkDisable;
	COLORREF m_clTextSelected;
	HBRUSH m_hbrBkSelected;

	char *m_lpszTextA;
	WCHAR *m_lpszTextW;

	HWND m_hParent;

	Image *m_imgContent;
	HGLOBAL m_hImgContent;

	virtual void _OnPaintNormal( HDC hdc );
	virtual void _OnPaintHover( HDC hdc );
	virtual void _OnPaintSelected( HDC hdc );
	virtual void _OnPaintDisable( HDC hdc );
	virtual void _OnPaintSelectedOut( HDC hdc );

	virtual void _OnPaintImage(HDC hdc, COLORREF color);
	virtual void _OnPaintText( HDC hdc, COLORREF color );
	virtual void _RecalcRectContent();
};
