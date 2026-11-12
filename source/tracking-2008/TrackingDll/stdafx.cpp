// stdafx.cpp : source file that includes just the standard includes
// TrackingDll.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CTrackingDllApp* GetApp()
{
	return ((CTrackingDllApp*)AfxGetApp());
}