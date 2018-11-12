
// FirstMFCApp.h : main header file for the FirstMFCApp application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFirstMFCAppApp:
// See FirstMFCApp.cpp for the implementation of this class
//

class CFirstMFCAppApp : public CWinApp
{
public:
	CFirstMFCAppApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFirstMFCAppApp theApp;
