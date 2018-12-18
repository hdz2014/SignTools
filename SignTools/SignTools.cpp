#include "SignTools.h"

#ifdef _UNICODE
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int nCmdShow)
#else
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	if (_tcslen(lpCmdLine) != 0)
	{
		Model SignFlag;
		TCHAR SZPath[MAX_PATH];
		GetModuleFileName(hInstance,SZPath,MAX_PATH);
		hstring SPath = SZPath;
		int Len = SPath.find_last_of('\\');
		SPath = SPath.substr(0,Len);
		SPath	+= _T("\\SignConfig.ini");
		SignFlag.SetIniPath(SPath);
		//�ļ����
		hstring m_strFileList(lpCmdLine);
		int m_pos = 0;		//�����ַ�����λ
		int m_rePost = 0;	//����λ��
		hstring m_StrMSg = L"ǩ�������\n";
		while (-1 != (m_pos = m_strFileList.find(L"|",m_rePost)))
		{
			hstring m_strFile = m_strFileList.substr(m_rePost,m_pos - m_rePost);

			m_StrMSg += m_strFile;
			m_StrMSg += L"\t\t\t";
			if (!SignFlag.CheckPE(m_strFile.c_str()))
			{
				//MessageBox(NULL,_T("���ļ�������Ч��PE�ļ�"),_T("����ǩ������---SignTools"),MB_OK);
				m_StrMSg += L"���ļ�������Ч��PE�ļ�\n";
				continue;
			}

			if (!SignFlag.Sign(m_strFile.c_str()))
			{
				//MessageBox(NULL,_T("���ļ�ǩ��ʧ�ܣ������Ƿ���ǩ����������Ϣ����ȷ��"),_T("����ǩ������---SignTools"),MB_OK);
				m_StrMSg += L"���ļ�ǩ��ʧ�ܣ������Ƿ���ǩ����������Ϣ����ȷ��\n";
			}
			else
			{
				//MessageBox(NULL,_T("��ϲ���ļ�ǩ���ɹ���"),_T("����ǩ������---SignTools"),MB_OK);
				m_StrMSg += L"��ϲ���ļ�ǩ���ɹ���\n";
			}

			m_rePost = m_pos + 1;

		}
		MessageBox(NULL,m_StrMSg.c_str(),_T("����ǩ������---SignTools"),MB_OK);
		/*if (!SignFlag.CheckPE(lpCmdLine))
		{
		MessageBox(NULL,_T("���ļ�������Ч��PE�ļ�"),_T("����ǩ������---SignTools"),MB_OK);
		return 0;
		}

		if (!SignFlag.Sign(lpCmdLine))
		{
		MessageBox(NULL,_T("���ļ�ǩ��ʧ�ܣ������Ƿ���ǩ����������Ϣ����ȷ��"),_T("����ǩ������---SignTools"),MB_OK);
		}
		else
		{
		MessageBox(NULL,_T("��ϲ���ļ�ǩ���ɹ���"),_T("����ǩ������---SignTools"),MB_OK);
		}*/
		return 0;
	}
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\ListRes"));
	//CPaintManagerUI::SetResourceZip(CPaintManagerUI::GetInstancePath() + _T("skin\\ListRes.zip"));
	/*
	Model a;
	if (a.Sign(_T("D:\\VC++\\SignTools\\Debug\\DuiLib_u.dll")))
		MessageBox(NULL,a.GetConfig(_T("sx")).c_str(),_T("dss"),MB_OK);
	*/
	
	MainInterFace* pFrame = new MainInterFace();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("SignTools"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 600, 320);
	pFrame->SetIcon(IDI_ICON1);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	return 0;
}