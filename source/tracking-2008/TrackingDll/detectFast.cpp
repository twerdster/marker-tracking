#include <stdio.h>
#include <string.h>
#include "stdafx.h"

#include "fast.h"
#include "mex.h"


//input
// 0 - the image. it must be of type uint8
// 1 - the threshold to use

//output
// 0 - the corners

void mexFunction(int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {
    
    const byte * image = (const byte*)mxGetData(prhs[0]);
    int width = mxGetM(prhs[0]); // maybe should be M?
    int height = mxGetN(prhs[0]); // maybe should be N?
    int threshold = mxGetScalar(prhs[1]);
    int numCorners;
    mexPrintf("width:%i height:%i\n",width,height);
    xy *corners;
    
    corners=fast9_detect_nonmax(image, width, height, width, threshold, &numCorners);
    mexPrintf("detected %i\n",numCorners);
    
    plhs[0] = mxCreateNumericMatrix(2, numCorners, mxINT32_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[0]), corners, numCorners*sizeof(xy));
    
    free(corners);
    
}