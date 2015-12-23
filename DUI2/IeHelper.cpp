#include "StdAfx.h"
#include "IeHelper.h"
#include "..\\DuiLib\UIlib.h"
#include "DUI2.h"

extern CFriendUI *g_friendUI;

CIeHelper::CIeHelper(void):m_refCnt(0),m_pWebBrowser(NULL)
{
}


CIeHelper::~CIeHelper(void)
{
}
//IUnknown methods
STDMETHODIMP CIeHelper::QueryInterface(REFIID riid,void **ppvObject)
{
	*ppvObject=nullptr;
	if (riid==IID_IUnknown)
	{
 		*ppvObject=this;
 	}
 	else if (riid==IID_IDispatch)
 	{
 		*ppvObject=(IDispatch*)this;
 	}
 	else if (riid==DIID_DWebBrowserEvents2) // __uuidof(DWebBrowserEvents2)
 	{
 		*ppvObject=(IDispatch*)this;
 	}
 	if (*ppvObject!=nullptr)
 	{
 		return S_OK;
 	}
 	return E_NOINTERFACE;
}
STDMETHODIMP_(ULONG) CIeHelper::AddRef()
{
 	return ::InterlockedIncrement(&m_refCnt);
}
STDMETHODIMP_(ULONG) CIeHelper::Release()
{
	return ::InterlockedDecrement(&m_refCnt);
}

// IDispatch Methods
HRESULT _stdcall CIeHelper::GetTypeInfoCount(unsigned int * pctinfo)
{
	//not implemented--未实现。
	return E_NOTIMPL;
}
HRESULT _stdcall CIeHelper::GetTypeInfo(unsigned int iTInfo,LCID lcid,ITypeInfo FAR* FAR* ppTInfo)
{
	return E_NOTIMPL;
}
HRESULT _stdcall CIeHelper::GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,unsigned int cNames,LCID lcid,DISPID FAR* rgDispId)
{
	return E_NOTIMPL;
}
HRESULT _stdcall CIeHelper::Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pDispParams,VARIANT FAR* pVarResult,EXCEPINFO FAR* pExcepInfo,unsigned int FAR* puArgErr)
{
	if( dispIdMember == DISPID_DOCUMENTCOMPLETE)
	{
		//DocumentCompleted();
		return S_OK;
	}
 	if( dispIdMember == DISPID_BEFORENAVIGATE2)
 	{
		//BOOL Cancel=false;
		IDispatch *pDisp=pDispParams->rgvarg[6].pdispVal;
		VARIANT *URL=pDispParams->rgvarg[5].pvarVal;
		VARIANT *Flags=pDispParams->rgvarg[4].pvarVal;
		VARIANT *TargetFrameName=pDispParams->rgvarg[3].pvarVal;
 		VARIANT *PostData=pDispParams->rgvarg[2].pvarVal;
		VARIANT *Headers=pDispParams->rgvarg[1].pvarVal;
		VARIANT_BOOL *Cancel=pDispParams->rgvarg[0].pboolVal;
		BeforeNavigate2(pDispParams->rgvarg[6].pdispVal,
			pDispParams->rgvarg[5].pvarVal, pDispParams->rgvarg[4].pvarVal, pDispParams->rgvarg[3].pvarVal, pDispParams->rgvarg[2].pvarVal, pDispParams->rgvarg[1].pvarVal, pDispParams->rgvarg[0].pboolVal);
		return S_OK;
	}
	if(dispIdMember==DISPID_NAVIGATECOMPLETE2)
	{
		return S_OK;
	}
	if(dispIdMember==DISPID_NEWWINDOW2)
	{
		IDispatch**	ppDisp;
		BOOL* bCancel;
		ppDisp=pDispParams->rgvarg[1].ppdispVal;
		bCancel=(BOOL*)pDispParams->rgvarg[0].pboolVal;
		NewWindow2(ppDisp,bCancel);
		return S_OK;
	}
 	if(dispIdMember==DISPID_NEWWINDOW3)
 	{
 		IDispatch *pDisp=pDispParams->rgvarg[4].pdispVal;
 		IDispatch **ppDisp=&pDisp;
 		NewWindow3(ppDisp,pDispParams->rgvarg[3].pboolVal,pDispParams->rgvarg[2].lVal,pDispParams->rgvarg[1].bstrVal,pDispParams->rgvarg[0].bstrVal);
 	}
 	return E_NOTIMPL;
}
 //利用DWebBrowserEvents2 接口接收WebBrowser事件
void _stdcall CIeHelper::DocumentCompleted(IDispatch *pDisp,VARIANT *URL)
{

}
void _stdcall CIeHelper::NewWindow2(IDispatch** ppDisp,BOOL* bCancel )
{
	//::MessageBox(NULL,_T("NewWindow2"),NULL,MB_OK);
	
// 	CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(g_friendUI->paint_manager_.FindControl(_T("tabHeader")));
// 	if(pHorLayout)
// 	{
// 		COptionUI* pOption=new COptionUI();
// 		pHorLayout->Add(pOption);
// 		int nCnt=pHorLayout->GetCount();
// 		CDuiString strName;
// 		strName.Format(_T("Tab%d"),nCnt);
// 		pOption->SetName(strName);
// 		//pOption->SetFixedWidth(78);
// 		pOption->SetText(strName);
// 		pOption->SetAttribute(_T("width"),_T("78"));
// 		pOption->SetAttribute(_T("textcolor"),_T("#FF386382"));
// 		pOption->SetAttribute(_T("normalimage"),_T("tabBtn/tabbar_normal.png"));
// 		pOption->SetAttribute(_T("hotimage"),_T("tabBtn/tabbar_hover.png"));
// 		pOption->SetAttribute(_T("pushedimage"),_T("tabBtn/tabbar_pushed.png"));
// 		pOption->SetAttribute(_T("selectedimage"),_T("tabBtn/tabbar_pushed.png"));
// 		pOption->SetAttribute(_T("group"),_T("listHeader"));
// 		pOption->SetAttribute(_T("selected"),_T("true"));
// 		//pOption->ApplyAttributeList(_T("width=\"78\" textcolor=\"#FF386382\" font=\"2\"normalimage=\"tabBtn/tabbar_normal.png\" hotimage=\"tabBtn/tabbar_hover.png\" pushedimage=\"tabBtn/tabbar_pushed.png\" selectedimage=\"tabBtn/tabbar_pushed.png\" group=\"demo_list_header\"selected=\"true\""));
// 		//pOption->ApplyAttributeList(_T("width=\"78\"")); 
// 	}
// 	CTabLayoutUI* pTabLayout=static_cast<CTabLayoutUI*>(g_friendUI->paint_manager_.FindControl(_T("tabSwitch")));
//  	if (pTabLayout)
//  	{
//  		//先创建一个水平布局
//  		CHorizontalLayoutUI* pHorLayout=new CHorizontalLayoutUI();
// 		
//  		//先添加水平布局
// 		pTabLayout->Add(pHorLayout);
// 		int nCnt=pTabLayout->GetCount();
// 		CDuiString strName;
// 		strName.Format(_T("ActiveXLayout_%d"),nCnt);
// 		pHorLayout->SetName(strName);
// 		pTabLayout->SelectItem(pHorLayout);
//  		CActiveXUI* pActiveUI=new CActiveXUI();
// 		strName.Format(_T("ActiveXUI_%d"),nCnt);
//  		pActiveUI->SetName(strName);
//  		//然后在水平布局添加ActiveXUI
//  		pHorLayout->Add(pActiveUI);
//  								//获取子控件的个数
//  		//int nCnt=pTabLayout->GetCount();
//  		pActiveUI->SetDelayCreate(false);
//  
//  		IWebBrowser2* pWebBrowser=NULL;
//  		bool bRet=pActiveUI->CreateControl(CLSID_WebBrowser);
//  		pActiveUI->GetControl(IID_IWebBrowser2,(void**)&pWebBrowser);
//  		//pActiveUI->SetName(_T("ActiveUI2"));
// // 			pActiveUI->ApplyAttributeList(_T("float=\"true\" pos=\"500,0,0,0\""));
// // 			pActiveUI->SetFixedWidth(500);
// // 			pActiveUI->SetFixedHeight(500);
// //  			RECT rc;
// //  			rc.left=500*(nCnt-1);rc.right=rc.left+500;
// //  			rc.top=0;rc.bottom=500;
// //  			pActiveUI->SetPos(rc);
//  		LPDISPATCH lpdisp;
//  		pWebBrowser->get_Application(&lpdisp);
//  		*ppDisp=lpdisp;
//  		ConnectEvent(pWebBrowser);
// 	}
}
void _stdcall CIeHelper::BeforeNavigate2(IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel)
{

}
void _stdcall CIeHelper::NewWindow3(IDispatch **&ppDisp,VARIANT_BOOL *&Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	CHorizontalLayoutUI* pHorLayout=static_cast<CHorizontalLayoutUI*>(g_friendUI->paint_manager_.FindControl(_T("tabHeader")));
	if(pHorLayout)
	{
		COptionUI* pOption=new COptionUI();
		pHorLayout->Add(pOption);
		int nCnt=pHorLayout->GetCount();
		CDuiString strName;
		strName.Format(_T("Tab%d"),nCnt);
		pOption->SetName(strName);
		//pOption->SetFixedWidth(78);
		pOption->SetText(strName);
		pOption->SetAttribute(_T("width"),_T("78"));
		pOption->SetAttribute(_T("textcolor"),_T("#FF386382"));
		pOption->SetAttribute(_T("normalimage"),_T("tabBtn/tabbar_normal.png"));
		pOption->SetAttribute(_T("hotimage"),_T("tabBtn/tabbar_hover.png"));
		pOption->SetAttribute(_T("pushedimage"),_T("tabBtn/tabbar_pushed.png"));
		pOption->SetAttribute(_T("selectedimage"),_T("tabBtn/tabbar_pushed.png"));
		pOption->SetAttribute(_T("group"),_T("listHeader"));
		pOption->SetAttribute(_T("selected"),_T("true"));
		//pOption->ApplyAttributeList(_T("width=\"78\" textcolor=\"#FF386382\" font=\"2\"normalimage=\"tabBtn/tabbar_normal.png\" hotimage=\"tabBtn/tabbar_hover.png\" pushedimage=\"tabBtn/tabbar_pushed.png\" selectedimage=\"tabBtn/tabbar_pushed.png\" group=\"demo_list_header\"selected=\"true\""));
		//pOption->ApplyAttributeList(_T("width=\"78\"")); 
	}
	CTabLayoutUI* pTabLayout=static_cast<CTabLayoutUI*>(g_friendUI->paint_manager_.FindControl(_T("tabSwitch")));
	if (pTabLayout)
	{
		//先创建一个水平布局
		CHorizontalLayoutUI* pHorLayout=new CHorizontalLayoutUI();

		//先添加水平布局
		pTabLayout->Add(pHorLayout);
		int nCnt=pTabLayout->GetCount();
		CDuiString strName;
		strName.Format(_T("ActiveXLayout_%d"),nCnt);
		pHorLayout->SetName(strName);
		pTabLayout->SelectItem(pHorLayout);
		CActiveXUI* pActiveUI=new CActiveXUI();
		strName.Format(_T("ActiveXUI_%d"),nCnt);
		pActiveUI->SetName(strName);
		//然后在水平布局添加ActiveXUI
		pHorLayout->Add(pActiveUI);
		//获取子控件的个数
		//int nCnt=pTabLayout->GetCount();
		pActiveUI->SetDelayCreate(false);
		//设置网页打开方式
		*Cancel =VARIANT_TRUE;
		IWebBrowser2* pWebBrowser=NULL;
		bool bRet=pActiveUI->CreateControl(CLSID_WebBrowser);
		pActiveUI->GetControl(IID_IWebBrowser2,(void**)&pWebBrowser);
		ConnectEvent(pWebBrowser);
		pWebBrowser->Navigate(bstrUrl,NULL,NULL,NULL,NULL);
		//pActiveUI->SetName(_T("ActiveUI2"));
		// 			pActiveUI->ApplyAttributeList(_T("float=\"true\" pos=\"500,0,0,0\""));
		// 			pActiveUI->SetFixedWidth(500);
		// 			pActiveUI->SetFixedHeight(500);
		//  			RECT rc;
		//  			rc.left=500*(nCnt-1);rc.right=rc.left+500;
		//  			rc.top=0;rc.bottom=500;
		//  			pActiveUI->SetPos(rc);
		//LPDISPATCH lpdisp;
		//pWebBrowser->get_Application(&lpdisp);
		//*ppDisp=lpdisp;
		//
	}
}


//挂载连接点
BOOL  CIeHelper::ConnectEvent(IWebBrowser2* pIE)
{
	DWORD dwCookie=0;
	HRESULT hr;
	m_pWebBrowser=pIE;
	if (pIE==nullptr)
	{
		return FALSE;
	}
	hr=m_pWebBrowser->QueryInterface(IID_IConnectionPointContainer,(void**)&m_pCPC);
	if (FAILED(hr))
	{
		return FALSE;
	}
	hr=m_pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&m_pCP);
	if (FAILED(hr))
	{
		return FALSE;
	}
	hr=m_pCP->Advise((IUnknown*)(void*)this,&dwCookie);
	if (FAILED(hr))
	{
		return FALSE;
	}
	return TRUE;
}

//IDocHostUIHandler methods
STDMETHODIMP CIeHelper::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);  

	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;  

	pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME |
		DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_DIALOG |
		DOCHOSTUIFLAG_DISABLE_HELP_MENU|DOCHOSTUIFLAG_SCROLL_NO;;


	return S_OK;
}
STDMETHODIMP CIeHelper::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::HideUI()
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::UpdateUI()
{
	return E_NOTIMPL;
}

STDMETHODIMP CIeHelper::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::TranslateAccelerator(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::GetOptionKeyPath(__out LPOLESTR *pchKey, DWORD dw)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::GetExternal(IDispatch **ppDispatch)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::TranslateUrl(DWORD dwTranslate, __in __nullterminated OLECHAR *pchURLIn, __out OLECHAR **ppchURLOut)
{
	return E_NOTIMPL;
}
STDMETHODIMP CIeHelper::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE CIeHelper::QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText )
{
	HRESULT hr = OLECMDERR_E_NOTSUPPORTED;
	return hr;
}

HRESULT STDMETHODCALLTYPE CIeHelper::Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut )
{
	HRESULT hr = S_OK;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{

		switch (nCmdID) 
		{

		case OLECMDID_SHOWSCRIPTERROR:
			{
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszNames[5] = 
				{ 
					SysAllocString(L"errorLine"),
					SysAllocString(L"errorCharacter"),
					SysAllocString(L"errorCode"),
					SysAllocString(L"errorMessage"),
					SysAllocString(L"errorUrl")
				};
				DISPID                      rgDispIDs[5];
				VARIANT                     rgvaEventInfo[5];
				DISPPARAMS                  params;
				BOOL                        fContinueRunningScripts = true;
				int                         i;

				params.cArgs = 0;
				params.cNamedArgs = 0;

				// Get the document that is currently being viewed.
				hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);    
				// Get document.parentWindow.
				hr = pDoc->get_parentWindow(&pWindow);
				pDoc->Release();
				// Get the window.event object.
				hr = pWindow->get_event(&pEventObj);
				// Get the error info from the window.event object.
				for (i = 0; i < 5; i++) 
				{  
					// Get the property's dispID.
					hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, 
						LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
					// Get the value of the property.
					hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
						LOCALE_SYSTEM_DEFAULT,
						DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
						NULL, NULL);
					SysFreeString(rgwszNames[i]);
				}

				// At this point, you would normally alert the user with 
				// the information about the error, which is now contained
				// in rgvaEventInfo[]. Or, you could just exit silently.

				(*pvaOut).vt = VT_BOOL;
				if (fContinueRunningScripts)
				{
					// Continue running scripts on the page.
					(*pvaOut).boolVal = VARIANT_TRUE;
				}
				else
				{
					// Stop running scripts on the page.
					(*pvaOut).boolVal = VARIANT_FALSE;   
				} 
				break;
			}
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);
}
