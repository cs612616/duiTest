// DUI2.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DUI2.h"
#include "IeHelper.h"

CFriendUI *g_friendUI;

void CMyFrameWnd::InitWindow()
{
	//SetIcon(IDR_MAINFRAME); // 设置任务栏图标
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
		//获取子控件的个数
		int nCnt=pHorLayout->GetCount();
		pActiveUI->SetDelayCreate(false);

		bool bRet=pActiveUI->CreateControl(CLSID_WebBrowser);
		pActiveUI->GetControl(IID_IWebBrowser2,(void**)&m_pWebBrowser);
		if( m_pWebBrowser != NULL ) 
		{
			m_ieHelper.ConnectEvent(m_pWebBrowser);
			m_pWebBrowser->Navigate(L"http://www.baidu.com/",NULL,NULL,NULL,NULL);  // 由于谷歌时不时被墙，所以换成反应快的网站
		}
	}
 	//初始化 progress控件
 	CProgressUI* pProgress=static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("ProgressUI1")));
 	if (pProgress)
 	{
 		pProgress->SetValue(70);
 	}
 	//初始化 slider控件
  	CSliderUI* pSlider=static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("SliderUI1")));
 	if (pSlider)
 	{
 		pSlider->SetValue(50);
 	}
 	//初始化 list 控件
 	CListUI*  pList=static_cast<CListUI*>(m_PaintManager.FindControl(_T("ListUI1")));
 	if (pList)
 	{
 		// 添加List列表内容，必须先Add(pListElement)，再SetText
 		CDuiString str;
 		for (int i = 0; i < 100; i++)
 		{
 			CListTextElementUI* pListElement = new CListTextElementUI;
 			pListElement->SetTag(i);
 			pList->Add(pListElement);
 
 			str.Format(_T("%d"), i);
 			pListElement->SetText(0, str);
 			pListElement->SetText(1, _T("白宇宏"));
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
			//当前输入焦点(有光标的tab页),但不一定是当前单击的tab页
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

// 一、ActiveX控件
// 	由于ActiveX控件必须要初始化，否则启动时会崩溃，所以第一个介绍它。
// 	因为ActiveX控件用到了COM，所以需要初始化COM，此时main.cpp的代码如下：

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
