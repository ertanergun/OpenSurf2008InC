#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include "type.h"

unsigned char data[262144];
//unsigned char tempData[1048576];
unsigned char tempData[512][512];
int width, height, vectorSize;

/*-------STRUCTURES---------*/
typedef struct {int rows; int cols; unsigned char* data;} sImage;

/*----------GET IMAGE INFO SUBPROGRAM--------------*/
long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
  unsigned char			*ptrC;
  long				value = 0L;
  unsigned char			dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, offset, SEEK_SET);

  for(i=1; i<=numberOfChars; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    /* calculate value based on adding bytes */
    value = (long)(value + (*ptrC)*(pow(256, (i-1))));
  }
  return(value);

} /* end of getImageInfo */

unsigned char* bmpImageLoad(char* fileName)
{
  FILE			*bmpInput;
  sImage		originalImage;
  unsigned char		someChar;
  unsigned char*	pChar;
  int			nColors;
  long			fileSize;
  int		    r, c;
  int count = 0;

  /* initialize pointer */
  someChar = '0';
  pChar = &someChar;

  printf("Reading filename %s\n", fileName);

  /*--------READ INPUT FILE------------*/
  bmpInput = fopen(fileName, "rb");
  fseek(bmpInput, 0L, SEEK_END);

  /*--------GET BMP DATA---------------*/
  originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
  originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
  fileSize = getImageInfo(bmpInput, 2, 4);
  nColors = getImageInfo(bmpInput, 46, 4);
  vectorSize = fileSize - (14 + 40 + 4*nColors);

  width = originalImage.cols;
  height = originalImage.rows;

  /*-------PRINT TO SCREEN-------------*/
  printf("Width: %d\n", originalImage.cols);
  printf("Height: %d\n", originalImage.rows);
  printf("File size: %ld\n", fileSize);
  printf("# Colors: %d\n", nColors);
  printf("Vector size: %d\n", vectorSize);

  /*----------READ RASTER DATA---------*/
  fseek(bmpInput, (54 + 4*nColors), SEEK_SET);
  
  for(r=0; r<=originalImage.rows - 1; r++)
  {
    for(c=0; c<=originalImage.cols - 1; c++)
    {
      fread(pChar, sizeof(char), 1, bmpInput);

	  *(data + count) = *pChar;
	  count++;
    }
  }

  fclose(bmpInput);
  return data;
}

int getWidthStep(int nOfChannels, int width)
{
	int widthstep = width*sizeof(float)*nOfChannels;
	return widthstep;
}

void constructIlpImage(char* fileName, IplImage* img)
{
	img->nChannels = 1;
	img->imageData = bmpImageLoad(fileName);
	img->width = width;
	img->height = height;
	img->widthStep = getWidthStep(img->nChannels, width);
	img->imageSize = vectorSize; //height * img->widthStep;
	img->intImage = 0;
}



