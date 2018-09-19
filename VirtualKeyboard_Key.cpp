#include "VirtualKeyboard_Key.h"

VirtualKeyboard_Key::VirtualKeyboard_Key()
:Button_Square(NULL, 0, 0, 0, 0, NULL, 0)
{
	this->_Init(FALSE, FALSE, TRUE, FALSE);
}

VirtualKeyboard_Key::VirtualKeyboard_Key( HWND hParent, int left, int top, int width, int height, int id, BOOL bPush, BOOL bControl, BOOL bFullDisplay, BOOL bSwap )
:Button_Square(NULL, left, top, width, height, hParent, id)
{
	this->_Init(bControl, bPush, bFullDisplay, bSwap);
}

void VirtualKeyboard_Key::_Init(BOOL bControl, BOOL bPush, BOOL bFullDisplay, BOOL bSwap)
{
	this->m_nIndexActive=0;
	this->m_nMaxIndex=0;
	this->m_lpszDisplay[0]=NULL;
	this->m_lpszDisplay[1]=NULL;
	this->m_bPush=bPush;
	this->m_bControlKey=bControl;
	this->m_bFullDisplay=bFullDisplay;
	this->m_bShowMask=FALSE;
	this->m_bSwap=bSwap;
}

VirtualKeyboard_Key::~VirtualKeyboard_Key()
{
	for (int i=0; i<2; i++)
	{
		if (this->m_lpszDisplay[i])
		{
			LocalFree(this->m_lpszDisplay[i]);
			this->m_lpszDisplay[i]=NULL;
		}
	}
}

void VirtualKeyboard_Key::SetText( char *lpszValue, char **lppszDisplay, int size )
{
	int len;

	if (lpszValue && lppszDisplay)
	{
		this->m_nMaxIndex=size;

		for (int i=0; i<size; i++)
		{
			this->m_lpszValue[i]=lpszValue[i];

			if (this->m_lpszDisplay[i])
			{
				LocalFree(this->m_lpszDisplay[i]);
				this->m_lpszDisplay[i]=NULL;
			}
			if (lppszDisplay[i])
			{
				len=strlen(lppszDisplay[i]);
				this->m_lpszDisplay[i]=(char*)LocalAlloc(LMEM_FIXED, len+1);
				CopyMemory(this->m_lpszDisplay[i], lppszDisplay[i], len+1);
			}
		}
	}
}

void VirtualKeyboard_Key::ShowMask(BOOL bShowMask)
{
	this->m_bShowMask=bShowMask;
	this->Invalidate();
}

void VirtualKeyboard_Key::_OnPaintText( HDC hdc, COLORREF color )
{
	SetBkMode(hdc, TRANSPARENT);

	if (this->m_bShowMask)
	{
		SetTextColor(hdc, color);
		DrawTextA(hdc, "*", -1, &this->m_rcContent, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else if (this->m_bFullDisplay)
	{
		if (this->m_nIndexActive==0)
		{
			SetTextColor(hdc, color);
		}
		else
		{
			SetTextColor(hdc, RGB(110, 110, 116));
		}
		DrawTextA(hdc, this->m_lpszDisplay[0], -1, &this->m_rcContent, DT_RIGHT|DT_SINGLELINE|DT_BOTTOM);

		if (this->m_nIndexActive==1)
		{
			SetTextColor(hdc, color);
		}
		else
		{
			SetTextColor(hdc, RGB(110, 110, 116));
		}
		DrawTextA(hdc, this->m_lpszDisplay[1], -1, &this->m_rcContent, DT_LEFT|DT_SINGLELINE|DT_TOP);
	}
	else
	{
		SetTextColor(hdc, color);
		DrawTextA(hdc, this->m_lpszDisplay[this->m_nIndexActive], -1, &this->m_rcContent, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	}
}

int VirtualKeyboard_Key::GetIndexActive()
{
	return this->m_nIndexActive;
}

void VirtualKeyboard_Key::SetIndexActive( int index )
{
	int old;

	if (this->m_nMaxIndex<2) return;

	old=this->m_nIndexActive;
	this->m_nIndexActive=index;
	if (old!=index)
		this->Invalidate();
}

void VirtualKeyboard_Key::SwitchActive()
{
	if (this->m_nMaxIndex>1)
	{
		this->m_nIndexActive=1-this->m_nIndexActive;
		this->Invalidate();
	}
}

BOOL VirtualKeyboard_Key::IsControl()
{
	return this->m_bControlKey;
}

int VirtualKeyboard_Key::GetMaxIndex()
{
	return this->m_nMaxIndex;
}

BOOL VirtualKeyboard_Key::IsFulLDisplay()
{
	return this->m_bFullDisplay;
}

BOOL VirtualKeyboard_Key::IsSwap()
{
	return this->m_bSwap;
}

char VirtualKeyboard_Key::GetValue()
{
	if (this->m_nMaxIndex>0)
	{
		return this->m_lpszValue[this->m_nIndexActive];
	}
	return 0;
}

char* VirtualKeyboard_Key::GetName()
{
	if (this->m_nMaxIndex>0)
	{
		return this->m_lpszDisplay[this->m_nIndexActive];
	}

	return NULL;
}

