#pragma once

#include "resource.h"

#include <objbase.h>
#include <zmouse.h>
#include <exdisp.h>
#include <comdef.h>
#include <map>
#include <ShellAPI.h>
#include "DuiLib\UIlib.h"
using namespace std;
using namespace DuiLib;
#include "WebEventEx.h"

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

#include "stdafx.h"

//记录每个标签页的顺序以及对应的CWebBrowserUI指针
map<int,CWebBrowserUI*> g_mapBrowser;
//记录当前选中的tab页
int g_nSelect=0;

class CMainFrame
	: public WindowImplBase
{
public:
	IWebBrowser2* m_pWebBrowser;
	CWebEventEx* m_pWebEventEx;
public:
	virtual LPCTSTR    GetWindowClassName() const   {   return _T("DUIMainFrame");  }
	virtual CDuiString GetSkinFile()                {   return _T("BrowserUI.xml");  }
	virtual CDuiString GetSkinFolder()              {   return _T("");  }
public:
	virtual void InitWindow();
	virtual void Notify(TNotifyUI& msg);
	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	void Search();
private:
	map<CDuiString,bool> m_MenuCheckInfo; //保存菜单的单选复选信息
};