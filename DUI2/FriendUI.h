#pragma once
#include <windows.h>
#include <objbase.h>
#include "..\\DuiLib\UIlib.h"
using namespace DuiLib;
class CFriendUI
{
public:
	CFriendUI(CPaintManagerUI& paint_manager);
	~CFriendUI(void);
	CPaintManagerUI& paint_manager_;
};

