#include "StdAfx.h"
#include "WebEventEx.h"
#include <atlconv.h>

CWebEventEx::CWebEventEx(void)
{
}


CWebEventEx::~CWebEventEx(void)
{
}
void CWebEventEx::BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel )
{

	CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(m_pPaintManager->FindControl(_T("tabHeader")));
	if(pHorLayout)
	{
		int nCnt=pHorLayout->GetCount();//���е�TabHeader���� һ�� "+"��ť
		CDuiString strName;
		strName.Format(_T("tabText%d"),nCnt-1);
		CHorizontalLayoutUI* pHorTab=static_cast<CHorizontalLayoutUI*>(pHorLayout->GetItemAt(nCnt-2));//�ҵ�tabText
		if(pHorTab)
		{
			COptionUI* pLastOption=static_cast<COptionUI*>(pHorTab->GetItemAt(0));//�ҵ�tab (option)
			if (pLastOption)
			{
				USES_CONVERSION;
				pLastOption->SetText(OLE2T(url->bstrVal));
				pLastOption->SetToolTip(OLE2T(url->bstrVal));
			}
		}
		else
		{

		}
	}
// 		COptionUI* pLastOption=static_cast<COptionUI*>(m_pPaintManager->FindControl(strName.GetData()));//�����ֺ����޷����Ҷ�Ӧ��COptionUI*�ؼ�
// 		pLastOption=static_cast<COptionUI*>(pHorLayout->GetItemAt(nCnt-1));//�ҵ��½���Option��ť
// 		//::MessageBox(NULL,pLastOption->GetName(),_T(""),MB_OK);
// 		LPVOID pOption=static_cast<COptionUI*>(pHorLayout->GetItemAt(nCnt-1))->GetInterface(DUI_CTR_OPTION);
}

void CWebEventEx::NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
	//��BeforeNavigate �����ַ
	CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(m_pPaintManager->FindControl(_T("tabHeader")));
	if(pHorLayout)
	{
		int nCnt=pHorLayout->GetCount();
		//CHorizontalLayoutUI* pHorTab=static_cast<CHorizontalLayoutUI*>(pHorLayout->GetItemAt(nCnt));
		CHorizontalLayoutUI* pHorTab=new CHorizontalLayoutUI();
		//pHorLayout->Add(pHorTab);���ܷŵ���������һ�� �ӺŰ�ť
		pHorLayout->AddAt(pHorTab,pHorLayout->GetCount()-1);
		m_pPaintManager->InitControls(pHorTab, pHorLayout);
		CDuiString strName;
		strName.Format(_T("tabText%d"),nCnt);
		pHorTab->SetName(strName);
		pHorTab->SetAttribute(_T("width"),_T("150"));

		COptionUI* pOption=new COptionUI();
		pHorTab->Add(pOption);
		m_pPaintManager->InitControls(pOption, pHorTab);
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
		m_pPaintManager->InitControls(pBtnClose, pHorTab);
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
		CTabLayoutUI* pTabLayout=static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl(_T("tabSwitch")));
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
			*Cancel=VARIANT_TRUE;
			pWebBrowser->SetWebBrowserEventHandler(this);
			pWebBrowser->DoCreateControl();
			pWebBrowser->Navigate2(bstrUrl);
			pOption->SetText(bstrUrl);
			pOption->SetToolTip(bstrUrl);
			g_mapBrowser.insert(map<int,CWebBrowserUI*>::value_type(nCnt,pWebBrowser));
			g_nSelect=nCnt;
		}
	}
}

HRESULT STDMETHODCALLTYPE CWebEventEx::GetHostInfo(
	/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	if (pInfo != NULL)
	{
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
	}
	return S_OK;
}