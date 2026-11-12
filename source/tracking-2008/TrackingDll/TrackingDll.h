// TrackingDll.h : main header file for the TrackingDll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "ProjTransform.h"
#include "Matcher.h"

// CTrackingDllApp
// See TrackingDll.cpp for the implementation of this class
//
// Class links between C# GUI application and C++ code - implementation of training and tracking algorithm.
class CTrackingDllApp : public CWinApp
{
public:
	CProjTransform m_ProjTransform; // Performs linear algebra calculations
	Matcher m_Matcher; //  The main class - performs all the train,track procedure.
	CString trainPath; // Path to directory to read/write training part of the algorithm
	CString trackPath; // Path to directory from which we read Tracking picture.
	FILE* logger;      // Debug - in this file speed measurements are written. 
	FILE* ransac_logger; // Debug - Information about ransac iterations.
public:
	CTrackingDllApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	void DrawLine(byte* pVis, int vWidth,int  vHeight, int vStride,int x1,int y1 ,int x2, int y2);
	DECLARE_MESSAGE_MAP()
};
