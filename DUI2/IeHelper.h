#pragma once
#include <MsHTML.h>	//HTML接口
#include <ExDisp.h>
#include <ExDispid.h>
#include "FriendUI.h"
using namespace DuiLib;
class CIeHelper :
	public IDispatch, //DWebBrowserEvent2
	public IDocHostUIHandler,//UI控制
	public IOleCommandTarget
{
public:
	CIeHelper(void);
	virtual ~CIeHelper(void);
private:

	long m_refCnt;
	bool DoCreateControl();
	void ReleaseControl();
	
	//IUnknown methods
	virtual STDMETHODIMP QueryInterface(REFIID riid,void **ppvObject);
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();
	// IDispatch Methods
	HRESULT _stdcall GetTypeInfoCount(unsigned int * pctinfo);
	HRESULT _stdcall GetTypeInfo(unsigned int iTInfo,LCID lcid,ITypeInfo FAR* FAR* ppTInfo);
	HRESULT _stdcall GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,unsigned int cNames,LCID lcid,DISPID FAR* rgDispId);
	HRESULT _stdcall Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pDispParams,VARIANT FAR* pVarResult,EXCEPINFO FAR* pExcepInfo,unsigned int FAR* puArgErr);
	//利用DWebBrowserEvents2 接口接收WebBrowser事件
	virtual void _stdcall DocumentCompleted(IDispatch *pDisp,VARIANT *URL);
	virtual void _stdcall NewWindow2(IDispatch** ppDisp,BOOL* bCancel );
	virtual void _stdcall BeforeNavigate2(IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel);
	virtual void _stdcall NewWindow3(IDispatch **&ppDisp,VARIANT_BOOL *&Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl);
	//IDocHostUIHandler methods
	virtual STDMETHODIMP ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	//这里可以去除边框，滚动条，等其他外观设置
	virtual STDMETHODIMP GetHostInfo(DOCHOSTUIINFO *pInfo);
	virtual STDMETHODIMP ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
	virtual STDMETHODIMP HideUI();
	virtual STDMETHODIMP UpdateUI();
	virtual STDMETHODIMP EnableModeless(BOOL fEnable);  
	virtual STDMETHODIMP OnDocWindowActivate(BOOL fActivate);
	virtual STDMETHODIMP OnFrameWindowActivate(BOOL fActivate);
	virtual STDMETHODIMP ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
	virtual STDMETHODIMP TranslateAccelerator(LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID);
	virtual STDMETHODIMP GetOptionKeyPath(__out LPOLESTR *pchKey, DWORD dw);
	virtual STDMETHODIMP GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	virtual STDMETHODIMP GetExternal(IDispatch **ppDispatch);
	virtual STDMETHODIMP TranslateUrl(DWORD dwTranslate, __in __nullterminated OLECHAR *pchURLIn, __out OLECHAR **ppchURLOut);
	virtual STDMETHODIMP FilterDataObject(IDataObject *pDO, IDataObject **ppDORet);
	// IOleCommandTarget
	virtual HRESULT STDMETHODCALLTYPE QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText);
	virtual HRESULT STDMETHODCALLTYPE Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut );
public:
	BOOL ConnectEvent(IWebBrowser2* pIE);
private:
	IWebBrowser2* m_pWebBrowser;
	IConnectionPointContainer*	m_pCPC;
	IConnectionPoint*			m_pCP;
};

