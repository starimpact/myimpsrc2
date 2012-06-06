#include "imp_ga_image_algo.h"

#include <stdlib.h>
#include <string.h>


#define cast_macro(x) (x)
#define cast_sqr_macro(x) ((x)*(x))
#define CV_NULLPTR_ERR (-1)
//The _nassert intrinsic can convey information about the alignment of pointers and arrays.
#define WORD_ALIGNED(x) (assert(((int)(x) & 0x3) == 0))



int ipIntegralOrigial(const IMP_U8 *src, //const unsigned char src
			   IMP_S32*  sum,       // int inter
               IMP_S32* sqsum,   //int inter1
               IMP_S32* tilted,    // no used
			   int height,
               int width,
			   int srcstep,
               int sumstep,
			   int sqsumstep,
               int tiltedstep      //no used
			)
{
    int x, y;
    IMP_S32 s, it, t, tq;
    IMP_S32 sq;
    WORD_ALIGNED(src);
    WORD_ALIGNED(sum);
    WORD_ALIGNED(sqsum);
    memset( sum, 0, (width+1)*sizeof(sum[0]));
    sum += sumstep + 1;

    memset( sqsum, 0, (width+1)*sizeof(sqsum[0]));
    sqsum += sqsumstep + 1;
    for( y = 0; y < height; y++, src += srcstep, sum += sumstep, sqsum += sqsumstep )
	{
		sum[-1] = 0;
		sqsum[-1] = 0;

		for( x = 0, s = 0, sq = 0; x < width; x++ )
		{
			it = src[x];
			t = cast_macro(it);
			tq = cast_sqr_macro(it);
			s += t;
			sq += tq;
			t = sum[x - sumstep] + s;
			tq = sqsum[x - sqsumstep] + sq;
			sum[x] = t;
			sqsum[x] = tq;
		}

    }
    return 1;
}

IMP_VOID ipIntegral(IMP_U8 *src, IMP_U16 stride1, IMP_U64 * dst, int stride2, IMP_U16 height, IMP_U16 width)
{
	int x, y;
	IMP_S32 s, it;
	IMP_U64 tq, t, tmp;
    IMP_U64 sq;
	int offset1 = stride1 - width;
	int offset2 = stride2 - width;
	IMP_U8 *p1;
	IMP_U64 *p2;

	p1 = src;
	p2 = dst;

	// clear first line
	memset( dst, 0, (width + 1) * sizeof(dst[0]));
	//skip one line and one pixel
	p2 += stride2 + 1;
	// for each line
	for ( y = 0; y < height; y++ )
	{
		*(p2 - 1) = 0;
		// for each pixel
		for ( x = 0, s = 0, sq = 0; x < width; x++, p1++, p2++ )
		{
			it = (*p1);
			t = cast_macro(it);
			tq = cast_sqr_macro(it);
			s += t;
			sq += tq;
			t = ((IMP_S32)((*(p2 - stride2)) >> 36) + s) & 0xFFFFFF ;//积分图占用36-60位, 共24位
			tq = (*(p2 - stride2) + sq) & 0xFFFFFFFFF;//平方积分图占用0-35位
			tmp = (t << 36);
			*p2 =  tmp+ tq;
		}
		p1 += offset1;
		p2 += offset2;
	}
}

