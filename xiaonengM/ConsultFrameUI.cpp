#include "StdAfx.h"
#include "ConsultFrameUI.h"

#include "ControlEx.h"

CConsultFrameUI::CConsultFrameUI(CPaintManagerUI& m_pm):m_PaintManager(m_pm)
{
	CDialogBuilder builder;
	CContainerUI* pConsultFrame = static_cast<CContainerUI*>(builder.Create(_T("ConsultFrame.xml"), (UINT)0,this,&m_pm));
	if( pConsultFrame ) {
		this->Add(pConsultFrame);
	}
	else {
		this->RemoveAll();
		return;
	}
}

CConsultFrameUI::~CConsultFrameUI(void)
{
}
