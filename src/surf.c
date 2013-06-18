#include "utils.h"
#include "surf.h"

//-------------------------------------------------------
//! SURF priors (these need not be done at runtime)
const float pi = 3.14159f;

//! lookup table for 2d gaussian (sigma = 2.5) where (0,0) is top left and (6,6) is bottom right
const double gauss25 [7][7] = {
  0.02546481,	0.02350698,	0.01849125,	0.01239505,	0.00708017,	0.00344629,	0.00142946,
  0.02350698,	0.02169968,	0.01706957,	0.01144208,	0.00653582,	0.00318132,	0.00131956,
  0.01849125,	0.01706957,	0.01342740,	0.00900066,	0.00514126,	0.00250252,	0.00103800,
  0.01239505,	0.01144208,	0.00900066,	0.00603332,	0.00344629,	0.00167749,	0.00069579,
  0.00708017,	0.00653582,	0.00514126,	0.00344629,	0.00196855,	0.00095820,	0.00039744,
  0.00344629,	0.00318132,	0.00250252,	0.00167749,	0.00095820,	0.00046640,	0.00019346,
  0.00142946,	0.00131956,	0.00103800,	0.00069579,	0.00039744,	0.00019346,	0.00008024
};

//-------------------------------------------------------

void Surf_Constractor(IplImage *img, FastHessian *fh, int upright, Ipoint *ipts, int size)
{
	int i;
	Surf Sthis = {img,fh->ipts ,0 }; 

	Sthis.img = img;

	for (i = 0; i<size; i++)
	Sthis.ipts[i] = fh->ipts[i];

	// Extract the descriptors for the ipts
	getDescriptors(upright, &Sthis, size);
	
	for (i = 0; i<size; i++)
	ipts[i] = Sthis.ipts[i];


}

void getDescriptors(int upright, Surf* S, int ipts_size)
{
	int i;

	if (ipts_size == 0 ) return;

	if (upright == 1)
	{	
		    // U-SURF loop just gets descriptors
		for (i = 0; i < ipts_size; ++i)
		{
			// Set the Ipoint to be described
			S->index = i;

			// Extract upright (i.e. not rotation invariant) descriptors
			getDescriptor(1, S);
		}
	}
	 else
	{	
		// Main SURF-64 loop assigns orientations and gets descriptors
		for (i = 0; i < ipts_size; ++i)
		{
			// Set the Ipoint to be described
			S->index = i;

			// Assign Orientations and extract rotation invariant descriptors
			getOrientation(S);
			getDescriptor(0,S);
		}
	}
}

//! Assign the supplied Ipoint an orientation
void getOrientation(Surf* S)
{
	Ipoint *ipt = &S->ipts[S->index];
	float gauss = 0.f, scale = ipt->scale;
	const int s = fRound(scale), r = fRound(ipt->y), c = fRound(ipt->x);
	float resX[109], resY[109], Ang[109];
	const int id[] = {6,5,4,3,2,1,0,1,2,3,4,5,6};
	int i,j;
	unsigned int k;
	

	int idx = 0;

	// calculate the dominant direction 
	float sumX=0.f, sumY=0.f;
	float max=0.f, orientation = 0.f;
	float ang1=0.f, ang2=0.f;
	float Ang_size;
	float ang;

	// calculate haar responses for points within radius of 6*scale
	for(i = -6; i <= 6; ++i) 
	{
		for(j = -6; j <= 6; ++j) 
		{
			if(i*i + j*j < 36) 
			{
				gauss = (float)(gauss25[id[i+6]][id[j+6]]);  // could use abs() rather than id lookup, but this way is faster
				resX[idx] = gauss * haarX(r+j*s, c+i*s, 4*s, S);
				resY[idx] = gauss * haarY(r+j*s, c+i*s, 4*s, S);
				Ang[idx] = getAngle(resX[idx], resY[idx]);
				++idx;
			}
		}
	}

	Ang_size = sizeof(Ang)/sizeof(float);

	// loop slides pi/3 window around feature point
	for(ang1 = 0; ang1 < 2*pi;  ang1+=0.15f) {
    ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
    sumX = sumY = 0.f; 
	
		for(k = 0; k < Ang_size; ++k) 
		{
			// get angle from the x-axis of the sample point
			 ang = Ang[k];

			// determine whether the point is within the window
			if (ang1 < ang2 && ang1 < ang && ang < ang2) 
			{
				sumX+=resX[k];  
				sumY+=resY[k];
			} 
			else if (ang2 < ang1 && 
				((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2*pi) )) 
			{
				sumX+=resX[k];  
				sumY+=resY[k];
			}
		}

		// if the vector produced from this window is longer than all 
		// previous vectors then this forms the new dominant direction
		if (sumX*sumX + sumY*sumY > max) 
		{
			// store largest orientation
			max = sumX*sumX + sumY*sumY;
			orientation = getAngle(sumX, sumY);
		}
	}
		// assign orientation of the dominant response vector
		ipt->orientation = orientation;
}


//! Get the modified descriptor. See Agrawal ECCV 08
//! Modified descriptor contributed by Pablo Fernandez
void getDescriptor(int bUpright, Surf* S)
{
  int y, x, sample_x, sample_y, count=0;
  int i = 0, ix = 0, j = 0, jx = 0, xs = 0, ys = 0;
  int k,l;
  float scale, *desc, dx, dy, mdx, mdy, co, si;
  float gauss_s1 = 0.f, gauss_s2 = 0.f;
  float rx = 0.f, ry = 0.f, rrx = 0.f, rry = 0.f, len = 0.f;
  float cx = -0.5f, cy = 0.f; //Subregion centers for the 4x4 gaussian weighting

 Ipoint *ipt = &S->ipts[S->index];
  scale = ipt->scale;
  x = fRound(ipt->x);
  y = fRound(ipt->y);  
  desc = ipt->descriptor;

    if (bUpright == 1)
  {
    co = 1;
    si = 0;
  }
  else
  {
    co = cos(ipt->orientation);
    si = sin(ipt->orientation);
  }

  i = -8;

    //Calculate descriptor for this interest point
  while(i < 12)
  {
    j = -8;
    i = i-4;

    cx += 1.f;
    cy = -0.5f;
	
	while(j < 12) 
    {
      dx=dy=mdx=mdy=0.f;
      cy += 1.f;

      j = j - 4;

      ix = i + 5;
      jx = j + 5;

      xs = fRound(x + ( -jx*scale*si + ix*scale*co));
      ys = fRound(y + ( jx*scale*co + ix*scale*si));
	

	  for (k = i; k < i + 9; ++k) 
      {
        for (l = j; l < j + 9; ++l) 
        {
		  //Get coords of sample point on the rotated axis
          sample_x = fRound(x + (-l*scale*si + k*scale*co));
          sample_y = fRound(y + ( l*scale*co + k*scale*si));

		  //Get the gaussian weighted x and y responses
          gauss_s1 = gaussian(xs-sample_x,ys-sample_y,2.5f*scale);
          rx = haarX(sample_y, sample_x, 2*fRound(scale), S);
          ry = haarY(sample_y, sample_x, 2*fRound(scale), S);

		  //Get the gaussian weighted x and y responses on rotated axis
          rrx = gauss_s1*(-rx*si + ry*co);
          rry = gauss_s1*(rx*co + ry*si);

          dx += rrx;
          dy += rry;
          mdx += fabs(rrx);
          mdy += fabs(rry);
		  
		}
	  }

	  //Add the values to the descriptor vector
      gauss_s2 = _gaussian(cx-2.0f,cy-2.0f,1.5f);

      desc[count++] = dx*gauss_s2;
      desc[count++] = dy*gauss_s2;
      desc[count++] = mdx*gauss_s2;
      desc[count++] = mdy*gauss_s2;

      len += (dx*dx + dy*dy + mdx*mdx + mdy*mdy) * gauss_s2*gauss_s2;

      j += 9;
	}
	i += 9;
  }
  //Convert to Unit Vector
  len = sqrt(len);
  for(i = 0; i < 64; ++i)
    desc[i] /= len;
}


//! Calculate the value of the 2d gaussian at x,y
_inline float gaussian(int x, int y, float sig)
{
  return (1.0f/(2.0f*pi*sig*sig)) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
_inline float _gaussian(float x, float y, float sig)
{
  return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in x direction
_inline float haarX(int row, int column, int s, Surf* S)
{
  return BoxIntegral(S->img, row-s/2, column, s, s/2) 
    -1 * BoxIntegral(S->img, row-s/2, column-s/2, s, s/2);
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in y direction
_inline float haarY(int row, int column, int s, Surf* S)
{
  return BoxIntegral(S->img, row, column-s/2, s/2, s) 
    -1 * BoxIntegral(S->img, row-s/2, column-s/2, s/2, s);
}

//-------------------------------------------------------

//! Get the angle from the +ve x-axis of the vector given by (X Y)
float getAngle(float X, float Y)
{
  if(X > 0 && Y >= 0)
    return atan(Y/X);

  if(X < 0 && Y >= 0)
    return pi - atan(-Y/X);

  if(X < 0 && Y < 0)
    return pi + atan(Y/X);

  if(X > 0 && Y < 0)
    return 2*pi - atan(-Y/X);

  return 0;
}