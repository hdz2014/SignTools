// CHandleMenu.cpp : CCHandleMenu 的实现

#include "stdafx.h"
#include "CHandleMenu.h"


// CCHandleMenu
#define SZ_MENUTEXT TEXT("使用signtools签名该PE文件")

STDMETHODIMP CCHandleMenu::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_ICHandleMenu
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CCHandleMenu::Initialize(LPCITEMIDLIST pidlFolder,
	/* [in] */ IDataObject *pdtobj,
	/* [in] */ HKEY hkeyProgID)
{
	HRESULT hr;
	UINT    nFileCount;
	UINT    nLen;

	FORMATETC fmt = 
	{
		CF_HDROP,
		NULL,
		DVASPECT_CONTENT,
		-1,
		TYMED_HGLOBAL
	};

	STGMEDIUM sm = 
	{
		TYMED_HGLOBAL
	};

	hr = pdtobj->GetData(&fmt, &sm);

	if (FAILED(hr))
	{
		return hr;
	}

	nFileCount = DragQueryFile((HDROP)sm.hGlobal, 0xFFFFFFFF, NULL, 0);

	if (nFileCount >= 1)
	{
		for (int i =0; i < nFileCount; ++i)
		{
			memset(m_szFile,0,260);
			nLen = DragQueryFile((HDROP)sm.hGlobal, i, m_szFile, sizeof(m_szFile));
			if (nLen >0 && nLen <MAX_PATH)
			{
				m_strFileList.append(m_szFile);
				m_strFileList.append(L"|");
				hr = S_OK;
			}
		}
	}
	else
	{            
		hr = E_INVALIDARG;
	}

	ReleaseStgMedium(&sm);

	return hr;
}

STDMETHODIMP CCHandleMenu::QueryContextMenu(HMENU hmenu,
	UINT indexMenu,
	UINT idCmdFirst,
	UINT idCmdLast,
	UINT uFlags)
{
	hstring m_ptname;
	m_ptname.append(m_szFile);
	if (!CheckPE(m_ptname))
		return S_FALSE;
	MENUITEMINFO mii;
    if (uFlags & CMF_DEFAULTONLY)
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }
    memset((void*)&mii, 0, sizeof(mii));
    mii.cbSize      = sizeof(mii);
    mii.fMask       = MIIM_STRING | MIIM_CHECKMARKS | MIIM_ID | MIIM_STATE;
    mii.cch         = lstrlen(SZ_MENUTEXT);
    mii.dwTypeData  = SZ_MENUTEXT;
    /*
        这里用hbmpChecked而不用hbmpItem的原因
        - -自己动手试试就知道了。
    */
    mii.hbmpChecked = m_hBitmap;
    mii.hbmpUnchecked = m_hBitmap;
    mii.fState      = MFS_ENABLED;
    mii.wID         = idCmdFirst + indexMenu;
    if (!InsertMenuItem(hmenu, indexMenu, TRUE, &mii))       
    {
            return E_FAIL;
    }
    lstrcpynA(m_pszVerb, "protected_run", 32);
    lstrcpynW(m_pwszVerb, L"protected_run", 32);
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, IDM_CTXMENU + 1);
}

STDMETHODIMP CCHandleMenu::InvokeCommand(CMINVOKECOMMANDINFO *lpici)
{
	BOOL fEx = FALSE;
	BOOL fUnicode = FALSE;

	if(lpici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
	{
		fEx = TRUE;
		if((lpici->fMask & CMIC_MASK_UNICODE))
		{
			fUnicode = TRUE;
		}
	}
	if( !fUnicode && HIWORD(lpici->lpVerb))
	{
		if(StrCmpIA(lpici->lpVerb, m_pszVerb))
		{
			return E_FAIL;
		}
	}
	else if( fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX *) lpici)->lpVerbW))
	{
		if(StrCmpIW(((CMINVOKECOMMANDINFOEX *)lpici)->lpVerbW, m_pwszVerb))
		{
			return E_FAIL;
		}
	}
	else if(LOWORD(lpici->lpVerb) != IDM_CTXMENU)
	{
		return E_FAIL;
	}
	else
	{
		TCHAR m_tempCur[MAX_PATH];
		GetModuleFileName(_hInstance,m_tempCur, MAX_PATH);
		hstring TempCurPath;
		hstring TempSzName;
		TempCurPath.append(m_tempCur);
		TempSzName.append(m_strFileList);
		int Len = TempCurPath.find_last_of('\\');
		TempCurPath = TempCurPath.substr(0,Len);
#ifdef _X86_
		TempCurPath += _T("\\SignTools.exe");
#else
		TempCurPath += _T("\\..\\SignTools.exe");
#endif

		//在此处理点击事件.
		ShellExecute(0,_T("open"),TempCurPath.c_str(), TempSzName.c_str(),_T(""),SW_SHOWNORMAL);
		return S_OK;

	}
	return E_FAIL;
}

STDMETHODIMP CCHandleMenu::GetCommandString(UINT_PTR    idCmd,
	UINT        uType,
	UINT      * pwReserved,
	LPSTR       pszName,
	UINT        cchMax)
{
	HRESULT  hr = E_INVALIDARG;
	static CHAR szHelpTextA[] = "PE签名!";
	static WCHAR szHelpTextW[] = L"PE签名!";

	if(idCmd != IDM_CTXMENU)
	{
		return hr;
	}

	switch(uType)
	{
	case GCS_HELPTEXTA:
		lstrcpynA((CHAR*)pszName, szHelpTextA, cchMax);
		break; 

	case GCS_HELPTEXTW: 
		lstrcpynW((WCHAR*)pszName, szHelpTextW, cchMax);;
		break; 

	case GCS_VERBA:
		lstrcpynA((CHAR*)pszName, m_pszVerb, cchMax);
		break; 

	case GCS_VERBW:
		lstrcpynW((WCHAR*)pszName, m_pwszVerb, cchMax);
		break;

	default:
		hr = S_OK;
		break; 
	}
	return hr;
}

bool CCHandleMenu::CheckPE(hstring Filepath)
{
	HANDLE handle = CreateFile(Filepath.c_str(),GENERIC_READ,FILE_SHARE_READ, NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (handle != INVALID_HANDLE_VALUE)
	{
		HANDLE hFileMapping = CreateFileMapping(handle,NULL,PAGE_READONLY,0, 0, NULL);  
		if (hFileMapping == NULL)    
		{  
			CloseHandle(handle);  
			return false;  
		}  
		LPVOID lpFile = MapViewOfFile(hFileMapping,FILE_MAP_READ,0, 0, 0);  
		if (lpFile==NULL)  //文件视图对象  
		{  
			CloseHandle(hFileMapping);  
			CloseHandle(handle);  
			return false;  
		}  
		PIMAGE_DOS_HEADER pDosHeader=NULL;  
		PIMAGE_NT_HEADERS32 pNtHeader32 = NULL;  
		//取得Dos头部  
		pDosHeader = (PIMAGE_DOS_HEADER)lpFile;  
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)  
		{  
			UnmapViewOfFile(lpFile);  
			CloseHandle(hFileMapping);  
			CloseHandle(handle);  
			return false;  
		}  
		//获取NT头  
		pNtHeader32 = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader + pDosHeader->e_lfanew);  
		//判断是不是PE文件  
		if (pNtHeader32->Signature != IMAGE_NT_SIGNATURE)  
		{  
			UnmapViewOfFile(lpFile);  
			CloseHandle(hFileMapping);  
			CloseHandle(handle);  
			return false;  
		}  
		UnmapViewOfFile(lpFile);  
		CloseHandle(hFileMapping);  
		CloseHandle(handle);  
		return true;  
	}
	return false;
}