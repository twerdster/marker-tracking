#pragma once
#include <vector>
using namespace std;

const int SELECT_POINTS = 100;
const int PARAMS_NUM = 6;

enum SvdStatus
{
    SVD_INPUT_NOT_SUFFICIENT,
    SVD_NOT_CONVERGE,
    SVD_LOW_QUALITY, 
    SVD_MIDDLE_QUALITY,
    SVD_HIGH_QUALITY
};

class PointPosition
{
public:
       int m_iPointNumber;
       int m_iXPos;
       int m_iYPos;
       PointPosition()
        {
            m_iPointNumber = m_iXPos = m_iYPos = 0;
        }
        PointPosition(int n, int x, int y)
        {
            m_iPointNumber = n;
            m_iXPos = x;
            m_iYPos = y;
        }
};


class SVM{

        float m_RMSDist;
        float m_MaxDist;
        float m_RMSDistTH;
        float m_MaxDistTH;

        float* m_pafTMatrix;

public:
	SVM()
        {
			m_pafTMatrix = new float[PARAMS_NUM+1];
            m_RMSDistTH = 5.0f;
            m_MaxDistTH = 10.0f;
        }
         float* GetTransformCoef()
        {
            return m_pafTMatrix;
        }
		bool SolveLinearParams(  vector<PointPosition>& MasterCorrelationPoints, 
							 vector<PointPosition>& CorrelationPoints, SvdStatus& pSvdStatus);

		 void svbksb(float* u[], float w[], float* v[], int m, int n, float b[], float x[]);
		 float pythag(float a, float b);
		  void svdcmp(float* a[], int m, int n, float w[], float* v[]);


};