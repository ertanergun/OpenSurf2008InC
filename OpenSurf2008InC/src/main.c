#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "surflib.h"

int mainImage()
{
	char *imageData, *_imageData;
	int i,ipts_size = 0;
	float tclock;
	clock_t start, end;
	Ipoint IpArray[N];
	IplImage img;

	//IplImage *img = cvLoadImage("imgs/lena.bmp",1);
	//IplImage img = 
    constructIlpImage("imgs/lena.bmp", &img);

	// Detect and describe interest points in the image
	start = clock();
	ipts_size = surfDetDes(&img, IpArray, 0, 5, 4, 2, 0.0004f, N);
	end = clock();

	tclock = (float)(end - start) / CLOCKS_PER_SEC;

	printf("OpenSURF found: %d interest points \n",ipts_size);
	printf("OpenSURF took: %f seconds \n",tclock);



	// Draw the detected points
	//drawIpoints(img, IpArray,0, ipts_size);

	// Display the result
	//showImage(img);
	
	free(IpArray);
	return 0;
}


int main()
{
	mainImage();
	return 0;
}
