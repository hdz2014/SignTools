#include "UIlib.h"
#include "Model.h"
#include <ShlObj.h>

using namespace DuiLib;
#define WM_ADDLISTITEM WM_USER + 50

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif

class CShadowWnd : public CWindowWnd
{
public:
	CShadowWnd() {};
	~CShadowWnd() {};

	LPCTSTR	GetWindowClassName() const { return _T("UIShadow"); };
	UINT		GetClassStyle() const {return UI_CLASSSTYLE_FRAME;};
	void			OnFinalMessage(HWND hWnd) {delete this; };

	void			RePaint();
	LRESULT	HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool			m_bNeedUpdate;
	CPaintManagerUI m_pm;
};

class CMenuWnd : public CWindowWnd, public INotifyUI
{
public:
	CMenuWnd() : m_pOwner(NULL), m_pShadowWnd(NULL) { };
	LPCTSTR			GetWindowClassName() const {return _T("MenuWnd"); };
	void			OnFinalMessage(HWND hWnd) {delete this; };

	void			Init(CControlUI* pOwner, POINT pt);
	void			AdjustPostion();
	void			Notify(TNotifyUI& msg);
	HWND			Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, 
										int y = CW_USEDEFAULT,int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	void			ShowWindow(bool bShow  = true , bool bTakeFocus  = true );
	LRESULT			OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT			OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT			OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT			OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT			OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT			OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT			HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CPaintManagerUI m_pm;
	CControlUI* m_pOwner;
	POINT m_ptPos;
	CShadowWnd* m_pShadowWnd;
};


class MainInterFace : public CWindowWnd, public INotifyUI, public IListCallbackUI
{
public:
	MainInterFace() { };
	~MainInterFace() {};

	LPCTSTR				GetWindowClassName() const {return _T("MainForm"); };
	UINT				GetClassStyle() const {return CS_DBLCLKS; };
	void				OnFinalMessage(HWND hWnd) { delete this; };

	void				Init();
	void				Notify(TNotifyUI& msg);
	LPCTSTR				GetItemText(CControlUI* pControl, int iIndex, int iSubItem);
	LRESULT				OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	hstring				GetDirName(bool IsDir,LPTSTR suffix);
	bool				CheckDataSave();
	int					GetCertName(vector<hstring> &vstr,CComboBoxUI *pValue);
	int					GetCspName(vector<hstring>& vstr,CComboBoxUI* pValue);

	static DWORD WINAPI	ThreadFunc(LPVOID lParam);
	
public:
	CPaintManagerUI m_pm;
private:
	CButtonUI*	m_pCloseBtn;
	CButtonUI*	m_pMaxBtn;
	CButtonUI*	m_pRestoreBtn;
	CButtonUI*	m_pMinBtn;
	Model		m_Model;
	vector<SignFileStr>	m_RetData;
	vector<hstring>		m_tabname;
	vector<bool>		m_tabbool;
	vector<hstring>		m_CertListName;
	vector<hstring>		m_CSPListName;
	CRITICAL_SECTION	m_cs; //ÁÙ½çÇø
};

