// SBrowser.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <CommDlg.h>
#include <atlconv.h>
#include "SBrowser.h"
/************************************************************************/
/* ��Ҫ�Ľ��ĵط�����Ҫһ��list�����е�IwebBrowser2ָ���ȡ������������ʷ��¼���棬
�Լ�ǰ�������ˣ�ˢ�£��ղؼеȹ��ܵ�ʵ��*/
/************************************************************************/
void CMainFrame::InitWindow()
{
	//SetIcon(IDR_MAINFRAME); // ����������ͼ��
	m_pWebBrowser=NULL;
	m_pWebEventEx=new CWebEventEx();
	m_pWebEventEx->SetPaintManager(&m_PaintManager);

	CenterWindow();
	CComboUI* pComboSearch = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboSearch")));
	CEditUI* pEditSearch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSearch")));
	if( pComboSearch && pEditSearch ) pEditSearch->SetText(pComboSearch->GetText());

	CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("ActiveXLayout_1")));
	if (pHorLayout)
	{
		CWebBrowserUI * pWebBrowser=new CWebBrowserUI();
		pWebBrowser->SetName(_T("WebBrowserUI_1"));
		pHorLayout->Add(pWebBrowser);
		pWebBrowser->SetWebBrowserEventHandler(m_pWebEventEx);
		pWebBrowser->DoCreateControl();
		pWebBrowser->Navigate2(_T("http://www.baidu.com"));
		//����
		g_mapBrowser.insert(map<int,CWebBrowserUI*>::value_type(1,pWebBrowser));
		g_nSelect=1;
	}
}

void CMainFrame::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("itemselect") ) 
	{
		if( msg.pSender->GetName() == _T("ComboSearch") ) 
		{
			CEditUI* pEditSearch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditSearch")));
			if( pEditSearch ) pEditSearch->SetText(msg.pSender->GetText());
		}
	}
	if (msg.sType==_T("textchanged"))
	{
		CComboUI* pComboSearch = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("ComboSearch")));
	}
	if(msg.sType==_T("selectchanged"))
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
				//m_nSel=i;
				CTabLayoutUI* pTabUI=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabSwitch")));
				pTabUI->SelectItem(i-1);
				CHorizontalLayoutUI* pHor=static_cast<CHorizontalLayoutUI*>(pTabUI->GetItemAt(0));
				CWebBrowserUI* pWeb=static_cast<CWebBrowserUI*>(pHor->GetItemAt(0));
				//pWeb->Navigate2(_T("www.tmall.com"));
				pWeb->SetFocus();
				IWebBrowser2* pWebBrowser2=NULL;
				pWebBrowser2=pWeb->GetWebBrowser2();
				if(pWebBrowser2)
				{
					IHTMLWindow2 * pHtmlWindow2=NULL;
					pWebBrowser2->QueryInterface(IID_IHTMLWindow2,(void**)&pHtmlWindow2 );
					if(pHtmlWindow2)
					{
						pHtmlWindow2->focus();
					}
				}

			}
		}
	}
	if (msg.sType==_T("click"))
	{
		CDuiString    strName     = msg.pSender->GetName();
		//��������
		if(strName.CompareNoCase(_T("btnSetting")) == 0)
		{
			CMenuWnd* pMenu = new CMenuWnd();  
			POINT point = msg.ptMouse;  
			ClientToScreen(m_hWnd, &point);  
			pMenu->Init(NULL,_T("SettingMenu.xml"),point,&m_PaintManager,&m_MenuCheckInfo,eMenuAlignment_Right);
			//pMenu->Init(NULL, _T("SettingMenu.xml"), point, &m_PaintManager, &m_MenuCheckInfo,eMenuAlignment_Right | eMenuAlignment_Bottom);
			CMenuUI* rootMenu = pMenu->GetMenuUI();
			if (rootMenu != NULL)
			{
				//CMenuElementUI* pNew = new CMenuElementUI;
				//pNew->SetName(_T("Menu_Dynamic"));
				//pNew->SetText(_T("��̬һ���˵�"));
				//pNew->SetShowExplandIcon(true);
				//pNew->SetIcon(_T("WebSit.png"));
				//pNew->SetIconSize(16,16);
				//CMenuElementUI* pSubNew = new CMenuElementUI;
				//pSubNew->SetText(_T("��̬�����˵�"));
				//pSubNew->SetName(_T("Menu_Dynamic"));
				//pSubNew->SetIcon(_T("Virus.png"));
				//pSubNew->SetIconSize(16,16);
				//pNew->Add(pSubNew);
				//rootMenu->Add(pNew);
				//rootMenu->AddAt(pNew2,2);
			}

			// ��̬��Ӻ��������ò˵��Ĵ�С
			pMenu->ResizeMenu();
		}
		//����ͷ��
		if(strName.CompareNoCase(_T("btnHead"))==0)
		{
			CMenuWnd* pMenu = new CMenuWnd();  
			POINT point;
			point.x=point.y=3;
			ClientToScreen(m_hWnd, &point);  
			pMenu->Init(NULL,_T("HeadMenu.xml"),point,&m_PaintManager,&m_MenuCheckInfo);
		}
		//��ͼ����
		if (strName.CompareNoCase(_T("btnSnapshot"))==0)
		{
			::WinExec("snapshot.exe",SW_SHOWNORMAL);
		}
		//��������������ť
		if (strName.CompareNoCase(_T("btnSearch"))==0)
		{
			Search();
		}
		//�������ϵ��������������ť
		if(strName.CompareNoCase(_T("btnEngine"))==0)
		{
			CMenuWnd* pMenu = new CMenuWnd();  
			POINT point = msg.ptMouse;  
			ClientToScreen(m_hWnd, &point);  
			pMenu->Init(NULL,_T("SearchMenu.xml"),point,&m_PaintManager,&m_MenuCheckInfo);
			CMenuUI* rootMenu = pMenu->GetMenuUI();
		}
		//�½���ǩҳ
		if (strName.CompareNoCase(_T("btnAdd"))==0)
		{
			CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("tabHeader")));
			if(pHorLayout)
			{
				int nCnt=pHorLayout->GetCount();
				//CHorizontalLayoutUI* pHorTab=static_cast<CHorizontalLayoutUI*>(pHorLayout->GetItemAt(nCnt));
				CHorizontalLayoutUI* pHorTab=new CHorizontalLayoutUI();
				//pHorLayout->Add(pHorTab);���ܷŵ���������һ�� �ӺŰ�ť
				pHorLayout->AddAt(pHorTab,pHorLayout->GetCount()-1);
				m_PaintManager.InitControls(pHorTab, pHorLayout);
				CDuiString strName;
				strName.Format(_T("tabText%d"),nCnt);
				pHorTab->SetName(strName);
				pHorTab->SetAttribute(_T("width"),_T("150"));

				COptionUI* pOption=new COptionUI();
				pHorTab->Add(pOption);
				m_PaintManager.InitControls(pOption, pHorTab);
				strName.Format(_T("Tab%d"),nCnt);
				pOption->SetName(strName);
				pOption->SetAttribute(_T("textcolor"),_T("#FF386382"));
				pOption->SetAttribute(_T("normalimage"),_T("file='tabBtn/tabbar_normal.png' corner='5,1,5,1'"));
				pOption->SetAttribute(_T("hotimage"),_T("file='tabBtn/tabbar_hover.png' corner='5,1,5,1'"));
				pOption->SetAttribute(_T("pushedimage"),_T("file='tabBtn/tabbar_pushed.png' corner='5,1,5,1'"));
				pOption->SetAttribute(_T("selectedimage"),_T("file='tabBtn/tabbar_pushed.png' corner='5,1,5,1'"));
				pOption->SetAttribute(_T("group"),_T("listHeader"));
				pOption->SetAttribute(_T("selected"),_T("true"));
				pOption->SetAttribute(_T("endellipsis"),_T("true"));

				CButtonUI* pBtnClose=new CButtonUI();
				pHorTab->Add(pBtnClose);
				m_PaintManager.InitControls(pBtnClose, pHorTab);
				strName.Format(_T("tabClose%d"),nCnt);
				pBtnClose->SetName(strName);
				pBtnClose->SetFloat(true);
				RECT rc;
				rc.left=135;rc.top=4;rc.right=140;rc.bottom=10;
				pBtnClose->SetPos(rc);
				rc=pBtnClose->GetPos();
				pBtnClose->SetAttribute(_T("width"),_T("14"));
				pBtnClose->SetAttribute(_T("height"),_T("14"));
				pBtnClose->SetAttribute(_T("bkcolor"),_T("#00FFFFFF"));
				pBtnClose->SetAttribute(_T("normalimage"),_T("file='tabBtn/00250.png'"));

				//��ӣ��л������ҳ��
				CTabLayoutUI* pTabLayout=static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabSwitch")));
				if (pTabLayout)
				{
					//�ȴ���һ��ˮƽ����
					CHorizontalLayoutUI* pHorLayout=new CHorizontalLayoutUI();

					//�����ˮƽ����
					pTabLayout->Add(pHorLayout);
					int nCnt=pTabLayout->GetCount();
					CDuiString strName;
					strName.Format(_T("ActiveXLayout_%d"),nCnt);
					pHorLayout->SetName(strName);
					pTabLayout->SelectItem(pHorLayout);

					CWebBrowserUI* pWebBrowser=new CWebBrowserUI();
					strName.Format(_T("WebBrowserUI_%d"),nCnt);
					pWebBrowser->SetName(strName);
					pHorLayout->Add(pWebBrowser);
					pWebBrowser->SetWebBrowserEventHandler(m_pWebEventEx);
					pWebBrowser->DoCreateControl();
					pWebBrowser->Navigate2(_T("http://www.baidu.com"));
					pOption->SetText(_T("http://www.baidu.com"));
					pOption->SetToolTip(_T("http://www.baidu.com"));
					g_mapBrowser.insert(map<int,CWebBrowserUI*>::value_type(nCnt,pWebBrowser));
					g_nSelect=nCnt;
				}
			}
		}

		//Tab�ؼ��ϵĹرհ�ť
		CDuiString strTmp=strName.Left(strName.GetLength()-1);
		if(strTmp.Compare(_T("tabClose"))==0)
		{
			CDuiString strPos;
			strPos=strName.Right(1);
			int nPos=_ttoi(strPos.GetData());
			std::map<int,CWebBrowserUI*>::iterator itr;
			itr=g_mapBrowser.find(nPos);
			if(itr!=g_mapBrowser.end())
			{
				CWebBrowserUI *pWebUI=itr->second;
				//
				CDuiString strTmp;
				strTmp.Format(_T("tabText%d"),nPos);
				CHorizontalLayoutUI* pHorLay=static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(strTmp));
				strTmp.Format(_T("tab%d"),nPos);
				COptionUI* pOptionUI=static_cast<COptionUI*>(m_PaintManager.FindControl(strTmp));
				CButtonUI* pBtnUI=static_cast<CButtonUI*>(m_PaintManager.FindControl(strTmp));
				//ȥ��tabͷ
				if (pHorLay&&pOptionUI)
				{
					CControlUI* pUI=pHorLay->GetParent();
					((CHorizontalLayoutUI*)pUI)->Remove(pHorLay);
				}
				//ȥ��webҳ
				strTmp.Format(_T("ActiveXLayout_1"));
				pHorLay=static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(strTmp));
				if (pHorLay)
				{
					CControlUI* pUI=pHorLay->GetParent();
					((CHorizontalLayoutUI*)pUI)->Remove(pHorLay);
				}
				//ɾ���� CWebBrowserUI*
				g_mapBrowser.erase(itr);
			}
		}
	}
	//��Ӧ�س��� �������򣬻��ߵ�ַ�� ������ɺ�Ļس���
	if(msg.sType==_T("return"))
	{
		Search();
	}
	__super::Notify(msg);
}

void CMainFrame::Search()
{
	CEditUI* pEdit=static_cast<CEditUI*>(m_PaintManager.FindControl(_T("search_edit")));
	CDuiString strText;
	strText=pEdit->GetText();
	CDuiString strUrl;
	//������Լ�������������ж�
	strUrl.Format(_T("http://www.baidu.com/s?wd=%s"),strText);
	std::map<int,CWebBrowserUI*>::iterator itr;
	itr=g_mapBrowser.find(1);
	if(itr!=g_mapBrowser.end())
	{
		CWebBrowserUI *pWebUI=itr->second;
		pWebUI->Navigate2(strUrl);
	}
}
LRESULT  CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}
LRESULT CMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	if (uMsg == WM_MENUCLICK)
	{
 		CDuiString *strMenuName = (CDuiString*)wParam;
		BOOL bChecked = (BOOL)lParam;		 
		//����ͷ��
		if(strMenuName->CompareNoCase(_T("Menu_SetPic"))==0)
		{
			OPENFILENAME ofn; 
			TCHAR szPath[MAX_PATH]; 
			ZeroMemory(szPath, sizeof(szPath)); 
			ZeroMemory(&ofn, sizeof(OPENFILENAME)); 
			ofn.lStructSize = sizeof(OPENFILENAME); 
			ofn.hwndOwner = NULL; 
			ofn.lpstrFilter = TEXT("ͼƬ\0*.png\0*.*\0*.*\0\0"); 
			ofn.lpstrFile = szPath; ofn.nMaxFile = MAX_PATH; 
			::GetCurrentDirectory(MAX_PATH,szPath);
			ofn.lpstrInitialDir=szPath;
			ofn.Flags = OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST; 
			if (GetOpenFileName(&ofn)) 
			{     
				//::MessageBox(NULL, szPath, TEXT("��ʾ"), MB_OK); 
				CButtonUI *pBtn=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnHead")));
				pBtn->SetForeImage(szPath);
			} 
		}
		if(strMenuName->CompareNoCase(_T("Menu_baidu"))==0)
		{
			CButtonUI* pBtn=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnEngine")));
			pBtn->SetBkImage(_T("searchBar\\00086.png"));
			CEditUI* pEditSearch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("search_edit")));
			pEditSearch->SetText(_T("�ٶ�"));
		}
		if(strMenuName->CompareNoCase(_T("Menu_Google"))==0)
		{
			CButtonUI* pBtn=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnEngine")));
			pBtn->SetBkImage(_T("searchBar\\00089.png"));
			CEditUI* pEditSearch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("search_edit")));
			pEditSearch->SetText(_T("�ȸ�"));
		}
		if(strMenuName->CompareNoCase(_T("Menu_haosou"))==0)
		{
			CButtonUI* pBtn=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnEngine")));
			pBtn->SetBkImage(_T("searchBar\\00084.png"));
			CEditUI* pEditSearch = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("search_edit")));
			pEditSearch->SetText(_T("����"));
		}
		delete strMenuName;
	}
	bHandled = false;
	return 0;
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
	CMainFrame duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.ShowModal();
	::CoUninitialize();
	return 0;
}

