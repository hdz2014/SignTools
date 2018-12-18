#include "Model.h"

int Model::SetConfig(hstring name, hstring Data)
{
	if (::WritePrivateProfileString(_T("Config"),name.c_str(), Data.c_str(), IniPath.c_str()))
		return 0;
	else
		return -1;
}

hstring Model::GetConfig(hstring name)
{
	hstring Ret;
	TCHAR RetStr[1024];
	::GetPrivateProfileString(_T("Config"),name.c_str(), _T("Error"), RetStr, 1024, IniPath.c_str());
	Ret.assign(RetStr);
	return Ret;
}

int Model::OpenDir(hstring dirpath)
{
	PEFile.clear();
	SignFile.clear();
	TCHAR szFile[MAX_PATH];
	TCHAR szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	_tcscpy(szFind,dirpath.c_str());
	_tcscat(szFind, _T("\\*.*"));
	HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		return -1;
	while(TRUE)
	{
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			
		}
		else
		{
			_tcscpy(szFile,dirpath.c_str());
			_tcscat(szFile,_T("\\"));
			_tcscat(szFile,FindFileData.cFileName);
			if (CheckPE(szFile))
			{
				PEFile.push_back(szFile);
				SignFileStr signStr;
				signStr.FilePath= FindFileData.cFileName;
				//std::map<hstring,bool>::iterator it = SignFile.begin();
				if (CheckSign(szFile))
				{
					//SignFile.insert(pair<hstring,bool>(FindFileData.cFileName, true));
					signStr.isSign = true;
					SignFile.push_back(signStr);
				}
				else
				{
					//SignFile.insert(pair<hstring,bool>(FindFileData.cFileName, false));
					signStr.isSign = false;
					SignFile.push_back(signStr);
				}
			}
		}
		if(!FindNextFile(hFind,&FindFileData))
			break;
	}
	FindClose(hFind);
	return 0;
}

bool Model::CheckPE(hstring Filepath)
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

bool Model::CheckSign(hstring FilePath)
{
	bool bRet = false;
	WINTRUST_DATA wd = { 0 };
	WINTRUST_FILE_INFO wfi = { 0 };
	WINTRUST_CATALOG_INFO wci = { 0 };
	CATALOG_INFO ci = { 0 };

	HCATADMIN hCatAdmin = NULL;
	if ( !CryptCATAdminAcquireContext( &hCatAdmin, NULL, 0 ) )
	{
		return false;
	}
	HANDLE hFile = CreateFile( FilePath.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		CryptCATAdminReleaseContext( hCatAdmin, 0 );
		return false;
	}
	DWORD dwCnt = 100;
	BYTE byHash[100];
	CryptCATAdminCalcHashFromFileHandle( hFile, &dwCnt, byHash, 0 );
	CloseHandle( hFile );
	LPTSTR pszMemberTag = new TCHAR[dwCnt * 2 + 1];
	for ( DWORD dw = 0; dw < dwCnt; ++dw )
	{
		wsprintf( &pszMemberTag[dw * 2], L"%02X", byHash[dw] );
	}

	HCATINFO hCatInfo = CryptCATAdminEnumCatalogFromHash( hCatAdmin,
		byHash, dwCnt, 0, NULL );
	if ( NULL == hCatInfo )
	{
		wfi.cbStruct       = sizeof( WINTRUST_FILE_INFO );
		wfi.pcwszFilePath  = FilePath.c_str();
		wfi.hFile          = NULL;
		wfi.pgKnownSubject = NULL;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_FILE;
		wd.pFile               = &wfi;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_REVOKE_NONE;
		wd.dwStateAction       = WTD_STATEACTION_IGNORE;
		wd.dwProvFlags         = WTD_SAFER_FLAG;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	else
	{
		CryptCATCatalogInfoFromContext( hCatInfo, &ci, 0 );
		wci.cbStruct             = sizeof( WINTRUST_CATALOG_INFO );
		wci.pcwszCatalogFilePath = ci.wszCatalogFile;
		wci.pcwszMemberFilePath  = FilePath.c_str();
		wci.pcwszMemberTag       = pszMemberTag;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_CATALOG;
		wd.pCatalog            = &wci;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;
		wd.dwProvFlags         = 0;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	HRESULT hr  = WinVerifyTrust( NULL, &action, &wd );
	bRet        = SUCCEEDED( hr );
	if ( NULL != hCatInfo )
	{
		CryptCATAdminReleaseCatalogContext( hCatAdmin, hCatInfo, 0 );
	}
	CryptCATAdminReleaseContext( hCatAdmin, 0 );
	delete[] pszMemberTag;
	return bRet;
}

vector<SignFileStr> Model::GetSignFile()
{
	return SignFile;
}

bool Model::DeleteFlag(int index)
{
	vector<hstring>::iterator Resvse = PEFile.begin();
	PEFile.erase(Resvse + index);
	return true;
}

bool Model::Sign(int index)
{
	if (index >= 0 && index < PEFile.size())
	{
		return Sign(PEFile.at(index));
		/*
		if (CheckSign(PEFile.at(index)))
			return false;
		hstring SignToolPath	= GetConfig(_T("SignToolPath"));
		hstring PassFile		= GetConfig(_T("PassFile"));
		hstring PassWord		= GetConfig(_T("PassWord"));
		hstring Description		= GetConfig(_T("Description"));
		hstring URL				= GetConfig(_T("URL"));
		hstring TimeURL			= GetConfig(_T("TimeURL"));
		hstring CommandLineStr	= SignToolPath + _T(" sign /f ") + PassFile ;
		CommandLineStr			+= _T(" /p ") + PassWord;
		if (!Description.empty())
		CommandLineStr			+= _T(" /d ") + Description;
		if (!URL.empty())
		CommandLineStr			+= _T(" /du ") + URL;
		if (!TimeURL.empty())
		CommandLineStr			+= _T(" /t ") + TimeURL;
		CommandLineStr				+= _T(" \"") + PEFile.at(index) + _T("\"");

		STARTUPINFO   StartupInfo;
		memset(&StartupInfo,0,sizeof(STARTUPINFO));
		StartupInfo.cb=sizeof(STARTUPINFO);
		StartupInfo.dwFlags=STARTF_USESHOWWINDOW;   
		StartupInfo.wShowWindow=SW_HIDE;   
		PROCESS_INFORMATION pi;
		if (CreateProcess(NULL, (LPWSTR)CommandLineStr.c_str(), NULL, NULL, TRUE , CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &pi))
		{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);       
		CloseHandle(pi.hProcess); 
		if (CheckSign(PEFile.at(index)))
		return true;
		}*/
	}
	return false;
}

bool Model::Sign(hstring FilePath)
{
	if (CheckSign(FilePath))
	{
		return false;
	}
	hstring SignToolPath		= GetConfig(_T("SignToolPath"));
	hstring PassFile			= GetConfig(_T("PassFile"));
	hstring PassWord			= GetConfig(_T("PassWord"));
	hstring Description			= GetConfig(_T("Description"));
	hstring URL					= GetConfig(_T("URL"));
	hstring TimeURL				= GetConfig(_T("TimeURL"));
	hstring SignType			= GetConfig(_T("Choice"));
	hstring SignCertName		= GetConfig(_T("SignCert"));
	hstring CspName				= GetConfig(_T("CSPName"));
	hstring ShaTimeUrl			= GetConfig(_T("TimeURL256"));

	hstring CommandLineStr;
	hstring CommandLineSha256;
	if (SignType == _T("0"))
	{
		CommandLineStr		= _T("\"");
		CommandLineStr		+= SignToolPath + _T("\" sign /n \"") + SignCertName ;
		if (!Description.empty())
			CommandLineStr			+= _T("\" /d \"") + Description;
		if (!URL.empty())
			CommandLineStr			+= _T("\" /du \"") + URL;
		if (!TimeURL.empty())
			CommandLineStr			+= _T("\" /t \"") + TimeURL;
		CommandLineStr				+= _T("\" \"") + FilePath + _T("\"");

		//组合256签名数据
		CommandLineSha256		= _T("\"");
		CommandLineSha256		+= SignToolPath + _T("\" sign /as /n \"") + SignCertName ;
		if (!Description.empty())
			CommandLineSha256			+= _T("\" /d \"") + Description;
		if (!URL.empty())
			CommandLineSha256			+= _T("\" /du \"") + URL;
		CommandLineSha256				+= _T("\" /fd sha256");
		if (!TimeURL.empty())
			CommandLineSha256			+= _T(" /tr \"") + ShaTimeUrl;
		CommandLineSha256				+= _T("\" \"") + FilePath + _T("\"");

	}
	else if (SignType == _T("1"))
	{
		CommandLineStr		= _T("\"");
		CommandLineStr		+= SignToolPath + _T("\" sign /f \"") + PassFile ;
		CommandLineStr				+= _T("\" /p \"") + PassWord;
		if (!Description.empty())
			CommandLineStr			+= _T("\" /d \"") + Description;
		if (!URL.empty())
			CommandLineStr			+= _T("\" /du \"") + URL;
		if (!TimeURL.empty())
			CommandLineStr			+= _T("\" /t \"") + TimeURL;
		CommandLineStr				+= _T("\" \"") + FilePath + _T("\"");

		//组合256签名数据
		CommandLineSha256		= _T("\"");
		CommandLineSha256		+= SignToolPath + _T("\" sign /as /f \"") + PassFile ;
		CommandLineSha256		+= _T("\" /p \"") + PassWord;
		if (!Description.empty())
			CommandLineSha256			+= _T("\" /d \"") + Description;
		if (!URL.empty())
			CommandLineSha256			+= _T("\" /du \"") + URL;
		CommandLineSha256				+= _T("\" /fd sha256");
		if (!TimeURL.empty())
			CommandLineSha256			+= _T(" /tr \"") + ShaTimeUrl;
		CommandLineSha256				+= _T("\" \"") + FilePath + _T("\"");
	}
	else if (SignType == _T("2"))
	{
		CommandLineStr = _T("\"");
		CommandLineStr += SignToolPath + _T("\" sign /csp \"") + CspName;
		if (!Description.empty())
			CommandLineStr += _T("\" /d \"") + Description;
		if (!URL.empty())
			CommandLineStr += _T("\" /du \"") + URL;
		if (!TimeURL.empty())
			CommandLineStr += _T("\" /t \"") + TimeURL;
		CommandLineStr += _T("\" \"") + FilePath + _T("\"");

		//组合256签名数据
		CommandLineSha256 = _T("\"");
		CommandLineSha256 += SignToolPath + _T("\" sign /csp \"") + CspName;
		if (!Description.empty())
			CommandLineSha256 += _T("\" /d \"") + Description;
		if (!URL.empty())
			CommandLineSha256 += _T("\" /du \"") + URL;
		CommandLineSha256 += _T("\" /fd sha256");
		if (!TimeURL.empty())
			CommandLineSha256 += _T(" /tr \"") + ShaTimeUrl;
		CommandLineSha256 += _T("\" \"") + FilePath + _T("\"");
	}
	/*hstring CommandLineStr		= SignToolPath + _T(" sign /f ") + PassFile ;
	CommandLineStr				+= _T(" /p ") + PassWord;
	if (!Description.empty())
	CommandLineStr			+= _T(" /d ") + Description;
	if (!URL.empty())
	CommandLineStr			+= _T(" /du ") + URL;
	if (!TimeURL.empty())
	CommandLineStr			+= _T(" /t ") + TimeURL;
	CommandLineStr				+= _T(" \"") + FilePath + _T("\"");*/

	STARTUPINFO   StartupInfo;
	memset(&StartupInfo,0,sizeof(STARTUPINFO));
	StartupInfo.cb=sizeof(STARTUPINFO);
	StartupInfo.dwFlags=STARTF_USESHOWWINDOW;   
	StartupInfo.wShowWindow=SW_HIDE;
	PROCESS_INFORMATION pi;
	if (CreateProcess(NULL, (LPWSTR)CommandLineStr.c_str(), NULL, NULL, TRUE , CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);       
		CloseHandle(pi.hProcess);
		memset(&pi,0,sizeof(pi));
		if (CheckSign(FilePath))
		{
			if (CreateProcess(NULL, (LPWSTR)CommandLineSha256.c_str(), NULL, NULL, TRUE , CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &pi))
			{
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hThread);       
				CloseHandle(pi.hProcess); 
			}
			return true;
		}
	}
	return false;
}

bool Model::SetIniPath(hstring inpath)
{
	IniPath = inpath;
	return true;
}
