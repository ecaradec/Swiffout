// bgamefu.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CbgamefuApp:
// See bgamefu.cpp for the implementation of this class
//

class CbgamefuApp : public CWinApp
{
public:
	CbgamefuApp();

// Overrides
	public:
	virtual BOOL InitInstance();
    int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CbgamefuApp theApp;