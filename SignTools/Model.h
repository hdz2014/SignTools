#include <Windows.h>
#include <WinTrust.h>
#include <SoftPub.h>
#include <mscat.h>
#include <tchar.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
using namespace std;

#pragma comment(lib,"WinTrust.lib")

#ifdef _UNICODE
#define hstring wstring
#else
#define hstring string
#endif

struct SignFileStr
{
	hstring FilePath;
	bool  isSign;
};


class Model
{
public:
	Model():IniPath(_T("")) { };
	~Model() {};
	int					SetConfig(hstring name, hstring Data);
	hstring				GetConfig(hstring name);
	int					OpenDir(hstring dirpath);
	bool				CheckPE(hstring Filepath);
	bool				CheckSign(hstring FilePath);
	bool				DeleteFlag(int index);
	bool				Sign(int index);
	bool				Sign(hstring FilePath);
	bool				SetIniPath(hstring inpath);

	vector<SignFileStr>		GetSignFile();
private:
	hstring					IniPath;
	vector<hstring>			PEFile;
	vector<SignFileStr>		SignFile;
};