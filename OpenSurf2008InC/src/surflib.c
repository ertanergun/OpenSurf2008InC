#include"surflib.h"

int surfDetDes(IplImage *img,  /* image to find Ipoints in */
                       Ipoint *ipts, /* reference to vector of Ipoints */
                       int upright, /* run in rotation invariant mode? */
                       int octaves, /* number of octaves to calculate */
                       int intervals, /* number of intervals per octave */
                       int init_sample, /* initial sampling step */
                       float thres, /* blob response threshold */
					   int size /*Size of Array */)
{	
  int ipts_size;
  FastHessian fh;
  IplImage int_img;

  // Create integral-image representation of the image
  //IplImage *int_img = Integral(img);
	Integral(img,&int_img);

  // Create Fast Hessian Object
  FastH(&fh,&int_img);

  // Extract interest points and store in vector ipts
  ipts_size = getIpoints(&fh);
  
  // Create Surf Descriptor Object
  //Surf_Constractor(&int_img, &fh, upright, ipts, size);

  // Deallocate the integral image
  //cvReleaseImage(&int_img);

  return ipts_size;
}