#ifndef __CVAUX_H__
#define __CVAUX_H__

#if _MSC_VER >= 1200
#pragma warning( disable: 4251 4710 4711 4514 4996 ) /* function AAA selected for automatic inline expansion */
#endif

#include "cvaux.h"
#include "cxmisc.h"
#include "_cvmatrix.h"
#include "cvinternal.h"

typedef unsigned short ushort;

CV_INLINE bool operator == (CvSize size1, CvSize size2 );
CV_INLINE bool operator == (CvSize size1, CvSize size2 )
{
    return size1.width == size2.width && size1.height == size2.height;
}

CV_INLINE bool operator != (CvSize size1, CvSize size2 );
CV_INLINE bool operator != (CvSize size1, CvSize size2 )
{
    return size1.width != size2.width || size1.height != size2.height;
}

#endif /* __CVAUX_H__ */