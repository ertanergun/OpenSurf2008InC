#ifndef UTILS_H
#define UTILS_H

//#include <cv.h>
#include "ipoint.h"
#include "type.h"
#include "cv_local.h"

//! Round float to nearest integer
_inline int fRound(float flt)
{
  return (int) floor(flt+0.5f);
}

//! Draw all the Ipoints in the provided vector
void drawIpoints(IplImage *img, Ipoint *ipts, int tailSize, int ipts_size);

#endif