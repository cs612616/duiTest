#pragma once

#include "resource.h"

#include <objbase.h>
#include <zmouse.h>
#include <exdisp.h>
#include <comdef.h>
#include "DuiLib\UIlib.h"
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "lib\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "lib\\DuiLib.lib")
#   endif
#endif 

#include <iostream>

#include "ConsultFrameUI.h"
#include "InteractRecordFrame.h"
using namespace std;
using namespace DuiLib;



class CMainFrame
	: public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("DUIMainFrame");  }
	virtual CDuiString GetSkinFile()                {   return _T("MainFrame.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("");  }
public:
	virtual void InitWindow();
	virtual void OnPrepare();
	virtual void Notify(TNotifyUI& msg);
	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, _T("ConsultFrame")) == 0 ) return new CConsultFrameUI(m_PaintManager);
		if( _tcscmp(pstrClass, _T("InteractRecordFrame")) == 0 ) return new CInteractRecordFrameUI(m_PaintManager);
		return NULL;
	}
};