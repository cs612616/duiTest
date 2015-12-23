// DUI2.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "DUI2.h"
#include "IeHelper.h"

CFriendUI *g_friendUI;

void CMyFrameWnd::InitWindow()
{
	//SetIcon(IDR_MAINFRAME); // ����������ͼ��
	g_friendUI=new CFriendUI(m_PaintManager);
	m_pWebBrowser=NULL;
	m_nSel=1;
	CenterWindow();
	CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("ActiveXLayout_1")));
	if (pHorLayout)
	{
		CActiveXUI* pActiveUI=new CActiveXUI();
		pActiveUI->SetName(_T("ActiveXUI_1"));
		pHorLayout->Add(pActiveUI);
		//��ȡ�ӿؼ��ĸ���
		int nCnt=pHorLayout->GetCount();
		pActiveUI->SetDelayCreate(false);

		bool bRet=pActiveUI->CreateControl(CLSID_WebBrowser);
		pActiveUI->GetControl(IID_IWebBrowser2,(void**)&m_pWebBrowser);
		if( m_pWebBrowser != NULL ) 
		{
			m_ieHelper.ConnectEvent(m_pWebBrowser);
			m_pWebBrowser->Navigate(L"http://www.baidu.com/",NULL,NULL,NULL,NULL);  // ���ڹȸ�ʱ��ʱ��ǽ�����Ի��ɷ�Ӧ�����վ
		}
	}
 	//��ʼ�� progress�ؼ�
 	CProgressUI* pProgress=static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("ProgressUI1")));
 	if (pProgress)
 	{
 		pProgress->SetValue(70);
 	}
 	//��ʼ�� slider�ؼ�
  	CSliderUI* pSlider=static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("SliderUI1")));
 	if (pSlider)
 	{
 		pSlider->SetValue(50);
 	}
 	//��ʼ�� list �ؼ�
 	CListUI*  pList=static_cast<CListUI*>(m_PaintManager.FindControl(_T("ListUI1")));
 	if (pList)
 	{
 		// ���List�б����ݣ�������Add(pListElement)����SetText
 		CDuiString str;
 		for (int i = 0; i < 100; i++)
 		{
 			CListTextElementUI* pListElement = new CListTextElementUI;
 			pListElement->SetTag(i);
 			pList->Add(pListElement);
 
 			str.Format(_T("%d"), i);
 			pListElement->SetText(0, str);
 			pListElement->SetText(1, _T("�����"));
 		}
 	}
 		
}
void CMyFrameWnd::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetName() == _T("btnMenu") ) 
		{

		}
		if (msg.pSender->GetName() == _T("btnGoBack"))
		{

			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabSwitch")));
			//��ǰ���뽹��(�й���tabҳ),����һ���ǵ�ǰ������tabҳ
			//int nCurSel=pControl->GetCurSel();
			CDuiString strName;
			strName.Format(_T("ActiveXUI_%d"),m_nSel);
			CActiveXUI* pActive=static_cast<CActiveXUI*>(m_PaintManager.FindControl(strName));
			IWebBrowser2* iCurWeb=NULL;
			if (pActive)
			{
				pActive->GetControl(IID_IWebBrowser2,(void**)&iCurWeb);
				iCurWeb->GoBack();
			}
			//m_pWebBrowser->GoBack();
		}
		if (msg.pSender->GetName() == _T("btnRefresh"))
		{

			m_pWebBrowser->Refresh();
		}
		if (msg.pSender->GetName() == _T("btnGoHome"))
		{
			m_pWebBrowser->GoHome();
		}
		if (msg.pSender->GetName() == _T("btnRestore"))
		{
			m_pWebBrowser->Stop();
		}
		if (msg.pSender->GetName() == _T("btnFavorite"))
		{
			//m_pWebBrowser->
		}
	}
	else if(msg.sType == _T("selectchanged"))
	{
		CDuiString    strName     = msg.pSender->GetName();
		CHorizontalLayoutUI* pControl = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("tabHeader")));
		int nCnt=pControl->GetCount();
		CDuiString strTmpName;
		for (int i=1;i<=nCnt;++i)
		{
			strTmpName.Format(_T("Tab%d"),i);
			if(strName.CompareNoCase(strTmpName)==0 )
			{
				m_nSel=i;
				CTabLayoutUI* pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabSwitch")));
				pTabUI->SelectItem(i-1);
			}
		}
	}

	__super::Notify(msg);
}

LRESULT  CMyFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
	BOOL bHandle=TRUE;
	switch(uMsg)
	{
	case WM_CREATE:                                                                      
		{

		}break;
	default:
		{
		}break;
	}
	return lRes;
}

// һ��ActiveX�ؼ�
// 	����ActiveX�ؼ�����Ҫ��ʼ������������ʱ����������Ե�һ����������
// 	��ΪActiveX�ؼ��õ���COM��������Ҫ��ʼ��COM����ʱmain.cpp�Ĵ������£�

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	CMyFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.ShowModal();
	::CoUninitialize();
}
