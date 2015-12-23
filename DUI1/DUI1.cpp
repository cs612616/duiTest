// DUI1.cpp : ����Ӧ�ó������ڵ㡣
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
  //				::MessageBox(NULL, _T("���ǰ�ť"), _T("����˰�ť"), NULL);
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
  //			//pWnd->SetName(_T("btnHello"));		// ��������
  //			//pWnd->SetText(_T("Hello World"));   // ��������
  //			//pWnd->SetBkColor(0xFF00F0F0);       // ���ñ���ɫ
  //
  //			m_PaintManager.Init(m_hWnd);
  //			//m_PaintManager.AttachDialog(pWnd);
  //
  //			CDialogBuilder builder;
  //			CControlUI* pRoot=builder.Create(_T("duilib.xml"),(UINT)0,NULL,&m_PaintManager);// duilib.xml��Ҫ�ŵ�exeĿ¼��
  //			ASSERT(pRoot && "Failed to parse XML");
  //			m_PaintManager.AttachDialog(pRoot);
  //			m_PaintManager.AddNotifier(this);// ��ӿؼ�����Ϣ��Ӧ��������Ϣ�ͻᴫ�ﵽduilib����Ϣѭ�������ǿ�����Notify����������Ϣ����
  //			return lRes;
  //		}
  //		// ����3����ϢWM_NCACTIVATE��WM_NCCALCSIZE��WM_NCPAINT��������ϵͳ������
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
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());   // ������Դ��Ĭ��·�����˴�����Ϊ��exe��ͬһĿ¼��

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.ShowModal();
	return 0;
}
