#ifndef SURF_H
#define SURF_H


#include <stdlib.h>
#include <math.h>
#include "cv_local.h"
#include "ipoint.h"
#include "integral.h"
#include "fasthessian.h"

typedef struct {

	//! Integral image where Ipoints have been detected
    IplImage *img;

    //! Ipoints vector
    Ipoint *ipts;

    //! Index of current Ipoint in the vector
    int index;

}Surf;

	//! Standard Constructor (img is an integral image)
	void Surf_Constractor(IplImage *img, FastHessian *fh, int upright, Ipoint *ipts, int size);

    //! Describe all features in the supplied vector
    void getDescriptors(int bUpright, Surf* S, int size);

	 //! Get the descriptor. See Agrawal ECCV 08
	void getDescriptor(int bUpright, Surf* S);

	//! Assign the current Ipoint an orientation
    void getOrientation(Surf* S);

	    //! Calculate the value of the 2d gaussian at x,y
    _inline float gaussian(int x, int y, float sig);
    _inline float _gaussian(float x, float y, float sig);

    //! Calculate Haar wavelet responses in x and y directions
    _inline float haarX(int row, int column, int size, Surf* S);
    _inline float haarY(int row, int column, int size, Surf* S);

    //! Get the angle from the +ve x-axis of the vector given by [X Y]
    float getAngle(float X, float Y);



#endif