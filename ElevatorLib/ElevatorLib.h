/**
 * \file ElevatorLib.h
 *
 * \author Charles B. Owen
 *
 * \brief The library App class.
 * \cond
 */


#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CElevatorLibApp
// See ElevatorLib.cpp for the implementation of this class
//

class CElevatorLibApp : public CWinApp
{
public:
	CElevatorLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

/// \endcond
