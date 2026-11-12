#include "StdAfx.h"
#include "Matcher.h"
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

#define MAX_FEATURE_POINTS 300

bool FilterByDistance(double x,double y,const vector<int>& passed_inds,xy* arr,double dist_thresh);

bool compareQuality(const ClassifierHistogram& p1,const ClassifierHistogram& p2)
{ 
	return (p1.quality>p2.quality);
}

Matcher::Matcher(void)
{
	trees_num = 0;
	trainDone = false;
	int quant = 1024;
	m_thresh = 0.05*TREES_COUNTER*quant;
	memset(time,0,sizeof(double)*e_total);
	m_ransac_thresh = 0.1;
}


void Matcher::SetTrainDone(bool flag)
{
	if (flag == false && trainDone == true)
	{
		train_points.clear();
		for (int i=0; i < trees_num; i++)
		{
			searchTree[i].clear();
		}
		searchTree = vector<SearchTree>();
	}
	trainDone = flag;
}

void Matcher::SetThreshold(double thr)
{
	int quant = 1024;
	//m_thresh = thr*TREES_COUNTER*quant;	
	m_thresh = thr*trees_num*quant;
}

void Matcher::SetRansacThreshold(double thresh)
{
	m_ransac_thresh = thresh;
}

bool Matcher::FilterByDistance(double x,double y,const vector<int>& passed_inds,xy* arr,double dist_thresh)
{
	for (int i=passed_inds.size()-1; i >=0; i--)
	{
		int ind = passed_inds[i];
		
		if ((abs(x-arr[ind].x) + abs(y-arr[ind].y)) <= dist_thresh)
			return false;
		if (abs(y-arr[ind].y) > dist_thresh)
			return true;
	}
	return true;

}

void Matcher::SetRansacIterations(int iters_num)
{
	ransac.SetIterations(iters_num);
}

void Matcher::TRACE_TIME_TO_FILE(FILE* file)
{
	char sMess[260];
	sprintf_s(sMess," Tracking time: filter_radius = %lf \n",
		time[e_filter_radius]);
	fprintf(file,"%s",sMess);
	sprintf_s(sMess," Tracking time: e_distance = %lf \n",
		time[e_distance]);
	fprintf(file,"%s",sMess);
	sprintf_s(sMess," Tracking time: e_search_tree = %lf \n",
		time[e_search_tree]);
	fprintf(file,"%s",sMess);
	sprintf_s(sMess," Tracking time: e_find_max = %lf \n",
		time[e_find_max]);
	fprintf(file,"%s",sMess);
	sprintf_s(sMess," Tracking time: e_vectors = %lf \n",
		time[e_vectors]);
	fprintf(file,"%s",sMess);
}


bool Matcher::LoadTrainData()
{
	CString sname;
	CString dir_name = GetApp()->trainPath;
 	sname = dir_name + _T("\\train_points.dat");
	int num_pnts;
	char buff[200]={0};
    fprintf(stdout,"1234\n");
	for (int i=0; i < sname.GetLength(); i++)
	{
		buff[i] = (char)sname.GetBuffer()[i];
	}
	FILE* file;
	if (fopen_s(&file,buff,"r") != 0)
	{
		fprintf(stdout,"OBLOM! \n");
		return false;
	}
	else
	{
		int x_point,y_point,z_point=1;
		fscanf(file, "%d %d",&trainWidth,&trainHeight);
		fscanf(file,"%d",&num_pnts);
		fscanf(file,"%d",&trees_num);
		for (int i=0; i < num_pnts; i++)
		{
			fscanf(file,"%d %d",&x_point,&y_point);
			train_points.push_back(Point3D(x_point,y_point,z_point));
		}
			
		fclose(file);
  	}

	//read trees info
	searchTree = vector<SearchTree>(trees_num,SearchTree());
 	for (int j=0; j < trees_num; j++)
	{
		char sname[200];
		CString dir_name = GetApp()->trainPath;
		sprintf_s(sname,"\\tree%d.dat",j);
		char buff[200]={0};
		for (int i=0; i < dir_name.GetLength(); i++)
		{
			buff[i] = (char)dir_name.GetBuffer()[i];
		}
		strcat(buff,sname);
		searchTree[j].Read(buff); 	
	}
	//TODO: check possible failure of this operation
	trainDone = true;
   fprintf(stdout,"3456\n");
	fflush(stdout);
	return true;
}
/*
* The main procedure of Tracking.
* Returns corners of the marker on the tracking image pSource.
*/
void Matcher::DistributePoints(byte* pSource,int sWidth, int sHeight,int sStride,int* x_corners,int* y_corners)
{
	// Clear previous matchings
	train_match.clear();
	track_match.clear();

	//Time measurements
	LARGE_INTEGER lFrequency5;
	QueryPerformanceFrequency( &lFrequency5);
	LARGE_INTEGER lPerformanceCount51, lPerformanceCount52;
	QueryPerformanceCounter(&lPerformanceCount51);		
	
	LARGE_INTEGER lPerformanceCount1, lPerformanceCount2;

	char sMess[260];
	
	int bpp = sStride/sWidth;
	

	//if track is done first time read all 
	// preprocessing information from files.
	if (!trainDone)
		LoadTrainData();
	

	// 1) Get grey level image from the color image
	// 2) Apply FAST to obtain feature points.
	// 3) Apply filters - by distanse and contrast, to the feature points.
	// 4) Find matching by using Search Trees. 
	// 5) Pass only points which get good grades from the trees.
	// 6) Apply ransac to find the exact homography.
	CFastStable fastStable;
	//@Step 1
	byte* greyLevelBuff = fastStable.CreateGreyBuff(pSource, sWidth, sHeight, sStride);
	//@Step 2
	int numcorners = 0;
	int threshold = 15; //check! change to some automatic value!
	xy* arr = fast9_detect_nonmax(greyLevelBuff, sWidth, sHeight, sWidth, threshold, &numcorners);
	
	//Debug
	QueryPerformanceCounter( &lPerformanceCount52);
	sprintf_s(sMess," Tracking time of fast=%lf\n", 
	(lPerformanceCount52.QuadPart - lPerformanceCount51.QuadPart) / (double)lFrequency5.QuadPart);
	fprintf(stdout,"%s",sMess);
	
	if (GetApp()->logger)  //TODO: some other file
		fprintf(GetApp()->logger,"%s",sMess);


	vector<CPoint> points;
	int distance = 5;
	
	int points_entrance[MAX_FEATURE_POINTS]={0};
	vector<int> matchings;
	fprintf(stdout,"Before: %d \n",numcorners);
		
	vector<int> train_ind;
	vector<Point3D> temp_train_match;
	vector<Point3D> temp_track_match;
	
	vector<int> passed_inds;
	double dist_thresh = 20;

	QueryPerformanceCounter( &lPerformanceCount51);
	int pass_thresh = (int)m_thresh;
	double ratio = 1.2;
	

	memset(time,0,sizeof(double)*e_total); //TIME MEASUREMENTS
	//@Steps 3,4,5
	for (int i=0; i < numcorners; i++)
	{
		QueryPerformanceCounter(&lPerformanceCount1);

		if (arr[i].x <= WIND/2 || arr[i].x >= sWidth - WIND/2 || 
			arr[i].y <= WIND/2 || arr[i].y >= sHeight - WIND/2)
			continue;
		int pos_x_change[4] = {0,6,8,6};
		int pos_y_change[4] = {8,6,0,6};
		int x = arr[i].x;
		int y = arr[i].y;
		//filter by neighborhood edges
		int index = y*sWidth + x ,index1,index2;
		int x1,y1,x2,y2;
		int tau = 10;  //as in article
		bool flag=true;
		for (int k=0;k < 4; k++)
		{
			x1 = x+pos_x_change[k];
			x2 = x-pos_x_change[k];
			y1 = y+pos_y_change[k];
			y2 = y-pos_y_change[k];
			index1 = y1*sWidth + x1;
			index2 = y2*sWidth + x2;
					
			if (abs(greyLevelBuff[index] - greyLevelBuff[index1]) <= tau &&
				abs(greyLevelBuff[index] - greyLevelBuff[index2]) <= tau)
				flag = false;
		}
	
		QueryPerformanceCounter(&lPerformanceCount2);
		time[e_filter_radius] += 
		(lPerformanceCount2.QuadPart - lPerformanceCount1.QuadPart) / (double)lFrequency5.QuadPart;
		if (flag == false)
			continue;
		if (FilterByDistance(arr[i].x,arr[i].y,passed_inds,arr,dist_thresh) == false)
			continue;
		
		QueryPerformanceCounter(&lPerformanceCount1);
		time[e_distance] += 
		(lPerformanceCount1.QuadPart - lPerformanceCount2.QuadPart) / (double)lFrequency5.QuadPart;
		
		
		//int startInd = arr[i].x*bpp + sStride*arr[i].y; //check!
		int startInd = arr[i].x-WIND/2 + sWidth*(arr[i].y-WIND/2);
		//int range = fastStable.FindRange(greyLevelBuff,startInd,sWidth,sWidth);
		int range = 1;
				
		unsigned int patch_index[MAX_FEATURE_POINTS]={0};
		
		int max_classifier = -1;
		double max_quality = -1;
		double second_max = -1;


		for (int j=0; j < trees_num/2; j++) 
		{
			searchTree[j].Find(greyLevelBuff, sWidth,sWidth,startInd,patch_index,range);
		}
		QueryPerformanceCounter(&lPerformanceCount2);
		time[e_search_tree] += 
		(lPerformanceCount2.QuadPart - lPerformanceCount1.QuadPart) / (double)lFrequency5.QuadPart;
	

		for (int j=0; j < MAX_FEATURE_POINTS; j++)
		{
			if (patch_index[j] !=0)
			{
				if (max_quality < patch_index[j])
				{
					second_max = max_quality;
					max_quality = patch_index[j];
					max_classifier = j;
				}
				else
				{
					if (second_max < patch_index[j])
						second_max = patch_index[j];
				}
			}
		}
		QueryPerformanceCounter(&lPerformanceCount1);
		time[e_find_max] += 
		(lPerformanceCount1.QuadPart - lPerformanceCount2.QuadPart) / (double)lFrequency5.QuadPart;
		if (max_quality < pass_thresh/3)
			continue;

		if (max_quality >= pass_thresh/3)
		{
			for (int j=trees_num/2+1; j < trees_num; j++) 
			{
				searchTree[j].Find(greyLevelBuff, sWidth,sWidth,startInd,patch_index,range);
			}
		}
		QueryPerformanceCounter(&lPerformanceCount2);
		time[e_search_tree] += 
		(lPerformanceCount2.QuadPart - lPerformanceCount1.QuadPart) / (double)lFrequency5.QuadPart;

		for (int j=0; j < MAX_FEATURE_POINTS; j++)  //TODO
		{
			if (patch_index[j] !=0)
			{
				if (max_quality < patch_index[j])
				{
					second_max = max_quality;
					max_quality = patch_index[j];
					max_classifier = j;
				}
				else
				{
					if (second_max < patch_index[j])
						second_max = patch_index[j];
				}
			}
		}
		QueryPerformanceCounter(&lPerformanceCount1);
		time[e_find_max] += 
		(lPerformanceCount1.QuadPart - lPerformanceCount2.QuadPart) / (double)lFrequency5.QuadPart;
	

		int quant = 1024; //TODO: check! 	
		if (max_quality >= ratio * second_max && max_quality >= pass_thresh)
		{
			train_ind.push_back(max_classifier);
			temp_train_match.push_back(train_points[max_classifier]);
			temp_track_match.push_back(Point3D(arr[i].x,arr[i].y,1));
			points_entrance[max_classifier]++;
			passed_inds.push_back(i);
			//fprintf(stdout,"(%d ,%d ) ---> (%d, %d) , qual = %d \n" , (int)train_points[max_classifier].x, (int)train_points[max_classifier].y,
			//	arr[i].x, arr[i].y, (int)max_quality);
			//fflush(stdout);
		
		}
		matchings.push_back(max_classifier);
		QueryPerformanceCounter(&lPerformanceCount2);
		time[e_vectors] += (lPerformanceCount2.QuadPart - lPerformanceCount1.QuadPart) / (double)lFrequency5.QuadPart;
	}
	

	//Debug
	QueryPerformanceCounter( &lPerformanceCount51);
	sprintf_s(sMess," Tracking time: trees search =%lf\n", 
	(lPerformanceCount51.QuadPart - lPerformanceCount52.QuadPart) / (double)lFrequency5.QuadPart);
	fprintf(stdout,"%s",sMess);
	if (GetApp()->logger)
		fprintf(GetApp()->logger,"%s",sMess);
	fflush(stdout);
	
	if (GetApp()->logger)
		TRACE_TIME_TO_FILE(GetApp()->logger);
	
	//double threshold_ransac = 10; //IMPORTANT - threshold for RANSAC
	vector<int> inliers;
	int max_entrance_thresh = 0;
	bool lookup_table[MAX_FEATURE_POINTS];
	for (int i=0; i < MAX_FEATURE_POINTS; i++)
	{
		lookup_table[i] = true;
	}
	for (int k=0; k <2;k++)
	{
		max_entrance_thresh = 0;
		for (int i=0; i < MAX_FEATURE_POINTS; i++)
		{
			if (points_entrance[i] > max_entrance_thresh && lookup_table[i])
				max_entrance_thresh = points_entrance[i];
		}

		for (int i=0; i < MAX_FEATURE_POINTS; i++)
		{
			if (points_entrance[i] >= max(2,max_entrance_thresh*0.8) )
			{
				lookup_table[i] = false;
			}
		}
	}

	// This is possible enhancement of RANSAC
	int point_index = 0; //point index in temp_train_match
	vector<vector<int> > temp_different_inds_vec(400);
	vector<vector<int> > different_inds_vec;

	for (size_t i=0; i < train_ind.size(); i++)
	{
		if (lookup_table[train_ind[i]])
		{
			train_match.push_back(temp_train_match[i]);
			track_match.push_back(temp_track_match[i]);
			temp_different_inds_vec[train_ind[i]].push_back(point_index);
			point_index++;
		}
	}

	for (size_t i=0; i < temp_different_inds_vec.size();i++)
	{
		if (temp_different_inds_vec[i].size() != 0)
		{
			different_inds_vec.push_back(temp_different_inds_vec[i]);
		}
	}
	ransac.SetDifferentIndsVector(different_inds_vec);


	if (train_match.size() < 5 || different_inds_vec.size() <=4 )
	{
		fprintf(stdout,"Not enough points :%d big OBLOM \n",train_match.size());
		fprintf(stdout,"Not enough points :Different points in train %d big OBLOM \n",different_inds_vec.size());		
		fflush(stdout);
		return;
	}

	//@Step 6: Ransac.
	double* H=ransac.ransacfithomography(train_match,track_match ,m_ransac_thresh, inliers);
	QueryPerformanceCounter( &lPerformanceCount52);
	sprintf_s(sMess," Tracking time: ransac=%lf\n", 
	(lPerformanceCount52.QuadPart - lPerformanceCount51.QuadPart) / (double)lFrequency5.QuadPart);
	fprintf(stdout,"%s",sMess);
	if (GetApp()->logger)
		fprintf(GetApp()->logger,"%s",sMess);
	fflush(stdout);


	if (GetApp()->ransac_logger)
	{
		fprintf(GetApp()->ransac_logger,"train_points: \n");
		for (size_t i=0; i < train_match.size(); i++)
		{
			fprintf(GetApp()->ransac_logger,"%7.4f %7.4f \n", train_match[i].x,train_match[i].y); 
		}
		fprintf(GetApp()->ransac_logger,"track_points: \n");
		for (size_t i=0; i < train_match.size(); i++)
		{
			fprintf(GetApp()->ransac_logger,"%7.4f %7.4f \n", track_match[i].x,track_match[i].y); 
		}
		fprintf(GetApp()->ransac_logger,"H: \n");
		for (int i=0; i < 3; i++)
		{
			for (int j=0; j<3; j++)
			{
				fprintf(GetApp()->ransac_logger, "%6.4f ",H[3*i+j]);
			}
			fprintf(GetApp()->ransac_logger,"\n");
		}
		fprintf(GetApp()->ransac_logger,"inliers: \n");
		for (size_t i=0; i < inliers.size(); i++)
		{
			fprintf(GetApp()->ransac_logger,"%d ",inliers[i]);
		}
		fprintf(GetApp()->ransac_logger,"\n");
	}

	//Draw green --|-- in the feature points
	for (size_t i=0; i < track_match.size(); i++)
	{
		Point3D p = track_match[i];
		int length = 5;
		int j= -length;
		int x = min(sWidth-1,max((int)(p.x),0));
			int y = min(sHeight-1,max((int)p.y ,0));
			for ( j=-length; j < length; j++)
			{
				x = min(sWidth-1,max((int)(p.x)+j,0));
				int ind = x*bpp+(int)y*sStride;
				pSource[ind] = 0;
				pSource[ind+1] = 255;
				pSource[ind+2] = 0;
			}
			x = min(sWidth-1,max((int)(p.x),0));
			for ( j=-length; j < length; j++)
			{
				y = min(sHeight-1,max((int)p.y + j,0));
				int ind = (int)x*bpp+y*sStride;
				pSource[ind] = 0;
				pSource[ind+1] = 255;
				pSource[ind+2] = 0;
			}
	}
	vector<Point3D> train_corners;
	train_corners.push_back(Point3D(0,0,1));
	train_corners.push_back(Point3D(trainWidth,0,1));
	train_corners.push_back(Point3D(trainWidth,trainHeight,1));
	train_corners.push_back(Point3D(0,trainHeight,1));
	
	
	res_corners.clear();
	for (int i=0; i < 4; i++)
	{
		Point3D p = ransac.multiply(H,train_corners[i]);
		if (p.z !=0)
		{
			p.x = p.x/p.z;
			p.y = p.y/p.z;
			p.z = 1;
		}
		res_corners.push_back(p);
		
		
			int length = 5;
			int j= -length;
			int x = min(sWidth-1,max((int)(p.x),0));
			int y = min(sHeight-1,max((int)p.y ,0));
			for ( j=-length; j < length; j++)
			{
				x = min(sWidth-1,max((int)(p.x)+j,0));
				int ind = x*bpp+(int)y*sStride;
				pSource[ind] = 0;
				pSource[ind+1] = 0;
				pSource[ind+2] = 255;
			}
			x = min(sWidth-1,max((int)(p.x),0));
			for ( j=-length; j < length; j++)
			{
				y = min(sHeight-1,max((int)p.y + j,0));
				int ind = (int)x*bpp+y*sStride;
				pSource[ind] = 0;
				pSource[ind+1] = 0;
				pSource[ind+2] = 255;
			}
	}


	if (CheckConvex(res_corners[0],res_corners[1],res_corners[2],res_corners[3]) &&
			CheckConvex(res_corners[1],res_corners[2],res_corners[3],res_corners[0]) )
	{
		// Visualisation - draw red line of width 5 on the image
		double area = GetArea(res_corners[0],res_corners[1],res_corners[2],res_corners[3]);
		double thresh = 10000;
		if (area > thresh)
		{
			for (int i=0; i < 4; i++)
				DrawThickLine( pSource,sWidth, sHeight,sStride,res_corners[i],res_corners[(i+1)%4],5);
		}
	}

	delete[] greyLevelBuff;
	delete[] arr;
	
}

double Matcher::GetArea(const Point3D& p1,const Point3D& p2,const Point3D& p3,const Point3D& p4)
{
	double xa = p1.x, xb=p2.x, xc=p3.x, xd=p4.x;
	double ya = p1.y, yb=p2.y, yc=p3.y, yd=p4.y;
	double area1 = 0.5*fabs((xc-xa)*(yb-ya) - (xb-xa)*(yc-ya)); //triange p1 --> p2 --> p3
	double area2 = 0.5*fabs((xd-xa)*(yd-yc) - (xc-xa)*(yd-ya)); //triange p1 --> p3 --> p4
	return (area1+area2);
}

bool Matcher::CheckConvex(const Point3D& p1,const Point3D& p2,const Point3D& p3,const Point3D& p4)
{
	double xa = p1.x, xb=p2.x, xc=p3.x, xd=p4.x;
	double ya = p1.y, yb=p2.y, yc=p3.y, yd=p4.y;
	double det = (xa-xb)*(yd-yc) - (xd-xc)*(ya-yb);
	double delta = (xd-xb)*(yd-yc) - (xd-xc)*(yd-yb);
	Line line(CPoint((int)p1.x,(int)p1.y),CPoint((int)p3.x,(int)p3.y));
	double a = line.a , b =line.b , c = line.c;
	if ((a*xb + b*yb + c)*(a*xd + b*yd + c) > 0)
		return false;
	if (det == 0)
		return true;
	if (delta/det >= 0 && delta/det <= 1)
		return false;
	else
		return true;


}

void Matcher::DrawLine(byte* pVis,int vWidth, int vHeight,int vStride,const Point3D& p1,const Point3D& p2)
{
	double slope;

	int x,y;
	double x1= p1.x , x2 = p2.x; 
	double y1 = p1.y, y2=p2.y;
	double b;
	int index;
	int bpp = vStride / vWidth;
	if (fabs(y2 - y1) > fabs(x2-x1))
	{
		slope = (double)(x2-x1) / (y2-y1);
		b = x1 - y1*slope;
		for (y=(int)max(min(y1,y2),0); y < (int)min(max(y1,y2),vHeight-1); y++)
		{
			x = (int)(y*slope + b+0.5); 
			if (x < 0 || x> vWidth - 1)
				continue;
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
		for (x=(int)max(0,min(x1,x2)); x < (int)min(max(x1,x2),vWidth-1); x++)
		{
			y = (int)(x*slope + b+0.5);
			if (y < 0 || y> vHeight - 1)
				continue;
			index = vStride * y + x*bpp;
			pVis[index] = 0;
			pVis[index + 1] = 0;
			pVis[index + 2] = 255;
		}
	}
}


void Matcher::DrawThickLine(byte* pVis,int vWidth, int vHeight,int vStride,const Point3D& p1,const Point3D& p2,int thickness)
{
	double slope;

	int x,y;
	double x1= p1.x , x2 = p2.x; 
	double y1 = p1.y, y2=p2.y;
	double b;
	int index;
	int bpp = vStride / vWidth;
	if (fabs(y2 - y1) > fabs(x2-x1))
	{
		slope = (double)(x2-x1) / (y2-y1);
		b = x1 - y1*slope;
		for (y=(int)max(min(y1,y2),0); y < (int)min(max(y1,y2),vHeight-1); y++)
		{
			x = (int)(y*slope + b+0.5); 
			if (x < 0 || x> vWidth - thickness)
				continue;
			index = vStride * y + x*bpp;
			//if (bpp == 3)
			for (int j=0; j <thickness; j++)
			{
				pVis[index+j*bpp] = 0;
				pVis[index +j*bpp+1] = 0;
				pVis[index +j*bpp+2] = 255;
			}
		}
	}
	else
	{
		slope = (double)(y2-y1) / (x2-x1);
		b = y1-x1*slope;
		for (x=(int)max(0,min(x1,x2)); x < (int)min(max(x1,x2),vWidth-1); x++)
		{
			y = (int)(x*slope + b+0.5);
			if (y < 0 || y> vHeight - thickness)
				continue;
			index = vStride * y + x*bpp;
			for (int j=0; j <thickness; j++)
			{
				pVis[index+j*vStride] = 0;
				pVis[index +j*vStride+1] = 0;
				pVis[index +j*vStride+2] = 255;
			}
		}
	}
}


void Matcher::DrawRect(byte* pSource, int sWidth, int sHeight, int sStride)
{
	if (res_corners.size() < 4)
		return;

	if (!CheckConvex(res_corners[0],res_corners[1],res_corners[2],res_corners[3]) ||
			!CheckConvex(res_corners[0],res_corners[3],res_corners[1],res_corners[2]) )
		return;
	
	

	for (int i=0; i < 4; i++)
	{
		if (i==3)
		{
			DrawThickLine( pSource,sWidth, sHeight,sStride,res_corners[i],res_corners[0],5);
		}
		if (i > 0)
		{
			DrawThickLine(pSource,sWidth,  sHeight,sStride,res_corners[i-1],res_corners[i],5);
		}
	}
}