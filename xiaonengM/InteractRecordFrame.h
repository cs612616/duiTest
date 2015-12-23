#pragma once
#include "DuiLib\UIlib.h"
using namespace DuiLib;
#include "WkeWebkit.h"
class CInteractRecordFrameUI:public CContainerUI,public IDialogBuilderCallback
{
public:
	CPaintManagerUI& m_PaintManager;
	CInteractRecordFrameUI(CPaintManagerUI& m_pm);
	~CInteractRecordFrameUI(void);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if (_tcsicmp(pstrClass, _T("wkeWebkit")) == 0) return  new CWkeWebkitUI;
		return NULL;
	}
};

