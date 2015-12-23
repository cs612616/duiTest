// DUI1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DUI1.h"

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>

#include "..\DuiLib\UIlib.h"
using namespace DuiLib;

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

  //class CDuiFrameWnd : public CWindowWnd, public INotifyUI
  //{
  //public:
  //	virtual LPCTSTR GetWindowClassName() const { return _T("DUIMainFrame"); }
  //	virtual void    Notify(TNotifyUI& msg) 
  //	{
  //		if (msg.sType==_T("click"))
  //		{
  //			if (msg.pSender->GetName()==_T("btnHello"))
  //			{
  //				::MessageBox(NULL, _T("我是按钮"), _T("点击了按钮"), NULL);
  //			}
  //		}
  //	}
  //
  //	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
  //	{
  //		LRESULT lRes = 0;
  //
  //		if( uMsg == WM_CREATE ) 
  //		{
  //			//CControlUI *pWnd = new CButtonUI;
  //			//pWnd->SetName(_T("btnHello"));		// 设置名字
  //			//pWnd->SetText(_T("Hello World"));   // 设置文字
  //			//pWnd->SetBkColor(0xFF00F0F0);       // 设置背景色
  //
  //			m_PaintManager.Init(m_hWnd);
  //			//m_PaintManager.AttachDialog(pWnd);
  //
  //			CDialogBuilder builder;
  //			CControlUI* pRoot=builder.Create(_T("duilib.xml"),(UINT)0,NULL,&m_PaintManager);// duilib.xml需要放到exe目录下
  //			ASSERT(pRoot && "Failed to parse XML");
  //			m_PaintManager.AttachDialog(pRoot);
  //			m_PaintManager.AddNotifier(this);// 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理
  //			return lRes;
  //		}
  //		// 以下3个消息WM_NCACTIVATE、WM_NCCALCSIZE、WM_NCPAINT用于屏蔽系统标题栏
  //		else if (uMsg==WM_NCACTIVATE)
  //		{
  //			if (!::IsIconic(m_hWnd))
  //			{
  //				return (wParam==0)?TRUE:FALSE;
  //			}
  //		}
  //		else if(uMsg==WM_NCCALCSIZE)
  //		{
  //			return 0;
  //		}
  //		else if (uMsg==WM_NCPAINT)
  //		{
  //			return 0;
  //		}
  //		if( m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes) ) 
  //		{
  //			return lRes;
  //		}
  //
  //		return __super::HandleMessage(uMsg, wParam, lParam);
  //	}
  //
  //protected:
  //	CPaintManagerUI m_PaintManager;
  //};

  class CDuiFrameWnd : public WindowImplBase
  {
  public:
	  virtual LPCTSTR    GetWindowClassName() const   {   return _T("DUIMainFrame");  }
	  virtual CDuiString GetSkinFile()                {   return _T("duilib.xml");  }
	  virtual CDuiString GetSkinFolder()              {   return _T("");  }
  };

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());   // 设置资源的默认路径（此处设置为和exe在同一目录）

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.ShowModal();
	return 0;
}
