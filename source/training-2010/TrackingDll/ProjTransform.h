#pragma once

/*
* This class performs some linear algebra procedures - inverse, multiplication of matrices,
* some transformations of random scaling , rotation.
*/
class CProjTransform
{
public:
	CProjTransform(void);
	double* Mult(double* p1,double* p2);
	double* Inverse(double* p);
	void ImTransform(byte* pSource,CSize sSize,int sStride,double xpivot,double ypivot
		,byte* pDest,CSize dSize,int dStride, double* p);
	CPoint GetTransformedCoords(double* p,CPoint point);
	void CreateRandomTrasformation(double transf[], double lambda_min,
		double lambda_max,double theta_max,double phi_max);
};
