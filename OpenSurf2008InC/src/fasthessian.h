#ifndef FASTHESSIAN_H
#define FASTHESSIAN_H

#include <stdlib.h>
#include <math.h>
#include "ipoint.h"
#include "responselayer.h"

static const int OCTAVES = 5;
static const int INTERVALS = 4;
static const float THRES = 0.0004f;
static const int INIT_SAMPLE = 2;


typedef struct{

	//! Pointer to the integral Image, and its attributes 
    IplImage *img;

    int i_width;

	int i_height;

    //! Reference to vector of features passed from outside 
    Ipoint ipts[N];

    //! Response stack of determinant of hessian values
    ResponseLayer responseMap[12];

    //! Number of Octaves
    int octaves;

    //! Number of Intervals per octave
    int intervals;

    //! Initial sampling step for Ipoint detection
    int init_sample;

    //! Threshold value for blob resonses
    float thresh;

}FastHessian;

	//! Constructor with image
	void FastH(FastHessian *fh, IplImage *img);

	//! Find the image features and write into vector of features
	int getIpoints(FastHessian *fh);


	//! Build map of DoH responses
    void buildResponseMap(FastHessian *fh);

    //! Calculate DoH responses for supplied layer
    void buildResponseLayer(FastHessian* fh, ResponseLayer *r);

    //! 3x3x3 Extrema test
    int isExtremum(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, FastHessian* fh);    
    
    //! Interpolation functions - adapted from Lowe's SIFT implementation
    void interpolateExtremum(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, FastHessian *fh);
    void interpolateStep(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b,
                          double* xi, double* xr, double* xc );
    CvMat* deriv3D(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
    CvMat* hessian3D(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);


#endif