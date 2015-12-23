#pragma once

#include "resource.h"

#include <windows.h>
#include <objbase.h>
#include "IeHelper.h"
#include "..\\DuiLib\UIlib.h"
using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\lib\\DuiLib.lib")
#   endif
#endif 

class CMyFrameWnd : public WindowImplBase
{
	CIeHelper m_ieHelper;
	IWebBrowser2* m_pWebBrowser;
	int m_nSel;
public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("DUIMainFrame");  }
	virtual CDuiString GetSkinFile()                {   return _T("BrowserUI.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("");  } 
	virtual void InitWindow();
	virtual void Notify(TNotifyUI& msg);

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};