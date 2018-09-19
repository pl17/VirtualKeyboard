#include "VirtualKeyboard.h"
#include "PinkResource.h"
#include <time.h>

#include "UtilityDebug.h"

#pragma warning(disable:4312)

IOutputDebug *dbgConsole;

//local marco
#define GET_CELLPADDING(cellUnit) ((int)(cellUnit-1)*3)
#define GET_CELL_WH(client, cellUnit) (client-10-GET_CELLPADDING(cellUnit))/cellUnit

void VirtualKeyboard::_InitControls()
{
	this->m_hCbxStyle=CreateWindow(TEXT("combobox"), NULL, WS_CHILD|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE, 0, 0, 0, 0, this->m_hWnd, NULL, NULL, NULL);
	for (int i=0; i<VirtualKeyboard::m_nListStyle; i++)
		SendMessage(this->m_hCbxStyle, CB_ADDSTRING, NULL, NULL);
	SendMessage(this->m_hCbxStyle, CB_SETCURSEL, NULL, NULL);

	this->m_keysIndex=new int[VirtualKeyboard::m_nListKey];
	this->m_keys=new VirtualKeyboard_Key*[VirtualKeyboard::m_nListKey];

	for (int i=0; i<VirtualKeyboard::m_nListKey; i++)
	{
		//Init index key
		this->m_keysIndex[i]=i;

		if (VirtualKeyboard::m_listKey[i].szValue==NULL && VirtualKeyboard::m_listKey[i].szDisplay[0]==NULL
			&& VirtualKeyboard::m_listKey[i].szDisplay[1]==NULL)
		{
			this->m_keys[i]=NULL;
			continue;
		}

		this->m_keys[i]=new VirtualKeyboard_Key(this->m_hWnd, 0, 0, 0, 0, 0
			, VirtualKeyboard::m_listKey[i].bPush,VirtualKeyboard::m_listKey[i].bControl, VirtualKeyboard::m_listKey[i].bFullDisplay, VirtualKeyboard::m_listKey[i].bSwap);
		this->m_keys[i]->SetPadding(1, 1, 1, 1);
		this->m_keys[i]->SetText(VirtualKeyboard::m_listKey[i].szValue
			, (char**)VirtualKeyboard::m_listKey[i].szDisplay
			, strlen(VirtualKeyboard::m_listKey[i].szValue));
		if (VirtualKeyboard::m_listKey[i].wImageId)
		{
			this->m_keys[i]->SetImageFromResource(GetModuleHandle(NULL), TEXT("PINKRESOURCE"), VirtualKeyboard::m_listKey[i].wImageId);
		}
	}
	this->m_nKeys=VirtualKeyboard::m_nListKey;

	this->m_kCapsLock=this->_FindKeyByName("caps");
	this->m_kDice=this->_FindKeyByName("dice");
	this->m_kShift=this->_FindKeyByName("Shift");
	this->m_kShowMask=this->_FindKeyByName("Show");
	this->m_kUndo=this->_FindKeyByName("Reset");
	this->m_kExpand=this->_FindKeyByName("Expand");

	//Init keys
	this->SetShowMask(TRUE);
	this->SetStyle(VirtualKeyboard::m_listStyle[0].style, TRUE);

	//Resize
	RECT rc;
	this->m_rcBox.left=0;
	this->m_rcBox.top=0;

	this->m_rcBox.right=10+(VirtualKeyboard::m_nMaxWidthUnit*VirtualKeyboard::m_nMinWidth)
		+GET_CELLPADDING(VirtualKeyboard::m_nMaxWidthUnit);
	this->m_rcBox.bottom=10+(VirtualKeyboard::m_nMaxHeightUnit*VirtualKeyboard::m_nMinHeight)
		+GET_CELLPADDING(VirtualKeyboard::m_nMaxHeightUnit);

	AdjustWindowRect(&this->m_rcBox, GetWindowLong(this->m_hWnd, GWL_STYLE), FALSE);
	GetWindowRect(this->m_hWnd, &rc);
	MoveWindow(this->m_hWnd, rc.left, rc.top, this->m_rcBox.right-this->m_rcBox.left, this->m_rcBox.bottom-this->m_rcBox.top, TRUE);

	this->ResetDice(TRUE);

	SetWindowLong(this->m_hWnd, GWL_USERDATA, (LONG)this);
}

void VirtualKeyboard::Close()
{
	if (VirtualKeyboard::m_pthis)
	{
		LogWriteLine(dbgConsole, TEXT("VirtualKeyboard found instance"));
		WaitForSingleObject(VirtualKeyboard::m_hevWinInit, INFINITE);
		LogWriteLine(dbgConsole, TEXT("Done Wait"));

		SendMessage(VirtualKeyboard::m_pthis->m_hWnd, WM_CLOSE, 0, 0);
		WaitForSingleObject(VirtualKeyboard::m_pthis->m_hThreadVk, INFINITE);
		ResetEvent(VirtualKeyboard::m_hevWinInit);
	}
	else
	{
		LogWriteLine(dbgConsole, TEXT("VirtualKeyboard not found instance"));
	}
}

void VirtualKeyboard::_InitInstance(int left, int top, const void *szConfigFileName, int unitSize)
{
	VirtualKeyboard::Close();
	VirtualKeyboard::m_pthis=this;
	ResetEvent(VirtualKeyboard::m_hevWinInit);

	LogWriteLine(dbgConsole, TEXT("---------------------"));
	LogWriteLine(dbgConsole, TEXT("New Instance VirtualKeyboard"));

	this->m_bShiftKeyboard=FALSE;
	this->m_rcBox.left=0;
	this->m_rcBox.top=0;
	this->m_rcBox.right=0;
	this->m_rcBox.bottom=0;
	this->m_nKeys=0;
	this->m_nStyleIndex=0;

	this->m_bTrackMouse=FALSE;

	this->m_nInitLeft=left;
	this->m_nInitTop=top;

	this->m_szConfigFileNameW=NULL;
	this->m_szConfigFileNameA=NULL;

	if (szConfigFileName)
	{
		int size;
		if (unitSize==sizeof(char))
		{
			size=(1+strlen((const char*)szConfigFileName))*unitSize;
			this->m_szConfigFileNameA=(char*)LocalAlloc(LMEM_FIXED, size);
			CopyMemory(this->m_szConfigFileNameA, szConfigFileName, size);
		}
		else if (unitSize==sizeof(WCHAR))
		{
			size=(1+wcslen((const WCHAR*)szConfigFileName))*unitSize;
			this->m_szConfigFileNameW=(WCHAR*)LocalAlloc(LMEM_FIXED, size);
			CopyMemory(this->m_szConfigFileNameW, szConfigFileName, size);
		}
	}

	this->m_hThreadVk=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)VirtualKeyboard::_ThreadGUI, this, 0, NULL);
	this->m_hThreadKey=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)VirtualKeyboard::_ThreadKey, this, 0, NULL);
}

VirtualKeyboard::VirtualKeyboard()
{
	this->_InitInstance(0, 0, NULL, 0);
}
VirtualKeyboard::VirtualKeyboard(int left, int top, const char *szConfigFileName)
{
	this->_InitInstance(left, top, szConfigFileName, sizeof(char));
}

VirtualKeyboard::VirtualKeyboard(int left, int top, const WCHAR *szConfigFileName)
{
	this->_InitInstance(left, top, szConfigFileName, sizeof(WCHAR));
}

VirtualKeyboard::~VirtualKeyboard()
{
	LogWriteLine(dbgConsole, TEXT("Delete instance virtual keyboard"));

	//End Thread Key
	TerminateThread(this->m_hThreadKey, 0);
	CloseHandle(this->m_hThreadKey);
	CloseHandle(this->m_hThreadVk);

	//Delete properties
	for (int i=0; i<VirtualKeyboard::m_nListKey; i++)
		if (this->m_keys[i]) delete this->m_keys[i];
	delete[] this->m_keys;
	delete[] this->m_keysIndex;
	if (this->m_szConfigFileNameA)
		LocalFree(this->m_szConfigFileNameA);
	if (this->m_szConfigFileNameW)
		LocalFree(this->m_szConfigFileNameW);

	VirtualKeyboard::m_pthis=NULL;

	LogWriteLine(dbgConsole, TEXT("---------------------"));
}

void VirtualKeyboard::_OnMouseLeave()
{
	for (int i=0; i<this->m_nKeys; i++)
		if (this->m_keys[i]) this->m_keys[i]->OnMouseLeaveParent();
}

void VirtualKeyboard::_OnLMouseDown( int x, int y, DWORD buttons )
{
	for (int i=0; i<this->m_nKeys; i++)
		if (this->m_keys[i]) this->m_keys[i]->OnLMouseDown(x, y, buttons);
}

void VirtualKeyboard::_OnLMouseUp( int x, int y, DWORD buttons )
{
	for (int i=0; i<this->m_nKeys; i++)
		if (this->m_keys[i]) this->m_keys[i]->OnLMouseUp(x, y, buttons);
}

void VirtualKeyboard::_OnMouseMove( int x, int y, DWORD buttons )
{
	for (int i=0; i<this->m_nKeys; i++)
		if (this->m_keys[i]) this->m_keys[i]->OnMouseMove(x, y, buttons);
}

void VirtualKeyboard::_OnPaint( HDC hdc )
{
	for (int i=0; i<this->m_nKeys; i++)
		if (this->m_keys[i]) this->m_keys[i]->OnPaint(hdc);
}

void VirtualKeyboard::_OnClick( VirtualKeyboard_Key *key )
{
	char *s;

	s=key->GetName();
	if (s[0]=='%' && s[1]==NULL)
		LogWriteLine(dbgConsole, TEXT("%%"));
	else
	{
		if (key->IsPushStyle())
			LogWriteLineA(dbgConsole, "%s %s", key->GetName(), key->IsSelected()?"down":"up");
		else
			LogWriteLineA(dbgConsole, key->GetName());
	}

	//Simulator keyboard
	VirtualKeyboard::_SimulatorKeyboard(key);

	//Process effect
	if (this->m_kShift->IsSelected() && !key->IsControl() && !this->m_bShiftKeyboard)
	{
		this->m_kShift->Click();
	}
	else if (key==this->m_kShift && !this->m_bShiftKeyboard)
	{
		this->SwitchActiveAll();
	}
	else if (key==this->m_kCapsLock)
	{
		for (int i=0; i<this->m_nListKey; i++)
		{
			if (this->m_keys[i] && !this->m_keys[i]->IsFulLDisplay() && !this->m_keys[i]->IsControl())
				this->m_keys[i]->SwitchActive();
		}
	}
	else if (key==this->m_kShowMask && !key->IsHover())
	{
		this->SetShowMask(!key->IsSelected());
	}
	else if (key==this->m_kDice)
	{
		this->SwapRandom();
	}
	else if (key==this->m_kUndo)
	{
		this->ResetDice(FALSE);
	}
	else if (key==this->m_kExpand)
	{
		RECT rcBox, rcWin;
		GetWindowRect(this->m_hWnd, &rcWin);
		GetClientRect(this->m_hWnd, &rcBox);

		if (key->IsSelected())
		{
			rcBox.right+=70;
			ShowWindow(this->m_hCbxStyle, SW_SHOW);
		}
		else
		{
			int w;

			w=10+(VirtualKeyboard::m_nMaxWidthUnit*VirtualKeyboard::m_nMinWidth)
				+GET_CELLPADDING(VirtualKeyboard::m_nMaxWidthUnit);
			if (rcBox.right-rcBox.left>=w)
			{
				if (rcBox.right-rcBox.left-70>w) rcBox.right-=70;
				else rcBox.right=rcBox.left+w;
			}
				
			ShowWindow(this->m_hCbxStyle, SW_HIDE);
		}

		AdjustWindowRect(&rcBox, GetWindowLong(this->m_hWnd, GWL_STYLE), FALSE);
		MoveWindow(this->m_hWnd, rcWin.left, rcWin.top, rcBox.right-rcBox.left, rcBox.bottom-rcBox.top, TRUE);
	}
}

void VirtualKeyboard::SetIndexActive( int index )
{
	for (int i=0; i<this->m_nKeys; i++)
	{
		if (this->m_keys[i]) this->m_keys[i]->SetIndexActive(index);
	}
}

void VirtualKeyboard::SwapButton( int isrc, int ides )
{
	RECT rcSrc, rcDes;
	int iisrc, iides;

	if (isrc<this->m_nKeys && ides<this->m_nKeys)
	{
		iisrc=this->m_keysIndex[isrc];
		iides=this->m_keysIndex[ides];

		this->m_keys[iisrc]->GetPosition(&rcSrc);
		this->m_keys[iides]->GetPosition(&rcDes);

		this->m_keys[iisrc]->SetPosition(rcDes.left, rcDes.top, rcDes.right-rcDes.left, rcDes.bottom-rcDes.top);
		this->m_keys[iides]->SetPosition(rcSrc.left, rcSrc.top, rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}
}

void VirtualKeyboard::SwapRandom()
{
	int ran[60], index[60];
	int nindex, i, k, t;
	int iOld, iNew;

	//Fetch all button is not control
	nindex=0;
	for (i=0; i<this->m_nKeys; i++)
	{
		if (this->m_keys[i] && !this->m_keys[i]->IsControl() && this->m_keys[i]->IsSwap())
		{
			index[nindex]=i;
			ran[nindex]=nindex;
			nindex++;
		}
	}

	//Random index and store in array "ran"
	for (i=0; i<nindex-1; i++)
	{
		k=rand()%(nindex-1-i)+1;
		
		t=ran[i];
		ran[i]=ran[i+k];
		ran[i+k]=t;
	}

	for (i=0; i<nindex; i++)
	{
		iOld=index[i];
		iNew=index[ran[i]];
		this->SwapButton(iOld, iNew);
		t=this->m_keysIndex[iOld];
		this->m_keysIndex[iOld]=this->m_keysIndex[iNew];
		this->m_keysIndex[iNew]=t;
	}
}

void VirtualKeyboard::ResetDice(BOOL bFullReset)
{
	for (int i=0; i<this->m_nKeys; i++)
		this->m_keysIndex[i]=i;

	this->ResizeKeyboard();
}

void VirtualKeyboard::ResizeKeyboard()
{
	int w, h, t, l, line, ck, index;
	float fw;
	RECT rcClient;

	GetClientRect(this->m_hWnd, &rcClient);

	if (this->m_kExpand->IsSelected()) rcClient.right-=70;//Expand setting width = 70

	w=GET_CELL_WH(rcClient.right-rcClient.left, VirtualKeyboard::m_nMaxWidthUnit);
	h=GET_CELL_WH(rcClient.bottom-rcClient.top, VirtualKeyboard::m_nMaxHeightUnit);

	if (w<VirtualKeyboard::m_nMinWidth && h<VirtualKeyboard::m_nMinHeight) return;

	if (w<VirtualKeyboard::m_nMinWidth) w=VirtualKeyboard::m_nMinWidth;
	if (h<VirtualKeyboard::m_nMinHeight) h=VirtualKeyboard::m_nMinHeight;

	t=-h; l=0;
	ck=0;

	line=0;
	
	int *rowButton=VirtualKeyboard::m_layout.rows;
	int rowCount=VirtualKeyboard::m_layout.rowCount;

	int odd;
	int delta;

	for (int i=0; i<rowCount; i++)
	{
		t+=h+(i==0?VirtualKeyboard::m_layout.margintop:VirtualKeyboard::m_layout.padding_inner);
		l=VirtualKeyboard::m_layout.marginleft;

		odd=0;
		delta=0;
		for (int j=0; j<rowButton[i]; j++)
		{
			index=this->m_keysIndex[ck];
			fw=VirtualKeyboard::m_listKey[index].fWidth;

			if (fw-(int)fw>0.4) odd++;
			if (odd==2) delta=3;

			if (this->m_keys[index])
				this->m_keys[index]->SetPosition(l, t, fw*w+((int)(fw+0.5)-1)*3-delta, h);

			l+=fw*w+((int)(fw+0.5)-1)*3+3-delta;
			ck++;

			if (odd==2)
			{
				delta=0; odd=0;
			}
		}
	}

	MoveWindow(this->m_hCbxStyle, 10+(VirtualKeyboard::m_nMaxWidthUnit*w)+GET_CELLPADDING(VirtualKeyboard::m_nMaxWidthUnit)+5, 5, 60, 500, FALSE);
}

void VirtualKeyboard::SwitchActiveNums()
{
	for (int i=0; i<15; i++)
		if (this->m_keys[i]) this->m_keys[i]->SwitchActive();
}

void VirtualKeyboard::SwitchActiveAll()
{
	for (int i=0; i<this->m_nKeys; i++)
		if (this->m_keys[i]) this->m_keys[i]->SwitchActive();
}

void VirtualKeyboard::SetShowMask( BOOL bMask )
{
	for (int i=0; i<this->m_nKeys; i++)
	{
		if (this->m_keys[i] && !this->m_keys[i]->IsControl())
		{
			this->m_keys[i]->ShowMask(bMask);
		}
	}
}

void VirtualKeyboard::SetStyle( VirtualKeyboard_Style style, BOOL bRedraw )
{
	int index;

	index=0;
	for (int i=0; i<VirtualKeyboard::m_nListStyle; i++)
	{
		if (VirtualKeyboard::m_listStyle[i].style==style)
		{
			index=i;
			break;
		}
	}

	for (int i=0; i<this->m_nKeys; i++)
	{
		if (this->m_keys[i]==NULL) continue;

		this->m_keys[i]->SetColor(VirtualKeyboard::m_listStyle[index].clTextNormal, VirtualKeyboard::m_listStyle[index].clTextHover
			, VirtualKeyboard::m_listStyle[index].clTextSelected, VirtualKeyboard::m_listStyle[index].clTextDisable);
		this->m_keys[i]->SetBorder(1, VirtualKeyboard::m_listStyle[index].clBorderNormal, 1, VirtualKeyboard::m_listStyle[index].clBorderHover
			, 3, VirtualKeyboard::m_listStyle[index].clBorderSelected, 1, VirtualKeyboard::m_listStyle[index].clBorderDisable);

		if (bRedraw)
		{
			this->m_keys[i]->Invalidate();
		}
	}
}

HWND VirtualKeyboard::GetWindow()
{
	return this->m_hWnd;
}

void VirtualKeyboard::_OnKeyUp( DWORD keycode )
{
	switch(keycode)
	{
	case VK_SHIFT:
		this->m_bShiftKeyboard=FALSE;
		if (this->m_kShift->IsSelected())
			this->m_kShift->Click();
		break;
	}
}

void VirtualKeyboard::_OnKeyDown( DWORD keycode )
{
	switch(keycode)
	{
	case VK_SHIFT:
		if (!this->m_kShift->IsSelected())
			this->m_kShift->Click();
		this->m_bShiftKeyboard=TRUE;
		break;
	}
}

void VirtualKeyboard::_OnHoverStart( VirtualKeyboard_Key *key )
{
	if (this->m_kShowMask==key && !this->m_kShowMask->IsSelected())
	{
		this->SetShowMask(FALSE);
	}
}

void VirtualKeyboard::_OnHoverEnd( VirtualKeyboard_Key *key )
{
	if (this->m_kShowMask==key && !this->m_kShowMask->IsSelected())
	{
		this->SetShowMask(TRUE);
	}
}

VirtualKeyboard_Key* VirtualKeyboard::_FindKeyByName( char *szName )
{
	char *szKeyName;
	
	for (int i=0; i<this->m_nKeys; i++)
	{
		if (this->m_keys[i])
		{
			szKeyName=this->m_keys[i]->GetName();
			if (szKeyName)
			{
				if (_stricmp(szKeyName, szName)==0)
					return this->m_keys[i];
			}
		}
	}
	return NULL;
}

void VirtualKeyboard::GlobalInit()
{
	LogCreateConsole(&dbgConsole);

	srand(time(NULL));
	VirtualKeyboard::_RegisterClass();
	VirtualKeyboard::m_hevWinInit=CreateEvent(NULL, TRUE, FALSE, NULL);


	float f, mf;
	int t2o=0;

	mf=0;
	for (int i=0; i<VirtualKeyboard::m_layout.rowCount; i++)
	{
		f=0;
		for (int j=0; j<VirtualKeyboard::m_layout.rows[i]; j++)
		{
			f+=VirtualKeyboard::m_listKey[t2o].fWidth;
			t2o++;
		}
		if (f>mf) mf=f;
	}

	VirtualKeyboard::m_nMaxHeightUnit=VirtualKeyboard::m_layout.rowCount;
	VirtualKeyboard::m_nMaxWidthUnit=mf;
}

void VirtualKeyboard::GlobalClose()
{
	LogClose(dbgConsole);

	VirtualKeyboard::_UnRegisterClass();
	CloseHandle(VirtualKeyboard::m_hevWinInit);
}

BOOL VirtualKeyboard::_ProcessMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	BOOL bHandled;
	HDC hdc;
	PAINTSTRUCT ps;

	bHandled=TRUE;
	
	switch(message)
	{
	case WM_SIZE:
		this->ResizeKeyboard();
		break;
	case WM_MOUSELEAVE:
		this->_OnMouseLeave();
		break;
	case WM_MOUSEMOVE:
		if (!this->m_bTrackMouse)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_LEAVE;
			tme.dwHoverTime=0;
			tme.hwndTrack=hWnd;
			TrackMouseEvent(&tme);
		}

		this->_OnMouseMove(LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_LBUTTONDOWN:
		this->_OnLMouseDown(LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_LBUTTONUP:
		this->_OnLMouseUp(LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		this->_OnPaint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WMM_ONCLICK:
		this->_OnClick((VirtualKeyboard_Key*)wParam);
		break;
	case WMM_ONHOVERSTART:
		this->_OnHoverStart((VirtualKeyboard_Key*)wParam);
		break;
	case WMM_ONHOVEREND:
		this->_OnHoverEnd((VirtualKeyboard_Key*)wParam);
		break;
	default:
		bHandled=FALSE;
		break;
	}

	return bHandled;
}

void VirtualKeyboard::_CreateKeyboardWindow()
{
	this->m_hWnd=CreateWindowEx(WS_EX_NOACTIVATE|WS_EX_TOPMOST, VirtualKeyboard::m_szClassName, TEXT("VirtualKeyboard")
		, WS_OVERLAPPEDWINDOW
		//, CW_USEDEFAULT
		, this->m_nInitLeft
		//, 0
		, this->m_nInitTop
		, CW_USEDEFAULT
		, 0
		, NULL
		, NULL
		, NULL
		, NULL);

	ShowWindow(this->m_hWnd, SW_SHOW);
	UpdateWindow(this->m_hWnd);
}

void VirtualKeyboard::_SaveConfig()
{
	HANDLE hFile;
	VKB_CONFIG config;
	DWORD dwWritten;

	if (this->m_szConfigFileNameA)
		hFile=CreateFileA(this->m_szConfigFileNameA, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	else if (this->m_szConfigFileNameW)
		hFile=CreateFileW(this->m_szConfigFileNameW, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hFile=NULL;

	if (hFile!=NULL && hFile!=INVALID_HANDLE_VALUE)
	{
		config.bShow=FALSE;
		config.nStyleIndex=this->m_nStyleIndex;

		if (this->m_kShowMask) config.bShow=this->m_kShowMask->IsSelected();

		WriteFile(hFile, &config, sizeof(VKB_CONFIG), &dwWritten, NULL);

		CloseHandle(hFile);
	}
}

void VirtualKeyboard::_LoadConfig()
{
	HANDLE hFile;
	DWORD dwRead;
	VKB_CONFIG config;

	if (this->m_szConfigFileNameA)
		hFile=CreateFileA(this->m_szConfigFileNameA, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else if (this->m_szConfigFileNameW)
		hFile=CreateFileW(this->m_szConfigFileNameW, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hFile=NULL;

	if (hFile!=NULL && hFile!=INVALID_HANDLE_VALUE)
	{
		ReadFile(hFile, &config, sizeof(VKB_CONFIG), &dwRead, NULL);
		if (dwRead==sizeof(VKB_CONFIG))
		{
			if (config.bShow)
				if (this->m_kShowMask && !this->m_kShowMask->IsSelected())
					this->m_kShowMask->Click();
			if (config.nStyleIndex<0 || config.nStyleIndex>=VirtualKeyboard::m_nListStyle)
				config.nStyleIndex=0;
			this->m_nStyleIndex=config.nStyleIndex;
			SendMessage(this->m_hCbxStyle, CB_SETCURSEL, (WPARAM)config.nStyleIndex, NULL);
			this->SetStyle(VirtualKeyboard::m_listStyle[config.nStyleIndex].style, TRUE);
		}

		CloseHandle(hFile);
	}
}

void VirtualKeyboard::_RegisterClass()
{
	WNDCLASSEX wcx;
	LPVOID lpResource;
	HRSRC hRsrc;
	HGLOBAL hMem;
	DWORD dwSize;
	int offset;

	hRsrc=FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_PINKRESOURCE_ICON_PINKVK), TEXT("PINKRESOURCE"));
	hMem=LoadResource(GetModuleHandle(NULL), hRsrc);
	lpResource=LockResource(hMem);
	dwSize=SizeofResource(GetModuleHandle(NULL), hRsrc);
	offset=LookupIconIdFromDirectoryEx((PBYTE)lpResource, TRUE, 16, 16, LR_DEFAULTCOLOR);

	VirtualKeyboard::m_hIconVk=CreateIconFromResourceEx((PBYTE)lpResource+offset, dwSize, TRUE, 0x00030000, 16, 16, LR_SHARED|LR_DEFAULTCOLOR);

	//UnlockResource(hMem);
	FreeResource(hMem);
	

	wcx.cbSize=sizeof(WNDCLASSEX);
	wcx.cbClsExtra=0;
	wcx.cbWndExtra=0;
	wcx.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.hCursor=LoadCursor(NULL, IDC_ARROW);
	wcx.hIcon=VirtualKeyboard::m_hIconVk;
	
	wcx.hIconSm=NULL;
	wcx.hInstance=NULL;
	wcx.lpfnWndProc=VirtualKeyboard::_WndProc;
	wcx.lpszClassName=VirtualKeyboard::m_szClassName;
	wcx.lpszMenuName=NULL;
	wcx.style=CS_HREDRAW|CS_VREDRAW;

	RegisterClassEx(&wcx);
}

void VirtualKeyboard::_UnRegisterClass()
{
	if (VirtualKeyboard::m_hIconVk)
		DestroyIcon(VirtualKeyboard::m_hIconVk);

	UnregisterClass(VirtualKeyboard::m_szClassName, GetModuleHandle(NULL));
}

LRESULT CALLBACK VirtualKeyboard::_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	VirtualKeyboard *vk;

	vk=(VirtualKeyboard*)GetWindowLong(hWnd, GWL_USERDATA);
	if (vk)
	{
 		if (vk->_ProcessMessage(hWnd, message, wParam, lParam))
 			return 0;
	}

	switch(message)
	{
	case WM_CREATE:
		SetEvent(VirtualKeyboard::m_hevWinInit);
		break;
	case WM_COMMAND:
		if ((HWND)lParam==vk->m_hCbxStyle)
		{
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				if (vk)
				{
					int index;
					index=SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
					if (index<0 || index>VirtualKeyboard::m_nListStyle) index=0;
					vk->m_nStyleIndex=index;
					vk->SetStyle(VirtualKeyboard::m_listStyle[index].style, TRUE);
				}
			}
		}
		break;
	case WM_DRAWITEM:
		LPDRAWITEMSTRUCT lpdis;
		HBRUSH hbr;
		RECT rcContent;

		lpdis=(LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID==-1)
			break;

		rcContent.left=lpdis->rcItem.left+1;
		rcContent.right=lpdis->rcItem.right-1;
		rcContent.top=lpdis->rcItem.top+1;
		rcContent.bottom=lpdis->rcItem.bottom-1;

		hbr=CreateSolidBrush(VirtualKeyboard::m_listStyle[lpdis->itemID].clTextNormal);
		FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)GetStockObject(GRAY_BRUSH));
		FillRect(lpdis->hDC, &rcContent, hbr);
		DeleteObject(hbr);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

DWORD VirtualKeyboard::_ThreadGUI( LPVOID lpParam )
{
	VirtualKeyboard* vk;

	vk=(VirtualKeyboard*)lpParam;
	LogWriteLine(dbgConsole, TEXT("StartVirtualKeyboard Thread"));
	if (vk)
	{
		vk->_CreateKeyboardWindow();
		vk->_InitControls();

		//Load Config Virtual keyboard
		LogWriteLine(dbgConsole, TEXT("VirtualKeyboard LoadConfig"));
		vk->_LoadConfig();


		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Save Config Virtual keyboard
		LogWriteLine(dbgConsole, TEXT("VirtualKeyboard SaveConfig"));
		vk->_SaveConfig();
	}
	delete vk;
	LogWriteLine(dbgConsole, TEXT("EndVirtualKeyboard Thread"));

	return 0;
}

DWORD VirtualKeyboard::_ThreadKey( LPVOID lpParam )
{
	VirtualKeyboard *vk;
	BOOL bShift;

	vk=(VirtualKeyboard*)lpParam;
	bShift=FALSE;
	
	if (vk)
	{
		while (1)
		{
			Sleep(100);

			if (GetAsyncKeyState(VK_SHIFT))
			{
				if (!bShift)
				{
					bShift=TRUE;
					vk->m_kShift->SetForceState(TRUE);
					vk->_OnKeyDown(VK_SHIFT);
				}
			}
			else if (bShift)
			{
				bShift=FALSE;
				vk->m_kShift->SetForceState(FALSE);
				vk->_OnKeyUp(VK_SHIFT);
			}
		}
	}
	LogWriteLine(dbgConsole, TEXT("End Thread Key"));

	return 0;
}

void VirtualKeyboard::_SimulatorKeyboard( VirtualKeyboard_Key *key )
{
	char c;
	DWORD dwThreadId;
	GUITHREADINFO info;
	UINT scan;
	
	if (!key) return;

	c=key->GetValue();
	if (!key->IsControl() || c==VK_BACK || c==VK_DELETE)
	{
		dwThreadId=GetWindowThreadProcessId(GetForegroundWindow(), NULL);
		info.cbSize=sizeof(GUITHREADINFO);
		GetGUIThreadInfo(dwThreadId, &info);

		if (key->IsControl())
		{
			scan=MapVirtualKey(c, 0);

			PostMessage(info.hwndFocus, WM_KEYDOWN, (WPARAM)c, 1|(scan<<16));
			Sleep(1);
			PostMessage(info.hwndFocus, WM_KEYUP, (WPARAM)c, 1|(scan<<16)|(1<<29)|(1<<30)|(1<<31));
		}
		else
		{
			if (VirtualKeyboard::_IsPrivateCommunication(info.hwndFocus))
			{
				LogWriteLine(dbgConsole, TEXT("Private communication [%c]"), c);
				VirtualKeyboard::_VkPostEncryptMessage(info.hwndFocus, WM_CHAR, (WPARAM)c, NULL);
			}
			else
			{
				LogWriteLine(dbgConsole, TEXT("Public communication [%c]"), c);
				PostMessage(info.hwndFocus, WM_CHAR, (WPARAM)c, NULL);
			}
		}
	}
}

void CALLBACK VirtualKeyboard::_VkDecryptMessage( HWND hWnd, UINT *message, WPARAM *wParam, LPARAM *lParam )
{
	switch(*message)
	{
	case WMM_KEYDOWN:
		*message=WM_KEYDOWN;
		*wParam=(*wParam)^77;
		break;
	case WMM_KEYUP:
		*message=WM_KEYUP;
		*wParam=(*wParam)^88;
		break;
	case WMM_CHAR:
		*message=WM_CHAR;
		*wParam=(*wParam)^99;
		break;
	}
}

BOOL VirtualKeyboard::_VkPostEncryptMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_KEYDOWN:
		message=WMM_KEYDOWN;
		wParam=wParam^77;
		break;
	case WM_KEYUP:
		message=WMM_KEYUP;
		wParam=wParam^88;
		break;
	case WM_CHAR:
		message=WMM_CHAR;
		wParam=wParam^99;
		break;
	}
	return PostMessage(hWnd, message, wParam, lParam);
}

BOOL VirtualKeyboard::_IsPrivateCommunication( HWND hWnd )
{
	return SendMessage(hWnd, WMM_VKPING, NULL, NULL)==VKPING_KEY;
}

BOOL VirtualKeyboard::_VkIsPingMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return message==WMM_VKPING;
}

int VirtualKeyboard::_VkProcessPingMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message==WMM_VKPING)
		return VKPING_KEY;
	
	return 0;
}

LRESULT CALLBACK VirtualKeyboard::WndEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC org;
	
	if (message==WMM_VKPING)
		return VKPING_KEY;

	VirtualKeyboard::_VkDecryptMessage(hWnd, &message, &wParam, &lParam);
	org=(WNDPROC)GetWindowLong(hWnd, GWL_USERDATA);
	if (org)
	{
		return CallWindowProc(org, hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//Init text, type, style of keys
const VirtualKeyboard::VKEYBOARD VirtualKeyboard::m_listKey[]={
	{"\x1b", {"Esc"}, NULL, 1.0f, FALSE, TRUE, FALSE, FALSE},
	{"`~", {"`", "~"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"1!", {"1", "!"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"2@", {"2", "@"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"3#", {"3", "#"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"4$", {"4", "$"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"5%%", {"5", "%"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"6^", {"6", "^"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"7&", {"7", "&&"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"8*", {"8", "*"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"9(", {"9", "("}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"0)", {"0", ")"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"-_", {"-", "_"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"=+", {"=", "+"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"\x08", {"Bsp"}, IDR_PINKRESOURCE_ICON_BACKSPACE, 1.5f, FALSE, TRUE, FALSE, FALSE},
	
	{"\x14", {"Caps"}, NULL, 1.5f, TRUE, TRUE, FALSE, FALSE},
	{"qQ", {"q", "Q"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"wW", {"w", "W"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"eE", {"e", "E"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"rR", {"r", "R"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"tT", {"t", "T"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"yY", {"y", "Y"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"uU", {"u", "U"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"iI", {"i", "I"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"oO", {"o", "O"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"pP", {"p", "P"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"[{", {"[", "{"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"]}", {"]", "}"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"\\|", {"\\", "|"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"\x2e", {"Del"}, NULL, 1.0f, FALSE, TRUE, FALSE, FALSE},

	{"\x10", {"Shift"}, NULL, 2.0f, TRUE, TRUE, FALSE, FALSE},
	{"aA", {"a", "A"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"sS", {"s", "S"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"dD", {"d", "D"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"fF", {"f", "F"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"gG", {"g", "G"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"hH", {"h", "H"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"jJ", {"j", "J"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"kK", {"k", "K"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"lL", {"l", "L"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{";:", {";", ":"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"'\"", {"'", "\""}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"\x10", {"Dice"}, IDR_PINKRESOURCE_ICON_DICE, 2.5f, FALSE, TRUE, FALSE, FALSE},
	
	{"\x10", {"Show"}, NULL, 2.5f, TRUE, TRUE, FALSE, FALSE},
	{"zZ", {"z", "Z"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"xX", {"x", "X"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"cC", {"c", "C"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"vV", {"v", "V"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"bB", {"b", "B"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"nN", {"n", "N"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{"mM", {"m", "M"}, NULL, 1.0f, FALSE, FALSE, FALSE, TRUE},
	{",<", {",", "<"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{".>", {".", ">"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"/?", {"/", "?"}, NULL, 1.0f, FALSE, FALSE, TRUE, TRUE},
	{"\x10", {"Reset"}, IDR_PINKRESOURCE_ICON_RESET, 2.0f, FALSE, TRUE, FALSE, FALSE},
	{"\x10", {"Expand"}, IDR_PINKRESOURCE_ICON_SETTING, 1.0f, TRUE, TRUE, FALSE, FALSE},
};

//Total keys
const int VirtualKeyboard::m_nListKey=sizeof(VirtualKeyboard::m_listKey)/sizeof(VirtualKeyboard::m_listKey[0]);

//Config layout
static int __buttonsPerRow[]={15, 15, 13, 13};
VirtualKeyboard::VKB_LAYOUT VirtualKeyboard::m_layout={
	5, 5, 3,
	__buttonsPerRow,
	sizeof(__buttonsPerRow)/sizeof(__buttonsPerRow[0]),
};

//Config style
VirtualKeyboard::VKB_STYLE VirtualKeyboard::m_listStyle[]={
	{VKSTYLE_WHITE, 0x00ffffff, 0x00ffffff, 0x00000000, 0x00000000
				, 0x00ffffff, 0x00ffffff, 0x00777777, 0x00ffffff},
	{VKSTYLE_GREEN, 0x0000ff00, 0x00000000, 0x000066ff, 0x00000000
				, 0x0000a000, 0x0000a000, 0x0000a000, 0x0000a000},
	{VKSTYLE_AQUABLUE, 0x00d6d54f, 0x00120d40, 0x000000ff, 0x00000000
				, 0x00d6d54f, 0x00d6d54f, 0x00d6d54f, 0x00d6d54f},
	{VKSTYLE_ORANGE, 0x000066cc, 0x00d0fbff, 0x000099ff, 0x00000000
				, 0x000066cc, 0x000066cc, 0x000066cc, 0x000066cc},
	{VKSTYLE_PINK, 0x00b469ff, 0x00b434ff, 0x00ff00ff, 0x00000000
				, 0x00a05aff, 0x00a05aff, 0x00a05aff, 0x00a05aff},
	{VKSTYLE_CRIMSON, 0x005d63a6, 0x00608fae, 0x00504174, 0x00000000
				, 0x005d63a6, 0x005d63a6, 0x005d63a6, 0x005d63a6},
	{VKSTYLE_OLIVE, 0x001e827c, 0x0038d9c3, 0x00772877, 0x00000000
				, 0x001e827c, 0x001e827c, 0x001e827c, 0x001e827c},
};
const int VirtualKeyboard::m_nListStyle=sizeof(VirtualKeyboard::m_listStyle)/sizeof(VirtualKeyboard::m_listStyle[0]);

//Minimum width and height of key
const int VirtualKeyboard::m_nMinHeight=30;
const int VirtualKeyboard::m_nMinWidth=30;

//Max unit width and height
int VirtualKeyboard::m_nMaxHeightUnit=0;
float VirtualKeyboard::m_nMaxWidthUnit=0;

//Config window
const TCHAR* VirtualKeyboard::m_szClassName=TEXT("Pink_Keyboard");
HICON VirtualKeyboard::m_hIconVk=NULL;
VirtualKeyboard* VirtualKeyboard::m_pthis=NULL;
HANDLE VirtualKeyboard::m_hevWinInit=NULL;