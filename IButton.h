#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifdef FIX_RUNTIME
#include "FixRuntime.h"
#endif

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#define WMM_BUTTONMSG_START	(WM_USER+0x1712)
#define WMM_ONCLICK			(WM_USER+0x1712)
#define WMM_ONHOVERSTART	(WM_USER+0x1713)
#define WMM_ONHOVEREND		(WM_USER+0x1714)
#define WMM_BUTTONMSG_END	(WM_USER+0x1811)

class IButton
{
public:
	//Mouse Event handler
	virtual void OnMouseMove(int x, int y, DWORD buttons)=0;
	virtual void OnLMouseDown(int x, int y, DWORD buttons)=0;
	virtual void OnLMouseUp(int x, int y, DWORD buttons)=0;
	virtual void OnMouseLeaveParent()=0;

	//Mouse Event Fire
	virtual void OnClick()=0;
	virtual void OnHoverStart()=0;
	virtual void OnHover()=0;
	virtual void OnHoverEnd()=0;

	//Paint Event handler
	virtual void OnPaint(HDC hdc)=0;
	virtual void Invalidate(BOOL bErase=FALSE)=0;

	//Method drive GUI
	virtual void SetForceState(BOOL bForce)=0;
	virtual BOOL IsForceState()=0;
	virtual void SetVisible(BOOL bVisible)=0;
	virtual BOOL IsVisible()=0;
	virtual void SetColor(COLORREF clNormal, COLORREF clHover, COLORREF clSelected, COLORREF clDisable)=0;
	virtual void SetBackground(HBRUSH hbrNormal, HBRUSH hbrHover, HBRUSH hbrSelected, HBRUSH hbrDisable)=0;
	virtual void SetBorder(int wNormal, COLORREF clNormal, int wHover, COLORREF clHover, int wSelected, COLORREF clSelected, int wDisable, COLORREF clDisable)=0;
	virtual void SetTextA(char *lpszText)=0;
	virtual void SetTextW(WCHAR *lpszText)=0;
	virtual void SetImageFromResource(HMODULE hModule, TCHAR *szType, DWORD id)=0;
	virtual void SetImageFromFile(TCHAR *szFileName)=0;
	virtual void SetPadding(int padlef, int padtop, int padright, int padbottom)=0;
	virtual void SetPosition(int left, int top)=0;
	virtual void SetPosition(int left, int top, int width, int height)=0;
	virtual void GetPosition(RECT *rcPos)=0;
	virtual void SetSize(int width, int height)=0;
	virtual void SetPush(BOOL bPush)=0;
	virtual int GetWidth()=0;
	virtual int GetHeight()=0;
	virtual void Click()=0;

	//Access method
	virtual BOOL IsSelected()=0;
	virtual BOOL IsPushStyle()=0;
	virtual BOOL IsHover()=0;
protected:
	//Check inside button
	virtual BOOL _IsInside(int x, int y)=0;

	//Paint Event Handler
	virtual void _OnPaintNormal(HDC hdc)=0;
	virtual void _OnPaintHover(HDC hdc)=0;
	virtual void _OnPaintSelected(HDC hdc)=0;
	virtual void _OnPaintDisable(HDC hdc)=0;
	virtual void _OnPaintSelectedOut(HDC hdc)=0;

	virtual void _OnPaintImage(HDC hdc, COLORREF color)=0;
	virtual void _OnPaintText(HDC hdc, COLORREF color)=0;
	virtual void _OnPaintShape(HDC hdc, HBRUSH hbrBackground, HPEN hPen)=0;

	virtual void _RecalcRectContent()=0;
};