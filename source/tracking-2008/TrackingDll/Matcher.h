#pragma once


#define TREES_COUNTER 100  // Parameter - num of Search Trees used in the algorithm.

// This enum is used to time measurements
enum {
	e_filter_radius,
	e_distance,
	e_search_tree,
	e_find_max,
	e_vectors,
	e_total
};

class ClassifierHistogram
{
public:
	int classifier;
	int quality;
	ClassifierHistogram()
	{classifier = quality = 0;}
	ClassifierHistogram(int _c,int _q)
	{
		classifier = _c;
		quality =_q;
	}
};


/*
* The main class - performs all parts of algorithm , train and track.
*/
class Matcher
{
private:
	int trees_num;  // size of vector searchTree.
	vector<SearchTree> searchTree;  // Search Trees - used to classify feature points on Track Object to
	                                       // the feature points on Marker.
	vector<Point3D> train_points;  //Feature points in the MARKER.
	bool trainDone;  // true if train is done previously
	                 // if trainDone == false then read info about Search Trees
	                // and stable points from file
	Ransac ransac;  // Performs Ransac algorithm to find homography
	int trainWidth;  // Width of marker picture
	int trainHeight;  // Height of marker picture
	double m_thresh;  // Threshold to filter feature points on Track object.
	double m_ransac_thresh;
public:
	vector<Point3D> train_match; //Matched points on Marker
	vector<Point3D> track_match;  // Matched points on Track Object.
	// The vectors train_match,track_match are of the same size and 
	// train_match[i] <---> track_match[i]

	Matcher(void); 
	void SetThreshold(double percent);
	void SetRansacThreshold(double thresh);
	void DistributePoints(byte* pSource,int sWidth, int sHeight,int sStride,int* x_corners,int* y_corners);
	void SetTrainDone(bool flag);
	void SetTrainWidth(int x){trainWidth = x;}
	void SetTrainHeight(int x){trainHeight = x;}
	void SetTreesNum(int x){trees_num = x;}
	void AddTrainPoint(int x,int y)  //Add train point (x,y) to set of feature points in marker. 
	{
		train_points.push_back(Point3D(x,y,1));
	}
	SearchTree& GetTrees(int i)  // Get SearchTree number i 
	{ 
		return searchTree[i];
	}

	void DrawLine(byte* pSource,int sWidth, int sHeight,int sStride,const Point3D& p1,const Point3D& p2);
	void DrawThickLine(byte* pVis,int vWidth, int vHeight,int vStride,const Point3D& p1,const Point3D& p2,int thickness);
	bool LoadTrainData();

	inline bool FilterByDistance(double x,double y,const vector<int>& passed_inds,xy* arr,double dist_thresh);

	void SetRansacIterations(int iters_num);

	void TRACE_TIME_TO_FILE(FILE* file);
	double time[e_total];  //array of time measurements - for debug

	void DrawRect(byte* pSource, int sWidth, int sHeight, int sStride);

	// Check where line that passes through p1,p2 and line that passes through p3,p4 intersects.
	// Return true if the intersection is outside the interior of the line p1,p2 
	// Return false otherwise ( in this case the polygon p1-->p2-->p3-->p4-->p1 is not convex.
	bool CheckConvex(const Point3D& p1,const Point3D& p2,const Point3D& p3,const Point3D& p4);

	// Assumes the polygon p1-->p2-->p3-->p4-->p1 is convex. Returns the area of this polygon
	double GetArea(const Point3D& p1,const Point3D& p2,const Point3D& p3,const Point3D& p4);


	//Contains 4 corners of supposed object in the picture.
	vector<Point3D> res_corners;

};
