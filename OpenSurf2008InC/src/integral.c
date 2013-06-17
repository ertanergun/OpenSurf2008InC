#include "utils.h"
#include "integral.h"
#include "bmpImageHeader.h"

float i_data[262144];
float x_data[262144];
char temp_data[262144];

//! Computes the integral image of image img.  Assumes source image to be a 
//! 32-bit floating point.  Returns IplImage of 32-bit float form.
IplImage *Integral(IplImage *source, IplImage *int_img)
{	
	
  // convert the image to single channel 32f
  //IplImage *img = source; //getGray(source);
  
  //IplImage int_img; //= cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);

  // set up variables for data access
  int height = source->height;
  int width = source->width;
  int j,i, count = 0;
  int step = source->widthStep/sizeof(float);
  float rs = 0.0f, temp = 0.0f, tempMove = 0.0f;

  //Allocate x_data indexes
  for(i=0; i<height; ++i) 
  {
    for(j=0; j<width; ++j) 
    {
		x_data[count] = (float ) source->imageData[count];
		count++;
	}
  }

  // first row only
  for(j=0; j<width; j++) 
  {
    rs += x_data[j]; 
    i_data[j] = rs;
  }
  // remaining cells are sum above and to the left
  for(i=1; i<height; ++i) 
  {
    rs = 0.0f;
	tempMove = 0.0f;
    for(j=0; j<width; ++j) 
    {
      rs += x_data[i*step+j]; 
	  temp = (j-1<0) ? 0 : i_data[(i-1)*step+j-1];

	  // integram Image Algorith s(x,y) = i(x,y) + s(x-1,y) + s(x,y-1) - s(x-1,y-1)
	  i_data[i*step+j] = x_data[i*step+j] + tempMove + i_data[(i-1)*step+j] - temp;
	  tempMove = i_data[i*step+j];
    }
  }

  int_img->height = height;
  int_img->width = width;
  int_img->nChannels = source->nChannels;
  int_img->widthStep = source->widthStep;
  int_img->imageSize = source->imageSize;
  int_img->imageData = i_data;

  count = 0;
  for(i=0; i<height; ++i) 
  {
    for(j=0; j<width; ++j) 
    {
		 *(temp_data + count) = i_data[count];
		 *(int_img->intImage + count) = i_data[count];
		count++;
	}
  }
	int_img->imageData = temp_data;

  // release the gray image
  //cvReleaseImage(&img);
//  free(img);
	
  // return the integral image
  return int_img;
}