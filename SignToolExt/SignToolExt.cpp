// SignToolExt.cpp : DLL 导出的实现。


#include "stdafx.h"
#include "resource.h"
#include "SignToolExt_i.h"
#include "dllmain.h"


// 用于确定 DLL 是否可由 OLE 卸载。
STDAPI DllCanUnloadNow(void)
{
			return _AtlModule.DllCanUnloadNow();
	}

// 返回一个类工厂以创建所请求类型的对象。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
		return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - 在系统注册表中添加项。
STDAPI DllRegisterServer(void)
{
	// 注册对象、类型库和类型库中的所有接口
	HRESULT hr;
	HKEY hKey;

	static char pszGUID[] = "{56BC0909-03F5-4EF9-AE7B-2846AB28B33E}";

	hr = _AtlModule.DllRegisterServer();
	if (FAILED(hr))
	{
		return hr;
	}

	if (RegCreateKeyA(HKEY_CLASSES_ROOT, 
		"*\\shellex\\ContextMenuHandlers\\CtxMenu", &hKey) != ERROR_SUCCESS)
	{
		return E_FAIL;
	}

	if (RegSetValueA(hKey, NULL, REG_SZ, pszGUID,
		(DWORD)strlen(pszGUID)) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return E_FAIL;
	}
	return hr;
}

// DllUnregisterServer - 在系统注册表中移除项。
STDAPI DllUnregisterServer(void)
{
	RegDeleteKeyA(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\CtxMenu");
	HRESULT hr = _AtlModule.DllUnregisterServer();
		return hr;
}

// DllInstall - 按用户和计算机在系统注册表中逐一添加/移除项。
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}


