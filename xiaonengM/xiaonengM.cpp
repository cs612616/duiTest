// xiaonengM.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "xiaonengM.h"

#include "LoginFrameWnd.h"

void CMainFrame::InitWindow()
{
	CenterWindow();
	GameListUI* pGameListUI=static_cast<GameListUI*>(m_PaintManager.FindControl(_T("workmateList")));
	CConsultFrameUI* pConsultFrameUI=static_cast<CConsultFrameUI*>(m_PaintManager.FindControl(_T("consult")));
	CTabLayoutUI* pTabLayUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabLeft")));
	CHorizontalLayoutUI* pHorLayUI=static_cast<CHorizontalLayoutUI*>(pTabLayUI->GetItemAt(2));
	GameListUI::Node* pCategoryNode = NULL;
	GameListUI::Node* pGameNode = NULL;
	GameListUI::Node* pServerNode = NULL;
	
	pCategoryNode = pGameListUI->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}我的同事"));
	pCategoryNode->data()._pListElement->SetFixedHeight(30);
	for( int i = 0; i < 6; ++i )
	{
		pGameNode = pGameListUI->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}客户组"), pCategoryNode);
		pGameNode->data()._pListElement->SetFixedHeight(30);
		for( int i = 0; i < 3; ++i )
		{
			pServerNode = pGameListUI->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}组员某某某"), pGameNode);
			pServerNode->data()._pListElement->SetFixedHeight(30);
		}
	}
}
void CMainFrame::OnPrepare()
{
	CWkeWebkitUI*	m_pWke = static_cast<CWkeWebkitUI*>(m_PaintManager.FindControl(_T("ie")));
	if (m_pWke)
	{
		//jsBindFunction("msgBox", js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数
		static wkeClientHandler hander;        //网页标题改变和URL改变的回调
		m_pWke->SetClientHandler(&hander);
		//m_pWke->SetFile(_T("Html/index.html")/*msg.pSender->GetText().GetData()*/);
		m_pWke->SetURL(L"http://blog.csdn.net/cs1992612616?viewmode=list"); 
		m_pWke->SetURL(L"http://www.baidu.com"); 

	}
	//ShowWindow(false);
	CLoginFrameWnd* pLoginFrame = new CLoginFrameWnd();
	if( pLoginFrame == NULL ) { Close(); return; }
	pLoginFrame->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
	pLoginFrame->SetIcon(IDI_SMALL);
	pLoginFrame->CenterWindow();
	pLoginFrame->ShowModal();
	if(pLoginFrame->m_bIsLogin==true)
	{
		ShowWindow(true);
	}

	CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
	pTabUI->SelectItem(4);
}

void CMainFrame::Notify(TNotifyUI& msg)
{
	if (msg.sType==_T("windowinit"))
	{
		OnPrepare();
	}
	if(msg.sType==_T("selectchanged"))
	{
		//MainFrame.xml 上方
		if (msg.pSender->GetName()==_T("consultBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(0);
		}
		if (msg.pSender->GetName()==_T("handSaleBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(1);
		}
		if (msg.pSender->GetName()==_T("userResBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(2);
		}
		if (msg.pSender->GetName()==_T("listBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(3);
		}
		if (msg.pSender->GetName()==_T("interactRecordtBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(4);
		}
		if (msg.pSender->GetName()==_T("reportBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(5);
		}
		if (msg.pSender->GetName()==_T("workmateBtn"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
			pTabUI->SelectItem(6);
		}
		//consultXMl 左侧tableft
		if(msg.pSender->GetName()==_T("myConsultOpt"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabLeft")));
			if (pTabUI)
			{
				pTabUI->SelectItem(0);
			}
		}
		if(msg.pSender->GetName()==_T("myContactOpt"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabLeft")));
			if (pTabUI)
			{
				pTabUI->SelectItem(1);
			}
		}
		if(msg.pSender->GetName()==_T("myWorkmateOpt"))
		{
			CTabLayoutUI * pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabLeft")));
			if (pTabUI)
			{
				pTabUI->SelectItem(2);
			}
		}


	}	
	if( msg.sType == _T("itemclick") ) 
	{
		GameListUI* pGameList = static_cast<GameListUI*>(m_PaintManager.FindControl(_T("workmateList")));
		if( pGameList->GetItemIndex(msg.pSender) != -1 )
		{
			if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
				GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();

				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_PaintManager.GetPaintWindow(), &pt);
				pt.x -= msg.pSender->GetX();
				SIZE sz = pGameList->GetExpanderSizeX(node);
				if( pt.x >= sz.cx && pt.x < sz.cy )                     
					pGameList->ExpandNode(node, !node->data()._expand);
			}
		}
	}
	else if( msg.sType == _T("itemactivate") ) {
		GameListUI* pGameList = static_cast<GameListUI*>(m_PaintManager.FindControl(_T("workmateList")));
		if( pGameList->GetItemIndex(msg.pSender) != -1 )
		{
			if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
				GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();
				pGameList->ExpandNode(node, !node->data()._expand);
				if( node->data()._level == 3 ) {
					COptionUI* pControl = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("roomswitch")));
					if( pControl ) {
						CHorizontalLayoutUI* pH = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("roomswitchpanel")));
						if( pH ) pH->SetVisible(true);
						pControl->SetText(node->parent()->parent()->data()._text);
						pControl->Activate();
					}
				}
			}
		}
	}
	__super::Notify(msg);
}

LRESULT  CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);

	return lRes;
}

LRESULT CMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	HRESULT Hr = ::CoInitialize(NULL);
	CWkeWebkitUI::WkeWebkit_Init();
	if( FAILED(Hr) ) return 0;
	CMainFrame duiFrame;
	duiFrame.Create(NULL, _T("xnMainFrame"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.ShowModal();
	CWkeWebkitUI::WkeWebkit_Shutdown();
	::CoUninitialize();
	return 0;
}
