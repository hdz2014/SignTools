// SignToolExt.cpp : DLL ������ʵ�֡�


#include "stdafx.h"
#include "resource.h"
#include "SignToolExt_i.h"
#include "dllmain.h"


// ����ȷ�� DLL �Ƿ���� OLE ж�ء�
STDAPI DllCanUnloadNow(void)
{
			return _AtlModule.DllCanUnloadNow();
	}

// ����һ���๤���Դ������������͵Ķ���
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
		return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - ��ϵͳע���������
STDAPI DllRegisterServer(void)
{
	// ע��������Ϳ�����Ϳ��е����нӿ�
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

// DllUnregisterServer - ��ϵͳע������Ƴ��
STDAPI DllUnregisterServer(void)
{
	RegDeleteKeyA(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\CtxMenu");
	HRESULT hr = _AtlModule.DllUnregisterServer();
		return hr;
}

// DllInstall - ���û��ͼ������ϵͳע�������һ���/�Ƴ��
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


