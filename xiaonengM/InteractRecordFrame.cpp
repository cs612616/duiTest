#include "StdAfx.h"
#include "InteractRecordFrame.h"


CInteractRecordFrameUI::CInteractRecordFrameUI(CPaintManagerUI& m_pm):m_PaintManager(m_pm)
{
	CDialogBuilder builder;
	CContainerUI* pInteractRecordFrame = static_cast<CContainerUI*>(builder.Create(_T("InteractRecordFrame.xml"), (UINT)0,this,&m_pm));
	if( pInteractRecordFrame ) {
		this->Add(pInteractRecordFrame);
	}
	else {
		this->RemoveAll();
		return;
	}
}

CInteractRecordFrameUI::~CInteractRecordFrameUI(void)
{
}
