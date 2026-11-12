#include "StdAfx.h"
#include "FastStable.h"
#include "SearchTree.h"
#include <math.h>
#include <algorithm>
#include <vector>


#define PI 3.141592
using namespace std;

//Compare qualirt of 2 feature points.
bool compareQuality (const PointQuality& p1,const PointQuality& p2)
{ 
	return (p1.counter>p2.counter);
}

CFastStable::CFastStable(void)
{
	counter = NULL;
}

/*
* Input: x,y - coordinates of the point.
         border_lines, corners - lines of the polygon (4 vertices)
  Output : returns true if (x,y) is inside the polygon, false - otherwise.
*/
bool isInside(double x,double y, Line border_lines[], CPoint corners[])
{
	for (int i=0;i < 2; i++)
	{
		Line l1 = border_lines[i];
		Line l2 = border_lines[(i+2)%4];
		// Linear algebra - check scalar products. 
		// Scalar product's (x,y,1)*(l.a,l.b,l.c) sign corresponds to 
		// position of point : upper half-plane, line itself (scalar product = 0)
		// and lower half-plane
												
		if ((l1.a * x + l1.b*y+ l1.c)* (l2.a*x + l2.b*y + l2.c ) >= 0)
		{
			return false;
		}
	}
	return true;

}


/*
* Create grey picture from color picture. 
* Function allocates memory of sWidth*sHeight bytes and returns
* pointer to the created picture.
*/
byte* CFastStable::CreateGreyBuff(byte* pSource,int sWidth,int sHeight,int sStride)
{
	byte* greyLevelBuff = new byte[sWidth*sHeight];
	int ind=0;
	int indc=0;
	int indStart = 0;
	int bpp = sStride/sWidth;
	for (int i=0; i < sHeight; i++,indStart+=sStride)
	{ 
		indc = indStart;
		for (int j=0; j < sWidth; j++)
		{
			greyLevelBuff[ind] =(byte)(pSource[indc]*0.11+pSource[indc+1]*0.59+pSource[indc+2]*0.3);
			ind++;
			indc += bpp;
		}
	}

	byte conv[] = {1,2,1,2,4,2,1,2,1};
	int indexes[] = {-sWidth-1 , -sWidth, -sWidth+1, -1, 0 ,1 , sWidth-1,sWidth , sWidth+1};
	byte* res = new byte[sWidth*sHeight];
	int temp;
	indc = 0;
	memcpy(res,greyLevelBuff,sWidth);
	indStart = sWidth;
	for (int i=1; i < sHeight - 1; i++ , indStart += sWidth)
	{
		indc = indStart;
		res[indc] = greyLevelBuff[indc];
		indc++;
		for (int j=1; j < sWidth - 1; j++,indc++)
		{
			temp = 0;
			for (int k=0; k < 9; k++)
			{
				 temp += greyLevelBuff[indc + indexes[k]]*conv[k];
			}
			res[indc] = temp/16;
		}
		res[indc] = greyLevelBuff[indc];
	}
	memcpy(res+indStart, greyLevelBuff + indStart, sWidth);

	delete[] greyLevelBuff;

	return res;
}



/*
*  One of the main functions in the train stage. Function finds stable 
* characteristic points on the MARKER picture.
* Returns points coordinates in the arrays x_points, y_points.
Returns up to max_ret_pnts. (Usually 150 <= max_ret_pnts <= 200)
*/
int CFastStable::GetStablePoints(byte* pSource,int sWidth,int sHeight,int sStride,double distance, 
		int max_ret_pnts,int x_points[],int y_points[])
{
	if (counter != NULL)
		delete[] counter;
	counter = new int[sWidth*sHeight];
	memset(counter,0,sWidth*sHeight*sizeof(int));

	int bpp = sStride/sWidth;
	
	int numcorners;
	int threshold = 20;

	//create grey level picture from the original image
	byte* greyLevelBuff = CreateGreyBuff(pSource, sWidth, sHeight, sStride);

	//apply FAST on the grey level picture
	xy* arr = fast9_detect_nonmax(greyLevelBuff, sWidth, sHeight, sWidth, threshold, &numcorners);
	for (int i=0; i < numcorners; i++)
	{
		counter[arr[i].y*sWidth + arr[i].x] ++;
	}
	delete[] arr;

	/*
	* Apply many iterations of FAST to get the stable points. 
	*/
	int num_iterations = 2000;
	for (int itern = 0; itern < num_iterations; itern++)
	{
		 // The parameters of the random transformation
		 double transf[9];
		 double lambda_min = 0.5;
		 double lambda_max = 1.5;
		 double theta_max = 2*PI;
		 double phi_max = PI;
		 m_ProjTransform.CreateRandomTrasformation(transf,lambda_min,lambda_max,theta_max,phi_max);
	
		 int xPivot = (int)transf[2]; 
		 int yPivot = (int)transf[5];

		 CPoint corners[4] ;
		 corners[0].x = 0;
		 corners[0].y = 0;

		 corners[1].x = sWidth;
		 corners[1].y = 0;

		 corners[2].x = sWidth;
		 corners[2].y = sHeight;
                
		 corners[3].x = 0;
		 corners[3].y = sHeight;

                
		 double x_min = INT_MAX , y_min = INT_MAX;
		 double x_max = -INT_MAX, y_max = -INT_MAX;
		 for (int i = 0; i < 4; i++)
		 { 
			double x,y,z;
			x = transf[0]*corners[i].x+transf[1]*corners[i].y+transf[2];
			y = transf[3] * corners[i].x + transf[4] * corners[i].y + transf[5];
			z = transf[6] * corners[i].x + transf[7] * corners[i].y + transf[8];
			x_min = min(x / z, x_min);
			y_min = min(y / z, y_min);
			x_max = max(x / z, x_max);
			y_max = max(y / z, y_max);
		 }
        
		 int dWidth = (int)(x_max-x_min);
		 int dHeight = (int)(y_max-y_min);
		 int dStride = dWidth;
		 byte* dst = new byte[dWidth*dHeight];
	 
		 double* inv = m_ProjTransform.Inverse(transf);
	
		 // Apply random transformation to the marker
		 m_ProjTransform.ImTransform(greyLevelBuff,CSize(sWidth,sHeight),sWidth,xPivot,yPivot, 
			dst,CSize(dWidth,dHeight), dStride, inv);

		 //find stable points in the transformed image
		arr = fast9_detect_nonmax(dst, dWidth, dHeight, dStride, threshold, &numcorners);
		for (int i=0; i <numcorners; i++)
		{
			double x,y,z;
			x = arr[i].x - xPivot;
			y = arr[i].y - yPivot;
			z=1;
			double x1,y1,z1;
			x1 = inv[0]*x + inv[1]*y +inv[2]*z;
			y1 = inv[3]*x + inv[4]*y +inv[5]*z;
			z1 = inv[6]*x + inv[7]*y +inv[8]*z;
			if (z1==0)
				continue;
			int j,k;
			j = (int)(x1/z1 +0.5);
			k=(int)(y1/z1 + 0.5);

			// Return to the coordinates of feature points on the original (not transformed) image 
			if (j>=0 && j < sWidth && k>=0 && k<sHeight)
				counter[k*sWidth + j]++;
		}
	 
		delete[] inv;
		delete[] arr;
	}

	// Sort feature points by quality
	vector<PointQuality> vec;
	int ind=0;
	for (int i=0; i < sHeight; i++)
	{
		for (int j=0; j < sWidth; j++)
		{
			if (counter[ind]!=0)
			{
				vec.push_back(PointQuality(j,i,counter[ind]));
			}
			ind++;
		}
	}
	sort(vec.begin(),vec.end(),compareQuality);
	int length = 10;
	int good_pnts=0;
	int border_thresh = 15;
	fprintf(stdout,"Total points = %d\n",vec.size());
		
	
	//Filter feature points (return up to max_ret_pnts)
	for (int i=0; i < (int)vec.size(); i++)
	{
		int x = vec[i].x;
		int y = vec[i].y;
		if (x<border_thresh || y < border_thresh || x>sWidth - border_thresh || y>sHeight - border_thresh)
			continue;

		// Filter points by the procedure written in the article
		bool flag = FilterByContrast(x,y,greyLevelBuff,sWidth);
		if (flag == false)
			continue;

		//filter by neighborhood edges
		for (int j=0;j<good_pnts; j++)
		{
			if (abs(x_points[j] - x)+abs(y_points[j]-y) < distance)
			{
				flag=false;
				continue;
			}

		}
		/*if (x < border_thresh || x > (sWidth - border_thresh) || y < border_thresh
			|| y > (sHeight -  border_thresh))
			flag=false;*/

		if (flag == false)
			continue;

		x_points[good_pnts] = vec[i].x;
		y_points[good_pnts] = vec[i].y;
		
	
		good_pnts++;
		if (good_pnts == max_ret_pnts)
			break;
	}

	fprintf(stdout,"Total points after filtration= %d\n",good_pnts);
	delete[] greyLevelBuff;
	return good_pnts;

}

/*
* Go over circle with radius R = WIND/4 and check that points on the 
* circle are differ by intensity from the center of the circle.
* If this happens return true, otherwise the point is not feature point,
* so return false.
*/
bool CFastStable::FilterByContrast(int x,int y,byte* pSource,int sWidth)
{
	double R = WIND/4; //R = 16
	double theta = 0;

	int index = y*sWidth + x ,index1,index2;
	double x1,y1,x2,y2;
	int tau = 10;  //as in article

	double alpha,beta;
	double val1,val2;

	for (int k=0; k < 8; k++,theta += PI/8)
	{
		x1 = x + R*cos(theta);
		x2 = x - R*cos(theta);
		y1 = y + R*sin(theta);
		y2 = y - R*sin(theta);
		
		alpha = y1 - (int)y1;
		beta = x1 - (int)x1;
		index1 = (int)y1*sWidth+(int)x1;
		val1 = alpha*(beta*pSource[index1] + (1-beta)*pSource[index1+1])
			+(1-alpha)*(beta*pSource[index1+sWidth] + (1-beta)*pSource[index1+1+sWidth]);

		index2 = (int)y2*sWidth + (int)x2;
		alpha = y2 - (int)y2;
		beta = x2 - (int)x2;
		val2 = alpha*(beta*pSource[index2] + (1-beta)*pSource[index2+1])
			+(1-alpha)*(beta*pSource[index2+sWidth] + (1-beta)*pSource[index2+1+sWidth]);
					
		if (abs(pSource[index] - pSource[index1]) <= tau &&
			abs(pSource[index] - pSource[index2]) <= tau)
			return false;
	}
	return true;
}

/*
* Create patches around feature points - x_points, y_points
* on the original image pSource. Write the result to buff.
* First parameters - params of the Marker (picture pSource).
* num_pnts - number of feature points in the marker
* x_points - x coordinates of the feature points
* y_points - y coordinates of the feature points.
*/
void CFastStable::FillFirstLine(double* buff,byte* pSource,int sWidth,int sHeight,int sStride,
							   int num_pnts, int x_points[],int y_points[])
{
 	 int wind = 64;
	 int step_y = num_pnts*wind;
	 int startIndex = 0;
	 int bytesPerPixel = sStride/sWidth;

	 // Loop over all feature points in the Marker
	 for (int i=0; i < num_pnts; i++, startIndex += wind)
	 {
		 CPoint q(x_points[i],y_points[i]);

		 //calculate coordinates of small rectangle around the feature point.
		 CPoint boundary[4];
		 boundary[0].x = q.x - wind/2;
		 boundary[0].y = q.y - wind/2;
		 boundary[1].x = q.x + wind/2;
		 boundary[1].y = q.y - wind/2;
		 boundary[2].x = q.x + wind/2;
		 boundary[2].y = q.y + wind/2;
		 boundary[3].x = q.x - wind/2;
		 boundary[3].y = q.y + wind/2;
		 
		 bool allPointsInside = true;
		 for (int j=0; j<4; j++)
		 {
			if (boundary[j].x < 0 || boundary[j].y < 0 || boundary[j].x >= sWidth || boundary[j].y >= sHeight)
			{
				allPointsInside = false;
				break;
			}
		 }

		 int x_coord = (int)(q.x - wind/2);
		 int y_coord = (int)(q.y - wind/2);

		 int index_y = startIndex;
		 int dest_ind = y_coord*sStride + x_coord*bytesPerPixel; //green band
		 double min_value = INT_MAX;
		 double max_value = INT_MIN;

		 // if all points are inside the picture just take them and fill the patch,
		 // otherwise in the points outside the picture fill by random values.
		 if (allPointsInside)
		 {			
			for (int j=0; j<wind; j++,index_y += step_y, dest_ind += sStride)
			{
				int index = index_y;
				int indexD = dest_ind;
				for (int k=0; k <wind; k++,index++,indexD+=bytesPerPixel)
				{
					
					buff[index] = pSource[indexD];
					min_value = min(min_value,buff[index]);
					max_value = max(max_value,buff[index]);
				}
			}
		 }
		 else
		 {
			for (int j=0; j<wind; j++,index_y += step_y,dest_ind += sStride)
			{
				int index = index_y;
				int indexD = dest_ind;
				for (int k=0; k <wind; k++,index++,indexD+=bytesPerPixel)
				{
					if (q.x -wind/2+ k < 0 || q.y -wind/2 + j < 0 || q.x -wind/2+ k >= sWidth || q.y -wind/2 + j >= sHeight)
						buff[index] = rand()%256;	
					else
						buff[index] = pSource[indexD];

					min_value = min(min_value,buff[index]);
					max_value = max(max_value,buff[index]);
				}
			}
		 }
		 index_y = startIndex;
	 }

}


/*
* Create picture from 2 pictures. Apply random transformation to the pSource
* and paste the result to the pDest.
*/

void CFastStable::CreateComplexPicture(byte* pSource,int sWidth,int sHeight,int sStride,
							   byte* pDest,int dWidth,int dHeight,int dStride)
{
	//Parameters of the random transformation
	 double transf[9];
	 double lambda_min = 0.5;
	 double lambda_max = 1.5;
	 double theta_max = 2*PI;
	 double phi_max = PI;
	 double x_min,x_max,y_min,y_max;
	 int xOffset,yOffset; 
	 int xPivot,yPivot;

	 while (true)
	 {
	 m_ProjTransform.CreateRandomTrasformation(transf,lambda_min,lambda_max,theta_max,phi_max);

     CPoint corners[5] ;
     corners[0].x = 0;
	 corners[0].y = 0;

     corners[1].x = sWidth;
	 corners[1].y = 0;

	 corners[2].x = sWidth;
	 corners[2].y = sHeight;
                
	 corners[3].x = 0;
	 corners[3].y = sHeight;
	 
	 corners[4].x = 0;
	 corners[4].y = 0;
                
     x_min = INT_MAX;
	 y_min = INT_MAX;
     x_max = -INT_MAX;
	 y_max = -INT_MAX;

	 CPoint transformed_corners[4];
	 
	 // Calculates size of new picure - used to allocate the right amount of memory,
	 // in this case used to know where the picture pSource will be after the transformation.
	 // Transformed pSource will be set in the pDest picture.
     for (int i = 0; i < 4; i++)
     { 
        double x,y,z;
        x = transf[0]*corners[i].x+transf[1]*corners[i].y+transf[2];
        y = transf[3] * corners[i].x + transf[4] * corners[i].y + transf[5];
        z = transf[6] * corners[i].x + transf[7] * corners[i].y + transf[8];
        transformed_corners[i].x = (int)(x/z);
		transformed_corners[i].y = (int)(y/z);
		x_min = min(x / z, x_min);
        y_min = min(y / z, y_min);
        x_max = max(x / z, x_max);
        y_max = max(y / z, y_max);
     }

	 xPivot = (int)x_min; 
     yPivot = (int)y_min;

	
	 
	 if ( dWidth < (int)(x_max-x_min) || dHeight < (int)(y_max - y_min) )
	 {
		 continue;
	 }
	 else
	 {
		 xOffset = rand()%(dWidth - ((int)(x_max - x_min)));
		 yOffset = rand()%(dHeight - (int)((y_max - y_min)));
		 break;
	 }
	 }
    // int dHeight = (int)(y_max-y_min);
	// int dStride = dWidth;
	 CSize dSize((int)(x_max - x_min),(int)(y_max - y_min));
	 CSize sSize(sWidth,sHeight);
	 double* p = m_ProjTransform.Inverse(transf); //inv
    
	int i, j, k, index, indexStart;
    int x0, x1, y0, y1;
    double t, u, t_, u_, tu, tu_, t_u, t_u_;
    double x, y,z;
	double x2,y2,z2;
    long Y0X0, Y0X1, Y1X0, Y1X1;
    double temp;
	int iBytesPerPixel = sStride/sWidth;
	int destBytesPerPixel = dStride/dWidth;
	
    indexStart = xOffset*destBytesPerPixel + yOffset * dStride;
	double coef[] = {0.11, 0.59, 0.3 };
    for (i = 0; i < dSize.cy; i++, indexStart += dStride)
    {
        index = indexStart;
        for (j = 0; j < dSize.cx; j++, index += destBytesPerPixel)
        {
            x2 = j+xPivot;
			y2 = i+yPivot;
			z2=1;
			
			x = p[0]*x2+p[1]*y2+p[2]*z2;
			y = p[3]*x2+p[4]*y2+p[5]*z2;
			z = p[6]*x2+p[7]*y2+p[8]*z2;
			x = x/z;
			y = y/z;

			y0 = (int)y;
            x0 = (int)x;
            x1 = x0 + 1;	// bottom right x1
            y1 = y0 + 1;	// bottom right y1
		    if (x0 < 0 || x1 < 0 || y0 < 0 || y1 < 0 ||
			    y0 >= sSize.cy || y1 >= sSize.cy || x0 >= sSize.cx || x1 >= sSize.cx)
			{
				continue;
			}
            t = (double)(x - x0);
            u = (double)(y - y0);
            u_ = 1.0f - u;
            t_ = 1.0f - t;
            tu = t * u;
            tu_ = t * u_;
            t_u = t_ * u;
            t_u_ = t_ * u_;
            Y0X0 = y0*sStride + x0*iBytesPerPixel;
            Y0X1 = y0 * sStride + x1 * iBytesPerPixel;
            Y1X0 = y1 * sStride + x0 * iBytesPerPixel;
            Y1X1 = y1 * sStride + x1 * iBytesPerPixel;
			if (iBytesPerPixel == destBytesPerPixel)
			{
				for (k = 0; k < iBytesPerPixel; k++)
				{
				    temp = (
                    t_u_ * pSource[Y0X0 + k] +
                    tu_ * pSource[Y0X1 + k] +
                    t_u * pSource[Y1X0 + k] +
                    tu * pSource[Y1X1 + k] + 0.5);
					pDest[index+k] = (byte)temp;
				}
			}
			else
			{
				temp = 0;
				for (k = 0; k < iBytesPerPixel; k++)
				{
				    temp +=	coef[k]* (
                    t_u_ * pSource[Y0X0 + k] +
                    tu_ * pSource[Y0X1 + k] +
                    t_u * pSource[Y1X0 + k] +
                    tu * pSource[Y1X1 + k] + 0.5);
				}
				pDest[index] = (byte)temp;
			}
        }
    }


}


/*
* The main function of the train:
1) Finds stable points in the Marker.
2) Creates patches around these points in different transformations on the Marker.
3) Returns pointer to array with all these patches in one huge picture. 
* (This picture is used only for debug and for report on the work) 
* After train stage it can be deleted.
*/
double* CFastStable::CreatePatches(Matcher& matcher, byte* pSource,int sWidth,int sHeight,int sStride,int num_transforms,
							   int num_pnts, int x_points[],int y_points[])
{
	//Parameters of the random transformations
	 double transf[9];
	 double lambda_min = 0.5;
	 double lambda_max = 1.5;
	 double theta_max = 2*PI;
	 double phi_max = PI;
	 
	 // Time measurements
	 LARGE_INTEGER lFrequency5;
	QueryPerformanceFrequency( &lFrequency5);
	LARGE_INTEGER lPerformanceCount51, lPerformanceCount52;
	QueryPerformanceCounter(&lPerformanceCount51);		
	char sMess[260];
	int wind = 64;
	int bpp =sStride/sWidth;
	byte* greySource = new byte[sWidth*sHeight ];
	/*
	//Create grey level buffer from the original image
	//TODO: add median filter to smooth the image
	*/
	double coef[] = {0.11, 0.59, 0.3 };
	double temp;
	int ind1,ind2=0,indexStart1=0;
	for (int i=0; i < sHeight; i++,indexStart1+=sStride)
	{
		ind1 = indexStart1;
		for (int j=0; j < sWidth; j++,ind1+=bpp,ind2++)
		{
			temp = 0;
			for (int k=0;k<bpp; k++)
			{
				temp += coef[k]*pSource[ind1+k];
			}
			greySource[ind2] = (byte)temp;
		}
	}

	// buff will contain patches. 
	double* buff = new double[num_pnts*(num_transforms+1)*wind*wind ];
	if (buff == NULL)
		return buff;

	//first line corresponds to the identity transformation
	FillFirstLine(buff,greySource,sWidth,sHeight,sWidth,num_pnts,x_points,y_points);

	int x0, x1, y0, y1;
    double t, u, t_, u_, tu, tu_, t_u, t_u_;
	double x2,y2,z2;
    long Y0X0, Y0X1, Y1X0, Y1X1;

	int step = wind*wind*num_pnts;
	int startIndex = step;
	// loop over transformations
	for (int itern = 0; itern < num_transforms; itern++, startIndex += step)
	{ 
	 m_ProjTransform.CreateRandomTrasformation(transf,lambda_min,lambda_max,theta_max,phi_max);

	 double transf_around_point[9];
	 double translation[9];  
	 translation[0] = 1; translation[1] =0; translation[2] = 0;
	 translation[3] = 0; translation[4] =1; translation[5] = 0;
	 translation[6] = 0; translation[7] =0; translation[8] = 1;
     		 
	 int patchStartIndex = startIndex;
	 int step_y = num_pnts*wind;
	 int x_offset, y_offset;
     x_offset = rand()%4-4;
     y_offset = rand()%4-4;
	 int index;

	 double z=1;

	 //loop over feature points on the MARKER
	 for (int i=0; i < num_pnts; i++, patchStartIndex += wind)
	 {
		 translation[2] = -x_points[i];
		 translation[5] = -y_points[i];
		 
		 double* A= m_ProjTransform.Mult(transf,translation);
		// double* B= m_ProjTransform.Mult(inv_translation,A);
		 for (int i=0; i < 9; i++)
		 {
			transf_around_point[i] = A[i];
		 }
		 delete[] A;
		 double* inv = m_ProjTransform.Inverse(transf_around_point);
		  
		 int start_x = -wind/2 + x_offset;
		 int start_y = -wind/2 + y_offset;
	     int  x = start_x ;
		 int  y = start_y ;
		 int startInd = patchStartIndex;
		 double min_value = INT_MAX;
		 double max_value = INT_MIN;

		 for (int i=0; i < wind;i++,y++,startInd += wind*num_pnts)
		 {
			x = start_x;
			index = startInd;
			for (int j=0; j < wind; j++,x++,index++)
			{
				x2 = inv[0]*x+inv[1]*y+inv[2]*z;
				y2 = inv[3]*x+inv[4]*y+inv[5]*z;
				z2 = inv[6]*x+inv[7]*y+inv[8]*z;
				x2 = x2/z2;
				y2 = y2/z2;

				y0 = (int)y2;
				x0 = (int)x2;
				x1 = x0 + 1;	// bottom right x1
				y1 = y0 + 1;	// bottom right y1
				if (x0 < 0 || x1 < 0 || y0 < 0 || y1 < 0 ||
			    y0 >= sHeight || y1 >= sHeight || x0 >= sWidth || x1 >= sWidth)
				{
					buff[index] = rand()%256; 
					min_value = min(min_value,buff[index]);
					max_value = max(max_value,buff[index]);
					continue;
				}

				t = (double)(x2 - x0);
				u = (double)(y2 - y0);
				u_ = 1.0f - u;
				t_ = 1.0f - t;
				tu = t * u;
				tu_ = t * u_;
				t_u = t_ * u;
				t_u_ = t_ * u_;
				Y0X0 = y0 * sWidth + x0;
				Y0X1 = y0 * sWidth + x1 ;
				Y1X0 = y1 * sWidth + x0 ;
				Y1X1 = y1 * sWidth + x1;
			
				
				temp = (
						t_u_ * greySource[Y0X0 ] +
						tu_ * greySource[Y0X1 ] +
						t_u * greySource[Y1X0 ]  +
						tu * greySource[Y1X1 ] + 0.5);
				buff[index] = temp;
				min_value = min(min_value,buff[index]);
				max_value = max(max_value,buff[index]);

			}

		
        }

		 delete[] inv;
     }		
	 }
 
	QueryPerformanceCounter( &lPerformanceCount52);
	sprintf_s(sMess," Create Patches time=%lf\n", 
	(lPerformanceCount52.QuadPart - lPerformanceCount51.QuadPart) / (double)lFrequency5.QuadPart);
	fprintf(stdout,"%s",sMess);
	fflush(stdout);
	if (GetApp()->logger)
		fprintf(GetApp()->logger,"%s",sMess);


	//Initially all SEARCH TREES are built from all the patches.
	// p containts indices of the patches around feature points.
	// In every stage node of the tree will split indices to 3 sets:
	// p = p_left union p_middle union p_right.
	vector<int> p;
	for (int i=0; i < num_pnts*num_transforms;i++)
	{
		p.push_back(i);
	}
	matcher.SetTrainDone(true);

	// Build Search Trees.
	for (int i=0; i < TREES_COUNTER; i++)
	{
		matcher.GetTrees(i).BuildTree(p,buff,num_pnts,num_transforms);
	}
	//Time measurements
	QueryPerformanceCounter( &lPerformanceCount51);
	sprintf_s(sMess," Trees Build time=%lf\n", 
	(lPerformanceCount51.QuadPart - lPerformanceCount52.QuadPart) / (double)lFrequency5.QuadPart);
	fprintf(stdout,"%s",sMess);
	if (GetApp()->logger)
		fprintf(GetApp()->logger,"%s",sMess);
	fflush(stdout);

	// Improve Search Trees by adding transformations (i.e patches) 
	// with known classifiers.
	// We create patch around feature point, pass this patch through the Search Tree
	// and add to leaf's hitogram one to the corresponding classifier.
	int num_transfs_to_add = 500;
	byte patch[WIND*WIND];

	int visIndStart = 0;
	int visIndStart1 = 0;
	int visIndex;
	for (int i=0; i < num_transfs_to_add; i++)
	{
		if (i%50 == 1)
		{
			fprintf(stdout," Krutizna! %d \n",i);
			fflush(stdout);
		}
		//todo 1) create random transf
		// 2)  create small patch around point 
		// 3) add to all existing trees 
		m_ProjTransform.CreateRandomTrasformation(transf,lambda_min,lambda_max,theta_max,phi_max);
		double transf_around_point[9];
		double translation[9];  
		double second_translation[9];
		translation[0] = 1; translation[1] =0; translation[2] = 0;
		translation[3] = 0; translation[4] =1; translation[5] = 0;
		translation[6] = 0; translation[7] =0; translation[8] = 1;
		for (int ii=0; ii < 9; ii++)
		{
			second_translation[ii] = translation[ii];
		}
		// Motivation : point (0,0) will correspond to 
		// (x,y) + (-WIND/2,-WIND/2) after the transformation
		second_translation[2] = -WIND/2 +rand()%4-4; //check sign +- WIND/2!!
		second_translation[5] = -WIND/2 +rand()%4-4;
		for (int j=0; j < num_pnts; j++)
		{
			translation[2] = -x_points[j];
			translation[5] = -y_points[j];
			double* A= m_ProjTransform.Mult(transf,translation);
		//	double* B = m_ProjTransform.Mult(second_translation,A);
			for (int k=0; k < 9; k++)
			 {
				//transf_around_point[k] = B[k];
				transf_around_point[k] = A[k];
			}
			delete[] A;
			//delete[] B;

			double* inv_transf = m_ProjTransform.Inverse(transf_around_point);
			int range = CreatePatch(greySource,sWidth,sHeight,WIND,WIND,inv_transf,patch); 

			for (int k=0; k < TREES_COUNTER; k++)
			{
				matcher.GetTrees(k).AddPoint(patch,WIND,j);	

			}
		}
	}


	//Normalization of the histograms. 
	// Motivation : all histograms must be balanced - they have to contain probabilities,
	// This means that sum of all entries must be 1. 
	// But in order to improve speed we make these entries to be integers and normalize their sum to be
	// some constant value - for example 1024.
	for (int i=0; i < TREES_COUNTER; i++)
	{
		matcher.GetTrees(i).CreateNonZeroIndicesAndNormalize();
	} 
			
	CString sname;
	CString dir_name = GetApp()->trainPath;
	sname = dir_name + _T("\\train_points.dat");
	char path_buff[200];
	for (int i=0; i < sname.GetLength(); i++)
	{
		path_buff[i] = sname.GetBuffer()[i];
	}

	//Write information on train points to file
	FILE* file = fopen(path_buff,"w");
	if (file==NULL)
		fprintf(stdout,"OBLOM! \n");
	else
	{
		fprintf(file, "%d %d\n",sWidth,sHeight);
		matcher.SetTrainWidth(sWidth);
		matcher.SetTrainHeight(sHeight);
		fprintf(file,"%d\n",num_pnts);
		fprintf(file,"%d\n",TREES_COUNTER);
		for (int i=0; i < num_pnts; i++)
		{
			fprintf(file,"%d %d \n",x_points[i],y_points[i]);
			matcher.AddTrainPoint(x_points[i],y_points[i]);
		}
		fclose(file);
	}

	
	for (int i=0; i < TREES_COUNTER; i++)
	{
		char sname2[200];
		sprintf_s(sname2,"\\tree%d.dat",i);
		 	
		sname = dir_name;
		memset(path_buff,0,200);
		for (int j=0; j < dir_name.GetLength(); j++)
		{
			path_buff[j] = dir_name.GetBuffer()[j];
		}

		strcat(path_buff,sname2);
		matcher.GetTrees(i).Write(path_buff);
	}


	//For debug only - Draw  -|- figure around every feature points.
	// This is done by making RED band be 255 and other bands 0.
	for (int i=0; i < num_pnts; i++)
	{
		int length = 10;
		for (int j=-length; j < length; j++)
		{
			int x = min(sWidth-1,max(x_points[i] + j,0));
			int ind = x*bpp+y_points[i]*sStride;
			pSource[ind] = 0;
			pSource[ind+1] = 0;
			pSource[ind+2] = 255;
		}
		for (int j=-length; j < length; j++)
		{
			int y = min(sHeight-1,max(y_points[i] + j,0));
			int ind = x_points[i]*bpp+y*sStride;
			pSource[ind] = 0;
			pSource[ind+1] = 0;
			pSource[ind+2] = 255;
		}
	}
	
	//Time measurements
	QueryPerformanceCounter( &lPerformanceCount52);
	sprintf_s(sMess," Tree Save time=%lf\n", 
	(lPerformanceCount52.QuadPart - lPerformanceCount51.QuadPart) / (double)lFrequency5.QuadPart);
	fprintf(stdout,"%s",sMess);
	if (GetApp()->logger)
		fprintf(GetApp()->logger,"%s",sMess);
	fflush(stdout);
	delete[] greySource;

	return buff;
}


// Create patch around feature point. pSource - parameter of the whole picture.
// transf - transformation, the result will be written in the patch.
int CFastStable::CreatePatch(byte* pSource,int sWidth,int sHeight,int dWidth,int dHeight,double transf[],byte patch[])
{
	double x,y,z;
	double min_value = INT_MAX;
	double max_value = INT_MIN;
	int x0, x1, y0, y1;
    double t, u, t_, u_, tu, tu_, t_u, t_u_;
    long Y0X0, Y0X1, Y1X0, Y1X1;

	int index=0;
	for (int i=-WIND/2; i < dHeight-WIND/2; i++)
	{
		for (int j=-WIND/2; j < dWidth-WIND/2; j++,index++)
		{
			x = transf[0]*j+transf[1]*i+transf[2];
		    y = transf[3]*j+transf[4]*i+transf[5];
		    z = transf[6]*j+transf[7]*i+transf[8];
			x = x/z;
			y = y/z;

			y0 = (int)y;
			x0 = (int)x;
			x1 = x0 + 1;	// bottom right x1
			y1 = y0 + 1;	// bottom right y1
				if (x0 < 0 || x1 < 0 || y0 < 0 || y1 < 0 ||
			    y0 >= sHeight || y1 >= sHeight || x0 >= sWidth || x1 >= sWidth)
				{
					patch[index] = rand()%256; 
					min_value = min(min_value,patch[index]);
					max_value = max(max_value,patch[index]);
					continue;
				}

				t = (double)(x - x0);
				u = (double)(y - y0);
				u_ = 1.0f - u;
				t_ = 1.0f - t;
				tu = t * u;
				tu_ = t * u_;
				t_u = t_ * u;
				t_u_ = t_ * u_;
				Y0X0 = y0 * sWidth + x0;
				Y0X1 = y0 * sWidth + x1 ;
				Y1X0 = y1 * sWidth + x0 ;
				Y1X1 = y1 * sWidth + x1;
			
				
				patch[index] = (byte) (
						t_u_ * pSource[Y0X0 ] +
						tu_ * pSource[Y0X1 ] +
						t_u * pSource[Y1X0 ]  +
						tu * pSource[Y1X1 ] + 0.5);
				min_value = min(min_value,patch[index]);
				max_value = max(max_value,patch[index]);
		}
	}
	return max_value-min_value;
}


// Create patch around feature point (x,y).
double* CFastStable::CreatePatch(byte* buff,int sWidth,int sHeight,int x,int y)
{
	int wind = WIND;
	int x_coord = (int)(x - wind/2);
	int y_coord = (int)(y - wind/2);

	int index_y = 0;
	int dest_ind = y_coord*sWidth + x_coord; 
	double min_value = INT_MAX;
	double max_value = INT_MIN;


	for (int j=0; j<wind; j++,index_y += wind,dest_ind += sWidth)
	{
		int index = index_y;
		int indexD = dest_ind;
		for (int k=0; k <wind; k++,index++,indexD++)
		{
			if (x -wind/2+ k < 0 || y -wind/2 + j < 0 || x -wind/2+ k >= sWidth || y -wind/2 + j >= sHeight)
				patch[index] = rand()%256;	
			else
				patch[index] = buff[indexD];

			min_value = min(min_value,patch[index]);
			max_value = max(max_value,patch[index]);
		}
	}

	index_y = 0;
	dest_ind = y_coord*sWidth + x_coord;
	double range = max_value-min_value;
	if (range!=0)
	for (int j=0;j<wind; j++,index_y += wind,dest_ind += sWidth)
	{
		for (int k=0; k <wind; k++)
		{
			patch[index_y+k] = (patch[index_y+k] - min_value)/range; 
		}
	}
	return patch;
}

// Returns (max intensity - min intensity) in the picture pSource.
int CFastStable::FindRange(byte* pSource,int startInd,int sWidth,int sStride)
{
	int min_value = INT_MAX;
	int max_value = INT_MIN;

	int index = startInd;
	int bpp = sStride/sWidth;
	for (int i=0; i < WIND; i++,index+=sStride)
	{
		int ind2 = index;
		for (int j=0; j < WIND; j++,ind2+=bpp)
		{
			min_value = min(min_value,pSource[ind2]);
			max_value = max(max_value,pSource[ind2]);
		}
	}
	return max_value - min_value;
}