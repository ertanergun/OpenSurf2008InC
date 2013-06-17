typedef struct _bmpImage
{
	int  widthStep;         /* Size of aligned image row in bytes.    */
	int  nChannels;         
	int  width;             /* Image width in pixels.                           */
    int  height;            /* Image height in pixels.                          */
	unsigned char imageData[512][512];      /* 2D image array */
	int  imageSize;         /* Image data size in bytes */
}
bmpImage;