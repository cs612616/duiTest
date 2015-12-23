#pragma once
#include <windows.h>
#include <objbase.h>
#include <zmouse.h>
#include <exdisp.h>
#include <comdef.h>


#include <windows.h>
#include <objbase.h>
#include <map>
#include "DuiLib\UIlib.h"
using namespace DuiLib;
using namespace std;

extern map<int,CWebBrowserUI*> g_mapBrowser;
extern int g_nSelect;
class CWebEventEx :
	public CWebBrowserEventHandler
{
public:
	CWebEventEx(void);
	~CWebEventEx(void);
public:
	CPaintManagerUI* m_pPaintManager;
	BOOL SetPaintManager(CPaintManagerUI* paint)
	{
		if (FAILED(paint))
		{
			return FALSE;
		}
		m_pPaintManager=paint;
		return TRUE;
	}
public:
	void BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel );
	virtual void NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);		
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);

};

