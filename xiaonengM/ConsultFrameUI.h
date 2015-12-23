#pragma once

#include "DuiLib\UIlib.h"
#include "ControlEx.h"
using namespace DuiLib;

class CConsultFrameUI : public CContainerUI,public IDialogBuilderCallback
{
public:
	CPaintManagerUI& m_PaintManager;
	CConsultFrameUI(CPaintManagerUI& m_pm);
	~CConsultFrameUI(void);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, _T("GameList")) == 0 ) return new GameListUI;
		return NULL;
	}
};
