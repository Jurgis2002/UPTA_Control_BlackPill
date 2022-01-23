
// UPTA_Control_BlackPill.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUPTAControlBlackPillApp:
// See UPTA_Control_BlackPill.cpp for the implementation of this class
//

class CUPTAControlBlackPillApp : public CWinApp
{
public:
	CUPTAControlBlackPillApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CUPTAControlBlackPillApp theApp;
