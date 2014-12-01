/**
 * \file ElevatorApp.h
 *
 * \author Charles B. Owen
 *
 * \brief main header file for the Elevator application
 * \cond
 */

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CElevatorApp:
// See Elevator.cpp for the implementation of this class
//

class CElevatorApp : public CWinApp
{
public:
	CElevatorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
};

extern CElevatorApp theApp;

/// \endcond
