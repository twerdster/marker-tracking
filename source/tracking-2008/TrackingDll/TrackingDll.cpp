// TrackingDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TrackingDll.h"
#include "fast.h"
#include "FastStable.h"
#include "SearchTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void DrawLine(byte* pVis, int vWidth,int  vHeight, int vStride,int x1,int y1 ,int x2, int y2);
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CTrackingDllApp

BEGIN_MESSAGE_MAP(CTrackingDllApp, CWinApp)
END_MESSAGE_MAP()


// CTrackingDllApp construction

CTrackingDllApp::CTrackingDllApp()
{
	logger = NULL;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTrackingDllApp object

CTrackingDllApp theApp;


// CTrackingDllApp initialization

BOOL CTrackingDllApp::InitInstance()
{
	CWinApp::InitInstance();
	srand((unsigned int)time(NULL));
	trainPath = "C:\\temp";
	if (fopen_s(&logger,"C:\\temp\\log.txt","w") != 0)
		logger = NULL;

	if (fopen_s(&ransac_logger,"C:\\temp\\log_ransac_points.txt","w") != 0)
		ransac_logger = NULL;
	//srand(20);
	return TRUE;
}

BOOL CTrackingDllApp::ExitInstance()
{
	fclose(logger);
	fclose(ransac_logger);
	CWinApp::ExitInstance();
	return TRUE;
}

extern "C" int PASCAL EXPORT Add(int x,int y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	m_ProjTransform.Transform();
	return x+y;
}

extern "C" void PASCAL EXPORT TransformImage(byte* pSource,int sWidth,int sHeight,int sStride,
											double xPivot,double yPivot, byte* pDest,int dWidth,int dHeight,int dStride, double* p)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double* inv = theApp.m_ProjTransform.Inverse(p);
	theApp.m_ProjTransform.ImTransform(pSource,CSize(sWidth,sHeight),sStride,xPivot,yPivot, 
		pDest,CSize(dWidth,dHeight), dStride, inv);
	delete[] inv;
}

extern "C" double* PASCAL EXPORT Inverse(double* p)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_ProjTransform.Inverse(p);
}

//xy* fast_corner_detect_X(const unsigned char * data, int xsize, int ysize, int threshold, int* numcorners)

extern "C" int PASCAL EXPORT GetCorners(byte* pSource,int sWidth,int sHeight,int sStride,int threshold, 
										byte* buff,
										int x_points[],int y_points[], int max_ret_points)
{
	int numcorners;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//xy* fast9_detect(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
	//StablePoints
	//byte* buff = new byte[sWidth*sHeight];
	/*int ind=0;
	int indc=0;
	int indStart = 0;
	int bpp = sStride/sWidth;
	for (int i=0; i < sHeight; i++,indStart+=sStride)
	{ 
		indc = indStart;
		for (int j=0; j < sWidth; j++)
		{
			buff[ind] =pSource[indc]*0.11+pSource[indc+1]*0.59+pSource[indc+2]*0.3;
			ind++;
			indc += bpp;
		}
	}
*/

	//image, width, height, width, threshold, &numCorners
	xy* arr = fast9_detect_nonmax(pSource, sWidth, sHeight, sStride, threshold, &numcorners);
	//xy* arr = fast9_detect_nonmax(buff, sWidth, sHeight, sWidth, threshold, &numcorners);
	//delete[] buff;
	numcorners = min(max_ret_points,numcorners);
	for (int i=0; i < numcorners; i++)
	{
		x_points[i] = arr[i].x;
		y_points[i] = arr[i].y;
	}
	
	delete[] arr;
	return numcorners;
}

//find_corners

extern "C" int PASCAL EXPORT FindStable(byte* pSource,int sWidth,int sHeight,int sStride,double distance, 
		int max_ret_pnts,int x_points[],int y_points[])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CFastStable fs;
	return fs.GetStablePoints(pSource,sWidth,sHeight, sStride, distance, 
		 max_ret_pnts, x_points, y_points);

}


extern "C" void PASCAL EXPORT CreateComplexPicture(byte* pSource,int sWidth,int sHeight,int sStride,
							   byte* pDest,int dWidth,int dHeight,int dStride)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CFastStable fs;
	fs.CreateComplexPicture(pSource,sWidth,sHeight,sStride,pDest,dWidth, dHeight, dStride);

}

extern "C" void PASCAL EXPORT CreatePatches(byte* pSource,int sWidth,int sHeight,int sStride,int num_transforms,
							   int num_pnts, int x_points[],int y_points[],
							   byte* pDest,int dWidth,int dHeight,int dStride)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CFastStable fs;
	double* buff=fs.CreatePatches(theApp.m_Matcher, pSource,sWidth,sHeight,sStride,num_transforms,
							   num_pnts,x_points, y_points);
	int ind = 0 ,ind2 = 0;
	int indexStart = 0;
	for (int i=0; i < dHeight; i++, indexStart += dStride)
	{
		ind = indexStart;
		for (int j=0; j < dWidth; j++,ind2++,ind++)
		{
			pDest[ind] = (byte)buff[ind2];//(byte)(buff[ind2]*256);
		}
	}
	delete[] buff;

}

extern "C" void PASCAL EXPORT SetThreshold(double thresh)
{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		theApp.m_Matcher.SetThreshold(thresh);
}


extern "C" void PASCAL EXPORT SetRansacThresh(double thresh)
{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		theApp.m_Matcher.SetRansacThreshold(thresh);
}

extern "C" void PASCAL EXPORT SetRansacIterations(int iters_num)
{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		theApp.m_Matcher.SetRansacIterations(iters_num);
}

extern "C" void PASCAL EXPORT DrawRect(byte* pSource, int sWidth, int sHeight, int sStride)
{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		theApp.m_Matcher.DrawRect(pSource,sWidth,sHeight,sStride);

}

extern "C" void PASCAL EXPORT  Track(byte* pSource,int sWidth,int sHeight,int sStride,int x_corners[],int y_corners[])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//t.Read("C:\\Temp\\1.txt");
	theApp.m_Matcher.DistributePoints(pSource, sWidth, sHeight, sStride, x_corners, y_corners);
}

extern "C" void PASCAL EXPORT SetTrainDirectory(char path[])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theApp.trainPath = path;
	theApp.m_Matcher.SetTrainDone(false);
}

extern "C" bool PASCAL EXPORT LoadTrainData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bool res = theApp.m_Matcher.LoadTrainData();
	if (res == false)
	{ //TODO - get alarm
	}
	return res;
}

extern "C" void PASCAL EXPORT TrackVisualisation(byte* pSource,int sWidth,int sHeight,int sStride,
	byte* pMarker,int mWidth,int mHeight,int mStride,
	byte* pVis,int vWidth,int vHeight,int vStride)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int indexStartMar= 0;
	int indexStartVis = 0;
	int indexStartSrc = 0;
	int bppm = mStride/mWidth;
	int bppv = vStride/vWidth;
	int bpp = min(bppv,bppm);
	int index1,index2;
	if (bppm!=bppv)
	{
		for (int i=0;i< mHeight; i++,indexStartMar += mStride, indexStartVis += vStride)
		{
			index1 = indexStartMar;
			index2 = indexStartVis;
			for (int j=0; j < mWidth; j++, index1 += bppm, index2 += bppv)
			{
			for (int k=0; k < bpp; k++)
				pVis[index2+k] = pMarker[index1+k];			
			}
		}
	}
	else
	{
	for (int i=0;i< mHeight; i++,indexStartMar += mStride, indexStartVis += vStride)
	{
		memcpy(pVis + indexStartVis, pMarker + indexStartMar,mStride);
	}
	}
	
	indexStartVis = (mStride/bpp)*bpp;
	for (int i=0; i < sHeight; i++, indexStartSrc += sStride, indexStartVis += vStride)
	{
		memcpy(pVis + indexStartVis,pSource + indexStartSrc, sStride);
	}

	//vector<Point3D> train_match, track_match;
	for (size_t i=0; i < theApp.m_Matcher.train_match.size(); i++)	
	{
		theApp.DrawLine( pVis, vWidth, vHeight, vStride,
		    (int)(theApp.m_Matcher.train_match[i]).x,(int)(theApp.m_Matcher.train_match[i]).y , 
			(int)(theApp.m_Matcher.track_match[i].x) + mWidth, 
			(int)(theApp.m_Matcher.track_match[i]).y);
	}
}

/*
* Draw line that passes through (x1,y1) , (x2,y2) on the image pVis
*/
void CTrackingDllApp::DrawLine(byte* pVis, int vWidth,int  vHeight, int vStride,int x1,int y1 ,int x2, int y2)
{
	double slope;

	int x,y;
	double b;
	int index;
	int bpp = vStride / vWidth;
	if (abs(y2 - y1) > abs(x2-x1))
	{
		slope = (double)(x2-x1) / (y2-y1);
		b = x1 - y1*slope;
		for (y=min(y1,y2); y < max(y1,y2); y++)
		{
			x = (int)(y*slope + b+0.5); 
			index = vStride * y + x*bpp;
			//if (bpp == 3)
			pVis[index] = 0;
			pVis[index + 1] = 0;
			pVis[index + 2] = 255;
		}
	}
	else
	{
		slope = (double)(y2-y1) / (x2-x1);
		b = y1-x1*slope;
		for (x=min(x1,x2); x < max(x1,x2); x++)
		{
			y = (int)(x*slope + b+0.5);
			index = vStride * y + x*bpp;
			pVis[index] = 0;
			pVis[index + 1] = 0;
			pVis[index + 2] = 255;
		}
	}
	//fprintf(stdout,"The points are : %d %d %d %d \n", x1,y1,x2,y2);
	//fflush(stdout);
	//2 cases: 1) go over x's 2) go over y's
	
}
