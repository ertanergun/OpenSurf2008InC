#include <stdio.h>
#include <stdlib.h>
#include "highgui_local.h"
#include "utils.h"


void showImage(const IplImage *img)
{
  cvNamedWindow("Surf", CV_WINDOW_AUTOSIZE); 
  cvShowImage("Surf", img);  
  cvWaitKey(0);
}

IplImage* getGray(const IplImage *img)
{
	IplImage* gray8, * gray32;

	gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
	if( img->nChannels == 1 )
	 gray8 = (IplImage *) cvClone( img );
	else {
    gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
    cvCvtColor( img, gray8, CV_BGR2GRAY );
  }

  cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

  cvReleaseImage( &gray8 );
  return gray32;
}

void drawIpoints(IplImage *img, Ipoint *ipts, int tailSize, int ipts_size)
{
	Ipoint *ipt;
	float s, o;
	int r1, c1, r2, c2, lap;
	unsigned int i;


	for(i = 0; i < ipts_size; i++) 
	{
		ipt = &ipts[i];
		s = (2.5f * ipt->scale);
		o = ipt->orientation;
		lap = ipt->laplacian;
		r1 = fRound(ipt->y);
		c1 = fRound(ipt->x);
		c2 = fRound(s * cos(o)) + c1;
		r2 = fRound(s * sin(o)) + r1;

		
	
		if (o) // Green line indicates orientation
			cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0, 0),1,8,0);
		else  // Green dot if using upright version
			cvCircle(img, cvPoint(c1,r1), 1, cvScalar(0, 255, 0, 0),-1, 8, 0);

		if (lap == 1)
		{ // Blue circles indicate dark blobs on light backgrounds
			cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(255, 0, 0, 0),1, 8, 0);
		}
		else if (lap == 0)
		{ // Red circles indicate light blobs on dark backgrounds
			cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 0, 255, 0),1, 8, 0);
		}
		else if (lap == 9)
		{ // Red circles indicate light blobs on dark backgrounds
			cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 255, 0, 0),1, 8, 0);
		}

		// Draw motion from ipoint dx and dy
		if (tailSize)
		{
			cvLine(img, cvPoint(c1,r1),
			cvPoint((int)(c1+ipt->dx*tailSize), (int)(r1+ipt->dy*tailSize)),
			cvScalar(255,255,255,0), 1, 8, 0);
		}
	}
}