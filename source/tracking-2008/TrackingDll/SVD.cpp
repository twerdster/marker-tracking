#include "stdafx.h"
#include "SVD.h"
#include <math.h>


bool SVM::SolveLinearParams( vector<PointPosition>& MasterCorrelationPoints, 
							vector<PointPosition>& CorrelationPoints, SvdStatus& pSvdStatus)
// calculates the linear transform parameters to transform
// location in the current image, to further interpolate it.
// c = Current  m = Master
//      |Xc|         |Xm|
// Pc = |  |    Pm = |  |
//      |Yc|         |Ym|
// Pc = S*R*Pm + T
// |Xc|                |Sx 0 | | C S| |Xm| |Tx|  |Sx*C  Sx*S| |Xm| |Tx|   |p1 p2| |Xm| |p5|
// |  | = S*R*Pm + T = |     |*|    |*|  |+|  |= |          |*|  |+|  | = |     |*|  |+|  |
// |Yc|                |0  Sy| |-S C| |Ym| |Ty|  |-Sy*S Sy*C| |Ym| |Ty|   |p3 p4| |Ym| |p6| 

// p[1] =  Sx*C   p[2] =  Sx*S   p[3] = -Sy*S   p[4] =  Sy*C   p[5] = Tx   p[6] = Ty
{
    int iterations = 0;
    int iVectorSize = MasterCorrelationPoints.size();

    for(int ii = 0; ii < 1; ii++)
    {
    int i, j,k,l;
    double wmax,wmin;
    float Distance = 0;
    float* Dist = new float[SELECT_POINTS*2]; 
    m_RMSDist = 0; m_MaxDist = 0;
    float MinDist = 999999.9f;
    // ROW_NUM is the number of linear eq. (two times the number of points)
    int Pts = min(SELECT_POINTS, CorrelationPoints.size());
    int SelectPts = min(SELECT_POINTS, Pts);
    int ROW_NUM = 2*SelectPts;
    float SampleStep = 1.0f, FloatSample = 0.0f;
    float factor = 1.0f;
    float TransPointX, TransPointY;

    if(Pts < factor*3)
    {
        fprintf(stdout," not enough correlation points,  ");//finetuning debug
	    pSvdStatus = SVD_INPUT_NOT_SUFFICIENT;
        return false;
    }

    float* b = new float[ROW_NUM+1];	// right side input vector - b
    float* p = new float[PARAMS_NUM+1];	// solution vector - x
    float* w = new float[PARAMS_NUM+1];	// Singular values of a
    // ROW_NUM rows, PARAMS_NUM columns
    float** v = new float*[PARAMS_NUM+1];// orthogonal decomposition of a

    float** a = new float*[ROW_NUM + 1];	// design matrix
    float** u = new float*[ROW_NUM + 1];	// orthogonal decomposition of a

	for(int i = 0; i < PARAMS_NUM + 1; i++)
		v[i] = new float[PARAMS_NUM + 1];

	for(int i = 0; i < ROW_NUM + 1; i++)
	{
		a[i] = new float[PARAMS_NUM + 1];
		u[i] = new float[PARAMS_NUM + 1];
	}
    // select M best points out of Total Pts points
    if(Pts > SELECT_POINTS)
        SampleStep = (float)Pts / (float)SELECT_POINTS;
    // for (j=0;j<Pts;j++)
    for (i=0;i<SelectPts; i++)
    {
        // j - point index
        j = (int)(FloatSample + 0.5);
        FloatSample += SampleStep;
        // k - row index; l - column index
        k = 2*i+1;
        // Current points
        PointPosition cp = (PointPosition)CorrelationPoints[j];
        PointPosition mp = (PointPosition)MasterCorrelationPoints[j];
        b[k] = (float)cp.m_iXPos;
        b[k + 1] = (float)cp.m_iYPos;
        // Master points  - design matrix
        a[k][ 1] = (float)mp.m_iXPos;
        a[k][ 2] = (float)mp.m_iYPos;
        a[k][3] = 0.0f;
        a[k][4] = 0.0f;
        a[k][5] = 1.0f;
        a[k][6] = 0.0f;
        k++;
        a[k][1] = 0.0f;
        a[k][2] = 0.0f;
        a[k][ 3] = (float)mp.m_iXPos;
        a[k][ 4] = (float)mp.m_iYPos;
        a[k][5] = 0.0f;
        a[k][6] = 1.0f;
    }
    svdcmp(a,ROW_NUM,PARAMS_NUM,w,v);

    // out-layers deletion
    /* find maximum singular value */
    wmax=0.0f;
    for (l=1;l<=PARAMS_NUM;l++)
        if (w[l] > wmax) wmax=w[l];
    /* define "small" */
    wmin=wmax*(1.0e-6);
    /* zero the "small" singular values */
    for (l=1;l<=PARAMS_NUM;l++)
        if (w[l] < wmin)
	        w[l]=0.0f;


    svbksb(a,w,v,ROW_NUM,PARAMS_NUM,b,p);

    // Check RMS error of master and transformed current points
    for (j=0;j<Pts;j++)
    {
        PointPosition mp = (PointPosition)MasterCorrelationPoints[j];
        PointPosition cp = (PointPosition)CorrelationPoints[j];
        TransPointX = (float)(p[1] * mp.m_iXPos + p[2] * mp.m_iYPos + p[5]);
        TransPointY = (float)(p[3] * mp.m_iXPos + p[4] * mp.m_iYPos + p[6]);

        Dist[j] = (float)(pow(cp.m_iXPos - TransPointX, 2) + pow(cp.m_iYPos - TransPointY, 2));
        m_RMSDist += Dist[j];

        Distance = (float)sqrt(Dist[j]);
        if(Distance>m_MaxDist)		m_MaxDist = Distance;
        else if(Distance<MinDist)	MinDist = Distance;
    }
    m_RMSDist = (float)sqrt(m_RMSDist) / Pts;

    // inverse p
    /*float det = p[1]*p[4]-p[3]*p[2];
    p[1] = p[1]/det;
    p[2] = -p[3]/det;
    p[3] = -p[2]/det;
    p[4] = p[4]/det;
    p[5] = -p[5];
    p[6] = -p[6];
    */
    if (m_RMSDist > m_RMSDistTH || m_MaxDist > m_MaxDistTH)
    {
        if (m_RMSDist > m_RMSDistTH*10)
        {
            MasterCorrelationPoints.erase(MasterCorrelationPoints.begin() +MasterCorrelationPoints.size()-1);
            CorrelationPoints.erase(CorrelationPoints.begin()+CorrelationPoints.size()-1);
        }
        else
        {
            Distance = 0;
            for (i=0,j=0;j<Pts;j++) 
            {
	            if (Dist[j] > Distance)
	            {
		            Distance = Dist[j];
		            i = j;
	            }
            }
    
			MasterCorrelationPoints.erase(MasterCorrelationPoints.begin()+i);
            CorrelationPoints.erase(CorrelationPoints.begin()+i);
        }
        if (MasterCorrelationPoints.size() >= 4)
        {
            iterations++;
            if (iterations > iVectorSize/2)
            {
                pSvdStatus =  SVD_NOT_CONVERGE;
                return false;
           }
	        ii = -1;
        }
        else
        {
	        if (m_RMSDist < m_RMSDistTH)
	        {
		        m_pafTMatrix = p;
                pSvdStatus = SVD_LOW_QUALITY;

		        return true;
	        }
        }
    }
    else
    {
        m_pafTMatrix = p;
        if (m_RMSDist > m_RMSDistTH/2 || m_MaxDist > m_MaxDistTH/2)
            pSvdStatus = SVD_MIDDLE_QUALITY;
        else
            pSvdStatus = SVD_HIGH_QUALITY;

        return true;
    }
    }
    pSvdStatus = SVD_NOT_CONVERGE;
    return false;
}
void SVM::svbksb(float* u[], float w[], float* v[], int m, int n, float b[], float x[])
{
    int jj,j,i;
    float s;
    float* tmp= new float[n+1];

	for (j=1;j<=n;j++) 
    {
        s=0.0f;
        if (w[j] != 0) 
        {
	        for (i=1;i<=m;i++) 
                s += u[i][j]*b[i];
	        s /= w[j];
        }
        tmp[j]=s;
    }
    for (j=1;j<=n;j++)
    {
        s=0.0f;
        for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
            x[j]=s;
    }
	delete [] tmp;
}

float SVM::pythag(float a, float b)
{
    float absa,absb;
    absa=abs(a);
    absb=abs(b);
    if (absa > absb) return absa * (float)sqrt(1.0f + absb * absb / absa / absa);
    else return (absb == 0.0f ? 0.0f : absb * (float)sqrt(1.0f + absa * absa / absb / absb));
}

void SVM::svdcmp(float* a[], int m, int n, float w[], float* v[])
{
    float fEpsilon = 0.00005f;
    //	float pythag(float a, float b);
    int   flag,i,its,j,jj,k,l=0,nm=0;
    float anorm,c,f,g,h,s,scale,x,y,z;
    float* rv1=new float[n+1];
    g=scale=anorm=0.0f;
    for (i=1;i<=n;i++) {
        l=i+1;
        rv1[i]=scale*g;
        g=s=scale=0.0f;
        if (i <= m) {
	        for (k=i;k<=m;k++) scale += abs(a[k][i]);
	        if (scale != 0) {
		        for (k=i;k<=m;k++) {
			        a[k][i] /= scale;
			        s += a[k][i]*a[k][i];
		        }
		        f=a[i][i];
		        g = (float)((f >= 0) ? -sqrt(s) : sqrt(s));
		        h=f*g-s;
		        a[i][i]=f-g;
		        for (j=l;j<=n;j++) {
			        for (s=0.0f,k=i;k<=m;k++) s += a[k][i]*a[k][j];
			        f=s/h;
			        for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
		        }
		        for (k=i;k<=m;k++) a[k][i] *= scale;
	        }
        }
        w[i]=scale *g;
        g=s=scale=0.0f;;
        if (i <= m && i != n) {
	        for (k=l;k<=n;k++) scale += abs(a[i][k]);
	        if (scale != 0) {
		        for (k=l;k<=n;k++) {
			        a[i][k] /= scale;
			        s += a[i][k]*a[i][k];
		        }

		        f=a[i][l];

                g = (float)((f >= 0) ? -sqrt(s) : sqrt(s));
		        h=f*g-s;
		        a[i][l]=f-g;
		        for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
		        for (j=l;j<=m;j++) {
			        for (s=0.0f,k=l;k<=n;k++) s += a[j][k]*a[i][k];
			        for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
		        }
		        for (k=l;k<=n;k++) a[i][k] *= scale;
	        }
        }
        anorm=max(anorm,(abs(w[i])+abs(rv1[i])));
    }
    for (i=n;i>=1;i--) {
        if (i < n) {
	        if (g != 0) {
		        for (j=l;j<=n;j++)
			        v[j][i]=(a[i][j]/a[i][l])/g;
		        for (j=l;j<=n;j++) {
			        for (s=0.0f,k=l;k<=n;k++) s += a[i][k]*v[k][j];
			        for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
		        }
	        }
	        for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0f;;
        }
        v[i][i]=1.0f;;
        g=rv1[i];
        l=i;
    }
    for (i=min(m,n);i>=1;i--) {
        l=i+1;
        g=w[i];
        for (j=l;j<=n;j++) a[i][j]=0.0f;
        if (g != 0) {
	        g=1.0f/g;
	        for (j=l;j<=n;j++) {
		        for (s=0.0f,k=l;k<=m;k++) s += a[k][i]*a[k][j];
		        f=(s/a[i][i])*g;
		        for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
	        }
	        for (j=i;j<=m;j++) a[j][i] *= g;
        } else for (j=i;j<=m;j++) a[j][i]=0.0f;
        ++a[i][i];
    }
    for (k=n;k>=1;k--) {
        for (its=1;its<=30;its++) {
	        flag=1;
	        for (l=k;l>=1;l--) {
		        nm=l-1;
//				if ((float)(abs(rv1[l])+anorm) == anorm) {
		        if (abs((float)(abs(rv1[l])+anorm) - anorm) < fEpsilon) {  // Changed by AndrewG
			        flag=0;
			        break;
		        }
		        //if ((float)(abs(w[nm])+anorm) == anorm) break;
		        if (abs((float)(abs(w[nm])+anorm) - anorm) < fEpsilon) break;  // Changed by AndrewG
	        }
	        if (flag != 0) {
		        c=0.0f;
		        s=1.0f;
		        for (i=l;i<=k;i++) {
			        f=s*rv1[i];
			        rv1[i]=c*rv1[i];
			        //if ((float)(abs(f)+anorm) == anorm) break;
			        if (abs((float)(abs(f)+anorm) - anorm) < fEpsilon) break;  // Changed by AndrewG
			        g=w[i];
			        h=pythag(f,g);
			        w[i]=h;
			        h=1.0f/h;
			        c=g*h;
			        s = -f*h;
			        for (j=1;j<=m;j++) {
				        y=a[j][nm];
				        z=a[j][i];
				        a[j][nm]=y*c+z*s;
				        a[j][i]=z*c-y*s;
			        }
		        }
	        }
	        z=w[k];
	        if (l == k) {
		        if (z < 0.0f) {
			        w[k] = -z;
			        for (j=1;j<=n;j++) v[j][k] = -v[j][k];
		        }
		        break;
	        }
            if (its == 30)
            {
                fprintf(stdout,"no convergence in 30 svdcmp iterations");
                return;
            }
	        x=w[l];
	        nm=k-1;
	        y=w[nm];
	        g=rv1[nm];
	        h=rv1[k];
	        f=(float)(((y-z)*(y+z)+(g-h)*(g+h))/(2.0f*h*y));
	        g=pythag(f,1.0f);
	        f=((x-z)*(x+z)+h*((y/(f+((f >= 0) ? g : -g)))-h))/x;
	        c=s=1.0f;;
	        for (j=l;j<=nm;j++) {
		        i=j+1;
		        g=rv1[i];
		        y=w[i];
		        h=s*g;
		        g=c*g;
		        z=pythag(f,h);
		        rv1[j]=z;
		        c=f/z;
		        s=h/z;
		        f=x*c+g*s;
		        g = g*c-x*s;
		        h=y*s;
		        y *= c;
		        for (jj=1;jj<=n;jj++) {
			        x=v[jj][j];
			        z=v[jj][i];
			        v[jj][j]=x*c+z*s;
			        v[jj][i]=z*c-x*s;
		        }
		        z=pythag(f,h);
		        w[j]=z;
		        if (z != 0) {
			        z=1.0f/z;
			        c=f*z;
			        s=h*z;
		        }
		        f=c*g+s*y;
		        x=c*y-s*g;
		        for (jj=1;jj<=m;jj++) {
			        y=a[jj][j];
			        z=a[jj][i];
			        a[jj][j]=y*c+z*s;
			        a[jj][i]=z*c-y*s;
		        }
	        }
	        rv1[l]=0.0f;;
	        rv1[k]=f;
	        w[k]=x;
        }
    }
}

