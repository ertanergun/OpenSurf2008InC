#ifndef SURFLIB_H
#define SURFLIB_H


#include "highgui_local.h"
#include "integral.h"
#include "fasthessian.h"
#include "surf.h"
#include "ipoint.h"
#include "utils.h"
#include "type.h"

//! Library function builds array of described interest points
int surfDetDes(IplImage *img,  /* image to find Ipoints in */
                       Ipoint ipts[], /* reference to vector of Ipoints */
                       int upright, /* run in rotation invariant mode? */
                       int octaves, /* number of octaves to calculate */
                       int intervals, /* number of intervals per octave */
                       int init_sample, /* initial sampling step */
                       float thres, /* blob response threshold */
					   int size /*Size of Array */);
#endif