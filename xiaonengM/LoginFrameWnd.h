#pragma once

#include "DuiLib\UIlib.h"
using namespace DuiLib;
class CLoginFrameWnd : public WindowImplBase
{
public:
	CLoginFrameWnd() { };
	virtual LPCTSTR GetWindowClassName() const { return _T("UILoginFrame"); };
	virtual CDuiString GetSkinFile()                {   return _T("login.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("");  }


	void Init() {
	}

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("click") ) {
			if( msg.pSender->GetName() == _T("closebtn") ) { PostQuitMessage(0); return; }
			else if( msg.pSender->GetName() == _T("loginBtn") ) { m_bIsLogin=true;Close(); return; }
		}
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);

		return lRes;
	}
public:
	BOOL m_bIsLogin;
};