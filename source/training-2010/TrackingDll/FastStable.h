#pragma once


/*
* This class is used to sort stable points by quality.
* Class contains point coordinates - x,y and number of times
* this point was chosen as stable point - counter.
*/
class PointQuality
{
public:
	int x;
	int y;
	int counter;
	PointQuality(int _x,int _y,int _c)
	{
		x=_x;
		y=_y;
		counter=_c;
	}
};


/*
* Class describes line in R^2: ax+by+c = 0.
*/
class Line
{
public:
	double a,b,c;
	Line()
	{a=b=c=0;}

	// Construct line that pass through p1,p2
	Line(CPoint p1, CPoint p2)
	{
		if (p1.x == p2.x)
		{
			a = 1;
			b = 0;
			c = -p1.x;
		}
		else
		{
			b = 1;
			a = ((double)(p2.y-p1.y))/(p1.x-p2.x);
			c = -(p1.y +p1.x*a);
		}
	}
};


class CFastStable
{
public:
	CFastStable(void);
	int GetStablePoints(byte* pSource,int sWidth,int sHeight,int sStride,double distance, 
		int max_ret_pnts,int x_points[],int y_points[]);


	double* CreatePatches(Matcher& matcher, byte* pSource,int sWidth,int sHeight,int sStride,int num_transforms,
							   int num_pnts, int x_points[],int y_points[]);


	byte* CreateGreyBuff(byte* pSource,int sWidth,int sHeight,int sStride);
	bool FilterByContrast(int x,int y,byte* pSource,int sWidth);

	int CreatePatch(byte* pSource,int sWidth,int sHeight,int dWidth,int dHeight,double transf[],byte patch[]);
	double* CreatePatch(byte* greyLevelBuff,int sWidth,int sHeight,int x,int y);	
	void CreateComplexPicture(byte* pSource,int sWidth,int sHeight,int sStride,
							   byte* pDest,int dWidth,int dHeight,int dStride);
	int FindRange(byte* pSource,int startInd,int sWidth,int sStride);
private:
	int* counter;  // array that contain information about quality of feature points.
				  // counter[i][j] = num of times pixel in position (i,j) was stable point.

	double patch[WIND*WIND];  //This variable contains patch around feature point.
	
	CProjTransform m_ProjTransform; 
	void FillFirstLine(double* buff,byte* pSource,int sWidth,int sHeight,int sStride,
							   int num_pnts, int x_points[],int y_points[]);
};
