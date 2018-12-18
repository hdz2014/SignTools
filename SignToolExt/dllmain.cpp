// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "SignToolExt_i.h"
#include "dllmain.h"

CSignToolExtModule _AtlModule;
HINSTANCE         _hInstance;

// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	_hInstance = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
