#include "integral.h"
#include "ipoint.h"
#include "utils.h"
#include "fasthessian.h"

static int count = 0;

//! Constructor with image
void FastH(FastHessian *fhts, IplImage *img)
{
	int i;
	fhts->img = img;
	fhts->i_height = img->height;
	fhts->i_width = img->width;
	for(i=0; i<N; i++){
		fhts->ipts[i].x = 0;
		fhts->ipts[i].y = 0;
		fhts->ipts[i].clusterIndex = 0;
		fhts->ipts[i].dx = 0;
		fhts->ipts[i].dy = 0;
		fhts->ipts[i].laplacian = 0;
		fhts->ipts[i].orientation = 0;
		fhts->ipts[i].scale = 0;
	}
	fhts->octaves = OCTAVES; 
    fhts->intervals = INTERVALS; 
    fhts->init_sample = INIT_SAMPLE;
    fhts->thresh = THRES;
	
}


int getIpoints(FastHessian *fh)
{
	int o,i,r,c;
	ResponseLayer *b, *m, *t;

	// filter index map // variables should be OCTAVES AND INTERVAL BUT THERE IS AN ERROR
	static const int filter_map [5][4] = {{0,1,2,3}, {1,3,4,5}, {3,5,6,7}, {5,7,8,9}, {7,9,10,11}};

	// Build the response map
	buildResponseMap(fh);

	// Get the response layers
	
	 for (o = 0; o < fh->octaves; ++o)
	 {
		 for (i = 0; i <= 1; ++i)
		{
			b = &fh->responseMap[filter_map[o][i]];
			m = &fh->responseMap[filter_map[o][i+1]];
			t = &fh->responseMap[filter_map[o][i+2]];

		// loop over middle response layer at density of the most 
    // sparse layer (always top), to find maxima across scale and space
		for (r = 0; r < t->height; ++r)
		{
			for (c = 0; c < t->width; ++c)
			{
				if (isExtremum(r, c, t, m, b,fh))
				{
				interpolateExtremum(r, c, t, m, b,fh);
				}
			}
		}
	   }
	}
	
	return count;
}

void buildResponseMap(FastHessian *fh)
{
	int w = 0,h = 0,s = 0;
	unsigned int i = 0;

	// Get image attributes
	w = (fh->i_width / fh->init_sample);
	h = (fh->i_height / fh->init_sample);
	s = (fh->init_sample);

	// Calculate approximated determinant of hessian values
  if (fh->octaves >= 1)
  {
		ResLayer(&fh->responseMap[0],w,   h,   s,   9);
		ResLayer2(&fh->responseMap[1],w,   h,   s,   15);
		ResLayer3(&fh->responseMap[2],w,   h,   s,   21);
		ResLayer4(&fh->responseMap[3],w,   h,   s,   27);
  }

   if (fh->octaves >= 2)
  {
		ResLayer5(&fh->responseMap[4],w/2, h/2, s*2, 39);
		ResLayer6(&fh->responseMap[5],w/2, h/2, s*2, 51);
  }

   if (fh->octaves >= 3)
  {
		ResLayer7(&fh->responseMap[6],w/4, h/4, s*4, 75);
		ResLayer8(&fh->responseMap[7],w/4, h/4, s*4, 99);
  }

	 if (fh->octaves >= 4)
  {
		ResLayer9(&fh->responseMap[8],w/8, h/8, s*8, 147);
		ResLayer10(&fh->responseMap[9],w/8, h/8, s*8, 195);
  }

	  if (fh->octaves >= 5)
  {
		ResLayer11(&fh->responseMap[10],w/16, h/16, s*16, 291);
		ResLayer12(&fh->responseMap[11],w/16, h/16, s*16, 387);
  }

	// Extract responses from the image
	for (i = 0; i<12 ; ++i)
	{
		buildResponseLayer(fh, &fh->responseMap[i]);
	}
}

void buildResponseLayer(FastHessian *fh, ResponseLayer *rl)
{
	float *responses;				// response storage
	unsigned char *laplacian;		// laplacian sign storage
	int step;						// step size for this filter
	int b;							// border for this filter
	int l;							// lobe for this filter (filter size / 3)
	int w;							// filter size
	float inverse_area;				// normalisation factor
	float Dxx, Dyy, Dxy;
	int r, c, ar = 0, index = 0,ac;

	responses = rl->responses;
	laplacian = rl->laplacian;
	step = rl->step;
	b = (rl->filter - 1) / 2;
	l = rl->filter / 3;
	w = rl->filter;
	inverse_area = 1.f/(w*w);

	for(ar = 0; ar < rl->height; ++ar) 
		{
			for(ac = 0; ac < rl->width; ++ac, index++) 
				{
					// get the image coordinates
					r = ar * step;
					c = ac * step; 

					// Compute response components
				  Dxx = BoxIntegral(fh->img, r - l + 1, c - b, 2*l - 1, w)
					  - BoxIntegral(fh->img, r - l + 1, c - l / 2, 2*l - 1, l)*3;

				  Dyy = BoxIntegral(fh->img, r - b, c - l + 1, w, 2*l - 1)
					  - BoxIntegral(fh->img, r - l / 2, c - l + 1, l, 2*l - 1)*3;

				  Dxy = + BoxIntegral(fh->img, r - l, c + 1, l, l)
						+ BoxIntegral(fh->img, r + 1, c - l, l, l)
						- BoxIntegral(fh->img, r - l, c - l, l, l)
						- BoxIntegral(fh->img, r + 1, c + 1, l, l);

				  // Normalise the filter responses with respect to their size
				  Dxx *= inverse_area;
				  Dyy *= inverse_area;
				  Dxy *= inverse_area;

				  // Get the determinant of hessian response & laplacian sign
					responses[index] = (Dxx * Dyy - 0.81f * Dxy * Dxy);
					laplacian[index] = (Dxx + Dyy >= 0 ? 1 : 0);

				}
		}
}

int isExtremum(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, FastHessian* fh)
{
	float candidate;
	int rr, cc;

	// bounds check
	int layerBorder = (t->filter + 1) / (2 * t->step);

	if (r <= layerBorder || r >= t->height - layerBorder || c <= layerBorder || c >= t->width - layerBorder)
    return 0;

	// check the candidate point in the middle layer is above thresh 
	candidate = getResponse(r, c, t, m);
	if (candidate < fh->thresh) 
    return 0; 

	for (rr = -1; rr <=1; ++rr)
		{
		for (cc = -1; cc <=1; ++cc)
			{
				if(
					get_Response(r+rr, c+cc, t) >= candidate ||
					((rr != 0 || cc != 0) && getResponse(r+rr, c+cc, t, m) >= candidate) ||
					getResponse(r+rr, c+cc, t, b) >= candidate
					)
					return 0;
		}
	}


	return 1;
}

//! Interpolate scale-space extrema to subpixel accuracy to form an image feature. 
void interpolateExtremum(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, FastHessian *fh)
{
	double xi = 0, xr = 0, xc = 0;

	// get the step distance between filters
	// check the middle filter is mid way between top and bottom
	
	int filterStep = (m->filter - b->filter);
	assert(filterStep > 0 && t->filter - m->filter == m->filter - b->filter);

	// Get the offsets to the actual location of the extremum
	interpolateStep(r, c, t, m, b, &xi, &xr, &xc );

	if( fabs( xi ) < 0.5f  &&  fabs( xr ) < 0.5f  &&  fabs( xc ) < 0.5f )
	{
		Ipoint ipt;
		ipt.x = (float)((c + xc) * t->step);
		ipt.y = (float)((r + xr) * t->step);
		ipt.scale = (float)((0.1333f) * (m->filter + xi * filterStep));
		ipt.laplacian = (int)(getLaplacian(r,c,t,m));
		fh->ipts[count] = ipt;
		count++;
	}
	
}

//! Performs one step of extremum interpolation. 
void interpolateStep(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b, 
                                  double* xi, double* xr, double* xc )
{
	CvMat* dD, * H, * H_inv, X;
	double x[3] = { 0 };

	dD = deriv3D( r, c, t, m, b );
	H = hessian3D( r, c, t, m, b );
	H_inv = cvCreateMat( 3, 3, CV_64FC1 );
	cvInvert( H, H_inv, CV_SVD );
	cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
	cvGEMM( H_inv, dD, -1, NULL, 0, &X, 0 );

	cvReleaseMat( &dD );
	cvReleaseMat( &H );
	cvReleaseMat( &H_inv );

	*xi = x[2];
	*xr = x[1];
	*xc = x[0];
}

//! Computes the partial derivatives in x, y, and scale of a pixel.
CvMat* deriv3D(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
  CvMat* dI;
  double dx, dy, ds;

  dx = (getResponse(r, c + 1, t, m) - getResponse(r, c - 1, t, m)) / 2.0;
  dy = (getResponse(r + 1, c, t, m) - getResponse(r - 1, c, t, m)) / 2.0;
  ds = (get_Response(r, c, t) - getResponse(r, c, t, b)) / 2.0;
  
  dI = cvCreateMat( 3, 1, CV_64FC1 );
  cvmSet( dI, 0, 0, dx );
  cvmSet( dI, 1, 0, dy );
  cvmSet( dI, 2, 0, ds );

  return dI;
}

//! Computes the 3D Hessian matrix for a pixel.
CvMat* hessian3D(int r, int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
  CvMat* H;
  double v, dxx, dyy, dss, dxy, dxs, dys;

  v = getResponse(r, c, t, m);
  dxx = getResponse(r, c + 1, t, m) + getResponse(r, c - 1, t, m) - 2 * v;
  dyy = getResponse(r + 1, c, t, m) + getResponse(r - 1, c, t, m) - 2 * v;
  dss = get_Response(r, c, t) + getResponse(r, c, t, b) - 2 * v;
  dxy = ( getResponse(r + 1, c + 1, t, m) - getResponse(r + 1, c - 1, t, m) - 
          getResponse(r - 1, c + 1, t, m) + getResponse(r - 1, c - 1, t, m) ) / 4.0;
  dxs = ( get_Response(r, c + 1, t) - get_Response(r, c - 1, t) - 
          getResponse(r, c + 1, t, b) + getResponse(r, c - 1, t, b) ) / 4.0;
  dys = ( get_Response(r + 1, c, t) - get_Response(r - 1, c, t) - 
          getResponse(r + 1, c, t, b) + getResponse(r - 1, c, t, b) ) / 4.0;

  H = cvCreateMat( 3, 3, CV_64FC1 );
  cvmSet( H, 0, 0, dxx );
  cvmSet( H, 0, 1, dxy );
  cvmSet( H, 0, 2, dxs );
  cvmSet( H, 1, 0, dxy );
  cvmSet( H, 1, 1, dyy );
  cvmSet( H, 1, 2, dys );
  cvmSet( H, 2, 0, dxs );
  cvmSet( H, 2, 1, dys );
  cvmSet( H, 2, 2, dss );

  return H;
}

