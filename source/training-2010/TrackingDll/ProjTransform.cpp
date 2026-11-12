#include "StdAfx.h"
#include "ProjTransform.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>



CProjTransform::CProjTransform(void)
{
}

/*
* Input: @ transformation - p ,  
         @ point coordinates in the old picture. 
*  Returns - point coordinates in the new (transformed) picture.
*/
CPoint CProjTransform::GetTransformedCoords(double* p,CPoint point)
{
	double x;
	double y;
	double z;
	double x1 = point.x;
	double y1 = point.y;
	double z1=1;
	x = x1*p[0]+y1*p[1]+z1*p[2];
	y = x1*p[3]+y1*p[4]+z1*p[5];
	z = x1*p[6]+y1*p[7]+z1*p[8];
	return CPoint((int)(x/z),(int)(y/z));
}

/*
* Multiply two 3x3 matrices. Allocate memory 
* and return the result.
*/
double* CProjTransform::Mult(double* p1,double* p2)
{
	double* res = new double[9];
	for (int i=0;i<3; i++)
	{
		for (int j=0;j<3; j++)
		{
			res[3*i+j] = 0;
			for (int k=0;k<3; k++)
			{
				res[3*i+j] += p1[3*i+k]*p2[j+3*k];
			}
		}
	}
	return res;
}


/*
* Calculate inverse of 3x3 matrix by the formula
  A*Adj(A) = Identity. 
*/
double* CProjTransform::Inverse(double* p)
{
	double minors[3][3];
	minors[0][0] = p[4]*p[8]-p[5]*p[7];
	minors[0][1] = p[3]*p[8]-p[5]*p[6];
	minors[0][2] = p[3]*p[7]-p[4]*p[6];
	minors[1][0] = p[1]*p[8]-p[2]*p[7];
	minors[1][1] = p[0]*p[8]-p[2]*p[6];
	minors[1][2] = p[0]*p[7]-p[1]*p[6];
	minors[2][0] = p[1]*p[5]-p[2]*p[4];
	minors[2][1] = p[0]*p[5]-p[2]*p[3];
	minors[2][2] = p[0]*p[4]-p[1]*p[3];
	double det = p[0]*minors[0][0]-p[1]*minors[0][1]+p[2]*minors[0][2];
	double* inv = new double[9];
	int j,k;
	for (int i=0;i < 9; i++)
	{
		j= i/3;
		k= i%3;
		inv[i] = minors[k][j]/det; //check!
		if ((j+k)%2 == 1)
			inv[i] = -inv[i];
	}
	
	return inv;
}

/*
* Create random projective transformation by formula
* T =  rot(theta) *rot(phi)^{-1) * diag(lambda_1 , lambda_2, 1) *  rot(phi).
* Here rot(x) = rotation matrix by angle x.
* The result is returned in transf.
*/
void CProjTransform::CreateRandomTrasformation(double transf[], double lambda_min,
		double lambda_max,double theta_max,double phi_max)
{
	double r = (double)(rand())/RAND_MAX;
	double lambda1 = (lambda_max-lambda_min)*r+lambda_min;
	r = (double)(rand())/RAND_MAX;
	double lambda2;
	lambda2 = (lambda_max-lambda_min)*r+lambda_min;
	r = (double)(rand())/RAND_MAX;
	double theta = 2*theta_max*r-theta_max;
	r = (double)(rand())/RAND_MAX;
	double phi = 2*phi_max*r-phi_max;

	double R_theta[9] = {cos(theta), sin(theta),0,
						 -sin(theta), cos(theta), 0 ,
						 0 , 0 , 1};
	double R_phi[9] = {cos(phi), sin(phi),0,
						 -sin(phi), cos(phi), 0 ,
						 0 , 0 , 1};
	double R_phi_inv[9] = {cos(phi), -sin(phi),0,
						 sin(phi), cos(phi), 0 ,
						 0 , 0 , 1};
	double S[9] = {lambda1,0 ,0,
					0,lambda2,0,
					0 ,0,1};
	double* A= Mult(S,R_phi);
	double* B= Mult(R_theta,R_phi_inv);
	double* C = Mult(B,A);
	for (int i=0; i < 9; i++)
	{
		transf[i] = C[i];
	}
	delete[] A;
	delete[] B;
	delete[] C;
}

/*
* Input : input image - parameters - pSource, sSize, sStride, xpivot, ypivot
* Input : transformation - p - array of size 9 of doubles
* Output: pDest - transformed image.
*/
void CProjTransform::ImTransform(byte* pSource,CSize sSize,int sStride,double xpivot,double ypivot,
	byte* pDest,CSize dSize,int dStride, double* p)
{
    int i, j, k, index, indexStart;
    int x0, x1, y0, y1;
    double t, u, t_, u_, tu, tu_, t_u, t_u_;
    double x, y,z;
	double x2,y2,z2;
    long Y0X0, Y0X1, Y1X0, Y1X1;
    double temp;
	int iBytesPerPixel = sStride/sSize.cx;
	int destBytesPerPixel = dStride/dSize.cx;
	
    indexStart = 0;
	double coef[] = {0.11, 0.59, 0.3 };
    for (i = 0; i < dSize.cy; i++, indexStart += dStride)
    {
        index = indexStart;
        for (j = 0; j < dSize.cx; j++, index += destBytesPerPixel)
        {
            x2 = j+xpivot;
			y2 = i+ypivot;
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
				for (k = 0; k < destBytesPerPixel; k++)
					pDest[index+k] = 0; 
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

