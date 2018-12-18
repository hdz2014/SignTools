#include "View.h"
#include <Wincrypt.h>
#pragma comment(lib,"Crypt32.lib")

inline HBITMAP CreateMemBitMap(HDC hdc, int cx, int cy, COLORREF **pBits)
{
	LPBITMAPINFO	lpbiSrc = NULL;
	lpbiSrc	= (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
		return NULL;
	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = cx;
	lpbiSrc->bmiHeader.biHeight = cy;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = cx * cy;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	HBITMAP hBitMap = CreateDIBSection(hdc, lpbiSrc, DIB_RGB_COLORS, (void **)pBits,NULL, NULL);
	if (lpbiSrc)
		delete[] lpbiSrc;
	return hBitMap;
}

void CShadowWnd::RePaint()
{
	RECT	 rcClient = { 0 };
	::GetClientRect(m_hWnd, &rcClient);
	DWORD	dwWidth	= rcClient.right - rcClient.left;
	DWORD	dwHeight	= rcClient.bottom - rcClient.top;

	HDC		hDcPaint	= ::GetDC(m_hWnd);
	HDC		hDcBackground = ::CreateCompatibleDC(hDcPaint);			//创建内存DC
	COLORREF *pBackgroundBits;
	HBITMAP hbmpBackground = CreateMemBitMap(hDcPaint, dwWidth, dwHeight, &pBackgroundBits);
	::ZeroMemory(pBackgroundBits, dwWidth * dwHeight * 4);
	HBITMAP hOldBitMap = (HBITMAP)::SelectObject(hDcBackground, hbmpBackground);
	CRenderEngine::DrawImageString(hDcBackground, &m_pm, rcClient, rcClient, _T("file='menu_bk.png' corner='40,8,8,8'"), NULL);

	RECT	rcWnd = { 0 };
	
	::GetWindowRect(m_hWnd, &rcWnd);
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	POINT	ptPos = { rcWnd.left, rcWnd.top };
	SIZE sizeWnd = {dwWidth, dwHeight};
	POINT ptSrc = { 0, 0 };
	UpdateLayeredWindow(m_hWnd, hDcPaint, &ptPos, &sizeWnd, hDcBackground, &ptSrc, 0, &bf, ULW_ALPHA);
	::SelectObject(hDcBackground, hOldBitMap);
	if (hDcBackground != NULL)
		::DeleteDC(hDcBackground);
	if (hbmpBackground != NULL)
		::DeleteObject(hbmpBackground);
	::ReleaseDC(m_hWnd, hDcPaint);
	m_bNeedUpdate = false;
}

LRESULT CShadowWnd::HandleMessage(UINT uMsg, WPARAM wParm, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
		SetTimer(m_hWnd, 9000, 100, NULL);
	else if (uMsg == WM_SIZE)
		m_bNeedUpdate = true;
	else if (uMsg == WM_CLOSE)
		KillTimer(m_hWnd, 9000);
	else if (uMsg == WM_TIMER)
	{
		if (LOWORD(wParm) == 9000 && m_bNeedUpdate == true)
		{
			if (!::IsIconic(m_hWnd))
				RePaint();
		}
	}
	return CWindowWnd::HandleMessage(uMsg, wParm, lParam);
}

//Menu
void CMenuWnd::Init(CControlUI* pOwner, POINT pt)
{
	if (pOwner == NULL)
		return;
	m_pOwner = pOwner;
	m_ptPos = pt;
	Create(pOwner->GetManager()->GetPaintWindow(), NULL, 
		WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW,pt.x, pt.y);
	HWND hWndParent = m_hWnd;
	while (::GetParent(hWndParent) != NULL)
		hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

void CMenuWnd::AdjustPostion()
{
	CDuiRect	rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.GetWidth();
	int nHeight = rcWnd.GetHeight();
	rcWnd.left	= m_ptPos.x;
	rcWnd.top	= m_ptPos.y;
	rcWnd.right	= rcWnd.left + nWidth;
	rcWnd.bottom	= rcWnd.top + nHeight;
	MONITORINFO	oMonitor = { };
	oMonitor.cbSize	= sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect	rcWork = oMonitor.rcWork;
	if (rcWnd.bottom > rcWork.bottom)
	{
		if (nHeight >= rcWork.GetHeight())
		{
			rcWnd.top = 0;
			rcWnd.bottom = nHeight;
		}else
		{
			rcWnd.bottom = rcWork.bottom;
			rcWnd.top = rcWnd.bottom - nHeight;
		}
	}

	if (rcWnd.right > rcWork.right)
	{
		if (nWidth >= rcWork.GetWidth())
		{
			rcWnd.left = 0;
			rcWnd.right = nWidth;
		}else
		{
			rcWnd.right = rcWork.right;
			rcWnd.left	= rcWnd.right - nWidth;
		}
	}
	::SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(),rcWnd.GetHeight(),SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CMenuWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("itemselect"))
		Close();
	else if ( msg.sType == _T("itemclick"))
	{
		if (msg.pSender->GetName() == _T("menu_Delete"))
		{
			if (m_pOwner)
				m_pOwner->GetManager()->SendNotify(m_pOwner, _T("menu_Delete"), 0, 0, true);
		}
	}
}

HWND CMenuWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x /* = CW_USEDEFAULT */, 
	int y /* = CW_USEDEFAULT */,int cx /* = CW_USEDEFAULT */, int cy /* = CW_USEDEFAULT */, HMENU hMenu /* = NULL */)
{
	if (m_pShadowWnd == NULL)
		m_pShadowWnd = new CShadowWnd;
	m_pShadowWnd->Create(hwndParent, _T(""), WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS, WS_EX_LAYERED | WS_EX_TOOLWINDOW, x, y, cx, cy, NULL);
	dwExStyle |= WS_EX_TOOLWINDOW;
	return	CWindowWnd::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu);
}

void CMenuWnd::ShowWindow(bool bShow /* = true  */, bool bTakeFocus /* = true */ )
{
	if (m_pShadowWnd != NULL)
		m_pShadowWnd->ShowWindow(bShow,false);
	CWindowWnd::ShowWindow(bShow, bTakeFocus);
}

LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	m_pm.Init(m_hWnd);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("menu.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
	m_pm.SetRoundCorner(3,3);

	CListUI *pList = static_cast<CListUI*>(m_pOwner);
	int nSel	 = pList->GetCurSel();
	if (nSel < 0)
	{
		CControlUI *pControl = m_pm.FindControl(_T("menu_Delete"));
		if (pControl)
			pControl->SetEnabled(false);
	}

	AdjustPostion();
	return 0;
}

LRESULT CMenuWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pShadowWnd != NULL)
		m_pShadowWnd->Close();
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if ( (HWND)wParam == m_hWnd)
		bHandled = TRUE;
	else if (m_pShadowWnd != NULL && (HWND)wParam == m_pShadowWnd->GetHWND())
	{
		CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		::SetFocus(m_hWnd);
		bHandled = TRUE;
	}else
	{
		Close();
		bHandled = FALSE;
	}
	return 0;
}

LRESULT CMenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (wParam == VK_ESCAPE)
		Close();
	return 0;
}

LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (!::IsIconic(*this))
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		HRGN hRgn = ::CreateRectRgn(rcWnd.left + 8, rcWnd.top + 8, rcWnd.right - 8, rcWnd.bottom -8);
		::SetWindowRgn(*this, hRgn,  TRUE);
		::DeleteObject(hRgn);
	}
	if (m_pShadowWnd != NULL)
	{
		RECT  rcWnd = { 0 };
		::GetWindowRect(m_hWnd, &rcWnd);
		::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left, rcWnd.top,  rcWnd.right -rcWnd.left,
			rcWnd.bottom -rcWnd.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pShadowWnd != NULL)
	{
		RECT rcWnd = { 0 };
		::GetWindowRect(m_hWnd, &rcWnd);
		::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left, rcWnd.top,  rcWnd.right -rcWnd.left,
			rcWnd.bottom -rcWnd.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:				lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		break;
	case WM_SIZE:					lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_MOVE:				lRes = OnMove(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

//MainInterFace
void MainInterFace::Init()
{
	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	CEditUI *pfxEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfx")));
	CEditUI *pfxpassEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfxpass")));
	//CEditUI *toolEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputsigntool")));
	CEditUI *DescptionEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputDscrption")));
	CEditUI *TimeUrlEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputTimeURL")));
	CEditUI *ShaTimeUrlEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputSha256Time")));
	CEditUI *URLEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputURL")));
	//toolEdit->SetText(m_Model.GetConfig(_T("SignToolPath")).c_str());
	pfxEdit->SetText(m_Model.GetConfig(_T("PassFile")).c_str());
	pfxpassEdit->SetText(m_Model.GetConfig(_T("PassWord")).c_str());
	DescptionEdit->SetText(m_Model.GetConfig(_T("Description")).c_str());
	URLEdit->SetText(m_Model.GetConfig(_T("URL")).c_str());
	TimeUrlEdit->SetText(m_Model.GetConfig(_T("TimeURL")).c_str());
	ShaTimeUrlEdit->SetText(m_Model.GetConfig(_T("TimeURL256")).c_str());


	//设置初始化信息
	CComboBoxUI *CertNameComBox = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("CertListData")));
	if (0 == GetCertName(m_CertListName, CertNameComBox))
	{
		CertNameComBox->SelectItem(0);
		hstring cername = m_Model.GetConfig(_T("SignCert"));
		for (int i = 0; i < m_CertListName.size(); ++i)
		{
			if (cername == m_CertListName.at(i))
			{
				CertNameComBox->SelectItem(i);
			}
		}
	}

	CComboBoxUI *CSPNameComBox = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("CSPListData")));
	if (0 == GetCspName(m_CSPListName, CSPNameComBox))
	{
		CSPNameComBox->SelectItem(0);
		hstring cspname = m_Model.GetConfig(_T("CSPName"));
		for (int i = 0; i < m_CSPListName.size(); ++i)
		{
			if (cspname == m_CSPListName.at(i))
			{
				CSPNameComBox->SelectItem(i);
			}
		}
	}

	TCHAR pCharPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, pCharPath, MAX_PATH);
	hstring strPath(pCharPath);
	int pos = strPath.rfind(_T("\\"));
	if (-1 != pos)
	{
		hstring tDirPath = _T("\"");
		tDirPath += strPath.substr(0, pos);
		tDirPath += _T("\\signtool.exe\"");
		m_Model.SetConfig(_T("SignToolPath"), tDirPath.c_str());
	}


	int iItem = _tstoi(m_Model.GetConfig(_T("Choice")).c_str());
	CComboBoxUI *pCommBox = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("SignChoice")));
	CListLabelElementUI *m_optioncontaner = new CListLabelElementUI;
	m_optioncontaner->SetText(L"证书容器选择代码签名证书");
	m_optioncontaner->SetFixedHeight(23);
	m_optioncontaner->SetMinHeight(23);
	m_optioncontaner->SetOwner(pCommBox);
	//pCommBox->Add(m_optioncontaner);

	CListLabelElementUI *m_optionpfx = new CListLabelElementUI;
	m_optionpfx->SetText(L"PFX证书进行代码签名");
	m_optionpfx->SetFixedHeight(23);
	m_optionpfx->SetMinHeight(23);
	m_optionpfx->SetOwner(pCommBox);

	CListLabelElementUI *m_optionCsp = new CListLabelElementUI;
	m_optionCsp->SetText(L"使用USBKey证书签名");
	m_optionCsp->SetFixedHeight(23);
	m_optionCsp->SetMinHeight(23);
	m_optionCsp->SetOwner(pCommBox);
	//pCommBox->Add(m_optionpfx);

	if (pCommBox)
	{
		pCommBox->Add(m_optioncontaner);
		pCommBox->Add(m_optionpfx);
		pCommBox->Add(m_optionCsp);
		pCommBox->SetItemFont(1);
		pCommBox->SelectItem(iItem);
	}

	CHorizontalLayoutUI *pContHor = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("cerstoreTab")));
	CHorizontalLayoutUI *pPfxHor = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("pfxsignTab")));
	CHorizontalLayoutUI *pCspHor = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("cspstoreTab")));
	if (0 == iItem)
	{
		pContHor->SetVisible(true);
		pPfxHor->SetVisible(false);
		pCspHor->SetVisible(false);
		
	}
	else if (1 == iItem)
	{
		pContHor->SetVisible(false);
		pPfxHor->SetVisible(true);
		pCspHor->SetVisible(false);
	}
	else if (2 == iItem)
	{
		pContHor->SetVisible(false);
		pPfxHor->SetVisible(false);
		pCspHor->SetVisible(true);
	}

}

DWORD WINAPI SignThread(LPVOID lpParam)
{
	MainInterFace *pPoint = (MainInterFace*)lpParam;
	CListUI* pList = static_cast<CListUI*>(pPoint->m_pm.FindControl(_T("domainlist")));
	return 0;
}

void MainInterFace::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender == m_pCloseBtn ) 
		{
			PostQuitMessage(0);
			return; 
		}
		else if( msg.pSender == m_pMinBtn ) 
		{ 
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			return; 
		}
		else if( msg.pSender == m_pMaxBtn ) 
		{ 
			return;
			//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; 
		}
		else if( msg.pSender == m_pRestoreBtn ) 
		{ 
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
		}
		else if (msg.pSender->GetName() == _T("btnPfx"))
		{
			hstring ret = GetDirName(false,_T("Pfx证书文件\0*.pfx\0"));
			if (ret == _T(""))
				return;
			CEditUI *PFXEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfx")));
			PFXEdit->SetText(ret.c_str());
		}
		else if (msg.pSender->GetName() == _T("btnSignPath"))
		{
			hstring ret = GetDirName(false,_T("signtool\0signtool.exe\0exe文件\0*.exe\0"));
			if (ret == _T(""))
				return;
			CEditUI *ToolEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputsigntool")));
			ToolEdit->SetText(ret.c_str());
		}
		else if (msg.pSender->GetName() == _T("btnDir"))
		{
			if (!CheckDataSave())
			{
				MessageBox(m_hWnd,_T("请先点击保存以上配置，再选择需要签名的目录！"),_T("提示"),MB_OK);
				return;
			}
			hstring ret = GetDirName(true,_T(""));
			if (ret == _T(""))
				return;
			CEditUI *DirEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputDir")));
			CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("domainlist")));
			DirEdit->SetText(ret.c_str());
			m_Model.OpenDir(ret);
			m_RetData = m_Model.GetSignFile();
			pList->RemoveAll();
			m_tabname.clear();
			m_tabbool.clear();
			//pList->SetTextCallback(this);
			//map<hstring,bool>::iterator	mapIterator;
			//mapIterator = m_RetData.begin();
			int StrLen = m_RetData.size();
			int i = 0;
			for (; i < StrLen; i++)
			{
				m_tabname.push_back(m_RetData[i].FilePath);
				m_tabbool.push_back(m_RetData[i].isSign);
				CListTextElementUI* pListElement = new CListTextElementUI;
				pListElement->SetTag(i);
				pListElement->SetOwner(pList);
				if (pListElement != NULL)
				{
					pListElement->SetText(0,m_tabname[i].c_str());
					if (m_tabbool[i])
						pListElement->SetText(1, _T("已签名"));
					else
						pListElement->SetText(1, _T("未签名"));
					pListElement->SetText(2, _T(""));
					pList->Add(pListElement);
				}
			}
			/*for (; mapIterator != m_RetData.end(); mapIterator++)
			{
			m_tabname.push_back(mapIterator->first);
			m_tabbool.push_back(mapIterator->second);
			CListTextElementUI* pListElement = new CListTextElementUI;
			pListElement->SetTag(i);
			pListElement->SetOwner(pList);
			if (pListElement != NULL)
			{
			pListElement->SetText(0,m_tabname[i].c_str());
			if (m_tabbool[i])
			pListElement->SetText(1, _T("已签名"));
			else
			pListElement->SetText(1, _T("未签名"));
			pListElement->SetText(2, _T(""));
			pList->Add(pListElement);
			}
			i++;
			}*/

		}
		else if (msg.pSender->GetName() == _T("btnSave"))
		{
			CEditUI *pfxEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfx")));
			CEditUI *pfxpassEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfxpass")));
			//CEditUI *toolEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputsigntool")));
			CEditUI *DescptionEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputDscrption")));
			CEditUI *TimeUrlEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputTimeURL")));
			CEditUI *ShaTimeUrlEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputSha256Time")));
			CEditUI *URLEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputURL")));
			CComboBoxUI *pCommBox =static_cast<CComboBoxUI*>(m_pm.FindControl(_T("SignChoice")));
			int iItem = pCommBox->GetCurSel();
			CComboBoxUI *CertNameComBox = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("CertListData")));
			int iCertSelItem = CertNameComBox->GetCurSel();
			CComboBoxUI *CspNameComBox = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("CSPListData")));
			int iCSPSelItem = CspNameComBox->GetCurSel();

			hstring szItem;
			if (iItem == 0)
				szItem = _T("0");
			else if (iItem == 1)
				szItem = _T("1");
			else if (iItem == 2)
				szItem = _T("2");

			//m_Model.SetConfig(_T("SignToolPath"),toolEdit->GetText().GetData());
			m_Model.SetConfig(_T("PassFile"),pfxEdit->GetText().GetData());
			m_Model.SetConfig(_T("PassWord"),pfxpassEdit->GetText().GetData());
			m_Model.SetConfig(_T("Description"),DescptionEdit->GetText().GetData());
			m_Model.SetConfig(_T("URL"),URLEdit->GetText().GetData());
			m_Model.SetConfig(_T("TimeURL"),TimeUrlEdit->GetText().GetData());
			m_Model.SetConfig(_T("TimeURL256"),ShaTimeUrlEdit->GetText().GetData());
			m_Model.SetConfig(_T("Choice"),szItem.c_str());
			m_Model.SetConfig(_T("SignCert"),m_CertListName.at(iCertSelItem));
			m_Model.SetConfig(_T("CSPName"), m_CSPListName.at(iCSPSelItem));
			MessageBox(m_hWnd,_T("保存以上配置成功！"),_T("提示"),MB_OK);
		}
		else if (msg.pSender->GetName() == _T("btnSign"))
		{
			if (!CheckDataSave())
			{
				MessageBox(m_hWnd,_T("请先点击保存以上配置，再选择需要签名的目录！"),_T("提示"),MB_OK);
				return;
			}
			msg.pSender->SetEnabled(false);
			//InitializeCriticalSection(&m_cs);
			DWORD Threadid = 0;
			HANDLE hThread = CreateThread(NULL,0,ThreadFunc,this,0,&Threadid);
			//等待所有进程结束  
			//WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);  
			//删除临界区  
			//DeleteCriticalSection(&m_cs);  
			
		}
	}
	else if(msg.sType==_T("setfocus"))
	{
	}
	else if( msg.sType == _T("itemclick") ) 
	{
	}
	else if( msg.sType == _T("itemactivate") ) 
	{

	}
	else if (msg.sType == _T("itemselect"))
	{
		if (msg.pSender->GetName() == _T("SignChoice"))
		{
			CComboBoxUI *pCommBox =static_cast<CComboBoxUI*>(msg.pSender);
			int iItem = pCommBox->GetCurSel();
			CHorizontalLayoutUI *pContHor = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("cerstoreTab")));
			CHorizontalLayoutUI *pPfxHor = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("pfxsignTab")));
			CHorizontalLayoutUI *pCspHor = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("cspstoreTab")));
			
			if (0 == iItem)
			{
				pContHor->SetVisible(true);
				pPfxHor->SetVisible(false);
				pCspHor->SetVisible(false);
			}
			else if (1 == iItem)
			{
				pContHor->SetVisible(false);
				pPfxHor->SetVisible(true);
				pCspHor->SetVisible(false);
			}
			else if (2 == iItem)
			{
				pContHor->SetVisible(false);
				pPfxHor->SetVisible(false);
				pCspHor->SetVisible(true);
			}
		}
	}
	else if(msg.sType == _T("menu")) 
	{
		if( msg.pSender->GetName() != _T("domainlist") ) return;
		CMenuWnd* pMenu = new CMenuWnd();
		if( pMenu == NULL ) { return; }
		POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
		::ClientToScreen(*this, &pt);
		pMenu->Init(msg.pSender, pt);
	}
	else if( msg.sType == _T("menu_Delete") )
	{
		CListUI* pList = static_cast<CListUI*>(msg.pSender);
		int nSel = pList->GetCurSel();
		if( nSel < 0 ) return;
		pList->RemoveAt(nSel);
		m_Model.DeleteFlag(nSel);
		
		vector<hstring>::iterator ResvseStr = m_tabname.begin();
		vector<bool>::iterator ResvseBool	= m_tabbool.begin();
		m_tabname.erase(ResvseStr + nSel);
		m_tabbool.erase(ResvseBool + nSel);
	}
}


LRESULT MainInterFace::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_pm.Init(m_hWnd);
	TCHAR SZPath[MAX_PATH];
	GetModuleFileName(m_pm.GetInstance(),SZPath,MAX_PATH);
	hstring SPath = SZPath;
	int Len = SPath.find_last_of('\\');
	SPath = SPath.substr(0,Len);
	SPath	+= _T("\\SignConfig.ini");
	m_Model.SetIniPath(SPath);
	//m_pm.SetTransparent(100);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("skin.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
	Init();
	return 0;
}

LRESULT MainInterFace::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT MainInterFace::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);
	bHandled = FALSE;
	return 0;
}

LRESULT MainInterFace::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT MainInterFace::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT MainInterFace::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT MainInterFace::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) ) {
		RECT rcSizeBox = m_pm.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT MainInterFace::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		RECT rc = { rcWnd.left, rcWnd.top + szRoundCorner.cx, rcWnd.right, rcWnd.bottom };
		HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
		HRGN hRgn2 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
		::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
		::SetWindowRgn(*this, hRgn1, TRUE);
		::DeleteObject(hRgn1);
		::DeleteObject(hRgn2);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT MainInterFace::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT MainInterFace::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}

LRESULT MainInterFace::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_NCLBUTTONDBLCLK: bHandled = TRUE; break; //在非客户区不响应双击消息
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LPCTSTR MainInterFace::GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
{
	TCHAR szBuf[MAX_PATH] = {0};

	switch (iSubItem)
	{
	case 0:
		_stprintf(szBuf, _T("%s"), m_tabname[iIndex].c_str());
		break;
	case 1:
		if (m_tabbool[iIndex])
			_stprintf(szBuf, _T("已签名"));
		else
			_stprintf(szBuf, _T("未签名"));
		break;
	case 2:
		_stprintf(szBuf, _T(""));
		break;
	}
	pControl->SetUserData(szBuf);
	return pControl->GetUserData();
}

hstring	MainInterFace::GetDirName(bool IsDir,LPTSTR suffix)
{
	
	hstring RetStr;
	if (IsDir)
	{
		TCHAR szPathName[MAX_PATH];
		BROWSEINFO bInfo={0}; 
		bInfo.hwndOwner=m_hWnd;//父窗口 
		bInfo.lpszTitle=TEXT("请选择目录："); 
		bInfo.ulFlags=BIF_RETURNONLYFSDIRS |BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/| 
			BIF_UAHINT/*带TIPS提示*/ |BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
		LPITEMIDLIST lpDlist; 
		lpDlist=SHBrowseForFolder(&bInfo); 
		if (lpDlist!=NULL)//单击了确定按钮 
		{ 
			SHGetPathFromIDList(lpDlist,szPathName);
			RetStr = szPathName;
			return RetStr;
		}
	}
	else
	{
		if (suffix == NULL || suffix == _T(""))
			return _T("");
		OPENFILENAME ofn;  
		TCHAR szFile[MAX_PATH];
		ZeroMemory(&ofn, sizeof(ofn));  
		ofn.lStructSize = sizeof(ofn);  
		ofn.hwndOwner = m_hWnd;  
		ofn.lpstrFilter = suffix;
		lstrcpy(szFile, TEXT("")); 
		ofn.lpstrFile = szFile; 
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrTitle = TEXT("选择文件");
		TCHAR szCurDir[MAX_PATH]; 
		GetCurrentDirectory(sizeof(szCurDir),szCurDir); 
		ofn.lpstrInitialDir=szCurDir; 
		ofn.Flags = OFN_EXPLORER |OFN_ALLOWMULTISELECT| OFN_FILEMUSTEXIST;
		if ( GetOpenFileName(&ofn) )  
		{  
			RetStr = szFile;
			return RetStr;
		}
	}
	return _T("");
}

bool MainInterFace::CheckDataSave()
{
	CEditUI *pfxEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfx")));
	CEditUI *pfxpassEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputpfxpass")));
	//CEditUI *toolEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputsigntool")));
	CEditUI *DescptionEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputDscrption")));
	CEditUI *TimeUrlEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputTimeURL")));
	CEditUI *URLEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("inputURL")));
	if (pfxEdit->GetText().GetData() != m_Model.GetConfig(_T("PassFile")) || 
		pfxpassEdit->GetText().GetData() != m_Model.GetConfig(_T("PassWord")) ||
		//toolEdit->GetText().GetData() != m_Model.GetConfig(_T("SignToolPath")) ||
		DescptionEdit->GetText().GetData() != m_Model.GetConfig(_T("Description")) ||
		TimeUrlEdit->GetText().GetData() != m_Model.GetConfig(_T("TimeURL")) ||
		URLEdit->GetText().GetData() != m_Model.GetConfig(_T("URL"))
		)
	{
		return false;
	}
	return true;
}

int MainInterFace::GetCertName(vector<hstring> &vstr,CComboBoxUI *pValue)
{
	PCCERT_CONTEXT   pCertContext=NULL; 
	HCERTSTORE hStore = CertOpenSystemStore(NULL,_T("MY"));
	if (NULL == hStore)
	{
		return -1;
	}
	while(pCertContext= CertEnumCertificatesInStore(
		hStore,
		pCertContext))
	{
		TCHAR Issuer[256];   
		CertGetNameString(pCertContext,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,NULL,Issuer,sizeof(Issuer)); 
		CListLabelElementUI *m_Certcontaner = new CListLabelElementUI;
		m_Certcontaner->SetText(Issuer);
		m_Certcontaner->SetFixedHeight(23);
		m_Certcontaner->SetMinHeight(23);
		m_Certcontaner->SetOwner(pValue);
		pValue->Add(m_Certcontaner);
		vstr.push_back(Issuer);
	}
	CertCloseStore(hStore,0);
	return 0;
}

int MainInterFace::GetCspName(vector<hstring> &vstr, CComboBoxUI *pValue)
{
	DWORD       cbName;
	DWORD       dwType;
	DWORD       dwIndex = 0;
	TCHAR        *pszName;

	while (CryptEnumProviders(dwIndex, NULL, 0, &dwType, NULL, &cbName))
	{
		if (NULL == (pszName = new TCHAR[cbName + 1]))
		{
			return -1;
		}
		if (CryptEnumProviders(dwIndex++, NULL, 0, &dwType, pszName, &cbName))
		{
			hstring tTempStr(pszName);
			std::transform(tTempStr.begin(), tTempStr.end(), tTempStr.begin(), ::tolower);
			if (-1 != tTempStr.find(_T("microsoft")))
				continue;

			CListLabelElementUI *m_Certcontaner = new CListLabelElementUI;
			m_Certcontaner->SetText(pszName);
			m_Certcontaner->SetFixedHeight(23);
			m_Certcontaner->SetMinHeight(23);
			m_Certcontaner->SetOwner(pValue);
			pValue->Add(m_Certcontaner);
			vstr.push_back(pszName);
		}
		else
		{
			delete[] pszName;
			pszName = NULL;
			return -2;
		}
		delete[] pszName;
		pszName = NULL;
	}
	return 0;
}

DWORD WINAPI MainInterFace::ThreadFunc(LPVOID lParam)
{
	MainInterFace *tInterface = (MainInterFace*)lParam;
	//EnterCriticalSection(&tInterface->m_cs);
	CListUI* pList = static_cast<CListUI*>(tInterface->m_pm.FindControl(_T("domainlist")));
	int count = pList->GetCount();
	for (int i = 0; i < count; ++i)
	{
		CListTextElementUI *pItem = static_cast<CListTextElementUI*>(pList->GetItemAt(i));
		if (tInterface->m_Model.Sign(i))
			pItem->SetText(2, _T("成功"));
		else
			pItem->SetText(2,_T("失败"));
				/*DWORD Threadid = 0;
				HANDLE tThread = CreateThread(NULL,0, SignThread,this,0,&Threadid);
				CloseHandle(tThread);*/
	}
	//LeaveCriticalSection(&tInterface->m_cs);
	CButtonUI* pBtnSign = static_cast<CButtonUI*>(tInterface->m_pm.FindControl(_T("btnSign")));
	pBtnSign->SetEnabled(true);
	return 0;
}