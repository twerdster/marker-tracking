// Adapted from Alex Kemarsky's recovered SearchTree.h; see ../PORTING.md.
#pragma once
#include "portable.h"
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

#define BACKGROUND -1  // Parameter indicates that feature point is not matched
					   //to CLASSIFIER_POINTS in the marker
#define NOT_FINISHED -2
#define WIND 64  // size of patch's side around feature point
#define MINIMAL_PARTITION 50  // Parameter to stop the process of decision rules.
                              // If num of patches that belong to this node less than MINIMAL_PARTITION
                              // then this node is leaf of the Search Tree.

#define MAX_CLASSIFIERS_NUM 300 // Maximum number of feature points in the marker.
#define TAU 10  // Parameter of the decision rules in all Search Trees.


/*
* This struct contains coordinates of two pixels (x1,y1), (x2,y2) in the rule;
* The decision will be
* Intensity(x1,y1) - Intensity(x2,y2) < -tau, > tau or between -tau and tau.
*/
struct Rule
{
	PixelByte x1,x2,y1,y2;
	//float alpha,beta; -- Previous versions
	//double entropy;
};


double FindOptimalRule(const vector<int>& p,double* buff,int width, int height,Rule& rule,int depth);
double CalcEntropy(int* histogram,int total);


class SearchNode
{
	PixelByte x1,x2,y1,y2;  // The points (x1,y1) , (x2,y2) of the decision rule

	int non_zero; // indicates whether this is leaf node (if non_zero = 0) or not
	// if not, non_zero is number of non zero indices in the histogram.


	int* non_zero_indices;  // Indices that are non_zero in thee histogram.

	unsigned int histogram[MAX_CLASSIFIERS_NUM];  // The histogram.
	// Histogram[i] = num of patches that belong to Classifier point number i in this leaf.
	//In practice most of entries will be zero. Non zero entries are in non_zero_indices

	int description_length; // Total num of bytes to describe tree with this node as root.
	// This variable is used to perform functions ReadTree() and WriteTree()

	int depth; //debug - depth of the node in the tree

	SearchNode* pLeft,*pMiddle,*pRight; //Pointers to sons
										// Null if this is leaf of the tree.

public:
	SearchNode();
	SearchNode(const vector<int>& p,double* buff,int width, int height,int dp);
	SearchNode(const vector<string>& vec,int start_ind);
	~SearchNode();
	void CreateHistogram(const vector<int>& p,int width);
	void CreateNonZeroIndicesAndNormalize();
	void AddPoint(PixelByte* pSource,int sWidth, int classifier_num);
	void Find(PixelByte* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[],int range);
	void Find(double* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[]);
	void Write(FILE* file);
};

class SearchTree
{
public:
	SearchNode* pRoot;
	SearchTree(){pRoot = NULL;}
	SearchTree(const SearchTree&) = delete;
	SearchTree& operator=(const SearchTree&) = delete;
	void BuildTree(const vector<int>& p, double* buff,int width, int height)
	{
		clear();
		pRoot = new SearchNode(p,buff,width,height,-1);
	}
	~SearchTree(){delete pRoot;}
	void Find(PixelByte* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[], int range);

	void Find(double* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[])
	{
		pRoot->Find(pSource, sWidth, sStride, startInd,  ret_hist);
	}

	void clear(){delete pRoot; pRoot = NULL;}
	void AddPoint(PixelByte* pSource,int sWidth, int classifier_num);
	void CreateNonZeroIndicesAndNormalize();

	void Write(char* path);
	void Read(char* path);

};
