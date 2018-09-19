#include "Button_Base.h"


void Button_Base::OnMouseMove( int x, int y, DWORD buttons )
{
	if (!this->m_bVisible) return;
	if ((buttons&MK_LBUTTON) && !this->m_bIsMouseDown) return;

	if (this->_IsInside(x, y))
	{
		if (!this->m_bIsHover)
		{
			this->m_bIsHover=TRUE;
			this->OnHoverStart();
		}
		this->OnHover();
	}
	else if (this->m_bIsHover)
	{
		this->m_bIsHover=FALSE;
		this->OnHoverEnd();
	}
}

void Button_Base::OnLMouseDown( int x, int y, DWORD buttons )
{
	if (!this->m_bVisible) return;

	if (this->_IsInside(x, y) && !this->m_bForceState)
	{
		if (!this->m_bIsMouseDown)
		{
			this->m_bIsMouseDown=TRUE;

			if (!this->m_bIsSelected)
				this->Invalidate();
		}
	}
}

void Button_Base::OnLMouseUp( int x, int y, DWORD buttons )
{
	if (!this->m_bVisible) return;

	if (this->_IsInside(x, y) && !this->m_bForceState)
	{
		if (!this->m_bIsHover)
		{
			this->m_bIsHover=TRUE;
			this->OnHoverStart();
		}

		if (this->m_bIsMouseDown)//OnClick
		{
			if (this->m_bPush)
				this->m_bIsSelected=!this->m_bIsSelected;
			this->OnClick();
		}
	}

	if (this->m_bIsMouseDown)
	{
		this->m_bIsMouseDown=FALSE;

		if (!this->m_bIsSelected)
			this->Invalidate();
	}
}

void Button_Base::SetVisible( BOOL bVisible )
{
	this->m_bVisible=bVisible;
	this->Invalidate(TRUE);
}

BOOL Button_Base::IsVisible()
{
	return this->m_bVisible;
}

Button_Base::Button_Base(HWND hParent)
{
	this->m_bIsHover=FALSE;
	this->m_bIsMouseDown=FALSE;
	this->m_bIsSelected=FALSE;
	this->m_bVisible=TRUE;
	this->m_bIsDisable=FALSE;
	this->m_bForceState=FALSE;

	this->SetColor(RGB(0xff, 0x69, 0xb4), RGB(0xff, 0x34, 0xb4), RGB(0xff, 0x00, 0xff), 0x00000000);

	this->m_hbrBkNormal=CreateSolidBrush(RGB(48, 48, 58));
	this->m_hbrBkHover=CreateSolidBrush(RGB(98, 98, 103));
	this->m_hbrBkSelected=CreateSolidBrush(RGB(255, 255, 255));
	this->m_hbrBkDisable=NULL;

	this->m_lpszTextA=NULL;
	this->m_lpszTextW=NULL;

	this->m_bPush=FALSE;

	this->m_nBorderWidthNormal=0;
	this->m_nBorderWidthHover=0;
	this->m_nBorderWidthSelected=0;
	this->m_nBorderWidthDisable=0;

	this->m_hpBorderNormal=NULL;
	this->m_hpBorderHover=NULL;
	this->m_hpBorderSelected=NULL;
	this->m_hpBorderDisable=NULL;

	this->m_rcBox.left=0;
	this->m_rcBox.top=0;
	this->m_rcBox.right=0;
	this->m_rcBox.bottom=0;

	this->m_nBoxWidth=0;
	this->m_nBoxHeight=0;

	this->SetPadding(3, 3, 3, 3);

	this->m_hParent=hParent;

	this->m_imgContent=NULL;
	this->m_hImgContent=NULL;
}

void Button_Base::OnPaint( HDC hdc )
{
	if (!this->m_bVisible) return;

	if (this->m_bIsDisable)
	{
		this->_OnPaintDisable(hdc);
	}
	else
	{
		if (this->m_bPush && this->m_bIsSelected)
		{
			this->_OnPaintSelected(hdc);
			return;
		}

		if (this->m_bIsMouseDown)
		{
			if (this->m_bIsHover)
			{
				this->_OnPaintSelected(hdc);
			}
			else
			{
				this->_OnPaintSelectedOut(hdc);
			}
		}
		else
		{
			if (this->m_bIsHover)
			{
				this->_OnPaintHover(hdc);
			}
			else
			{
				this->_OnPaintNormal(hdc);
			}
		}
	}
}

void Button_Base::SetColor( COLORREF clNormal, COLORREF clHover, COLORREF clSelected, COLORREF clDisable )
{
	this->m_clTextNormal=clNormal;
	this->m_clTextHover=clHover;
	this->m_clTextSelected=clSelected;
	this->m_clTextDisable=clDisable;
}

void Button_Base::SetBackground( HBRUSH hbrNormal, HBRUSH hbrHover, HBRUSH hbrSelected, HBRUSH hbrDisable )
{
	this->m_hbrBkNormal=hbrNormal;
	this->m_hbrBkHover=hbrHover;
	this->m_hbrBkSelected=hbrSelected;
	this->m_hbrBkDisable=hbrDisable;
}

Button_Base::~Button_Base()
{
	if (this->m_hbrBkNormal)
		DeleteObject(this->m_hbrBkNormal);
	if (this->m_hbrBkHover)
		DeleteObject(this->m_hbrBkHover);
	if (this->m_hbrBkSelected)
		DeleteObject(this->m_hbrBkSelected);
	if (this->m_hbrBkDisable)
		DeleteObject(this->m_hbrBkDisable);
	if (this->m_hbrBkHover)
		DeleteObject(this->m_hbrBkHover);

	if (this->m_hpBorderNormal)
		DeleteObject(this->m_hpBorderNormal);
	if (this->m_hpBorderHover)
		DeleteObject(this->m_hpBorderHover);
	if (this->m_hpBorderSelected)
		DeleteObject(this->m_hpBorderSelected);
	if (this->m_hpBorderDisable)
		DeleteObject(this->m_hpBorderSelected);
	if (this->m_hpBorderDisable)
		DeleteObject(this->m_hpBorderDisable);

	if (this->m_lpszTextA) LocalFree(this->m_lpszTextA);
	if (this->m_lpszTextW) LocalFree(this->m_lpszTextW);

	if (this->m_hImgContent)
	{
		GlobalFree(this->m_hImgContent);
		this->m_hImgContent=NULL;
	}
	if (this->m_imgContent)
	{
		delete this->m_imgContent;
		this->m_imgContent=NULL;
	}
}

void Button_Base::_OnPaintNormal( HDC hdc )
{
	this->_OnPaintShape(hdc, this->m_hbrBkNormal, this->m_hpBorderNormal);
	if (this->m_imgContent)
	{
		this->_OnPaintImage(hdc, this->m_clTextNormal);
	}
	else
		this->_OnPaintText(hdc, this->m_clTextNormal);
}

void Button_Base::_OnPaintHover( HDC hdc )
{
	this->_OnPaintShape(hdc, this->m_hbrBkHover, this->m_hpBorderHover);
	if (this->m_imgContent)
	{
		this->_OnPaintImage(hdc, this->m_clTextHover);
	}
	else
		this->_OnPaintText(hdc, this->m_clTextHover);
}

void Button_Base::_OnPaintSelected( HDC hdc )
{
 	this->_OnPaintShape(hdc, this->m_hbrBkSelected, this->m_hpBorderSelected);

	if (this->m_imgContent)
	{
		this->_OnPaintImage(hdc, this->m_clTextSelected);
	}
	else
		this->_OnPaintText(hdc, this->m_clTextSelected);
}

void Button_Base::_OnPaintDisable( HDC hdc )
{
	this->_OnPaintShape(hdc, this->m_hbrBkDisable, this->m_hpBorderDisable);
	this->_OnPaintText(hdc, this->m_clTextDisable);
}

void Button_Base::_OnPaintSelectedOut( HDC hdc )
{
	this->_OnPaintHover(hdc);
}

void Button_Base::_OnPaintImage( HDC hdc, COLORREF color )
{
	Graphics graphics(hdc);
	ImageAttributes imgatt;
	float fcolor[5][5];
	int left, top, w, h, i, j;

	if (this->m_imgContent)
	{
		for (i=0; i<5; i++)
		{
			for (j=0; j<5; j++)
			{
				if (i!=j) fcolor[i][j]=0.0f;
				else fcolor[i][j]=1.0f;
			}
		}
		for (i=0; i<3; i++)
			fcolor[4][i]=(float)((BYTE*)&color)[i]/255;
		imgatt.SetColorMatrix((ColorMatrix*)fcolor);

		w=this->m_imgContent->GetWidth();
		h=this->m_imgContent->GetHeight();
		left=this->m_rcBox.left+(this->m_rcBox.right-this->m_rcBox.left-w)/2;
		top=this->m_rcBox.top+(this->m_rcBox.bottom-this->m_rcBox.top-h)/2;

		graphics.DrawImage(this->m_imgContent, Rect(left, top, w, h)
			, 0, 0, w, h
			, UnitPixel, &imgatt);
	}
}

void Button_Base::_OnPaintText( HDC hdc, COLORREF color )
{
	//COLORREF colorOld;

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, color);

	if (this->m_lpszTextW)
	{
		DrawTextW(hdc, this->m_lpszTextW, -1, &this->m_rcContent, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	}
	else if (this->m_lpszTextA)
	{
		DrawTextA(hdc, this->m_lpszTextA, -1, &this->m_rcContent, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	}
}

void Button_Base::SetTextA( char *lpszText )
{
	int len;

	if (this->m_lpszTextA)
	{
		LocalFree(this->m_lpszTextA);
		this->m_lpszTextA=NULL;
	}

	if (lpszText)
	{
		len=strlen(lpszText);
		this->m_lpszTextA=(char*)LocalAlloc(LMEM_FIXED, len+1);
		CopyMemory(this->m_lpszTextA, lpszText, len+1);
	}
	else this->m_lpszTextA=NULL;
}

void Button_Base::SetTextW( WCHAR *lpszText )
{
	int len;

	if (this->m_lpszTextW)
	{
		LocalFree(this->m_lpszTextW);
		this->m_lpszTextW=NULL;
	}

	if (lpszText)
	{
		len=wcslen(lpszText);
		this->m_lpszTextW=(WCHAR*)LocalAlloc(LMEM_FIXED, (len+1)*sizeof(WCHAR));
		CopyMemory(this->m_lpszTextW, lpszText, (len+1)*sizeof(WCHAR));
	}
	else this->m_lpszTextW=NULL;
}

void Button_Base::OnMouseLeaveParent()
{
	BOOL bNeedValid;

	bNeedValid=this->m_bIsHover||this->m_bIsMouseDown;

	this->m_bIsHover=FALSE;
	this->m_bIsMouseDown=FALSE;
	if (bNeedValid)
		this->Invalidate();
}

void Button_Base::_RecalcRectContent()
{
	this->m_rcContent.left=this->m_rcBox.left+this->m_rcPadding.left+this->m_nBorderWidthSelected;
	this->m_rcContent.top=this->m_rcBox.top+this->m_rcPadding.top+this->m_nBorderWidthSelected;
	this->m_rcContent.right=this->m_rcBox.right-this->m_rcPadding.right-this->m_nBorderWidthSelected;
	this->m_rcContent.bottom=this->m_rcBox.bottom-this->m_rcPadding.bottom-this->m_nBorderWidthSelected;
}

void Button_Base::SetPadding( int padleft, int padtop, int padright, int padbottom )
{
	this->m_rcPadding.left=padleft;
	this->m_rcPadding.top=padtop;
	this->m_rcPadding.right=padright;
	this->m_rcPadding.bottom=padbottom;
	this->_RecalcRectContent();
}

void Button_Base::SetBorder( int wNormal, COLORREF clNormal, int wHover, COLORREF clHover, int wSelected, COLORREF clSelected, int wDisable, COLORREF clDisable )
{
	this->m_nBorderWidthNormal=wNormal;
	this->m_clBorderNormal=clNormal;
	if (this->m_hpBorderNormal) DeleteObject(this->m_hpBorderNormal);
	this->m_hpBorderNormal=CreatePen(PS_SOLID|PS_INSIDEFRAME, wNormal, clNormal);

	this->m_nBorderWidthHover=wHover;
	this->m_clBorderHover=clHover;
	if (this->m_hpBorderHover) DeleteObject(this->m_hpBorderHover);
	this->m_hpBorderHover=CreatePen(PS_SOLID|PS_INSIDEFRAME, wHover, clHover);

	this->m_nBorderWidthSelected=wSelected;
	this->m_clBorderSelected=clSelected;
	if (this->m_hpBorderSelected) DeleteObject(this->m_hpBorderSelected);
	this->m_hpBorderSelected=CreatePen(PS_SOLID|PS_INSIDEFRAME, wSelected, clSelected);

	this->m_nBorderWidthDisable=wDisable;
	this->m_clBorderDisable=clDisable;
	if (this->m_hpBorderDisable) DeleteObject(this->m_hpBorderDisable);
	this->m_hpBorderDisable=CreatePen(PS_SOLID|PS_INSIDEFRAME, wDisable, clDisable);


	this->_RecalcRectContent();
}

void Button_Base::SetPosition( int left, int top )
{
	this->m_rcBox.left=left;
	this->m_rcBox.top=top;
	this->m_rcBox.right=left+this->m_nBoxWidth;
	this->m_rcBox.bottom=top+this->m_nBoxHeight;

	this->_RecalcRectContent();
	this->Invalidate();
}

void Button_Base::SetPosition( int left, int top, int width, int height )
{
	this->m_rcBox.left=left;
	this->m_rcBox.top=top;
	this->m_rcBox.right=left+width;
	this->m_rcBox.bottom=top+height;
	this->m_nBoxHeight=height;
	this->m_nBoxWidth=width;

	this->_RecalcRectContent();
	this->Invalidate();
}

void Button_Base::SetSize( int width, int height )
{
	this->m_rcBox.right=this->m_rcBox.left+width;
	this->m_rcBox.bottom=this->m_rcBox.top+height;
	this->m_nBoxWidth=width;
	this->m_nBoxHeight=height;

	this->_RecalcRectContent();
}

void Button_Base::SetPush( BOOL bPush )
{
	this->m_bPush=bPush;
}

void Button_Base::OnClick()
{
	SendMessage(this->m_hParent, WMM_ONCLICK, (WPARAM)this, NULL);
}

BOOL Button_Base::IsSelected()
{
	return this->m_bIsSelected;
}

BOOL Button_Base::IsPushStyle()
{
	return this->m_bPush;
}

BOOL Button_Base::IsHover()
{
	return this->m_bIsHover;
}

void Button_Base::Click()
{
	this->m_bIsSelected=!this->m_bIsSelected;

	this->Invalidate();
	this->OnClick();
}

void Button_Base::GetPosition( RECT *rcPos )
{
	if (rcPos)
	{
		rcPos->left=this->m_rcBox.left;
		rcPos->top=this->m_rcBox.top;
		rcPos->right=this->m_rcBox.right;
		rcPos->bottom=this->m_rcBox.bottom;
	}
}

void Button_Base::SetImageFromMemory(void *mem, DWORD dwSize)
{
	LPVOID lpBuffer;
	IStream *stream;

	if (mem && dwSize>0)
	{
		this->m_hImgContent=GlobalAlloc(GMEM_MOVEABLE, dwSize);

		if (this->m_hImgContent)
		{
			lpBuffer=GlobalLock(this->m_hImgContent);
			stream=NULL;
			if (lpBuffer)
			{
				CopyMemory(lpBuffer, mem, dwSize);
				GlobalUnlock(this->m_hImgContent);

				if (CreateStreamOnHGlobal(this->m_hImgContent, TRUE, &stream)==S_OK)
				{
					this->m_imgContent=new Image(stream);
					stream->Release();
				}
			}
		}
	}
}

void Button_Base::SetImageFromResource( HMODULE hModule, TCHAR *szType, DWORD id )
{
	HRSRC hRsrc;
	DWORD dwSize;
	HGLOBAL hResData;
	LPVOID lpResData, lpBuffer;
	IStream *stream;

	if (this->m_imgContent)
	{
		delete this->m_imgContent;
	}

	if (this->m_hImgContent)
	{
		GlobalFree(this->m_hImgContent);
	}

	hRsrc=FindResource(hModule, MAKEINTRESOURCE(id), szType);
	if (hRsrc)
	{
		dwSize=SizeofResource(hModule, hRsrc)		;
		hResData=LoadResource(hModule, hRsrc);
		if (hResData)
		{
			lpResData=LockResource(hResData);
			if (lpResData)
			{
				//hBuffer=GlobalAlloc(GMEM_MOVEABLE, dwSize);
				this->m_hImgContent=GlobalAlloc(GMEM_MOVEABLE, dwSize);

				//if (hBuffer)
				if (this->m_hImgContent)
				{
					//lpBuffer=GlobalLock(hBuffer);
					lpBuffer=GlobalLock(this->m_hImgContent);
					stream=NULL;
					if (lpBuffer)
					{
						CopyMemory(lpBuffer, lpResData, dwSize);
						GlobalUnlock(this->m_hImgContent);

						stream=NULL;
						if (CreateStreamOnHGlobal(this->m_hImgContent, TRUE, &stream)==S_OK)
						{
							this->m_imgContent=new Image(stream);
							stream->Release();
						}

						//GlobalUnlock(hBuffer);
					}

					//GlobalFree(hBuffer);
				}

				UnlockResource(hResData);
			}

			FreeResource(hRsrc);
		}
	}
}

void Button_Base::SetImageFromFile( TCHAR *szFileName )
{
	if (this->m_imgContent)
	{
		delete this->m_imgContent;
	}

	//this->m_imgContent=new Image(szFileName);
	WCHAR tmp[512];
	wsprintfW(tmp, L"%S", szFileName);
	this->m_imgContent=new Image(tmp);
}

void Button_Base::OnHoverStart()
{
	if (!this->m_bIsSelected)
		this->Invalidate();
	SendMessage(this->m_hParent, WMM_ONHOVERSTART, (WPARAM)this, NULL);
}

void Button_Base::OnHoverEnd()
{
	if (!this->m_bIsSelected)
		this->Invalidate();
	SendMessage(this->m_hParent, WMM_ONHOVEREND, (WPARAM)this, NULL);
}

int Button_Base::GetWidth()
{
	return this->m_nBoxWidth;
}

int Button_Base::GetHeight()
{
	return this->m_nBoxHeight;
}

void Button_Base::Invalidate(BOOL bErase)
{
	InvalidateRect(this->m_hParent, &this->m_rcBox, bErase);
}

void Button_Base::SetForceState(BOOL bForce)
{
	this->m_bForceState=bForce;
}

BOOL Button_Base::IsForceState()
{
	return this->m_bForceState;
}
