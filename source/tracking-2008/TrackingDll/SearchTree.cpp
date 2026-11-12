#include "stdafx.h"
#include "SearchTree.h"
#include <sstream>
#include <string>
#include <math.h>
using namespace std;

// Empty constructor.
SearchNode::SearchNode()
{
	pLeft = pMiddle = pRight = NULL;
	non_zero = 0;
	non_zero_indices = NULL;
	x1 = y1 = x2 = y2 = 0;
	description_length =0;
}


void SearchNode::CreateHistogram(const vector<int>& p,int width)
{
	//create histogram
	if (p.size() != 0)
		non_zero = 1; //all that matters in this stage is that non_zero != 0
	memset(histogram,0,MAX_CLASSIFIERS_NUM*sizeof(unsigned int));
	for (size_t i=0; i < p.size(); i++)
	{
		int ind = p[i]%width;
		histogram[ind]++;
	}
}


SearchNode::SearchNode(const vector<int>& p,double* buff,int width, int height,int dp)
{
	//fprintf(stdout,"p.size() = %d\n", p.size());
	double alpha = -TAU;
	double beta = TAU;
	Rule rule;
	depth = dp+1;
	if (depth > 10)
	{
		int d = p.size();
	}
	pLeft = pMiddle = pRight = NULL;
	
	int yPatchOffset = WIND*WIND*width;
	int yLineOffset = WIND*width;
	non_zero = 0;
	non_zero_indices = NULL;
	
	if (p.size() < MINIMAL_PARTITION || depth == 8)
	{
		pLeft = pMiddle = pRight = NULL;
		x1 = y1 = x2 = y2 = 0;
		//alpha = beta = 0;
		CreateHistogram(p,width);
	}
	else
	{
		// TODO: add depth to OPTIMALRULE
		double entropy = FindOptimalRule(p, buff, width, height, rule,depth);		
		vector<double> intensity;

		x1 = rule.x1;
		x2 = rule.x2;
		y1 = rule.y1;
		y2 = rule.y2;
		//alpha = rule.alpha;
		//beta = rule.beta;
		for (size_t i=0; i < p.size(); i++)
		{
			int row = p[i]/width;
			int col = p[i]%width;
			int index1 = row*yPatchOffset + col*WIND + y1*yLineOffset+x1;
			int index2 = row*yPatchOffset + col*WIND + y2*yLineOffset+x2;
			intensity.push_back(buff[index1] - buff[index2]);
		}
	
	/*if (alpha == beta)
	{
		x1 =y1 = x2 = y2 = alpha = beta = 0;
		pLeft = pMiddle = pRight = NULL;
		CreateHistogram(p,width);
	}
	else*/
	{
		vector<int> v_left,v_middle,v_right;
		for (size_t i=0; i <p.size();i++)
		{
			if (intensity[i] < alpha)
				v_left.push_back(p[i]);
			else if (intensity[i] < beta)
				v_middle.push_back(p[i]);
			else
				v_right.push_back(p[i]);
		}
		intensity.clear();
		if (p.size() > 7000)
		{
			fprintf(stdout,"Sizes of trees :%d %d %d \n",v_left.size(),v_middle.size(),v_right.size());
			fflush(stdout);
		}


		if (v_left.size() != 0)
		{
			pLeft = new SearchNode(v_left,buff, width, height,depth);
			v_left.clear();
		}
		if (v_middle.size() != 0)
		{
			pMiddle = new SearchNode(v_middle,buff, width, height,depth);
			v_middle.clear();
		}
		if (v_right.size() != 0)
		{
			pRight = new SearchNode(v_right,buff, width, height,depth);
			v_right.clear();
		}
	}
	}
}


//formula - sigma p_i*log(p_i)
double CalcEntropy(int* histogram,int total)
{
	double entropy = total*total;
	double p = 0;
	for (int i=0; i < MAX_CLASSIFIERS_NUM; i++)
	{
		p = (double)(histogram[i]);
		if (p!=0)
			entropy -= p*p;
			//entropy -= p*log(p);
	}
	entropy /= (total*total);
	return entropy;
}

//returns the entropy of the best decision
double FindOptimalRule(const vector<int>& p,double* buff,int width, int height,
	Rule& rule,int depth)
{
	int thresh = 10;
	byte x1,x2,y1,y2;
	double alpha,beta;
	int yPatchOffset = WIND*WIND*width;
	int yLineOffset = WIND*width;

	if ((int)p.size() < thresh)
	{
		rule.x1 = rand()%WIND;
		rule.y1 = rand()%WIND;
		do
		{
			rule.x2 = rand()%WIND;
			rule.y2 = rand()%WIND;
		} while (rule.x1 == rule.x2 && rule.y1 == rule.y2);
	
		vector<double> intensity;
		for (size_t i=0; i < p.size(); i++)
		{
			int row = p[i]/width;
			int col = p[i]%width;
			int index1 = row*yPatchOffset + col*WIND + rule.y1*yLineOffset+rule.x1;
			int index2 = row*yPatchOffset + col*WIND + rule.y2*yLineOffset+rule.x2;
			double intensity_diff = buff[index1] - buff[index2];
			intensity.push_back(intensity_diff);
		}
		sort(intensity.begin(), intensity.end());
		int d = intensity.size()/3;
		//rule.alpha = intensity[d];
		//rule.beta = intensity[2*d];
		return 0; //in this case entropy grade is not rellevant
	}

	double* intensity = new double[p.size()];
	memset(intensity,0,p.size()*sizeof(double));

	int num_trials = 30;
	if (depth == 0)
	{
		num_trials = 10;
	}
	else
	{
		num_trials = min(100*depth,600);
		//num_trials = 50*depth;

	}
	double min_entropy = INT_MAX;
	//Rule temp_rule;
	
	for (int i=0; i < num_trials; i++)
	{
		x1 = rand()%WIND;
		y1 = rand()%WIND;
		do
		{
			x2 = rand()%WIND;
			y2 = rand()%WIND;
		} while (x1 == x2 && y1 == y2);
	
	
	double min_intensity = INT_MAX, max_intensity = INT_MIN;
	
	for (size_t i=0; i < p.size(); i++)
	{
		int row = p[i]/width;
		int col = p[i]%width;
		int index1 = row*yPatchOffset + col*WIND + y1*yLineOffset+x1;
		int index2 = row*yPatchOffset + col*WIND + y2*yLineOffset+x2;
		double intensity_diff = buff[index1] - buff[index2];
		if (intensity_diff < min_intensity)
		{
			min_intensity = intensity_diff;
		}
		if (intensity_diff > max_intensity)
		{
			max_intensity = intensity_diff;
		}
		intensity[i] = intensity_diff;
	}

	int left_histogram[MAX_CLASSIFIERS_NUM];
	int middle_histogram[MAX_CLASSIFIERS_NUM];
	int right_histogram[MAX_CLASSIFIERS_NUM];
	
	
	//create random between 0 and 1
	/*alpha = (double)(rand())/RAND_MAX;
	alpha = alpha*(max_intensity-min_intensity) + min_intensity;
	beta = (double)(rand())/RAND_MAX;
	beta = beta*(max_intensity - alpha) + alpha;
	*/
	alpha = -TAU;
	beta = TAU;
	
	int left_tot = 0;
	int mid_tot = 0;
	int right_tot = 0;
	for (int i=0; i < MAX_CLASSIFIERS_NUM; i++)
	{
		left_histogram[i] = middle_histogram[i] = right_histogram[i] = 0;
	}


	for (size_t i=0; i < p.size(); i++)
	{
		//int row = p[i]/width;
		int col = p[i]%width;
		//int index1 = row*yPatchOffset + col*WIND + y1*yLineOffset+x1;
		//int index2 = row*yPatchOffset + col*WIND + y2*yLineOffset+x2;
		//double intensity_diff = buff[index1] - buff[index2];
		double intensity_diff = intensity[i];
		if (intensity_diff < alpha)
		{
			left_histogram[col]++;
			left_tot++;
		}
		else if (intensity_diff < beta)
		{
			middle_histogram[col]++;
			mid_tot++;
		}
		else
		{
			right_histogram[col]++;
			right_tot++;
		}
	}

     //check that the partition is in general state
		if (left_tot*right_tot*mid_tot == 0)
		{
			continue;
		}
	//if (min(left_tot,min(mid_tot,right_tot))*4 < max(left_tot,max(mid_tot,right_tot)))
	//	 continue;

		double entropy = 0;
		double left_entropy = CalcEntropy(left_histogram,left_tot); //TODO
		double middle_entropy = CalcEntropy(middle_histogram,mid_tot);
		double right_entropy = CalcEntropy(right_histogram,right_tot);
		int total = p.size();
		entropy = left_entropy*left_tot + middle_entropy*mid_tot + right_entropy*right_tot;
		entropy = entropy/total; //normalization;
		if (entropy < min_entropy)
		{
			min_entropy = entropy;
			rule.x1 = x1;
			rule.x2 = x2;
			rule.y1 = y1;
			rule.y2 = y2;
			//rule.alpha = alpha;
			//rule.beta = beta;
		}		
	}

	delete[] intensity;
	return min_entropy;
}

/*
* Current version of find
*/

void SearchNode::Find(byte* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[],int range)
{
	double alpha = -TAU;
	double beta = TAU;

	if (non_zero != 0)
	{
		int ind;
		for (int i=0; i < non_zero; i++)
		{
			ind = non_zero_indices[i];
			ret_hist[ind] += histogram[ind];
		}
		return;			
	}
	int bpp = sStride/sWidth;

	int ind1 = x1*bpp + y1*sStride + startInd;
	int ind2 = x2*bpp + y2*sStride + startInd;
	double x = (double)(pSource[ind1] - pSource[ind2]) /range;

	if (x < alpha)
	{
		if (pLeft) 
			return pLeft->Find(pSource,sWidth,sStride, startInd, ret_hist, range);
	}
	else if (x < beta)
	{
		if (pMiddle) 
			return pMiddle->Find(pSource,sWidth,sStride, startInd, ret_hist,range);
	}
	else
	{
		if (pRight) 
			return pRight->Find(pSource,sWidth,sStride, startInd, ret_hist,range);
	}
	
	return;
}


void SearchNode::Find(double* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[])
{
	double alpha = -TAU;
	double beta = TAU;

	if (non_zero != 0)
	{
		int ind;
		for (int i=0; i < non_zero; i++)
		{
			ind = non_zero_indices[i];
			ret_hist[ind] += histogram[ind];
		}
		return;			
	}
	int bpp = sStride/sWidth;

	int ind1 = x1*bpp + y1*sStride + startInd;
	int ind2 = x2*bpp + y2*sStride + startInd;
	double x = (double)(pSource[ind1] - pSource[ind2]);

	if (x < alpha)
	{
		if (pLeft) 
			return pLeft->Find(pSource,sWidth,sStride, startInd, ret_hist);
	}
	else if (x < beta)
	{
		if (pMiddle) 
			return pMiddle->Find(pSource,sWidth,sStride, startInd, ret_hist);
	}
	else
	{
		if (pRight) 
			return pRight->Find(pSource,sWidth,sStride, startInd, ret_hist);
	}
	
	return;
}

/*
* Previous version of find
void SearchNode::Find(double* patch,double ret_hist[])
{
	//if (classifier != NOT_FINISHED)
	//	return classifier;
	if (non_zero != 0)
	{
		int ind;
		for (int i=0; i < non_zero; i++)
		{
			ind = non_zero_indices[i];
			ret_hist[ind] += histogram[i];
		}
		return;			
	}

	int ind1 = x1 + y1*WIND;
	int ind2 = x2 + y2*WIND;
	double x = patch[ind1] - patch[ind2];
	if (x < alpha)
	{
		if (pLeft) 
			return pLeft->Find(patch,ret_hist);
	}
	else if (x < beta)
	{
		if (pMiddle) 
			return pMiddle->Find(patch,ret_hist);
	}
	else
	{
		if (pRight) 
			return pRight->Find(patch,ret_hist);
	}
	
	return;
}
*/

void SearchNode::AddPoint(byte* pSource,int sWidth , int classifier_num)
{
	int alpha = -TAU, beta = TAU;
	//we assume the patch of source is grey level
	if (non_zero != 0)
	{
		histogram[classifier_num]++;
		return;
	}

	int ind1 = x1 + y1*sWidth;
	int ind2 = x2 + y2*sWidth;
	int x = pSource[ind1] - pSource[ind2] ;
	if (x < alpha)
	{
		if (pLeft) 
			pLeft->AddPoint(pSource, sWidth, classifier_num);
	}
	else if (x < beta)
	{
		if (pMiddle) 
			pMiddle->AddPoint(pSource, sWidth, classifier_num);
	}
	else
	{
		if (pRight) 
			pRight->AddPoint(pSource, sWidth, classifier_num);
	}
	
	return;
}


void SearchNode::CreateNonZeroIndicesAndNormalize()
{
	//the main body - case of leaf of the tree
	if (non_zero != 0)
	{
		non_zero = 0;
		int total = 0;
		for (int i=0; i < MAX_CLASSIFIERS_NUM; i++)
		{
			total+=histogram[i];
			if (histogram[i] != 0)
				non_zero++;
		}
		if (total >= 1000)
		{
			total = total;
		}
		non_zero_indices = new int[non_zero];
		int quant = 1024;
		int j=0;
		for (int i=0; i < MAX_CLASSIFIERS_NUM; i++)
		{
			if (histogram[i] != 0)
			{
				histogram[i] = (unsigned int)(histogram[i]*1./total * quant);
				non_zero_indices[j] = i;
				j++;
			}
		}

		//small check
		int sum=0;
		if (total >= 1000)
		{
		for  (int i=0; i < MAX_CLASSIFIERS_NUM; i++)
		{
			sum += histogram[i];
		}
		sum = sum;
		}
		return;
	}
	
	if (pLeft)
		pLeft->CreateNonZeroIndicesAndNormalize();
	if (pMiddle)
		pMiddle->CreateNonZeroIndicesAndNormalize();
	if (pRight)
		pRight->CreateNonZeroIndicesAndNormalize();
}

void SearchTree::CreateNonZeroIndicesAndNormalize()
{
	if (pRoot)
		pRoot->CreateNonZeroIndicesAndNormalize();
}

void SearchTree::AddPoint(byte* pSource,int sWidth , int classifier_num)
{
	if (pRoot)
		pRoot->AddPoint( pSource, sWidth ,  classifier_num);
}

void SearchTree::Find(byte* pSource,int sWidth,int sStride,int startInd,unsigned int ret_hist[], int range)
{
	if (pRoot)
		pRoot->Find(pSource,sWidth, sStride,startInd,ret_hist,range); 
}

void SearchTree::Write(char* path)
{
	FILE* file;
	if (fopen_s(&file,path,"w") == 0)
		pRoot->Write(file);
	fclose(file);
}

void SearchNode::Write(FILE* file)
{
	//CPoint p1,p2;
	//double alpha,beta;
	//int classifier;
	fprintf(file,"%d %d %d %d %d ",
		x1,y1, x2,y2, non_zero); 
	if (non_zero != 0)
	{
		for (int i=0; i < non_zero; i++)
		{
			int ind = non_zero_indices[i];
			fprintf(file, "%d ",ind);
			fprintf(file, " %d ",histogram[ind]);
		}
	}
	
	fprintf(file," ( ");
	if (pLeft)
		pLeft->Write(file);
	fprintf(file," ) ");
	
	fprintf(file," ( ");
	if (pMiddle)
		pMiddle->Write(file);
	fprintf(file," ) ");
	fprintf(file," ( ");
	if (pRight)
		pRight->Write(file);
	fprintf(file," ) ");
	
}

//SearchNode* SearchNode::Read(const vector<string>& vec)

SearchNode::SearchNode(const vector<string>& vec,int start_ind)
{
	//Assumption - not empty node
	x1 = atoi(vec[start_ind].c_str());
	y1 = atoi(vec[start_ind+1].c_str());
	x2 = atoi(vec[start_ind+2].c_str());
	y2 = atoi(vec[start_ind+3].c_str());
	non_zero = atoi(vec[start_ind+4].c_str());
	description_length = 5;
	int index = start_ind + description_length;

	if (non_zero == 0)
	{
		//histogram = NULL;
		non_zero_indices = NULL;
	}
	else
	{
		//histogram = new unsigned int[non_zero];
		memset(histogram,0,MAX_CLASSIFIERS_NUM*sizeof(unsigned int));
		non_zero_indices = new int[non_zero];
		for (int i=0; i < non_zero; i++)
		{
			non_zero_indices[i] =  atoi(vec[index].c_str());
			index++;
			histogram[ non_zero_indices[i] ] = atoi(vec[index].c_str());
			index++;
		}
		description_length += 2*non_zero;
	}
		
	
	if (vec[index] != "(")
	{
		fprintf(stdout,"Karaul \n");
	}

	if (vec[index+1] == ")")
	{
		pLeft = NULL;
	}
	else
	{
		pLeft = new SearchNode(vec,index+1);
		description_length += pLeft->description_length ;
	}
	
	description_length += 2;
	index = start_ind+description_length;
	if (vec[index] != "(")
	{
		fprintf(stdout,"Karaul2 \n");
	}

	if (vec[index+1] == ")")
	{
		pMiddle = NULL;
	}
	else
	{
		pMiddle = new SearchNode(vec,index+1);
		description_length += pMiddle->description_length ;
	}
	
	description_length += 2;
	index = start_ind+description_length;
	if (vec[index] != "(")
	{
		fprintf(stdout,"Karaul3 \n");
	}

	if (vec[index+1] == ")")
	{
		pRight = NULL;
	}
	else
	{
		pRight = new SearchNode(vec,index+1);
		description_length += pRight->description_length ;
	}
	description_length += 2;
}


void SearchTree::Read(char* path)
{
//	FILE* f = fopen("C:\\Temp\\1.txt","r"); //TODO, path
	FILE* f;
	if (fopen_s(&f,path,"r") != 0)
		return;
	
	fseek(f, 0L, SEEK_END);
	int sz = ftell(f);
	char* buff = new char[sz+1];
	int i=0;
	fseek(f, 0L, SEEK_SET);
	CString str;
	if (f!=NULL)
	{
		char c=fgetc(f);
		while (c!=EOF)
		{
			buff[i]=c;
			i++;
			c=fgetc(f);
		}
	}
	buff[i] = 0;
	string s(buff);
	string word;
	vector<string> vec;
	stringstream stream(s);
	while (getline(stream,word,' ') )
	{
		if (word!="")
			vec.push_back(word);
	}
	int value = atoi(vec[0].c_str());
	fclose(f);
	pRoot = new SearchNode(vec,0);
}

SearchNode::~SearchNode()
{
	if (pLeft)
		delete pLeft;
	if (pMiddle)
		delete pMiddle;
	if (pRight)
		delete pRight;
	if (non_zero_indices)
		delete[] non_zero_indices;
}

SearchTree::~SearchTree()
{
	if (pRoot)
		delete pRoot;
}



