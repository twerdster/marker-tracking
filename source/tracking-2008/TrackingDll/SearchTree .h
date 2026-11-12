#pragma once
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

#define BACKGROUND -1
#define NOT_FINISHED -2
#define WIND 64
#define MINIMAL_PARTITION 10

class SearchNode
{
	CPoint p1,p2;
	double alpha,beta;
	int classifier;
	//double quality; 
	//vector<int> patches; //indices to patches - need f
	SearchNode* pLeft,*pMiddle,*pRight;
	
public:
	SearchNode();
	SearchNode(const vector<int>& p,double* buff,int width, int height);
	int Find(double* patch);
	void Write(FILE* file);
	SearchNode* Read(const vector<string>& vec);
};

class SearchTree
{
public:
	SearchNode* pRoot;	
	SearchTree(){pRoot = NULL;}
	void BuildTree(const vector<int>& p, double* buff,int width, int height)
	{ 
		pRoot = new SearchNode(p,buff,width,height);
	}
	void Write(char* path); 
	void Read(char* path);
	
};