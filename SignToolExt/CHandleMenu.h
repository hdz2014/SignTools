// CHandleMenu.h : CCHandleMenu 的声明

#pragma once
#include "resource.h"       // 主符号
#include "shlobj.h"
#include "comdef.h"

#include "SignToolExt_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;
using namespace std;
#ifdef _UNICODE
#define hstring wstring
#else
#define hstring string
#endif

extern HINSTANCE         _hInstance;
// CCHandleMenu

class ATL_NO_VTABLE CCHandleMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCHandleMenu, &CLSID_CHandleMenu>,
	public ISupportErrorInfo,
	public IDispatchImpl<ICHandleMenu, &IID_ICHandleMenu, &LIBID_SignToolExtLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CCHandleMenu()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CHANDLEMENU)


BEGIN_COM_MAP(CCHandleMenu)
	COM_INTERFACE_ENTRY(ICHandleMenu)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	//shellext
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder,
		/* [in] */ IDataObject *pdtobj,
		/* [in] */ HKEY hkeyProgID);
	STDMETHOD(QueryContextMenu)(HMENU hmenu,
		UINT indexMenu,
		UINT idCmdFirst,
		UINT idCmdLast,
		UINT uFlags);
	HRESULT STDMETHODCALLTYPE InvokeCommand( 
		/* [annotation][in] */ 
		__in  CMINVOKECOMMANDINFO *pici);
	STDMETHOD(GetCommandString)(UINT_PTR,UINT,UINT *,LPSTR,UINT);
	


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_hBitmap = LoadBitmap(_hInstance, MAKEINTRESOURCE(IDB_MENU));
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_hBitmap)
		{
			DeleteObject(m_hBitmap);
		}
	}
protected:
	bool CheckPE(hstring Filepath);

public:
	enum 
	{
		IDM_CTXMENU = 0,
	};

private:
	TCHAR	m_szFile[MAX_PATH];
	hstring	m_strFileList;
	HBITMAP m_hBitmap;
	CHAR    m_pszVerb[32];
	WCHAR   m_pwszVerb[32];

};

OBJECT_ENTRY_AUTO(__uuidof(CHandleMenu), CCHandleMenu)
