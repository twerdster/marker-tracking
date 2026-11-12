Usage of the programs:
Training 2010:
Use this program to do the TRAINING stage. 
1) Menu: Set Marker/From Directory:
 Choose the appropriate image. The best resolution for the folowing tracking from the camera
is near 550x400.
2) Menu: Set Train Directory : Here you set the output directory of train stage (by default 
it is directory in which you choosed marker).
3) Menu: Train

Mode CREATE PICTURE :  I used this funvtion for debug. You choose 2 pictures. The function 
applies random transformation to the first image and puts it into the second image.
Almost all pictures in the presentation and word document created by this function.

The program assumes existence of the directory C:\\Temp. 


Tracking 2008:
Use this program to do the TRACKING stage.
1) Menu:Set marker/From directory.
The function check the existence of file "Tree0.dat" in the directory and if
 it exists it read all training information.
2) Menu: Camera / Start. 
Starts grabbing from the camera.
3) Menu:Camera / Stop . If you want to stop the camera.

Thresholds : 1)Menu: Set Thresholod - threshold of Classification Trees.
If you set small threshold - it will pass many points.
2) Menu: SetRansacThreshold : threshold for point to be inlier in RANSAC.
3) Menu: Set Ransac Iterations: maximal number of iterations in RANSAC.