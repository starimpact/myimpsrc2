#include "imp_ga_image_algo.h"


IMP_VOID ipImageSub(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 offset3 = stride3 - width;
	IMP_U8 *p1, *p2, *p3;
	p1 = src1;
	p2 = src2;
	p3 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++, p3++)
		{
			(*p3) = abs((*p1) - (*p2));
		}
		p1 += offset1;
		p2 += offset2;
		p3 += offset3;
	}
}


IMP_VOID ipImageDiff(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_S32 threshold, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	const IMP_U8 cst0 = 0;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 offset3 = stride3 - width;
	IMP_U8 *p1, *p2, *p3;
	IMP_S8 det;
	p1 = src1;
	p2 = src2;
	p3 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++, p3++)
		{
			det = ( ((*p1) >> 1) & 0x7F ) - ( ((*p2) >> 1) & 0x7F );
			(*p3) = abs( det ) >= threshold ? (IMP_U8)det : cst0;
		}
		p1 += offset1;
		p2 += offset2;
		p3 += offset3;
	}
}


IMP_VOID ipImageSubNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++)
		{
			(*p2) = abs((*p1) - n);
		}
		p1 += offset1;
		p2 += offset2;
	}
}

IMP_VOID ipImageAddNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst,  IMP_U16 stride2, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++)
		{
			(*p2) = IMP_MIN((*p1) + n, 255);
		}
		p1 += offset1;
		p2 += offset2;
	}
}



IMP_VOID ipImageAdd(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 offset3 = stride3 - width;
	IMP_U8 *p1, *p2, *p3;
	p1 = src1;
	p2 = src2;
	p3 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++, p3++)
		{
			(*p3) = IMP_MIN((*p1) + (*p2), 255);
		}
		p1 += offset1;
		p2 += offset2;
		p3 += offset3;
	}
}

IMP_VOID ipImageAnd(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height)
{

	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 offset3 = stride3 - width;
	IMP_U8 *p1, *p2, *p3;
	p1 = src1;
	p2 = src2;
	p3 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++, p3++)
		{
			(*p3) = (*p1) & (*p2);
		}
		p1 += offset1;
		p2 += offset2;
		p3 += offset3;
	}
}

IMP_VOID ipImageAndNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n, IMP_U8 *dst,  IMP_U16 stride2, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++)
		{
			(*p2) = (*p1) & n;
		}
		p1 += offset1;
		p2 += offset2;
	}
}


IMP_VOID ipImageOr(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 offset3 = stride3 - width;
	IMP_U8 *p1, *p2, *p3;
	p1 = src1;
	p2 = src2;
	p3 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++, p3++)
		{
			(*p3) = (*p1) | (*p2);
		}
		p1 += offset1;
		p2 += offset2;
		p3 += offset3;
	}
}

IMP_VOID ipImageOrNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst,  IMP_U16 stride2, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 i, j;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;
	for (j =0; j< height; j++)
	{
		for (i = 0; i< width; i++, p1++, p2++)
		{
			(*p2) = (*p1) | n;
		}
		p1 += offset1;
		p2 += offset2;
	}
}


IMP_VOID ipImageBinary(IMP_U8 *src,IMP_U8 *dst,IMP_U16 width,IMP_U16 height,IMP_U8 threshold)
{
	IMP_S32 i;
	for (i = 0; i < width * height; i++)
		dst[i] = src[i] > threshold ? 255 : 0;
}


IMP_VOID ipImageThreshold (IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2,
					   IMP_U16 width, IMP_U16 height,
					   IMP_U8 threshold, IMP_U8 maxValue, IMP_U8 minValue, IMP_U8 mode)
{
	IMP_S32 x, y;

	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// for each line
	for ( y = 0; y < height; y++ )
	{
		// for each pixel
		for ( x = 0; x < width; x++, p1++, p2++ )
		{
			if (mode == 0)
			{
				(*p1) > threshold ? ((*p2) = maxValue) : ((*p2) = minValue);
			}
			else if (mode == 1)
			{
				*p1 > threshold ? ((*p2) = maxValue) : ((*p2) = (*p1));
			}
			else if (mode == 2)
			{
				*p1 > threshold ? ((*p2) = (*p1)) : ((*p2) = minValue);
			}
		}
		p1 += offset1;
		p2 += offset2;
	}
}

IMP_S32 ipOtsu (IMP_U8 *image, IMP_S32 rows, IMP_S32 cols, IMP_S32 x0, IMP_S32 y0, IMP_S32 dx, IMP_S32 dy, IMP_S32 vvv)
{
	IMP_U8 *np;
	IMP_S32 thresholdValue=1;
	IMP_S32 ihist[256];
	IMP_S32 i, j, k;
	IMP_S32 n, n1, n2, gmin, gmax;
	IMP_DOUBLE m1, m2, sum, csum, fmax, sb;

	memset(ihist, 0, sizeof(ihist));
	gmin=255; gmax=0;

	for (i = y0 + 1; i < y0 + dy - 1; i++)
	{
		np = &image[i*cols+x0+1];
		for (j = x0 + 1; j < x0 + dx - 1; j++)
		{
			ihist[*np]++;
			if(*np > gmax) gmax=*np;
			if(*np < gmin) gmin=*np;
			np++;
		}
	}
	sum = csum = 0.0;
	n = 0;
	for (k = 0; k <= 255; k++)
	{
		sum += (IMP_DOUBLE) k * (IMP_DOUBLE) ihist[k];
		n += ihist[k];
	}
	if (!n)
	{

		fprintf (stderr, "NOT NORMAL thresholdValue = 160 ");
		return (160);
	}

	fmax = -1.0;
	n1 = 0;
	for (k = 0; k < 255; k++)
	{
		n1 += ihist[k];
		if (!n1) { continue; }
		n2 = n - n1;
		if (n2 == 0) { break; }
		csum += (IMP_DOUBLE) k *ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (IMP_DOUBLE) n1 *(IMP_DOUBLE) n2 *(m1 - m2) * (m1 - m2);

		if (sb > fmax) {
			fmax = sb;
			thresholdValue = k;
		}
	}
	if ( vvv & 1 )
		fprintf(stderr,"# OTSU: thresholdValue = %d gmin=%d gmax=%d ",
		thresholdValue, gmin, gmax);
	return(thresholdValue);
}

IMP_S32 ipImageAdptiveBinary(IMP_U8 *src,IMP_U8 *dst,IMP_U16 width,IMP_U16 height)
{

	IMP_S32 i, threshold;

	threshold = ipOtsu(src,height,width,0,0,width,height,0);
	for (i = 0; i < width * height; i++)
		dst[i] = src[i] > threshold ? 255 : 0;

	return threshold;
}



IMP_VOID ipMedianFilter3_3(IMP_U8 * src,IMP_U8 * dst,IMP_U16 width,IMP_U16 height)
{

	IMP_S32 size=3;
	IMP_S32 offset = width;
	IMP_S32 i, j, t,x,y;

	IMP_S32 radius = size >> 1;
	IMP_U8 g[3 * 3];
	IMP_S32 c;

	IMP_U8 * src1 = (IMP_U8 *) src;
	IMP_U8 * dst1 = (IMP_U8 *) dst;

	// for each line
	for ( y = 0; y < height; y++ )
	{
		// for each pixel
		for ( x = 0; x < width; x++, src1++, dst1++ )
		{
			c = 0;

			// for each kernel row
			for ( i = -radius; i <= radius; i++ )
			{
				t = y + i;

				// skip row
				if ( t < 0 )
					continue;
				// break
				if ( t >= height )
					break;

				// for each kernel column
				for ( j = -radius; j <= radius; j++ )
				{
					t = x + j;

					// skip column
					if ( t < 0 )
						continue;

					if ( t < width )
					{
						g[c++] = *(src1+i * offset + j);
					}
				}
			}
			// sort elements
			ipQuickSort( g, 0, c );
			// get the median
			*dst1 = g[c >> 1];
		}
	}
}

IMP_VOID ipMedianFilter5_5(IMP_U8 * src,IMP_U8 * dst,IMP_U16 width,IMP_U16 height)
{
	IMP_S32 size=5;
	IMP_S32 offset = width;
	IMP_S32 i, j, t,x,y;

	IMP_S32 radius = size >> 1;
	IMP_U8 g[5 * 5];
	IMP_S32 c;

	IMP_U8 * src1 = (IMP_U8 *) src;
	IMP_U8 * dst1 = (IMP_U8 *) dst;

	// for each line
	for ( y = 0; y < height; y++ )
	{
		// for each pixel
		for ( x = 0; x < width; x++, src1++, dst1++ )
		{
			c = 0;

			// for each kernel row
			for ( i = -radius; i <= radius; i++ )
			{
				t = y + i;

				// skip row
				if ( t < 0 )
					continue;
				// break
				if ( t >= height )
					break;

				// for each kernel column
				for ( j = -radius; j <= radius; j++ )
				{
					t = x + j;

					// skip column
					if ( t < 0 )
						continue;

					if ( t < width )
					{
						g[c++] = *(src1+i * offset + j);
					}
				}
			}
			// sort elements
			ipQuickSort( g, 0, c );
			// get the median
			*dst1 = g[c >> 1];
		}
	}
}

IMP_DOUBLE ipGaussFunction( IMP_DOUBLE x, IMP_DOUBLE sigma, IMP_DOUBLE sqrSigma )
{
	return exp( x * x / ( -2 * sqrSigma ) ) / ( sqrt(2 * IMP_PI ) * sigma );
}
IMP_DOUBLE ipGaussFunction2D( IMP_DOUBLE x, IMP_DOUBLE y,IMP_DOUBLE sigma, IMP_DOUBLE sqrSigma )
{
	return exp( ( x * x + y * y ) / ( -2 * sqrSigma ) ) / ( 2 * IMP_PI * sqrSigma );
}
IMP_S32 ipGaussKernel( IMP_S32 size, IMP_DOUBLE sigma, IMP_DOUBLE sqrSigma, IMP_DOUBLE kernel[] )
{
	// check for evem size and for out of range
	IMP_S32 r,x,i;
	if ( ( (size % 2 ) == 0 ) || ( size < 3 ) || ( size > 101 ) )
	{
		return -1;
	}

	// raduis
	r = size / 2;
	// compute kernel
	for ( x = -r, i = 0; i < size; x++, i++ )
	{
		kernel[i] = ipGaussFunction( x,sigma,sqrSigma );
	}

	return 1;
}
IMP_S32 ipGaussKernel2D( IMP_DOUBLE sigma, IMP_DOUBLE sqrSigma, IMP_DOUBLE kernel[3][3])
{
	IMP_S32 r,x,y,i,j;
	IMP_S32 size = 3;
	// check for evem size and for out of range
	if ( ( ( size % 2 ) == 0 ) || ( size < 3 ) || ( size > 101 ) )
	{
		return -1;
	}

	// raduis
	r = size / 2;
	// compute kernel
	for ( y = -r, i = 0; i < size; y++, i++ )
	{
		for ( x = -r, j = 0; j < size; x++, j++ )
		{
			kernel[i][j] = ipGaussFunction2D( x, y,sigma,sqrSigma );
		}
	}

	return 1;
}



IMP_VOID ipImageConvolution( IMP_U8 *src, IMP_U8 *dst,IMP_U16 width,IMP_U16 height,IMP_S32 *kernel,IMP_S32 size )
{
	// get source image size
	IMP_S32 offset = width;

	// loop and array indexes
	IMP_S32 i, j, t, k, ir, jr,x,y;
	// kernel's radius
	IMP_S32 radius = size >> 1;
	// color sums
	IMP_S64 g, div;

	IMP_U8 * src1 = (IMP_U8 *) src;
	IMP_U8 * dst1 = (IMP_U8 *) dst;

	// do the processing job
	// grayscale image

	// for each line
	for ( y = 0; y < height; y++ )
	{
		// for each pixel
		for ( x = 0; x < width; x++, src1++, dst1++ )
		{
			g = div = 0;

			// for each kernel row
			for ( i = 0; i < size; i++ )
			{
				ir = i - radius;
				t = y + ir;

				// skip row
				if ( t < 0 )
					continue;
				// break
				if ( t >= height )
					break;

				// for each kernel column
				for ( j = 0; j < size; j++ )
				{
					jr = j - radius;
					t = x + jr;

					// skip column
					if ( t < 0 )
						continue;

					if ( t < width )
					{
						k = kernel[i * size + j];

						div += k;
						g += k * src1[ir * offset + jr];
					}
				}
			}

			// check divider
			if ( div != 0 )
			{
				g /= div;
			}
			*dst1 = ( g > 255 ) ? (IMP_U8) 255 : ( ( g < 0 ) ? (IMP_U8) 0 : (IMP_U8) g );
		}
	}
}

IMP_VOID ipGaussFilter3_3(IMP_U8 * src, IMP_U16 stride1,IMP_U8 * dst, IMP_U16 stride2,IMP_S8 * mask, IMP_U16 width,IMP_U16 height)
{
	IMP_S32 x, y, sum;

	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;

	IMP_S32 i00, i01, i02;
	IMP_S32 i10, i11, i12;
	IMP_S32 i20, i21, i22;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip one stride
	p1 += stride1;
	p2 += stride2;

	// for each line
	for ( y = 1; y < heightM1; y++ )
	{
		p1++;
		p2++;
		// for each pixel
		for ( x = 1; x < widthM1; x++, p1++, p2++ )
		{
			sum = 0;
			i00 = mask[0] * (*(p1 - stride1 - 1)); i01 = mask[1] * (*(p1 - stride1)); i02 = mask[2] * (*(p1 - stride1 + 1));
			i10 = mask[3] * (*(p1 - 1));           i11 = mask[4] * (*(p1));           i12 = mask[5] * (*(p1 + 1));
			i20 = mask[6] * (*(p1 + stride1 - 1)); i21 = mask[7] * (*(p1 + stride1)); i22 = mask[8] * (*(p1 + stride1 + 1));

			sum += i00;	sum += i01;	sum += i02;
			sum += i10;	sum += i11;	sum += i12;
			sum += i20;	sum += i21; sum += i22;

			sum = (sum >> 4);
			(*p2) = sum;
		}
		p1 += (offset1 + 1);
		p2 += (offset2 + 1);
	}
}

IMP_VOID ipGaussFilter5_5(IMP_U8 * src, IMP_U16 stride1,IMP_U8 * dst, IMP_U16 stride2, IMP_S8 * mask, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y, sum;

	IMP_S32 widthM1 = width - 2;
	IMP_S32 heightM1 = height - 2;

	IMP_S32 i00, i01, i02, i03, i04;
	IMP_S32 i10, i11, i12, i13, i14;
	IMP_S32 i20, i21, i22, i23, i24;
	IMP_S32 i30, i31, i32, i33, i34;
	IMP_S32 i40, i41, i42, i43, i44;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 tmp1 = (stride1 << 1);
	IMP_S32 tmp2 = (stride2 << 1);

	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip two stride
	p1 += tmp1;
	p2 += tmp2;

	// for each line
	for ( y = 2; y < heightM1; y++ )
	{
		//skip two pixels
		p1 += 2;
		p2 += 2;
		// for each pixel
		for ( x = 2; x < widthM1; x++, p1++, p2++ )
		{
			i00 = *(p1 - tmp1 - 2);
			i01 = *(p1 - tmp1 - 1);
			i02 = *(p1 - tmp1);
			i03 = *(p1 - tmp1 + 1);
			i04 = *(p1 - tmp1 + 2);

			i10 = *(p1 - stride1 - 2);
			i11 = *(p1 - stride1 - 1);
			i12 = *(p1 - stride1 );
			i13 = *(p1 - stride1 + 1);
			i14 = *(p1 - stride1 + 2);

			i20 = *(p1  - 2);
			i21 = *(p1  - 1);
			i22 = *(p1);
			i23 = *(p1  + 1);
			i24 = *(p1  + 2);

			i30 = *(p1 + stride1 - 2);
			i31 = *(p1 + stride1 - 1);
			i32 = *(p1 + stride1 );
			i33 = *(p1 + stride1 + 1);
			i34 = *(p1 + stride1 + 2);

			i40 = *(p1 + tmp1 - 2);
			i41 = *(p1 + tmp1 - 1);
			i42 = *(p1 + tmp1);
			i43 = *(p1 + tmp1 + 1);
			i44 = *(p1 + tmp1 + 2);

			sum = mask[0] * i00 + mask[1] * i01 + mask[2] * i02 + mask[3] * i03 + mask[4] * i04
				+ mask[5] * i10 + mask[6] * i11 + mask[7] * i12 + mask[8] * i13 + mask[9] * i14
				+ mask[10] * i20 + mask[11] * i21 + mask[12] * i22 + mask[13] * i23 + mask[14] * i24
				+ mask[15] * i30 + mask[16] * i31 + mask[17] * i32 + mask[18] * i33 + mask[19] * i34
				+ mask[20] * i40 + mask[21] * i41 + mask[22] * i42 + mask[23] * i43 + mask[24] * i44;

			sum /= 84;
			*(p2) = sum;
		}
		p1 += (offset1 + 2);
		p2 += (offset2 + 2);
	}
}

IMP_VOID ipMeanFilter(IMP_U8 * src,IMP_U8 * dst,IMP_U16 width, IMP_U16 height)
{
	IMP_S32 i, sum;
	IMP_S32 i00, i01, i02;
	IMP_S32 i10, i11, i12;
	IMP_S32 i20, i21, i22;
	IMP_S32 w = width;

	for (i = 0; i < width*(height-2) - 2; i++)
	{
		sum = 0;
		i00=src[i ]; i01=src[i +1]; i02=src[i +2];
		i10=src[i+ w]; i11=src[i+ w+1];i12=src[i+ w+2];
		i20=src[i+2*w]; i21=src[i+2*w+1]; i22=src[i+2*w+2];

		sum += i00;	sum += i01;	sum += i02;
		sum += i10;	sum += i11;	sum += i12;
		sum += i20;	sum += i21; sum += i22;

		sum /=9;
		if (sum > 255) sum = 255;
		dst[i + 1] = sum;
	}
}

#if 0

IMP_VOID ipMedianImage( IMP_U8 *src, IMP_U8 *dst, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y;
	IMP_U8 *ptrSrc11, *ptrDst11, *ptrSrc01, *ptrDst01;

	memcpy( dst, src, (width << 1) );
	for( y = 1; y< (height - 1); y++ )
	{
		ptrSrc11 = src + width * ( y + 1) + 1;
		ptrDst11 = dst + width * ( y + 1) + 1;
		ptrSrc01 = src + width * y + 1;
		ptrDst01 = dst + width * y + 1;
		for( x = 1; x < (width - 1); x += 1 )
		{
			ptrDst11[0] = (ptrSrc11[-1] + (ptrSrc11[0] << 1)+ptrSrc11[1]) >> 2;
			ptrSrc01[0] = (ptrDst01[-width] + (ptrDst01[0] << 1) + ptrDst01[width]) >> 2;

			ptrSrc11++; ptrDst11++; ptrSrc01++; ptrDst01++;
		}
	}
}

#else

IMP_VOID ipMedianImage( IMP_U8 *src, IMP_U8 *dst, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y;
	IMP_U8 *ptrSrc11, *ptrDst11, *ptrSrc01, *ptrDst01;
	IMP_U8 i10, i11, i12;
	IMP_U8 d10, d11, d12;
	IMP_U8 *ptrSrc12, *ptrDst12, *ptrSrc02, *ptrDst02;
	IMP_U8 i20, i21, i22;
	IMP_U8 d20, d21, d22;

	memcpy( dst, src, ( width >> 1 ) );
	for( y = 1; y < (height - 1); y++ )
	{
		ptrSrc11 = src + width*(y+1) + 1;
		ptrDst11 = dst + width*(y+1) + 1;
		ptrSrc01 = src + width*y + 1;
		ptrDst01 = dst + width*y + 1;
		ptrSrc12 = src + width*(y+1) + 2;
		ptrDst12 = dst + width*(y+1) + 2;
		ptrSrc02 = src + width*y + 2;
		ptrDst02 = dst + width*y + 2;
		for( x=1; x<(width-1); x+=2 )
		{
			i10 = ptrSrc11[-1];
			i20 = ptrSrc12[-1];
			i11 = ptrSrc11[0];
			i21 = ptrSrc12[0];
			i12 = ptrSrc11[1];
			i22 = ptrSrc12[1];
			d10 = ptrDst01[-width];
			d20 = ptrDst02[-width];
			d11 = ptrDst01[0];
			d21 = ptrDst02[0];

			d12 = (i10 + (i11 << 1) + i12) >> 2;
			d22 = (i20 + (i21 << 1) + i22 ) >>2;
			ptrDst11[0] = d12;
			ptrDst12[0] = d22;
			ptrSrc01[0] = (d10 + (d11 << 1) + d12) >> 2;
			ptrSrc02[0] = (d20 + (d21 << 1) + d22) >> 2;

			ptrSrc11 += 2; ptrDst11 += 2; ptrSrc01 += 2; ptrDst01 += 2;
			ptrSrc12 += 2; ptrDst12 += 2; ptrSrc02 += 2; ptrDst02 += 2;
		}
	}
}

#endif



IMP_VOID ipConvertImageYUV2RGB(YUV_IMAGE422_S *src, RGB_IMAGE_S *dst )
{
	IMP_S32  width, height;
	IMP_S32 i, j, count8, count;
	PIXEL_S *pRGB, *p;
	IMP_U8 *Yt, *Ut, *Vt;
	IMP_U8 *Y, *U, *V;
	IMP_U8 vy, vu, vv;
	IMP_S32 dy, delta;

	width = src->s32W;
	height = src->s32H;
	pRGB = (PIXEL_S*)(dst->pu8Data);

	Yt = (IMP_U8*)(src->pu8Y);
	Ut = (IMP_U8 *)(src->pu8U);
	Vt = (IMP_U8*)(src->pu8V);

	count = width * height;
	count8 = count/8;
	for( j = 0; j < 8; j++ )
	{
		p = pRGB+j*count8;
		Y = Yt+j*count8;
		U = Ut+j*(count8 >> 1);
		V = Vt+j*(count8 >> 1);

		for( i = 0; i< count8; i++ )
		{
			vy = *Y;
			vu = *U;
			vv = *V;

			dy = 76283 * (vy - 16);
			delta = dy + 104595 * (vv - 128);
			p->u8R = (IMP_U8)min((delta >0 ? delta >> 16 : 0), 255);
			delta = dy - 53280 * (vv - 128) - 25690 * (vu - 128);
			p->u8G= (IMP_U8)min((delta>0 ? delta>>16 : 0), 255);
			delta = dy + 132186 * (vu - 128);
			p->u8B = (IMP_U8)min((delta>0 ? delta>>16 : 0), 255);
			if (i%2 == 0)
			{
				++U;
				++V;
			}
			++p;
			++Y;
		}
	}
}
IMP_VOID ipFloatConvertImageYUV2RGB(YUV_IMAGE422_S *src, RGB_IMAGE_S *dst )
{
	IMP_S32  width, height;
	IMP_S32 i, j, count8, count;
	PIXEL_S *pRGB, *p;
	IMP_U8 *Yt, *Ut, *Vt;
	IMP_U8 *Y, *U, *V;
	IMP_U8 vy, vu, vv;
	IMP_S32 dy, delta;

	width = src->s32W;
	height = src->s32H;
	pRGB = (PIXEL_S*)(dst->pu8Data);

	Yt = (IMP_U8*)(src->pu8Y);
	Ut = (IMP_U8 *)(src->pu8U);
	Vt = (IMP_U8*)(src->pu8V);

	count = width * height;
	count8 = count/8;
	for( j = 0; j < 8; j++ )
	{
		p = pRGB+j*count8;
		Y = Yt+j*count8;
		U = Ut+j*(count8 >> 1);
		V = Vt+j*(count8 >> 1);

		for( i = 0; i< count8; i++ )
		{
			vy = *Y;
			vu = *U;
			vv = *V;

			dy = 76283 * (vy - 16);
			delta = dy + 104595 * (vv - 128);
			p->u8R = (IMP_U8)min((delta >0 ? delta >> 16 : 0), 255);
			delta = dy - 53280 * (vv - 128) - 25690 * (vu - 128);
			p->u8G= (IMP_U8)min((delta>0 ? delta>>16 : 0), 255);
			delta = dy + 132186 * (vu - 128);
			p->u8B = (IMP_U8)min((delta>0 ? delta>>16 : 0), 255);
			if (i%2 == 0)
			{
				++U;
				++V;
			}
			++p;
			++Y;
		}
	}
}


IMP_U8 *clp = NULL;
IMP_U8 *clp1;
IMP_S64 crv_tab[256];
IMP_S64 cbu_tab[256];
IMP_S64 cgu_tab[256];
IMP_S64 cgv_tab[256];
IMP_S64 tab_76309[256];
IMP_U8 hist[256];
IMP_U8 hhist[64];
IMP_U8 hhhist[8];

IMP_VOID IMP_GA_InitDitherTab ()
{
	IMP_S64 crv, cbu, cgu, cgv;
	IMP_S32 i;
	crv = 104597;
	cbu = 132201;                 /* fra matrise i global.h */
	cgu = 25675;
	cgv = 53279;
	for (i = 0; i < 256; i++)
	{
		crv_tab[i] = (i - 128) * crv;
		cbu_tab[i] = (i - 128) * cbu;
		cgu_tab[i] = (i - 128) * cgu;
		cgv_tab[i] = (i - 128) * cgv;
		tab_76309[i] = 76309 * (i - 16);
		//printf("%d ",tab_76309[i]);
	}
	if (!(clp = (IMP_U8 *)malloc(sizeof(IMP_U8)*1024)))
	{
		printf("failed to malloc memory for clp!\n");
	}
	clp1 = clp;
	clp += 384;
	for (i = -384; i < 640; i++)
	{
		clp[i] = (i < 0) ? 0 : ((i > 255) ? 255 : i);
	}
}
IMP_VOID IMP_GA_FreeDitherTab ()
{
    if(clp1 != NULL)
    {
        free(clp1);
	clp1 = NULL;
    }
}

IMP_VOID ipConvertYUVtoRGB(IMP_U8 *src0, //in:
					 IMP_U8 *src1, //i
					 IMP_U8 *src2, //in:
					 IMP_U8 *dst_ori,//out :
					 IMP_S32 width,
					 IMP_S32 height)
{

  IMP_S32 y11, y21;
  IMP_S32 y12, y22;
  IMP_S32 y13, y23;
  IMP_S32 y14, y24;
  IMP_S32 u, v;
  IMP_S32 i, j;
  IMP_S32 c11, c21, c31, c41;
  IMP_S32 c12, c22, c32, c42;
  IMP_U32 DW;
  IMP_U32 *id1, *id2;
  IMP_U8 *py1, *py2, *pu, *pv;
  IMP_U8 *d1, *d2;

  d1 = dst_ori;
  d1 += width * height * 3 - width * 3;
  d2 = d1 - width * 3;

  py1 = src0;
  pu = src1;
  pv = src2;
  py2 = py1 + width;

  id1 = (IMP_U32 *) d1;
  id2 = (IMP_U32 *) d2;

  for (j = 0; j < height; j += 2)
  {
    /* line j + 0 */
    for (i = 0; i < width; i += 4)
    {
      u = *pu++;
      v = *pv++;
      c11 = crv_tab[v];
      c21 = cgu_tab[u];
      c31 = cgv_tab[v];
      c41 = cbu_tab[u];
      u = *pu++;
      v = *pv++;
      c12 = crv_tab[v];
      c22 = cgu_tab[u];
      c32 = cgv_tab[v];
      c42 = cbu_tab[u];

      y11 = tab_76309[*py1++];  /* (255/219)*65536 */
      y12 = tab_76309[*py1++];
      y13 = tab_76309[*py1++];  /* (255/219)*65536 */
      y14 = tab_76309[*py1++];

      y21 = tab_76309[*py2++];
      y22 = tab_76309[*py2++];
      y23 = tab_76309[*py2++];
      y24 = tab_76309[*py2++];

      /* RGBR */
      DW = ((clp[(y11 + c41) >> 16])) |
        ((clp[(y11 - c21 - c31) >> 16]) << 8) |
        ((clp[(y11 + c11) >> 16]) << 16) |
        ((clp[(y12 + c41) >> 16]) << 24);
      *id1++ = DW;

      /* GBRG */
      DW = ((clp[(y12 - c21 - c31) >> 16])) |
        ((clp[(y12 + c11) >> 16]) << 8) |
        ((clp[(y13 + c42) >> 16]) << 16) |
        ((clp[(y13 - c22 - c32) >> 16]) << 24);
      *id1++ = DW;

      /* BRGB */
      DW = ((clp[(y13 + c12) >> 16])) |
        ((clp[(y14 + c42) >> 16]) << 8) |
        ((clp[(y14 - c22 - c32) >> 16]) << 16) |
        ((clp[(y14 + c12) >> 16]) << 24);
      *id1++ = DW;

      /* RGBR */
      DW = ((clp[(y21 + c41) >> 16])) |
        ((clp[(y21 - c21 - c31) >> 16]) << 8) |
        ((clp[(y21 + c11) >> 16]) << 16) |
        ((clp[(y22 + c41) >> 16]) << 24);
      *id2++ = DW;

      /* GBRG */
      DW = ((clp[(y22 - c21 - c31) >> 16])) |
        ((clp[(y22 + c11) >> 16]) << 8) |
        ((clp[(y23 + c42) >> 16]) << 16) |
        ((clp[(y23 - c22 - c32) >> 16]) << 24);
      *id2++ = DW;

      /* BRGB */
      DW = ((clp[(y23 + c12) >> 16])) |
        ((clp[(y24 + c42) >> 16]) << 8) |
        ((clp[(y24 - c22 - c32) >> 16]) << 16) |
        ((clp[(y24 + c12) >> 16]) << 24);
      *id2++ = DW;
    }
    id1 -= (9 * width) >> 2;
    id2 -= (9 * width) >> 2;
    py1 += width;
    py2 += width;
  }
}

IMP_VOID ipConvertImageRGB2YUV( RGB_IMAGE_S *src, YUV_IMAGE422_S *dst )
{
	PIXEL_S *p;
	IMP_U8 *Y, *U, *V;

	IMP_S32 i, width, height, count;
	width = src->s32W;
	height = src->s32H;
	count = width * height;

	p = (PIXEL_S*)(src->pu8Data);

	Y = (IMP_U8*)(dst->pu8Y);
	U = (IMP_U8 *)(dst->pu8U);
	V = (IMP_U8*)(dst->pu8V);
	for( i = 0; i< count; i++ )
	{
		*Y =  (( (p->u8R) * (16842) + (p->u8G) * (33030) + (p->u8B)  * (6423) + 16*65536) >> 16) ;
		if (i%2==0)
		{
			*U = ( (p->u8R) * (-9699) + (p->u8G) * (-19071) + (p->u8B) * (28770) + 128*65536) >> 16;
			*V = ( (p->u8R) * (28770) + (p->u8G) * (-24117) + (p->u8B) * (-4653) + 128*65536) >> 16;
			++U;
			++V;
		}
		++p;
		++Y;
	}
}



IMP_VOID ipConvertImageRgb2HSV( IMP_U8 *pRGB, IMP_FLOAT *pHSV, IMP_U16 width, IMP_U16 heigth)
{
	IMP_U8 r, g, b;
	IMP_FLOAT h, s, v;
	IMP_U8 min, max;
    IMP_FLOAT delta;
	IMP_S32 i;
	for (i = 0;i < width * heigth; i++)
	{
		b = *pRGB; pRGB++;
		g = *pRGB; pRGB++;
		r = *pRGB; pRGB++;

		min = IMP_MIN( IMP_MIN(r, g), b );
		max = IMP_MAX( IMP_MAX(r, g), b );
		v = max; // v
		*pHSV = v; *pHSV++;

		delta = (IMP_FLOAT)(max - min);

		if( max != 0 )
		{
			s = delta / max; // s
		}
		else
		{
			// r = g = b = 0 // s = 0, v is undefined
			s = 0;
			*pHSV = s; *pHSV++;
			h = -1;
			*pHSV = h; *pHSV++;
			return;
		}

		if( r == max )
		{
			h = ( g - b ) / delta; // between yellow & magenta
		}
		else if( g == max )
		{
			h = 2 + ( b - r ) / delta; // between cyan & yellow
		}
		else
		{
			h = 4 + ( r - g ) / delta; // between magenta & cyan
		}

		h *= 60; // degrees

		if( h < 0 )
		{
			h += 360;
		}

		*pHSV = s; *pHSV++;
		*pHSV = h; *pHSV++;
	}
}


static IMP_VOID ipRoundParameters( IMP_S32 *pStartRow, IMP_S32 *pStartCol, IMP_S32 *pEndRow, IMP_S32 *pEndCol, IMP_S32 height, IMP_S32 width )
{
	IMP_S32 startRow = *pStartRow;
	IMP_S32 startCol = *pStartCol;
	IMP_S32 endRow = *pEndRow;
	IMP_S32 endCol = *pEndCol;

	if( endRow & 1 )
		endRow += 1;
	if( endCol & 1 )
		endCol += 1;

	if( endRow < IP_EDGE_MIN_ROWS )
		endRow = IP_EDGE_MIN_ROWS;
	if( startRow > height-endRow )
		startRow = height-endRow;

	if( endCol < IP_EDGE_MIN_COLS )
		endCol = IP_EDGE_MIN_COLS;
	if( startCol > width-endCol )
		startCol = width-endCol;

	startRow -= 2; startCol -= 2;
	endRow += 4; endCol += 4;

	if( startRow < 0 )
		startRow = 0;
	if( endRow > height-startRow )
	{
		endRow = height-startRow;
		if( endRow&1 )
			endRow -= 1;
	}

	if( startCol < 0 )
		startCol = 0;
	if( endCol > width-startCol )
	{
		endCol = width-startCol;
		if( endCol&1 )
			endCol -= 1;
	}

	*pStartRow = startRow;
	*pStartCol = startCol;
	*pEndRow = endRow;
	*pEndCol = endCol;
}

IMP_VOID ipCannyCalcGradmagT( IMP_U8 *src, IMP_S8 *grad, IMP_U8 *mag,
								IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
								IMP_S32 width, IMP_S32 height )
{
	IMP_S32 x, y;
	IMP_U8 *pGray1, *pGray2, *pMag1, *pMag2;
	IMP_S8 *pGradxy1, *pGradxy2;
	IMP_S32 value1, value2, value3, value12, value22, value32;
	IMP_S32 p1, p2, p3, p4, p12, p22, p32, p42;
	IMP_S32 stride = width;

	src += width * startRow + startCol;
	grad += ((width * startRow + startCol) << 1);
	mag += width * startRow + startCol;

	for( y = 1; y< endRow - 1; y++ )
	{
		pGray1 = src + stride * y+1;
 		pGradxy1 = grad + ((stride * y + 1) << 1);
		pMag1 = mag + stride * y + 1;
		pGray2 = src + stride * y + 2;
 		pGradxy2 = grad + ((stride * y + 2) << 1);
		pMag2 = mag + stride * y + 2;

		for( x = 1; x < endCol - 1; x += 2 )
		{
			p1 = pGray1[-1];
			p2 = pGray1[1];
			p3 = pGray1[-stride];
			p4 = pGray1[stride];
			p12 = pGray2[-1];
			p22 = pGray2[1];
			p32 = pGray2[-stride];
			p42 = pGray2[stride];

			value1 = ( p1 - p2 ) >> 1;
			value2 = ( p3 - p4 ) >> 1;
			value12 = ( p12 - p22 ) >> 1;
			value22 = ( p32 - p42 ) >> 1;

			pGradxy1[0] = value1;
			pGradxy1[1] = value2;
			pGradxy2[0] = value12;
			pGradxy2[1] = value22;

			value3 = ( abs(value1) + abs(value2) );
			pMag1[0] = value3;
			value32 = ( abs(value12) + abs(value22) );
			pMag2[0] = value32;

			pGray1 += 2;
			pGradxy1 += 4;
			pMag1 += 2;
			pGray2 += 2;
			pGradxy2 += 4;
			pMag2 += 2;
		}
	}
}

IMP_VOID ipCannyCalcGradienti( IMP_U8 *src, IMP_S8 *grad,
								IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
								IMP_S32 width, IMP_S32 height )
{
	IMP_S32 x, y;
	IMP_U8 *pGray1, *pGray2;
	IMP_S8 *pGradxy1, *pGradxy2;
	IMP_S32 value1, value2, value12, value22;
	IMP_S32 p1, p2, p3, p4, p12, p22, p32, p42;
	IMP_S32 stride = width;

	src += width * startRow + startCol;
	grad += ((width * startRow + startCol) << 1);

	for( y = 1; y < endRow - 1; y++ )
	{
		pGray1 = src + stride * y+1;
 		pGradxy1 = grad + ((stride * y + 1) << 1);
		pGray2 = src + stride * y + 2;
 		pGradxy2 = grad + ((stride * y + 2) << 1);

		for( x = 1; x < endCol - 1; x += 2 )
		{
			p1 = pGray1[-1]; p2 = pGray1[1];
			p3 = pGray1[-stride]; p4 = pGray1[stride];

			p12 = pGray2[-1]; p22 = pGray2[1];
			p32 = pGray2[-stride]; p42 = pGray2[stride];

			value1 = ( p1 - p2 ) >> 1; value2 = ( p3 - p4 ) >> 1;
			value12 = ( p12 - p22 ) >> 1; value22 = ( p32 - p42 ) >> 1;

			pGradxy1[0] = value1; pGradxy1[1] = value2;
			pGradxy2[0] = value12; pGradxy2[1] = value22;

			pGray1 += 2;
			pGradxy1 += 4;
			pGray2 += 2;
			pGradxy2 += 4;
		}
	}
}

IMP_VOID ipCannyCalcMagnitudei( IMP_S8 *grad, IMP_U8 *mag,
								IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
								IMP_S32 width, IMP_S32 height )
{
	IMP_S32 x, y;
	IMP_S8 *pGradxy1, *pGradxy2;
	IMP_U8 *pMag1, *pMag2;
	IMP_S32 value1, value2, value3, value12, value22, value32;
	IMP_S32 stride = width;

	grad += ((width * startRow + startCol) << 1);
	mag += width * startRow + startCol;

	for( y = 1; y < endRow - 1; y++ )
	{
 		pGradxy1 = grad + ((stride * y + 1) << 1);
		pMag1 = mag + stride * y + 1;
 		pGradxy2 = grad + ((stride * y + 2) << 1);
		pMag2 = mag + stride * y + 2;

		for( x = 1; x <endCol - 1; x += 2 )
		{
			value1 = pGradxy1[0]; value2 = pGradxy1[1];
			value12 = pGradxy2[0]; value22 = pGradxy2[1];

			value3 = ( abs(value1) + abs(value2) ); pMag1[0] = value3;
			value32 = ( abs(value12) + abs(value22) ); pMag2[0] = value32;

			pGradxy1 += 4;
			pMag1 += 2;
			pGradxy2 += 4;
			pMag2 += 2;
		}
	}
}

static IMP_VOID ipCannyNonMaxSuppi( IMP_S8 *grad, IMP_U8 *mag, IMP_U8 *edge,
								IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
								IMP_S32 width, IMP_S32 height, IMP_S32 tgrad )
{
    IMP_S32 x, y, i, nx1, ny1, nx2, ny2;
    IMP_U8 *pMagRow, *pMagTmp1, *pMagTmp2, *pResRow, *pTmpRes1, *pTmpRes2;
    IMP_S8 *pGradxyRow, *pGradxy1, *pGradxy2;
	IMP_S32 mag11, mag12, mag21, mag22, mag11_2, mag12_2, mag21_2, mag22_2;
    IMP_S32 m00, m00_t, grad1, grad2, m00_2, grad12, grad22;
    IMP_S32 nMag1,nMag2;
	IMP_U32 gradx1abs, grady1abs, gradx2abs, grady2abs;
	IMP_S32 signx, signx1, signx2, signy, signy1, signy2, signy1_2, signx1_2, signy2_2, signx2_2;
    IMP_S32 nMag1_2,nMag2_2;
	IMP_S32 signx_2, signy_2;
	IMP_S32 stride = width;
	IMP_S32 stride1;
	IMP_U8 *pRes;

	grad += ((width * startRow + startCol) << 1);
	mag += width * startRow + startCol;
	edge += width * startRow + startCol;
	pRes = edge;

	// Zero the edges of the result image.
    for (i = 0, pResRow = pRes, pTmpRes1 = pRes + stride * (endRow - 1);
        i < endCol; pTmpRes1++, pResRow++, i++)
	{
        pResRow[0] = 0;
        pResRow[stride] = 0;
		pTmpRes1[0] = 0;
		pTmpRes1[-stride] = 0;
	}

    for (i = 0, pTmpRes1 = pRes, pResRow = pRes + endCol - 1;
        i < endRow; i++, pTmpRes1 += stride, pResRow += stride)
	{
        pTmpRes1[0] = 0;
        pTmpRes1[1] = 0;
		pResRow[-1] = 0;
		pResRow[0] = 0;
	}

	stride1 = (stride << 1);

	// Suppress non-maximum points.
	for( x = 2, pMagRow = mag + ( stride1 + 2), pGradxyRow = grad + ((stride1 + 2) << 1),
		pResRow = pRes + (stride1 + 2);
        x < endRow - 2;
        x++, pMagRow += stride, pGradxyRow += stride1,
        pResRow += stride )
	{
		for( y = 2,
			pMagTmp1 = pMagRow, pGradxy1 = pGradxyRow, pTmpRes1 = pResRow,
			pMagTmp2 = pMagRow + 1, pGradxy2 = pGradxyRow + 2, pTmpRes2 = pResRow + 1;
			y < endCol - 2;
			y += 2,
			pMagTmp1 += 2, pGradxy1 += 4, pTmpRes1 += 2,
			pMagTmp2 += 2, pGradxy2 += 4, pTmpRes2 += 2
			)
		{
			grad1 = *pGradxy1;	grad2 = *(pGradxy1 + 1);
			grad12 = *pGradxy2;	grad22 = *(pGradxy2 + 1);

			m00 = *(pMagTmp1);
			m00_2 = *(pMagTmp2);

			gradx1abs = abs(grad1);	grady1abs = abs(grad2);
			signx = grad1>=0 ? 1 : -1;
			signy = grad2>=0 ? 1 : -1;
			signx2 = signx;
			signy2 = stride * signy;
			if( gradx1abs >= grady1abs )
			{
				nx1 = -grad1 * signx;
				ny1 = grad2 * signy;
				signx1 = signx;
				signy1 = 0;
			}
			else
			{
				ny1 = grad1 * signx;
				nx1 = -grad2 * signy;
				signx1 = 0;
				signy1 = stride * signy;
			}

	        mag11 = *( pMagTmp1 - signy1 - signx1 );
	        mag12 = *( pMagTmp1 - signy2 - signx2 );
	        mag21 = *( pMagTmp1 + signy1 + signx1 );
	        mag22 = *( pMagTmp1 + signy2 + signx2 );

			gradx2abs = abs(grad12);	grady2abs = abs(grad22);
			signx_2 = grad12>=0 ? 1 : -1;
			signy_2 = grad22>=0 ? 1 : -1;
			signx2_2 = signx_2;
			signy2_2 = stride * signy_2;
			if( gradx2abs >= grady2abs )
			{
				nx2 = -grad12 * signx_2;
				ny2 = grad22 * signy_2;
				signx1_2 = signx_2;
				signy1_2 = 0;
			}
			else
			{
				ny2 = grad12 * signx_2;
				nx2 = -grad22 * signy_2;
				signx1_2 = 0;
				signy1_2 = stride * signy_2;
			}

	        mag11_2 = *( pMagTmp2 - signy1_2 - signx1_2 );
	        mag12_2 = *( pMagTmp2 - signy2_2 - signx2_2 );
	        mag21_2 = *( pMagTmp2 + signy1_2 + signx1_2 );
	        mag22_2 = *( pMagTmp2 + signy2_2 + signx2_2 );

			m00_t = ( m00 & 0xFF );
			nMag1 = (m00 - mag11) * nx1 + (mag12 - mag11) * ny1;
			nMag2 = (m00 - mag21) * nx1 + (mag22 - mag21) * ny1;
			*pTmpRes1 = ( m00<=tgrad || nMag1>=0 || nMag2>=0 ) ? IP_NO_EDGE : m00_t;

			m00_t = ( m00_2 & 0xFF );
			nMag1_2 = (m00_2 - mag11_2) * nx2 + (mag12_2 - mag11_2) * ny2;
			nMag2_2 = (m00_2 - mag21_2) * nx2 + (mag22_2 - mag21_2) * ny2;
			*pTmpRes2 = ( m00_2<=tgrad || nMag1_2>=0 || nMag2_2>=0 ) ? IP_NO_EDGE : m00_t;
        }
	}
}

IMP_VOID ipCannyCalcGradmag( IMP_U8 *src, IMP_S8 *grad, IMP_U8 *mag,
					IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
					IMP_S32 width, IMP_S32 height )
{
	ipRoundParameters( &startRow, &startCol, &endRow, &endCol, height, width );
	ipCannyCalcGradmagT( src, grad, mag, startCol, startRow, endCol, endRow, width, height );
}

IMP_VOID ipCannyCalcGradient( IMP_U8 *src, IMP_S8 *grad,
					IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
					IMP_S32 width, IMP_S32 height )
{
	ipRoundParameters( &startRow, &startCol, &endRow, &endCol, height, width );
	ipCannyCalcGradienti( src, grad, startCol, startRow, endCol, endRow, width, height );
}

IMP_VOID ipCannyCalcMagnitude( IMP_S8 *grad, IMP_U8 *mag,
					IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
					IMP_S32 width, IMP_S32 height )
{
	ipRoundParameters( &startRow, &startCol, &endRow, &endCol, height, width );
	ipCannyCalcMagnitudei( grad, mag, startCol, startRow, endCol, endRow, width, height );
}

IMP_VOID ipCannyNonMaxSupp( IMP_S8 *grad, IMP_U8 *mag, IMP_U8 *edge,
					IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
					IMP_S32 width, IMP_S32 height, IMP_S32 tgrad )
{
	ipRoundParameters( &startRow, &startCol, &endRow, &endCol, height, width );
	ipCannyNonMaxSuppi( grad, mag, edge, startCol, startRow, endCol, endRow, width, height, tgrad );
}

IMP_VOID ipCannyProcess2( IMP_U8 *src, IMP_S8 *grad, IMP_U8 *mag, IMP_U8 *edge,
					IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
					IMP_S32 width, IMP_S32 height, IMP_S32 tgrad )
{
	ipRoundParameters( &startRow, &startCol, &endRow, &endCol, height, width );
	if( src )
		ipCannyCalcGradmagT( src, grad, mag, startCol, startRow, endCol, endRow, width, height );
	else
		ipCannyCalcMagnitudei( grad, mag, startCol, startRow, endCol, endRow, width, height );
	ipCannyNonMaxSuppi( grad, mag, edge, startCol, startRow, endCol, endRow, width, height, tgrad );
}

/**
* Canny边缘提取
* @param src 灰度图像
* @param grad 梯度图像
* @param mag 幅度图像
* @param edge 边缘图像
* @param startCol 起始列
* @param startRow 起始行
* @param endCol 结束列
* @param endRow 结束行
* @param width 列（图像宽度）
* @param height 行（图像高度）
* @param tgrad 阈值
*/
IMP_VOID ipCanny( IMP_U8 *src, IMP_S8 *grad, IMP_U8 *mag, IMP_U8 *edge,
			  IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
			  IMP_S32 width, IMP_S32 height, IMP_S32 tgrad )
{
    IMP_S32 x, y, count;
	IMP_U8 *pRow, *ptr1, *ptr2;
	IMP_U8 *pMagRow, *pResRow, *pMagTmp1, *pMagTmp2, *pTmpRes1, *pTmpRes2;
    IMP_S8 *pGradxyRow, *pGradxy1, *pGradxy2;
	IMP_S32 mag11, mag12, mag21, mag22, nMag1,nMag2;
    IMP_S32 m00, m00_t, m00_2, m00_t_2, grad1, grad12, grad2, grad22;
    IMP_S32 nx1, ny1, nx2, ny2;
	IMP_U32 gradx1abs, grady1abs, gradx2abs, grady2abs;
	IMP_S32 signx, signy, signy1, signx1, signy2, signx2, signx_2, signy_2, signy1_2, signx1_2, signy2_2, signx2_2;
	IMP_S32 mag11_2, mag12_2, mag21_2, mag22_2, nMag1_2, nMag2_2;
	IMP_S32 value1, value2, value3, value12, value22, value32;
	IMP_S32 p1, p2, p3, p4, p12, p22, p32, p42;
	IMP_S32 stride = width;
	IMP_U8 *pRes;
	IMP_S32 offset;

	offset = width * startRow + startCol;
	src += offset;
	grad += (offset << 1);
	mag += offset;
	edge += offset;
	pRes = edge;

	offset = stride * (endRow - 1);
	pResRow = pRes;
	pTmpRes1 = pRes + offset;
	pGradxyRow = grad;
	pGradxy1 = grad + (offset << 1);
	pMagRow = mag;
	pMagTmp1 = mag + offset;
	for( count=0; count < endCol; count++ )
	{
        pResRow[count] = 0;     pResRow[count + stride] = 0;
		pTmpRes1[count] = 0;    pTmpRes1[count - stride] = 0;

        ((IMP_U16*)pGradxyRow)[count] = 0;
        ((IMP_U16*)pGradxy1)[count] = 0;

        pMagRow[count] = 0;
		pMagTmp1[count] = 0;
	}
	offset = endCol - 1;
	pResRow = pRes;
	pTmpRes1 = pRes + offset;
	pGradxyRow = grad;
	pGradxy1 = grad + (offset << 1);
	pMagRow = mag;
	pMagTmp1 = mag + offset;
    for( count = 0; count < endRow; count++ )
	{
		IMP_S32 idx = count * stride;

		pResRow[idx] = 0;	pResRow[idx+1] = 0;
        pTmpRes1[idx] = 0;  pTmpRes1[idx-1] = 0;

        ((IMP_U16*)pGradxyRow)[idx] = 0;
        ((IMP_U16*)pGradxy1)[idx] = 0;

        pMagRow[idx] = 0; pMagTmp1[idx] = 0;
	}

	for( x = 1; x <= 2; x++ )
	{
		offset = (stride * x + 1);
		ptr1 = src + offset;
		pGradxy1 = grad + (offset << 1);
		pMagTmp1 = mag + offset;
		ptr2 = ptr1 + 1;
		pGradxy2 = pGradxy1 + 2;
		pMagTmp2 = pMagTmp1 + 1;

		for( y = 1; y < endCol - 1; y += 2 )
		{
			p1 = ptr1[-1];	p2 = ptr1[1];  p3 = ptr1[-stride];	p4 = ptr1[stride];
			p12 = ptr2[-1];	p22 = ptr2[1]; p32 = ptr2[-stride];	p42 = ptr2[stride];

			value1 = ( p1 - p2 ) >> 1;   	value2 = ( p3 - p4 ) >> 1;
			value12 = ( p12 - p22 ) >> 1;	value22 = ( p32 - p42 ) >> 1;

			pGradxy1[0] = value1;	pGradxy1[1] = value2;
			pGradxy2[0] = value12;	pGradxy2[1] = value22;

			value3 = ( abs(value1) + abs(value2) );
			pMagTmp1[0] = value3;
			value32 = ( abs(value12) + abs(value22) );
			pMagTmp2[0] = value32;

			ptr1 += 2;
			pGradxy1 += 4;
			pMagTmp1 += 2;
			ptr2 += 2;
			pGradxy2 += 4;
			pMagTmp2 +=2;
		}
	}
	pRow = src + (stride * 3 + 3);
	pGradxyRow = grad + (((stride << 1) + 2) << 1);
	pMagRow = mag + ((stride << 1) + 2);
	pResRow = pRes + ((stride << 1) + 2);
	for( x = 2; x < endRow - 2; x++ )
	{
		offset = (stride * (x + 1) + 1);
		ptr1 = src + offset;
		pGradxy1 = grad + (offset << 1);
		pMagTmp1 = mag + offset;
		ptr2 = ptr1 + 1;
		pGradxy2 = pGradxy1 + 2;
		pMagTmp2 = pMagTmp1 + 1;

		p1 = ptr1[-1];	p2 = ptr1[1];	p3 = ptr1[-stride];	 p4 = ptr1[stride];
		p12 = ptr2[-1];	p22 = ptr2[1];	p32 = ptr2[-stride]; p42 = ptr2[stride];

		value1 = ( p1 - p2 ) >> 1;		value2 = ( p3 - p4 ) >> 1;
		value12 = ( p12 - p22 ) >> 1;	value22 = ( p32 - p42 ) >> 1;

		pGradxy1[0] = value1;	pGradxy1[1] = value2;
		pGradxy2[0] = value12;	pGradxy2[1] = value22;

		value3 = ( abs(value1) + abs(value2) );
		pMagTmp1[0] = value3;
		value32 = ( abs(value12) + abs(value22) );
		pMagTmp2[0] = value32;

		ptr1 = pRow;
		pGradxy1 = pGradxyRow;
		pMagTmp1 = pMagRow;
		pTmpRes1 = pResRow;
		ptr2 = pRow + 1;
		pGradxy2 = pGradxyRow + 2;
		pMagTmp2 = pMagRow + 1;
		pTmpRes2 = pResRow + 1;

		for( y = 2; y < endCol - 2; y += 2 )
		{
			p1 = ptr1[-1];	p2 = ptr1[1];  p3 = ptr1[-stride];	p4 = ptr1[stride];
			p12 = ptr2[-1];	p22 = ptr2[1]; p32 = ptr2[-stride];	p42 = ptr2[stride];

			grad1 = *pGradxy1;		grad2 = *(pGradxy1 + 1);
			m00 = *( pMagTmp1 );

			grad12 = *pGradxy2;		grad22 = *(pGradxy2 + 1);
			m00_2 = *( pMagTmp2);

			value1 = ( p1 - p2 ) >> 1;		value2 = ( p3 - p4 ) >> 1;
			value12 = ( p12 - p22 ) >> 1;	value22 = ( p32 - p42 ) >> 1;

			offset = (( 1 + stride) << 1);
			pGradxy1[0+offset] = value1;	pGradxy1[1+offset] = value2;
			pGradxy2[0+offset] = value12;	pGradxy2[1+offset] = value22;

			value3 = ( abs(value1) + abs(value2) );
			pMagTmp1[0+1+stride] = value3;
			value32 = ( abs(value12) + abs(value22) );
			pMagTmp2[0+1+stride] = value32;

			gradx1abs = abs(grad1);	grady1abs = abs(grad2);
			signx = grad1 >= 0 ? 1 : -1;
			signy = grad2 >= 0 ? 1 : -1;
			signx2 = signx;
			signy2 = stride * signy;
			if( gradx1abs >= grady1abs )
			{
				nx1 = -grad1 * signx;
				ny1 = grad2 * signy;
				signx1 = signx;
				signy1 = 0;
			}
			else
			{
				ny1 = grad1 * signx;
				nx1 = -grad2 * signy;
				signx1 = 0;
				signy1 = stride * signy;
			}

	        mag11 = *( pMagTmp1 - signy1 - signx1 );
	        mag12 = *( pMagTmp1 - signy2 - signx2 );
	        mag21 = *( pMagTmp1 + signy1 + signx1 );
	        mag22 = *( pMagTmp1 + signy2 + signx2 );

			gradx2abs = abs(grad12);	grady2abs = abs(grad22);
			signx_2 = grad12 >= 0 ? 1 : -1;
			signy_2 = grad22 >= 0 ? 1 : -1;
			signx2_2 = signx_2;
			signy2_2 = stride * signy_2;
			if( gradx2abs >= grady2abs )
			{
				nx2 = -grad12 * signx_2;
				ny2 = grad22 * signy_2;
				signx1_2 = signx_2;
				signy1_2 = 0;
			}
			else
			{
				ny2 = grad12 * signx_2;
				nx2 = -grad22 * signy_2;
				signx1_2 = 0;
				signy1_2 = stride * signy_2;
			}

	        mag11_2 = *( pMagTmp2 - signy1_2 - signx1_2 );
	        mag12_2 = *( pMagTmp2 - signy2_2 - signx2_2 );
	        mag21_2 = *( pMagTmp2 + signy1_2 + signx1_2 );
	        mag22_2 = *( pMagTmp2 + signy2_2 + signx2_2 );

			m00_t = ( m00 & 0xFF );
			nMag1 = (m00 - mag11) * nx1 + (mag12 - mag11) * ny1;
			nMag2 = (m00 - mag21) * nx1 + (mag22 - mag21) * ny1;
			*pTmpRes1 = ( m00 <= tgrad || nMag1 >=0 || nMag2 >= 0 ) ? IP_NO_EDGE : m00_t;

			m00_t_2 = ( m00_2 & 0xFF );
			nMag1_2 = (m00_2 - mag11_2) * nx2 + (mag12_2 - mag11_2) * ny2;
			nMag2_2 = (m00_2 - mag21_2) * nx2 + (mag22_2 - mag21_2) * ny2;
			*pTmpRes2 = ( m00_2 <= tgrad || nMag1_2 >= 0 || nMag2_2 >= 0 ) ? IP_NO_EDGE : m00_t_2;

			ptr1 += 2;
			pGradxy1 += 4;
			pMagTmp1 += 2;
			pTmpRes1 += 2;
			ptr2 += 2;
			pGradxy2 += 4;
			pMagTmp2 +=2;
			pTmpRes2 += 2;
        }

		pRow += stride;
		pGradxyRow += (stride << 1);
		pMagRow += stride;
        pResRow += stride;
	}
}

IMP_VOID ipGradientX(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height)
{
	IMP_S32 r, c, pos;
	for(r = 0; r < height; r++)
	{
		pos = r * width;
		dst[pos] = abs(src[pos+1] - src[pos]); //第一个元素
		pos++;
		for(c = 1;c < (width - 1); c++, pos++)
		{
			dst[pos] = abs(src[pos+1] - src[pos-1]);
		}
		dst[pos] = abs(src[pos] - src[pos-1]); //最后一个元素
   }
}

IMP_VOID ipGradientY(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height)
{
	IMP_S32 r, c, pos;
	for(c = 0; c < width; c++)
	{
		pos = c;
		dst[pos] = abs(src[pos+width] - src[pos]);
		pos += width;
		for(r = 1; r<(height-1);r++,pos += width)
		{
			dst[pos] = abs(src[pos+width] - src[pos-width]);
		}
		dst[pos] = abs(src[pos] - src[pos-width]);
   }
}

IMP_VOID ipGradientXY(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height)
{
	IMP_S32 r, c, pos;
	//水平方向
	for(r = 0; r < height; r++)
	{
		pos = r * width;
		dst[pos] = abs(src[pos+1] - src[pos]); //第一个元素
		pos++;
		for(c = 1;c < (width - 1); c++, pos++)
		{
			dst[pos] = abs(src[pos+1] - src[pos-1]);
		}
		dst[pos] = abs(src[pos] - src[pos-1]); //最后一个元素
   }

	//垂直方向
	for(c = 0; c < width; c++)
	{
		pos = c;
		dst[pos] += abs(src[pos+width] - src[pos]);
		pos += width;
		for(r = 1; r<(height-1);r++,pos += width)
		{
			dst[pos] += abs(src[pos+width] - src[pos-width]);
		}
		dst[pos] += abs(src[pos] - src[pos-width]);
   }
}


/***************************************************
name:ipSobel3_3
do:3Ｘ3 sobel算梯度
input:
	src:灰度图像
	stride1:src图像的内存跨度,当图像宽是４的倍数时,它等行图像宽
	stride2:dst图像的内存跨度,当图像宽是４的倍数时,它等行图像宽
	width:图像宽
	height:图像高
output:
	dst:目标梯度大小图像
history:
****************************************************/
IMP_VOID ipSobel3_3(IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 H, O, V;
	IMP_S32 x, y;
	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;

	IMP_S32 i00, i01, i02;
	IMP_S32 i10, i11, i12;
	IMP_S32 i20, i21, i22;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip one stride
	p1 += stride1;
	p2 += stride2;

	// for each line
	for ( y = 1; y < heightM1; y++ )
	{
		p1++;
		p2++;
		// for each pixel
		for ( x = 1; x < widthM1; x++, p1++, p2++ )
		{

			i00 = (*(p1 - stride1 - 1)); i01 = (*(p1 - stride1)); i02 = (*(p1 - stride1 + 1));
			i10 = (*(p1 - 1));           i11 = (*(p1));           i12 = (*(p1 + 1));
			i20 = (*(p1 + stride1 - 1)); i21 = (*(p1 + stride1)); i22 = (*(p1 + stride1 + 1));

			H = - i00 - 2 * i01 - i02 +
				+ i20 + 2 * i21 + i22;
			V = - i00 + i02
				- 2 * i10 + 2 * i12
				- i20 + i22;
			O = abs(H) + abs(V);
			if (O > 255) O = 255;
			(*p2) = O;
		}
		p1 += (offset1 + 1);
		p2 += (offset2 + 1);
	}
}
/**
* Sobel边缘提取(5*5)
* @param src 源图像
* @param stride1 源图像 stride
* @param dst 目的图像
* @param stride2 目的图像 stride
* @param width 图像宽度
* @param height 图像高度
* 模板
* Horizontal Mask     Vertical Mask
* -1 -4  -6  -4  -1     1   2  0  -2  -1
* -2 -8 -12  -8  -2     4   8  0  -8  -4
*  0  0   0   0   0     6  12  0  -12 -6
*  2  8  12   8   2     4   8  0  -8  -4
*  1  4   6   4   1     1   2  0  -2  -1
*/
IMP_VOID ipSobel5_5(IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 H, O, V;
	IMP_S32 x, y;
	IMP_S32 widthM1 = width - 2;
	IMP_S32 heightM1 = height - 2;

	IMP_S32 i00, i01, i02, i03, i04;
	IMP_S32 i10, i11, i12, i13, i14;
	IMP_S32 i20, i21, i22, i23, i24;
	IMP_S32 i30, i31, i32, i33, i34;
	IMP_S32 i40, i41, i42, i43, i44;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 tmp1 = (stride1 << 1);
	IMP_S32 tmp2 = (stride2 << 1);

	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip two stride
	p1 += tmp1;
	p2 += tmp2;

	// for each line
	for ( y = 2; y < heightM1; y++ )
	{
		//skip two pixels
		p1 += 2;
		p2 += 2;
		// for each pixel
		for ( x = 2; x < widthM1; x++, p1++, p2++ )
		{
			i00 = *(p1 - tmp1 - 2);
			i01 = *(p1 - tmp1 - 1);
			i02 = *(p1 - tmp1);
			i03 = *(p1 - tmp1 + 1);
			i04 = *(p1 - tmp1 + 2);

			i10 = *(p1 - stride1 - 2);
			i11 = *(p1 - stride1 - 1);
			i12 = *(p1 - stride1 );
			i13 = *(p1 - stride1 + 1);
			i14 = *(p1 - stride1 + 2);

			i20 = *(p1  - 2);
			i21 = *(p1  - 1);
			i22 = *(p1);
			i23 = *(p1  + 1);
			i24 = *(p1  + 2);

			i30 = *(p1 + stride1 - 2);
			i31 = *(p1 + stride1 - 1);
			i32 = *(p1 + stride1 );
			i33 = *(p1 + stride1 + 1);
			i34 = *(p1 + stride1 + 2);

			i40 = *(p1 + tmp1 - 2);
			i41 = *(p1 + tmp1 - 1);
			i42 = *(p1 + tmp1);
			i43 = *(p1 + tmp1 + 1);
			i44 = *(p1 + tmp1 + 2);

			H = - i00 - 4 * i01 - 6 * i02 - 4 * i03 - i04
				- 2 * i10 - 8 * i11 -12 * i12 - 8 * i13 - 2 * i14
				+ 2 * i30 + 8 * i31 +12 * i32 + 8 * i33 + 2 * i34
				+ i40 + 4 * i41 + 6 * i42 + 4 * i43 + i44;
			V = + i00 + 2 * i01 - 2 * i03 - i04
				+ 4 * i10 + 8 * i11 - 8 * i13 - 4 * i14
				+ 6 * i20 +12 * i21 -12 * i23 - 6 * i24
				+ 4 * i30 + 8 * i31 - 8 * i33 - 4 * i34
				+ i40 + 2 * i41 - 2 * i43 - i44;
			O = abs(H) + abs(V);
			O = (O > 255) ? 255 : O;
			*(p2) = O;
		}
		p1 += (offset1 + 2);
		p2 += (offset2 + 2);
	}

}
/**
* 梯度提取(使用sobel3*3模板)
* @param src 源图像
* @param stride1 源图像 stride
* @param edge 边缘图像
* @param stride2 边缘图像 stride
* @param mag 梯度幅度图像
* @param stride3 梯度幅度图像 stride
* @param gradDir 梯度方向图像
* @param stride4 梯度方向图像 stride
* @param width 图像宽度
* @param height 图像高度
* 模板
* Horizontal Mask Vertical Mask
* -1   -2   -1      -1   0   1
* 0     0    0      -2   0   2
* 1     2    1      -1   0   1
*/
IMP_VOID ipSobelGrad3_3(IMP_U8 *src, IMP_U16 stride1, IMP_U8 *edge, IMP_U16 stride2, IMP_U16 * mag, IMP_U16 stride3, IMP_FLOAT * gradDir, IMP_U16 stride4, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 H, O, V;
	IMP_S32 x, y;
	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;
	IMP_S32 sq1, sq2;
	IMP_DOUBLE dx, dy;
	IMP_DOUBLE xu, yu, ang;
	IMP_S32 i00, i01, i02;
	IMP_S32 i10, i11, i12;



	IMP_S32 i20, i21, i22;
	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;
	IMP_S32 offset3 = stride3 - width;
	IMP_S32 offset4 = stride4 - width;
	IMP_U8 *p1, *p2;
	IMP_U16 *p3;
	IMP_FLOAT *p4;
	p1 = src;
	p2 = edge;
	p3 = mag;
	p4 = gradDir;


	// skip one stride
	p1 += stride1;
	p2 += stride2;
	p3 += stride3;
	p4 += stride4;
	// for each line
	for ( y = 1; y < heightM1; y++ )
	{
		p1++;
		p2++;
		p3++;
		p4++;
		// for each pixel
		for ( x = 1; x < widthM1; x++, p1++, p2++, p3++, p4++ )
		{

			i00 = (*(p1 - stride1 - 1)); i01 = (*(p1 - stride1)); i02 = (*(p1 - stride1 + 1));
			i10 = (*(p1 - 1));           i11 = (*(p1));           i12 = (*(p1 + 1));
			i20 = (*(p1 + stride1 - 1)); i21 = (*(p1 + stride1)); i22 = (*(p1 + stride1 + 1));

			H = - i00 - 2 * i01 - i02 +
				+ i20 + 2 * i21 + i22;
			V = - i00 + i02
				- 2 * i10 + 2 * i12
				- i20 + i22;

			//sobel edge image
			O = abs(H) + abs(V);
			if (O > 255) O = 255;
			(*p2) = O;

			//magnitude of the gradient
			sq1 = (IMP_S32)H * (IMP_S32)H;
			sq2 = (IMP_S32)V * (IMP_S32)V;
            (*p3) = (IMP_U16)(0.5 + sqrt((IMP_FLOAT)sq1 + (IMP_FLOAT)sq2));

			//direction up the gradient
			dx = (IMP_DOUBLE)H;
			dy = -(IMP_DOUBLE)V;
			xu = fabs(dx);
			yu = fabs(dy);

			if((xu == 0) && (yu == 0))
			{
				(*p4) = 0;
			}
			else
			{
				ang = atan(yu/xu);

				if(dx >= 0)
				{
					if(dy >= 0)
					{
						(*p4) = ang;
					}
					else
					{
						(*p4) = 2 * IMP_PI - ang;
					}
				}
				else
				{
					if(dy >= 0)
					{
						(*p4) = IMP_PI - ang;
					}
					else
					{
						(*p4) = IMP_PI + ang;
					}
				}

			}
		}
		p1 += (offset1 + 1);
		p2 += (offset2 + 1);
		p3 += (offset3 + 1);
		p4 += (offset4 + 1);
	}
}
/**
* Laplace边缘提取(3*3)
* @param src 源图像
* @param dst 目的图像
* @param width 图像宽度
* @param height 图像高度
* 模板
*        0  -1  0
*        -1  4  -1
*        0  -1  0
*/
IMP_VOID ipLaplace3_3(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height)
{
	IMP_S32 O, i;
	IMP_S32 i00, i01, i02;
	IMP_S32 i10, i11, i12;
	IMP_S32 i20, i21, i22;
	IMP_S32 w = width;

	for (i = 0; i < width*(height-2) - 2; i++)
	{
		i00=src[i ]; i01=src[i +1]; i02=src[i +2];
		i10=src[i+ w]; i11=src[i+ w+1];i12=src[i+ w+2];
		i20=src[i+2*w]; i21=src[i+2*w+1]; i22=src[i+2*w+2];
		O = - i01
			- i10 + 4*i11 - i12
			- i21;

		if (O > 255) O = 255;
		else if (O < 0)
		{
			O = 0;
		}
		dst[i + 1] = O;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// 直方图统计
/// 灰度直方图、彩色直方图、梯度直方图
//////////////////////////////////////////////////////////////////////////

/**
* 计算32级灰度直方图
* @param src 输入的灰度图像指针
* @param width 图像宽度
* @param height 图像高度
* @param pRect 指定计算灰度直方图的区域
* @param pHistogram32 灰度直方图指针,长度为32
*/
IMP_VOID ipHistogramGray32(IMP_U8 *src,IMP_U16 width,IMP_U16 height,IMP_RECT_S *pRect,IMP_U32 *pHistogram32)
{
	IMP_S32 y, x, xsum, ysum;
	IMP_U8 value;
	IMP_U8 *pSrc = src + pRect->s16Y1 * width;
	xsum = ysum = 0;

	for ( y = pRect->s16Y1; y <= pRect->s16Y2; y++ )
	{
		for ( x = pRect->s16X1; x <= pRect->s16X2; x++ )
		{
			if ( pSrc[x] )
			{
				xsum += x;
				ysum += y;
				value = pSrc[x];
				value = value>>IP_GRAY_LEVEL_B;
				pHistogram32[value]++;
			}
		}
		pSrc += width;
	}
}
/**
* 计算256级灰度直方图
* @param src 输入的灰度图像指针
* @param stride 输入的灰度图像的stride
* @param width 图像宽度
* @param height 图像高度
* @param pRect 指定计算灰度直方图的区域
* @param pHistogram256 灰度直方图指针,长度为256
*/
IMP_VOID ipHistogramGray256(IMP_U8 *src, IMP_U16 stride, IMP_U16 width,IMP_U16 height,IMP_RECT_S *pRect,IMP_U32 *pHistogram256)
{
	IMP_S32 y, x, xsum, ysum;
	IMP_U8 value;

	IMP_U8 *pData = src + pRect->s16Y1 * stride;
	xsum = ysum = 0;

	for ( y=pRect->s16Y1; y <= pRect->s16Y2; y++ )
	{
		for ( x=pRect->s16X1; x <= pRect->s16X2; x++ )
		{
			if ( pData[x] )
			{
				xsum += x;
				ysum += y;
				value = pData[x];
				pHistogram256[value]++;
			}
		}
		pData += stride;
	}
}

/**
* 计算区域的彩色直方图
* @param src 输入的彩色图像指针
* @param pRect 指定计算灰度直方图的区域
* @param pHistogram 彩色直方图指针
* @param pHistgramRGB 彩色直方图矩特征
*/
IMP_VOID ipHistogramRGB(IMP_U8 *src, IMP_U16 width, IMP_U16 height,
						IMP_RECT_S *pRect,IpHistgramRGB *pHistgramRGB)
{
	IMP_S32 i, j;
	IMP_U32 *pHistogram = pHistgramRGB->pHistogram;

	IMP_S32 rgb_level;
	IMP_S32 left = pRect->s16X1;
	IMP_S32 right = pRect->s16X2;
	IMP_S32 top = pRect->s16Y1;
	IMP_S32 bottom = pRect->s16Y2;
	IMP_U8 *pRgb = src+ top * width * 3;

	for ( j = top; j <= bottom; j++ )
	{
        for ( i = left; i <= right; i++ )
		{
				// 统计彩色直方图
				rgb_level = ((IMP_S32)(pRgb[i * 3] >> IP_RGB_LEVEL_B) << (8 - IP_RGB_LEVEL_B) * 2)
					+((IMP_S32)(pRgb[i * 3 + 1] >> IP_RGB_LEVEL_B) << (8 - IP_RGB_LEVEL_B))
					+((pRgb[i * 3 + 2] >> IP_RGB_LEVEL_B));
				pHistogram[rgb_level]++;

		}
		pRgb += width * 3;
	}
}



/**
* 计算区域中心点、彩色直方图及其123阶矩
* @param pRGBImage 输入的彩色图像指针
* @param src 输入的灰度图像指针
* @param pRect 指定计算灰度直方图的区域
* @param pHistogram 彩色直方图指针
* @param pHistgramRGB 彩色直方图矩特征
* @param flagUseBottom 是否使用目标脚底作为轨迹跟踪点(取值范围：0(否)，1(是)；缺省值：0)
*/
IMP_VOID ipHistogramRGBStatistics(const RGB_IMAGE_S *pRGBImage,GRAY_IMAGE_S *src,
						IMP_RECT_S *pRect,IpHistgramRGBStatistics *pHistgramRGB,
						IMP_S32 flagUseBottom)
{
	IMP_S32 i, j;
	IMP_U32 *pHistogram = pHistgramRGB->pHistogram;
	IMP_FLOAT *E = pHistgramRGB->E;
	IMP_FLOAT *sigma = pHistgramRGB->sigma;
	IMP_FLOAT *skewness = pHistgramRGB->skewness;
	IMP_S32 no_divide = 0, xsum = 0, ysum = 0;
	IMP_S32 rgb_level;
	IMP_S32 left = pRect->s16X1;
	IMP_S32 right = pRect->s16X2;
	IMP_S32 top = pRect->s16Y1;
	IMP_S32 bottom = pRect->s16Y2;
	IMP_S32 x, y;
	IMP_S32 width = src->s32W;
	IMP_U8 *pFg = src->pu8Data + top * width;
	IMP_U8 *pRgb = pRGBImage->pu8Data + top * width * 3;
	IMP_U8 r, g, b;

	for ( j = top; j <= bottom; j++ )
	{
        for ( i = left; i <= right; i++ )
		{
			if ( pFg[i] )
			{
				no_divide++;
				xsum += i;
				ysum += j;
				// 统计彩色直方图
				rgb_level = ((IMP_S32)(pRgb[i * 3] >> IP_RGB_LEVEL_B) << (8 - IP_RGB_LEVEL_B) * 2)
					+((IMP_S32)(pRgb[i * 3 + 1] >> IP_RGB_LEVEL_B) << (8 - IP_RGB_LEVEL_B))
					+((pRgb[i * 3 + 2] >> IP_RGB_LEVEL_B));
				pHistogram[rgb_level]++;

				E[0] += pRgb[i * 3];
				E[1] += pRgb[i * 3 + 1];
				E[2] += pRgb[i * 3 + 2];
			}
		}
		pFg += width; pRgb += width * 3;
	}
	//一阶矩
	E[0] /= no_divide;
	E[1] /= no_divide;
	E[2] /= no_divide;

	//二三阶矩，求标准差sigma和偏斜度skewness
	pFg = src->pu8Data + top * width;
	pRgb = pRGBImage->pu8Data + top * width * 3;

	for ( j = top; j <= bottom; j++ )
	{
        for ( i = left; i <= right; i++ )
		{
			if ( pFg[i] )
			{
				b = pRgb[i * 3];
				g = pRgb[i * 3 + 1];
				r = pRgb[i * 3 + 2];

				sigma[0] += (b - E[0])*(b - E[0]);
				sigma[1] += (g - E[1])*(g - E[1]);
				sigma[2] += (r - E[2])*(r - E[2]);

				skewness[0] += (b - E[0])*(b - E[0])*(b - E[0]);
				skewness[1] += (g - E[1])*(g - E[1])*(g - E[1]);
				skewness[2] += (r - E[2])*(r - E[2])*(r - E[2]);
			}
		}
		pFg += width; pRgb += width * 3;
	}
	sigma[0] = (IMP_FLOAT)sqrt(sigma[0] / no_divide);
	sigma[1] = (IMP_FLOAT)sqrt(sigma[1] / no_divide);
	sigma[2] = (IMP_FLOAT)sqrt(sigma[2] / no_divide);

	if (skewness[0] >= 0.0)
		skewness[0] = (IMP_FLOAT)pow(skewness[0] / no_divide,1.0 / 3);
	else
		skewness[0] = (IMP_FLOAT)(pow(fabs(skewness[0]) / no_divide,1.0 / 3) * (-1.0));

	if (skewness[1] >= 0.0)
		skewness[1] = (IMP_FLOAT)pow(skewness[1] / no_divide,1.0 / 3);
	else
		skewness[1] = (IMP_FLOAT)(pow(fabs(skewness[1]) / no_divide,1.0 / 3) * (-1.0));

	if (skewness[2]>=0.0)
		skewness[2] = (IMP_FLOAT)pow(skewness[2] / no_divide,1.0 / 3);
	else
		skewness[2] = (IMP_FLOAT)(pow(fabs(skewness[2]) / no_divide,1.0 / 3) * (-1.0));

	// 计算区域质心
	if( flagUseBottom == 1 )
	{
		pHistgramRGB->centroid.s16X = ((left + right) >> 1);
		pHistgramRGB->centroid.s16Y = bottom;
	}
	else
	{
		x = xsum / no_divide;
		y = ysum / no_divide;
		pHistgramRGB->centroid.s16X = x > 1 ? x : 1;
		pHistgramRGB->centroid.s16Y = y > 1 ? y : 1;
	}
}



//////////////////////////////////////////////////////////////////////////
/// 形态学处理
/// 腐蚀、膨胀、开运算、闭运算
//////////////////////////////////////////////////////////////////////////
#if 0
static IMP_VOID ipMorphKernelImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, GRAY_IMAGE_S *tmp,
						   const IMP_U8 cd, const IMP_U8 ce,
						   const IMP_S32 cd0, const IMP_S32 ce0 )
{
	IMP_S32 width, height, x, y, tar;
	IMP_U8 *ptrD1, *ptrTmpD1, *ptrE1, *ptrTmpE1;
	width = dst->s32W;
	height = dst->s32H;

	memcpy( tmp->pu8Data, src->pu8Data, (width<<1) );
	for( y = 1; y < height-2; y++ )
	{
		ptrE1 = dst->pu8Data + width * y + 1;
		ptrTmpE1 = tmp->pu8Data + width * y + 1;
		ptrD1 = src->pu8Data + width * (y+1) + 1;
		ptrTmpD1 = tmp->pu8Data + width * (y+1) + 1;

		for( x = 1; x < width-1; x++ )
		{
			tar = (	ptrD1[0] + ptrD1[-1] + ptrD1[1] + ptrD1[-width] + ptrD1[width] );

			ptrTmpD1[0] = (tar==ce0 ? ce : cd);

			tar = (	ptrTmpE1[0] + ptrTmpE1[-1] + ptrTmpE1[1] + ptrTmpE1[-width] + ptrTmpE1[width] );

			ptrE1[0] = (tar==cd0 ? cd : ce);

			ptrD1++; ptrTmpD1++;
			ptrE1++; ptrTmpE1++;
		}
	}

	if( src != dst )
	{
		ptrE1 = dst->pu8Data;
		ptrD1 = src->pu8Data;
		for( y = 0; y < height; y++ )
		{
			ptrE1[0] = ptrD1[0];
			ptrE1[width-1] = ptrD1[width-1];
			ptrE1 += width;
			ptrD1 += width;
		}
		memcpy( dst->pu8Data, src->pu8Data, width);
		memcpy( dst->pu8Data + width * (height-2), src->pu8Data + width * (height-2), (width<<1));
	}
}

#else

IMP_VOID ipMorphKernelImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, GRAY_IMAGE_S *tmp,
						   const IMP_U8 cd, const IMP_U8 ce, const IMP_S32 cd0, const IMP_S32 ce0 )
{
	IMP_S32 width, height, x, y, tar;
	IMP_U8 *ptrD1, *ptrTmpD1, *ptrD2, *ptrTmpD2;
	IMP_U8 *ptrE1, *ptrTmpE1, *ptrE2, *ptrTmpE2;
	IMP_U8 i10, i11, i12, i13, i14;
	IMP_U8 i22, i23, i24;
	IMP_U8 tmp10, tmp11, tmp12, tmp13, tmp14;
	IMP_U8 tmp22, tmp23, tmp24;

	width = dst->s32W;
	height = dst->s32H;

	memcpy( tmp->pu8Data, src->pu8Data, (width << 1) );

	for( y = 1; y < height-2; y++ )
	{
		ptrE1 = dst->pu8Data + width*y + 1;
		ptrD1 = src->pu8Data + width*(y + 1) + 1;
		ptrTmpE1 = tmp->pu8Data + width*y + 1;
		ptrTmpD1 = tmp->pu8Data + width*(y + 1) + 1;

		ptrE2 = dst->pu8Data + width*y + 2;
		ptrD2 = src->pu8Data + width*(y + 1) + 2;
		ptrTmpE2 = tmp->pu8Data + width*y + 2;
		ptrTmpD2 = tmp->pu8Data + width*(y + 1) + 2;

		for( x = 1; x < width-1; x += 2 )
		{
			i10 = ptrD1[-1];     i11 = ptrD1[0];      i12 = ptrD1[1];
			i22 = ptrD2[1];      i13 = ptrD1[-width]; i23 = ptrD2[-width];
			i14 = ptrD1[width];	 i24 = ptrD2[width];

			tmp10 = ptrTmpE1[-1]; tmp11 = ptrTmpE1[0];      tmp12 = ptrTmpE1[1];
			tmp22 = ptrTmpE2[1];  tmp13 = ptrTmpE1[-width];	tmp23 = ptrTmpE2[-width];

			tar = (	i10 + i11 + i12 + i13 + i14 );
 			tmp14 = (tar==ce0 ? ce : cd);
			ptrTmpD1[0] = tmp14;

			tar = (	i11 + i12 + i22 + i23 + i24 );
			tmp24 = (tar==ce0 ? ce : cd);
			ptrTmpD2[0] = tmp24;

			tar = (	tmp10 + tmp11 + tmp12 + tmp13 + tmp14 );
			ptrE1[0] = (tar==cd0 ? cd : ce);

			tar = (	tmp11 + tmp12 + tmp22 + tmp23 + tmp24 );
			ptrE2[0] = (tar==cd0 ? cd : ce);

			ptrD1 += 2; ptrTmpD1 += 2;
			ptrE1 += 2; ptrTmpE1 += 2;

			ptrD2 += 2; ptrTmpD2 += 2;
			ptrE2 += 2; ptrTmpE2 += 2;

		}
	}

	if( src != dst )
	{
		ptrE1 = dst->pu8Data;
		ptrD1 = src->pu8Data;
		for( y = 0; y < height; y++ )
		{
			ptrE1[0] = ptrD1[0];
			ptrE1[width-1] = ptrD1[width-1];
			ptrE1 += width;
			ptrD1 += width;
		}
		memcpy( dst->pu8Data, src->pu8Data, width);
		memcpy( dst->pu8Data + width * (height - 2), src->pu8Data + width * (height - 2), (width << 1) );
	}
}

#endif

#if 0

static IMP_VOID ipMorphKernel2Image( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst,
							const IMP_S32 ce0, const IMP_U8 ce, const IMP_U8 cd )
{
	IMP_S32 width, height, x, y, tar;
	IMP_U8 *ptrD1, *ptrS1;
	width = dst->s32W; height = dst->s32H;

	for( y = 1; y < height - 1; y++ )
	{
		ptrS1 = src->pu8Data + width * y + 1;
		ptrD1 = dst->pu8Data + width * y + 1;

		ptrD1[-1] = ptrS1[-1];
		ptrD1[-2+width] = ptrS1[-2+width];

		for( x = 1; x < width - 1; x++ )
		{
			tar = (	ptrS1[0] + ptrS1[-1] + ptrS1[1] + ptrS1[-width] + ptrS1[width] );

			ptrD1[0] = (tar==ce0 ? ce : cd);

			ptrD1++; ptrS1++;
		}
	}

	memcpy( dst->pu8Data, src->pu8Data, width );
	memcpy( dst->pu8Data+width * (height - 1), src->pu8Data + width * (height - 1), width );
}

#else

// static IMP_VOID ipMorphKernel2Image( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, const IMP_S32 c0, const IMP_U8 ce, const IMP_U8 cd )
// {
// 	IMP_S32 width, height, x, y, tar;
// 	IMP_U8 *ptr1, *ptr2;
// 	IMP_U8 i10, i11, i12, i13, i21, i22, i31, i32;
// 	width = dst->s32W;
// 	height = dst->s32H;
//
// 	for( y = 1; y < height - 1; y++ )
// 	{
// 		ptr2 = dst->pu8Data + width * y + 1;
// 		ptr1 = src->pu8Data + width * y + 1;
//
// 		ptr2[-1] = ptr1[-1];
// 		ptr2[-2 + width] = ptr1[-2 + width];
//
// 		for( x = 1; x < width - 1; x += 2 )
// 		{
// 			i10 = ptr1[-1];
// 			i11 = ptr1[0];
// 			i12 = ptr1[1];
// 			i13 = ptr1[2];
// 			i21 = ptr1[-width];
// 			i31 = ptr1[width];
// 			i22 = ptr1[-width+1];
// 			i32 = ptr1[width+1];
//
// 			tar = (	i10 + i11 + i12 + i21 + i31 );
// 			ptr2[0] = (tar==c0 ? ce : cd);
//
// 			tar = (	i11 + i12 + i13 + i22 + i32 );
// 			ptr2[1] = (tar==c0 ? ce : cd);
//
// 			ptr1 += 2;
// 			ptr2 += 2;
// 		}
// 	}
//
// 	memcpy( dst->pu8Data, src->pu8Data, width );
// 	memcpy( dst->pu8Data + width * (height - 1), src->pu8Data + width * (height - 1), width );
// }
static IMP_VOID ipMorphKernel2Image( IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height, const IMP_U8 *mask)
{
	IMP_S32 x, y, result;

	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;

	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;

	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip one stride
	p1 += stride1;
	p2 += stride2;

	// for each line
	for ( y = 1; y < heightM1; y++ )
	{
		// skip one pixel
		p1++;
		p2++;
		// for each pixel
		for ( x = 1; x < widthM1; x++, p1++, p2++ )
		{
			if (mask[0] != 0) result &= *(p1 - stride1 - 1);
			if (mask[1] != 0) result &= *(p1 - stride1);
			if (mask[2] != 0) result &= *(p1 - stride1 + 1);
			if (mask[3] != 0) result &= *(p1 - 1);
			if (mask[4] != 0) result &= *(p1);
			if (mask[5] != 0) result &= *(p1 + 1);
			if (mask[6] != 0) result &= *(p1 + stride1 - 1);
			if (mask[7] != 0) result &= *(p1 + stride1);
			if (mask[8] != 0) result &= *(p1 + stride1 + 1);

			(*p2) = result ==0 ? 0: 255;
		}
		p1 += (offset1 + 1);
		p2 += (offset2 + 1);
	}
}
#endif

IMP_VOID ipAndTwoImages( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst )
{
	IMP_S32 width, height, x, y;
	IMP_U8 *ptrD1, *ptrS1;
	IMP_S32 i11, i12, i13, i14;
	IMP_S32 i21, i22, i23, i24;
	IMP_S32 i31, i32, i33, i34;
	IMP_S32 i41, i42, i43, i44;
	width = dst->s32W; height = dst->s32H;

	for( y = 0; y < height; y ++ )
	{
		ptrS1 = src->pu8Data + width * y;
		ptrD1 = dst->pu8Data + width * y;

		for( x = 0; x < width; x += 4 )
		{
			i11 = ptrD1[0];
			i12 = ptrS1[0];

			i21 = ptrD1[1];
			i22 = ptrS1[1];

			i31 = ptrD1[2];
			i32 = ptrS1[2];

			i41 = ptrD1[3];
			i42 = ptrS1[3];

			i13 = ( i11 && i12 );
			i23 = ( i21 && i22 );
			i33 = ( i31 && i32 );
			i43 = ( i41 && i42 );

			i14 = i13 ? i11 : 0;
			i24 = i23 ? i21 : 0;
			i34 = i33 ? i31 : 0;
			i44 = i43 ? i41 : 0;

			ptrD1[0] = i14;
			ptrD1[1] = i24;
			ptrD1[2] = i34;
			ptrD1[3] = i44;

			ptrD1 += 4; ptrS1 += 4;
		}
	}
}
/**
* 形态学闭运算
* @param src 源图像
* @param dst 目的图像
* @param tmp 中间缓冲图像
* @param cnt 运算次数
*/
IMP_VOID ipMorphCloseImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, GRAY_IMAGE_S *tmp, IMP_S32 cnt )
{
	ipMorphKernelImage( src, dst, tmp, 255, 0, 255*5, 0 );
}
/**
* 形态学开运算
* @param src 源图像
* @param dst 目的图像
* @param tmp 暂存图像
* @param cnt 运算次数
*/
IMP_VOID ipMorphOpenImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, GRAY_IMAGE_S *tmp, IMP_S32 cnt )
{
	ipMorphKernelImage( src, dst, tmp, 0, 255, 0, 255*5 );
}
/**
* 形态学膨胀运算
* @param src 源图像
* @param stride1 源图像 stride
* @param dst 目的图像
* @param stride2 源图像 stride
* @param width 图像宽度
* @param height 图像高度
* @param mask 膨胀使用的模板
*/
IMP_VOID ipMorphDilateImage ( IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height, const IMP_U8 *mask)
{
	IMP_S32 x, y, result;

	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;

	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;

	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip one stride
	p1 += stride1;
	p2 += stride2;

	// for each line
	for ( y = 1; y < heightM1; y++ )
	{
		// skip one pixel
		p1++;
		p2++;
		// for each pixel
		for ( x = 1; x < widthM1; x++, p1++, p2++ )
		{
			result = 0;
			if (mask[0] != 0) result |= *(p1 - stride1 - 1);
			if (mask[1] != 0) result |= *(p1 - stride1);
			if (mask[2] != 0) result |= *(p1 - stride1 + 1);
			if (mask[3] != 0) result |= *(p1 - 1);
			if (mask[4] != 0) result |= *(p1);
			if (mask[5] != 0) result |= *(p1 + 1);
			if (mask[6] != 0) result |= *(p1 + stride1 - 1);
			if (mask[7] != 0) result |= *(p1 + stride1);
			if (mask[8] != 0) result |= *(p1 + stride1 + 1);

			(*p2) = (result !=0? 255 : 0);
		}
		p1 += (offset1 + 1);
		p2 += (offset2 + 1);
	}
}
// IMP_VOID ipMorphDilateImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, IMP_S32 cnt )
// {
// 	ipMorphKernel2Image( src, dst, 0, 0, 255 );
// }
/**
* 形态学腐蚀运算
* @param src 源图像
* @param stride1 源图像 stride
* @param dst 目的图像
* @param stride2 源图像 stride
* @param width 图像宽度
* @param height 图像高度
* @param mask 膨胀使用的模板
*/
IMP_VOID ipMorphErodeImage( IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height, const IMP_U8 *mask)
{
	IMP_S32 x, y, result;

	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;

	IMP_S32 offset1 = stride1 - width;
	IMP_S32 offset2 = stride2 - width;

	IMP_U8 *p1, *p2;
	p1 = src;
	p2 = dst;

	// skip one stride
	p1 += stride1;
	p2 += stride2;

	// for each line
	for ( y = 1; y < heightM1; y++ )
	{
		// skip one pixel
		p1++;
		p2++;
		// for each pixel
		for ( x = 1; x < widthM1; x++, p1++, p2++ )
		{
			result = 255;
			if (mask[0] != 0) result &= *(p1 - stride1 - 1);
			if (mask[1] != 0) result &= *(p1 - stride1);
			if (mask[2] != 0) result &= *(p1 - stride1 + 1);
			if (mask[3] != 0) result &= *(p1 - 1);
			if (mask[4] != 0) result &= *(p1);
			if (mask[5] != 0) result &= *(p1 + 1);
			if (mask[6] != 0) result &= *(p1 + stride1 - 1);
			if (mask[7] != 0) result &= *(p1 + stride1);
			if (mask[8] != 0) result &= *(p1 + stride1 + 1);

			(*p2) = (result != 0 ? 255 : 0);
		}
		p1 += (offset1 + 1);
		p2 += (offset2 + 1);
	}
}


////////////////////////////////////////////////////////////////////////
//BOOL MakeDilation()
//----------------------------------------------------------------------
//基本功能：本函数对图像数据执行膨胀操作。
//----------------------------------------------------------------------
//参数说明：IMP_S32		*nMask			结构元素数组指针
//			IMP_S32		 nMaskLen		结构元素长度（以点数为计数单位）
//			IMP_U8 *pOut		输出图像数据指针
//			IMP_U8 *pIn		输入图像数据指针
//			IMP_S32		 nWidthBytes	图像宽度（以字节表示）
//			IMP_S32		 nWidth			图像宽度（以像素表示）
//			IMP_S32		 nHeight		图像高度
//----------------------------------------------------------------------
//返    回：BOOL
//			成功返回TRUE，失败返回FALSE。
//----------------------------------------------------------------------
//注    意：此函数声明为保护型，只能在CMorphPro类中使用。
//----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////
IMP_S32 MakeDilation(IMP_S32 *nMask, IMP_S32 nMaskLen, GRAY_IMAGE_S *pOut,GRAY_IMAGE_S *pIn)
{
	//定义变量
	IMP_S32 x, y, k;
	IMP_U8 Mark;
	IMP_S32 nWidth=pIn->s32W;
	IMP_S32 nHeight=pIn->s32H;
	//若传入的图像数据为空，将无法完成操作，直接返回。
	if(pOut == NULL || pIn == NULL) return 0;

	for( y = 0; y < nHeight; y++ )
	{
		IMP_U8 *pOutTemp = pOut->pu8Data+y * nWidth;
		for( x = 0; x < nWidth; x++ )
		{
			Mark = 0;
			for (k = 0; k < nMaskLen; k++)
			{
				//不能处理边界像素
				if ((x + nMask[2 * k] >= 0 ) &&
					(x + nMask[2 * k] < nWidth) &&
					(y + nMask[2 * k + 1] >= 0) &&
					(y + nMask[2 * k + 1] < nHeight))
				{
					IMP_U8 Data;
					//取模板中的像素值
					IMP_U8 *pTemp = pIn->pu8Data+y * nWidth;
					pTemp += nMask[2 * k + 1] * nWidth;
					Data = pTemp[x + nMask[2 * k]];
					if(Data == 255)
					{
						Mark = 1;
						k = nMaskLen;
					}
				}
			}
			if (Mark == 1)	pOutTemp[x] = 255;
		}
	}

	return 1;
}


// 使用数学形态学方法填充前景图像中的孔洞和过滤噪声
IMP_VOID ipRemoveStablizationForeground( GRAY_IMAGE_S *img, IMP_POINT_S *mv )
{
	IMP_S32 width, height, mvx, mvy;
	IMP_U8 *data;
	IMP_S32 xx, yy, lb_x, lb_y, ru_x, ru_y;

	width = img->s32W;
	height = img->s32H;
	mvx = mv->s16X; mvy = mv->s16Y;

	if( mvy )
	{
		lb_x = 0; ru_x = width-1;
		if( mvy>0 )
		{
			lb_y = height-mvy;
			ru_y = height-1;
		}
		else
		{
			lb_y = 0;
			ru_y = -mvy;
		}
		for( yy=lb_y; yy<=ru_y; yy++ )
		{
			data = img->pu8Data + width*yy;
			for( xx=lb_x; xx<=ru_x; xx++ )
				data[xx] = 0;
		}
	}
	if( mvx )
	{
		lb_y = 0; ru_y = height-1;
		if( mvx>0 )
		{
			lb_x = width-mvx;
			ru_x = width-1;
		}
		else
		{
			lb_x = 0;
			ru_x = -mvx;
		}
		for( yy=lb_y; yy<=ru_y; yy++ )
		{
			data = img->pu8Data + width*yy;
			for( xx=lb_x; xx<=ru_x; xx++ )
				data[xx] = 0;
		}
	}

}

#if 0

IMP_VOID ipRemoveShakingForeground( GRAY_IMAGE_S *img_bg, GRAY_IMAGE_S *img_in, GRAY_IMAGE_S *img_fg, IMP_S32 gray_thld )
{
	IMP_S32 height, width, x, y;
	IMP_U8 *dat_bg, *dat_in, *pFg;
	IMP_S32 val0, value1, value2, value3, val4, val5, val6, val7, val8, val9, val_out;

	width = img_bg->s32W;
	height = img_bg->s32H;
	for( y=1; y<height-1; y++ )
	{
		dat_bg = img_bg->pu8Data + width*y+1;
		dat_in = img_in->pu8Data + width*y+1;
		pFg = img_fg->pu8Data + width*y+1;
		for( x=1; x<width-1; x++ )
		{
			val0 = dat_in[0];
			value1 = dat_bg[-width-1];
			value2 = dat_bg[-width];
			value3 = dat_bg[-width+1];
			val4 = dat_bg[-1];
			val5 = dat_bg[0];
			val6 = dat_bg[1];
			val7 = dat_bg[width-1];
			val8 = dat_bg[width];
			val9 = dat_bg[width+1];
			val_out = (	0
					|| ( abs( val0 - value1 ) <= gray_thld )
					|| ( abs( val0 - value2 ) <= gray_thld )
					|| ( abs( val0 - value3 ) <= gray_thld )
					|| ( abs( val0 - val4 ) <= gray_thld )
					|| ( abs( val0 - val5 ) <= gray_thld )
					|| ( abs( val0 - val6 ) <= gray_thld )
					|| ( abs( val0 - val7 ) <= gray_thld )
					|| ( abs( val0 - val8 ) <= gray_thld )
					|| ( abs( val0 - val9 ) <= gray_thld )
				);

			pFg[0] = ( pFg[0] && !val_out ) ? 255 : 0;

			dat_in++;
			dat_bg++;
			pFg++;
		}
	}
}

#else

#define SHAKING_ROW_INCR	0
IMP_VOID ipRemoveShakingForeground( GRAY_IMAGE_S *img_bg, GRAY_IMAGE_S *img_in, GRAY_IMAGE_S *img_fg, IMP_S32 gray_thld )
{
	IMP_S32 height, width, x, y;
	IMP_U8 *dat_in0, *dat_fg0;
	IMP_U8 *dat_bg1, *dat_bg2, *dat_bg3;
	IMP_S32 val_fg00, val_fg01;
	IMP_S32 val_in00, val_in01;
	IMP_S32 val_out00, val_out01;
	IMP_S32 val11, val12, val13, val14;
	IMP_S32 val21, val22, val23, val24;
	IMP_S32 val31, val32, val33, val34;
#if SHAKING_ROW_INCR
	IMP_U8 *dat_in1, *dat_fg1;
	IMP_U8 *dat_bg4;
	IMP_S32 val_fg10, val_fg11;
	IMP_S32 val_in10, val_in11;
	IMP_S32 val_out10, val_out11;
	IMP_S32 val41, val42, val43, val44;
#endif

	width = img_bg->s32W;
	height = img_bg->s32H;
	for( y=1; y<height-1; y+=(SHAKING_ROW_INCR+1) )
	{
		dat_bg1 = img_bg->pu8Data + width*(y-1);
		dat_bg2 = img_bg->pu8Data + width*y;
		dat_bg3 = img_bg->pu8Data + width*(y+1);
		dat_in0 = img_in->pu8Data + width*y+1;
		dat_fg0 = img_fg->pu8Data + width*y+1;
#if SHAKING_ROW_INCR
		dat_bg4 = img_bg->pu8Data + width*(y+2);
		dat_in1 = img_in->pu8Data + width*(y+1)+1;
		dat_fg1 = img_fg->pu8Data + width*(y+1)+1;
#endif
		for( x=1; x<width-1; x+=2 )
		{
			val_in00 = dat_in0[0];
			val_in01 = dat_in0[1];
			dat_in0 += 2;

#if SHAKING_ROW_INCR
			val_in10 = dat_in1[0];
			val_in11 = dat_in1[1];
			dat_in1 += 2;
#endif

			val11 = dat_bg1[0];
			val12 = dat_bg1[1];
			val13 = dat_bg1[2];
			val14 = dat_bg1[3];
			dat_bg1 += 2;

			val21 = dat_bg2[0];
			val22 = dat_bg2[1];
			val23 = dat_bg2[2];
			val24 = dat_bg2[3];
			dat_bg2 += 2;

			val31 = dat_bg3[0];
			val32 = dat_bg3[1];
			val33 = dat_bg3[2];
			val34 = dat_bg3[3];
			dat_bg3 += 2;

#if SHAKING_ROW_INCR
			val41 = dat_bg4[0];
			val42 = dat_bg4[1];
			val43 = dat_bg4[2];
			val44 = dat_bg4[3];
			dat_bg4 += 2;
#endif

			val_fg00 = dat_fg0[0];
			val_fg01 = dat_fg0[1];

#if SHAKING_ROW_INCR
			val_fg10 = dat_fg1[0];
			val_fg11 = dat_fg1[1];
#endif

			val_out00 = ( 0
						|| ( abs( val_in00 - val11 ) <= gray_thld )
						|| ( abs( val_in00 - val12 ) <= gray_thld )
						|| ( abs( val_in00 - val13 ) <= gray_thld )
						|| ( abs( val_in00 - val21 ) <= gray_thld )
						|| ( abs( val_in00 - val22 ) <= gray_thld )
						|| ( abs( val_in00 - val23 ) <= gray_thld )
						|| ( abs( val_in00 - val31 ) <= gray_thld )
						|| ( abs( val_in00 - val32 ) <= gray_thld )
						|| ( abs( val_in00 - val33 ) <= gray_thld )
						);

			val_out01 = ( 0
						|| ( abs( val_in01 - val12 ) <= gray_thld )
						|| ( abs( val_in01 - val13 ) <= gray_thld )
						|| ( abs( val_in01 - val14 ) <= gray_thld )
						|| ( abs( val_in01 - val22 ) <= gray_thld )
						|| ( abs( val_in01 - val23 ) <= gray_thld )
						|| ( abs( val_in01 - val24 ) <= gray_thld )
						|| ( abs( val_in01 - val32 ) <= gray_thld )
						|| ( abs( val_in01 - val33 ) <= gray_thld )
						|| ( abs( val_in01 - val34 ) <= gray_thld )
						);

#if SHAKING_ROW_INCR
			val_out10 = ( 0
						|| ( abs( val_in10 - val21 ) <= gray_thld )
						|| ( abs( val_in10 - val22 ) <= gray_thld )
						|| ( abs( val_in10 - val23 ) <= gray_thld )
						|| ( abs( val_in10 - val31 ) <= gray_thld )
						|| ( abs( val_in10 - val32 ) <= gray_thld )
						|| ( abs( val_in10 - val33 ) <= gray_thld )
						|| ( abs( val_in10 - val41 ) <= gray_thld )
						|| ( abs( val_in10 - val42 ) <= gray_thld )
						|| ( abs( val_in10 - val43 ) <= gray_thld )
						);

			val_out11 = ( 0
						|| ( abs( val_in11 - val22 ) <= gray_thld )
						|| ( abs( val_in11 - val23 ) <= gray_thld )
						|| ( abs( val_in11 - val24 ) <= gray_thld )
						|| ( abs( val_in11 - val32 ) <= gray_thld )
						|| ( abs( val_in11 - val33 ) <= gray_thld )
						|| ( abs( val_in11 - val34 ) <= gray_thld )
						|| ( abs( val_in11 - val42 ) <= gray_thld )
						|| ( abs( val_in11 - val43 ) <= gray_thld )
						|| ( abs( val_in11 - val44 ) <= gray_thld )
						);
#endif

 			dat_fg0[0] = ( val_fg00 && !val_out00 ) ? 255 : 0;
 			dat_fg0[1] = ( val_fg01 && !val_out01 ) ? 255 : 0;
			dat_fg0 += 2;

#if SHAKING_ROW_INCR
			dat_fg1[0] = ( val_fg10 && !val_out10 ) ? 255 : 0;
			dat_fg1[1] = ( val_fg11 && !val_out11 ) ? 255 : 0;
			dat_fg1 += 2;
#endif

		}
	}
}

#endif



/**
* 计算图像指定区域的平均值和标准差
* @param src 图像指针
* @param width 图像宽度
* @param height 图像高度
* @param mean rect区域的均值
* @param std_dev rect区域的方差
* @param rect 区域
* @return
*  - -1计算失败
*  -  1计算成功
*/
IMP_S32 ipAvgSdv( const IMP_U8 *src, IMP_U16 width, IMP_U16 height, IMP_U8* _mean, IMP_FLOAT* _std_dev, const IMP_RECT_S* rect )
{
	IMP_U8 mean, *p;
	IMP_FLOAT sdv;
	IMP_S32 sum, i, j, count;

	//检测区域是否在图像的范围内
	if (rect->s16X1 < 0 || rect->s16X2 >= width || rect->s16Y1 <=0 || rect->s16Y2 >= height)
	{
		return -1;
	}

	sum = 0;
	count = 0;
	for ( j=rect->s16Y1; j< rect->s16Y2; j++ )
	{
		p = src + width * rect->s16Y1; //指向行
		for (i=rect->s16X1; i<rect->s16X2; i++ )
		{
			sum += p[i];
			count++;
		}
	}

	//计算均值
	if (count ==0 )
	{
		return -1;
	}
	mean = sum / count;
	*_mean = mean;

	//计算方差
	sum = 0;
	for ( j=rect->s16Y1; j< rect->s16Y2; j++ )
	{
		p = src + width * rect->s16Y1; //指向行
		for (i=rect->s16X1; i<rect->s16X2; i++ )
		{
			sum += (p[i] - mean) * (p[i] - mean);
		}
	}

	sdv = (IMP_FLOAT)sqrt( sum / count );

	*_std_dev = sdv;

	return 1;
}

/************************************************************************/
/*
* Func: rgb2hsi
*
*
* Desc: converts a value from RGB to HSI color space
*
*
* Params: r - normalized red value
*    g - normalized green value
*    b - normalized blue value
*    h - hue in degrees
*    s - normalized saturation
*    i - normalized intensity
*                                                                     */
/************************************************************************/


IMP_VOID rgb2hsi(IMP_DOUBLE *r1, IMP_DOUBLE *p1,IMP_DOUBLE *b1, IMP_DOUBLE *h, IMP_DOUBLE *s, IMP_DOUBLE *v )
{
	IMP_FLOAT min1, max1, delta;
	IMP_FLOAT r,g,b;

	r = (IMP_FLOAT)*r1;
    g = (IMP_FLOAT)*p1;
    b = (IMP_FLOAT)*b1;
    //     min1 = min( r, g );
    min1 = min(min(r,g),b);
    //     max1 = max( r, g);
    max1 = max(max(r,g),b);
    *v = max1;
    delta = max1 - min1;
    if( max1 != 0 )
		*s = (delta / max1);
    else
    {
		// r = g = b = 0
		// s = 0, v 未定义
		*s = 0;
		*h = -1;
		return;
    }

    if( r == max1 )
		*h = ( g - b ) / delta;         // 在 yellow &amt; magenta 之间
    else if( g == max1 )
		*h = 2 + ( b - r ) / delta;     // 在 cyan &amt; yellow 之间
    else
		*h = 4 + ( r - g ) / delta;     // 在 magenta &amt; cyan 之间

    *h *= 60;                               // degrees
    if( *h < 0 )
    {
		*h += 360;
    }

}

IMP_VOID rgb2hsi_uint8(IMP_U8* pRGB, IMP_U8* pHSV )
{
	IMP_U8 r1, p1,b1, *h1, *s1, *v1;
	IMP_FLOAT min1, max1, delta;
	IMP_FLOAT r,g,b,h,s,v;

	b1 = pRGB[0];
	p1 = pRGB[1];
	r1 = pRGB[2];

	h1 = pHSV;
	s1 = pHSV + 1;
	v1 = pHSV + 2;


	r = (IMP_FLOAT)(r1 / 256.0);
    g = (IMP_FLOAT)(p1 / 256.0);
    b = (IMP_FLOAT)(b1 / 256.0);

    min1 = min(min(r,g),b);
    max1 = max(max(r,g),b);
    v = max1;
    delta = max1 - min1;
    if ( max1 ==0)
	{
		s = 0;
	}
	else
	{
		s = s = (delta / max1);
	}
	if (s == 0)
	{
		h = 0;
	}
	else
	{
		if( r == max1 )
			h = ( g - b ) / delta;         // 在 yellow &amt; magenta 之间
		else if( g == max1 )
			h = (IMP_FLOAT)( 2.0 + ( b - r ) / delta);     // 在 cyan &amt; yellow 之间
		else
			h = (IMP_FLOAT)( 4.0 + ( r - g ) / delta);     // 在 magenta &amt; cyan 之间

		h *= 6.0;                               // degrees   //to bring it to a number between 0 and 1
		if( h < 0 )
		h++;
	}


	*h1=(IMP_U8)(h * 255.0);
	*s1=(IMP_U8)(s * 255.0);
	*v1=(IMP_U8)(v * 255.0);
    *h1 = icvHue255To180[*h1];//调整到180
}
IMP_VOID HSVtoRGB( IMP_U8 *r, IMP_U8 *g, IMP_U8 *b, IMP_FLOAT h, IMP_FLOAT s, IMP_FLOAT v )
{
	IMP_S32 i;
	IMP_FLOAT f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;   // sector 0 to 5
	i = (IMP_S32)floor( h );
	f = h - i;   // factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
	case 0:
		*r = (IMP_U8)v;
		*g = (IMP_U8)t;
		*b = (IMP_U8)p;
		break;
	case 1:
		*r = (IMP_U8)q;
		*g = (IMP_U8)v;
		*b = (IMP_U8)p;
		break;
	case 2:
		*r = (IMP_U8)p;
		*g = (IMP_U8)v;
		*b = (IMP_U8)t;
		break;
	case 3:
		*r = (IMP_U8)p;
		*g = (IMP_U8)q;
		*b = (IMP_U8)v;
		break;
	case 4:
		*r = (IMP_U8)t;
		*g = (IMP_U8)p;
		*b = (IMP_U8)v;
		break;
	default:  // case 5:
		*r = (IMP_U8)v;
		*g = (IMP_U8)p;
		*b = (IMP_U8)q;
		break;
	}
}

IMP_VOID ipImageConvertHSV2RGB(IMP_FLOAT* pHSV, IMP_U8* pRGB, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 i;
	IMP_FLOAT h, s, v;
	IMP_U8 *r, *g, *b;


	for (i = 0;i < width * height; i++)
	{
		h = *pHSV; pHSV++;
		s = *pHSV; pHSV++;
		v = *pHSV; pHSV++;
		b = pRGB; pRGB++;
		g = pRGB; pRGB++;
		r = pRGB; pRGB++;
		HSVtoRGB(r,g,b,h,s,v);
	}

}

IMP_VOID ipRGB2HSL( PIXEL_S rgb, HSL_PIXEL_S *hsl )
{
	IMP_FLOAT	r = ( rgb.u8R   / 255.0 );
	IMP_FLOAT	g = ( rgb.u8G / 255.0 );
	IMP_FLOAT	b = ( rgb.u8B  / 255.0 );

	IMP_FLOAT	min = IMP_MIN( IMP_MIN( r, g ), b );
	IMP_FLOAT	max = IMP_MAX( IMP_MAX( r, g ), b );
	IMP_FLOAT	delta = max - min;

	IMP_FLOAT del_r, del_g, del_b, hue;

	// get luminance value
	hsl->f32L = ( max + min ) / 2;

	if ( delta == 0 )
	{
		// pGray1 color
		hsl->f32H = 0;
		hsl->f32S = 0.0;
	}
	else
	{
		// get saturation value
		hsl->f32S = ( hsl->f32L < 0.5 ) ? ( delta / ( max + min ) ) : ( delta / ( 2 - max - min ) );

		// get hue value
		del_r = ( ( ( max - r ) / 6 ) + ( delta / 2 ) ) / delta;
		del_g = ( ( ( max - g ) / 6 ) + ( delta / 2 ) ) / delta;
		del_b = ( ( ( max - b ) / 6 ) + ( delta / 2 ) ) / delta;


		if ( r == max )
			hue = del_b - del_g;
		else if ( g == max )
			hue = ( 1.0 / 3 ) + del_r - del_b;
		else
			hue = ( 2.0 / 3 ) + del_g - del_r;

		// correct hue if needed
		if ( hue < 0 )
			hue += 1;
		if ( hue > 1 )
			hue -= 1;

		hsl->f32H = (IMP_FLOAT) ( hue * 360 );
	}
}

IMP_FLOAT Hue_2_RGB( IMP_FLOAT v1, IMP_FLOAT v2, IMP_FLOAT vH )
{
	if ( vH < 0 )
		vH += 1;
	if ( vH > 1 )
		vH -= 1;
	if ( ( 6 * vH ) < 1 )
		return ( v1 + ( v2 - v1 ) * 6 * vH );
	if ( ( 2 * vH ) < 1 )
		return v2;
	if ( ( 3 * vH ) < 2 )
		return ( v1 + ( v2 - v1 ) * ( ( 2.0 / 3 ) - vH ) * 6);
	return v1;
}

IMP_VOID ipHSL2RGB( HSL_PIXEL_S hsl, PIXEL_S *rgb )
{
	if ( hsl.f32S == 0 )
	{
		// pGray1 values
		rgb->u8B = rgb->u8G = rgb->u8R = (IMP_U8) ( hsl.f32L * 255 );
	}
	else
	{
		IMP_FLOAT	v1, v2;
		IMP_FLOAT	hue = (IMP_FLOAT) hsl.f32H / 360;

		v2 = ( hsl.f32L < 0.5 ) ?
			( hsl.f32L * ( 1 + hsl.f32S ) ) :
		( ( hsl.f32L + hsl.f32S ) - ( hsl.f32L * hsl.f32S ) );
		v1 = 2 * hsl.f32L - v2;

		rgb->u8R	= (IMP_U8)( 255 * Hue_2_RGB( v1, v2, hue + ( 1.0 / 3 ) ) );
		rgb->u8G	= (IMP_U8)( 255 * Hue_2_RGB( v1, v2, hue ) );
		rgb->u8B	= (IMP_U8)( 255 * Hue_2_RGB( v1, v2, hue - ( 1.0 / 3 ) ) );
	}
}

IMP_VOID ipImageConvertRGB2HSL(IMP_U8* pRGB, IMP_FLOAT* pHSL, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y;
	IMP_S32 offset = width;

	PIXEL_S rgb;
	HSL_PIXEL_S hsl;
	// do the job
	IMP_U8 *ptr = pRGB;
	IMP_FLOAT *ptrHsl = pHSL;

	// for each row
	for ( y = 0; y < height; y++ )
	{
		// for each pixel
		for ( x = 0; x < width; x++, ptr += 3, ptrHsl +=3)
		{
			rgb.u8B	= ptr[0];
			rgb.u8G	= ptr[1];
			rgb.u8R	= ptr[2];

			// convert to HSL
			ipRGB2HSL( rgb, &hsl );
			// check HSL values
			if (
				 ( hsl.f32S >= 0 ) && ( hsl.f32S <= 1.0 ) &&( hsl.f32L >= 0 ) && ( hsl.f32L <= 1.0 ) &&
				 ( hsl.f32H >= 0 ) && ( hsl.f32H <= 359 )
				)
			{
				ptrHsl[0] = hsl.f32H;
				ptrHsl[1] = hsl.f32S;
				ptrHsl[2] = hsl.f32L;
			}
			else
			{
				ptrHsl[0] = 0;
				ptrHsl[1] = 0;
				ptrHsl[2] = 0;
			}
		}
	}
}


IMP_VOID ipImageConvertHSL2RGB(IMP_FLOAT* pHSL, IMP_U8* pRGB, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y;
	IMP_S32 offset = width;

	PIXEL_S rgb;
	HSL_PIXEL_S hsl;
	// do the job
	IMP_U8 *ptr = pRGB;
	IMP_FLOAT *ptrHsl = pHSL;

	// for each row
	for ( y = 0; y < height; y++ )
	{
		// for each pixel
		for ( x = 0; x < width; x++, ptr += 3, ptrHsl +=3)
		{
			rgb.u8B	= ptr[0];
			rgb.u8G	= ptr[1];
			rgb.u8R	= ptr[2];

			hsl.f32H = ptrHsl[0];
			hsl.f32S = ptrHsl[1];
			hsl.f32L = ptrHsl[2];

			// convert back to RGB
			ipHSL2RGB(hsl, &rgb);

			ptr[0] = rgb.u8B;
			ptr[1] = rgb.u8G;
			ptr[2] = rgb.u8R;

		}
	}
}








IMP_VOID ipNearestNeighborResize(IMP_U8* src, IMP_U8* dst, IMP_U16 srcH, IMP_U16 srcW, IMP_U16 dstH, IMP_U16 dstW, IMP_U16 pixelSize)
{
	IMP_U8* p;

	IMP_S32 ox, oy, x, y, i;

	IMP_DOUBLE xFactor = (IMP_DOUBLE) srcW / dstW;
    IMP_DOUBLE yFactor = (IMP_DOUBLE) srcH / dstH;

	// for each line
	for ( y = 0; y < dstH; y++ )
	{
		// Y coordinate of the nearest point
		oy = (IMP_S32) ( y * yFactor );

		// for each pixel
		for ( x = 0; x < dstW; x++ )
		{
			// X coordinate of the nearest point
			ox = (IMP_S32) ( x * xFactor );

			p = src + oy * srcW + ox * pixelSize;

			for ( i = 0; i < pixelSize; i++, dst++, p++ )
			{
				*dst = *p;
			}
		}
	}
}


IMP_VOID ipBiLinearResize(IMP_U8* src, IMP_U8* dst, IMP_U16 srcH, IMP_U16 srcW, IMP_U16 dstH, IMP_U16 dstW, IMP_U16 pixelSize)
{

	IMP_S32 x, y, i;
	IMP_DOUBLE xFactor = (IMP_DOUBLE) srcW / dstW;
    IMP_DOUBLE yFactor = (IMP_DOUBLE) srcH / dstH;

	// coordinates of source points
	IMP_DOUBLE  ox, oy, dx1, dy1, dx2, dy2;
	IMP_S32     ox1, oy1, ox2, oy2;
	// width and height decreased by 1
	IMP_S32 ymax = srcH - 1;
	IMP_S32 xmax = srcW - 1;
	// temporary pointers
	IMP_U8 *tp1, *tp2;
	IMP_U8 *p1, *p2, *p3, *p4;

	// for each line
	for ( y = 0; y < dstH; y++ )
	{
		// Y coordinates
		oy  = (IMP_DOUBLE) y * yFactor;
		oy1 = (IMP_S32) oy;
		oy2 = ( oy1 == ymax ) ? oy1 : oy1 + 1;
		dy1 = oy - (IMP_DOUBLE) oy1;
		dy2 = 1.0 - dy1;

		// get temp pointers
		tp1 = src + oy1 * srcW;
		tp2 = src + oy2 * srcW;

		// for each pixel
		for ( x = 0; x < dstW; x++ )
		{
			// X coordinates
			ox  = (IMP_DOUBLE) x * xFactor;
			ox1 = (IMP_S32) ox;
			ox2 = ( ox1 == xmax ) ? ox1 : ox1 + 1;
			dx1 = ox - (IMP_DOUBLE) ox1;
			dx2 = 1.0 - dx1;

			// get four points
			p1 = tp1 + ox1 * pixelSize;
			p2 = tp1 + ox2 * pixelSize;
			p3 = tp2 + ox1 * pixelSize;
			p4 = tp2 + ox2 * pixelSize;

			// interpolate using 4 points
			for ( i = 0; i < pixelSize; i++, dst++, p1++, p2++, p3++, p4++ )
			{
				*dst = (IMP_U8) (dy2 * ( dx2 * ( *p1 ) + dx1 * ( *p2 ) ) + dy1 * ( dx2 * ( *p3 ) + dx1 * ( *p4 ) ) );
			}
		}
     }
}


IMP_DOUBLE BiCubicKernel( IMP_DOUBLE x)
{
	IMP_DOUBLE a, b, c, d, xm1, xp1, xp2;
	if ( x > 2.0 )
		return 0.0 ;
	xm1 = x - 1.0;
	xp1 = x + 1.0;
	xp2 = x + 2.0;

	a = ( xp2 <= 0.0 ) ? 0.0 : xp2 * xp2 * xp2;
	b = ( xp1 <= 0.0 ) ? 0.0 : xp1 * xp1 * xp1;
	c = ( x   <= 0.0 ) ? 0.0 : x * x * x;
	d = ( xm1 <= 0.0 ) ? 0.0 : xm1 * xm1 * xm1;

	return ( 0.16666666666666666667 * ( a - ( 4.0 * b ) + ( 6.0 * c ) - ( 4.0 * d ) ) );
}

IMP_VOID ipBiCubicResize(IMP_U8* src, IMP_U8* dst, IMP_U16 srcH, IMP_U16 srcW, IMP_U16 dstH, IMP_U16 dstW, IMP_U16 pixelSize)
{
	IMP_S32 x, y, m, n;
	IMP_DOUBLE xFactor = (IMP_DOUBLE) srcW / dstW;
    IMP_DOUBLE yFactor = (IMP_DOUBLE) srcH / dstH;

	// coordinates of source points and cooefficiens
	IMP_DOUBLE  ox, oy, dx, dy, k1, k2;
	IMP_S32     ox1, oy1, ox2, oy2;
	// destination pixel values
	IMP_DOUBLE r, g, b;
	// width and height decreased by 1
	IMP_S32 ymax = srcH - 1;
	IMP_S32 xmax = srcW - 1;
	// temporary pointer
    IMP_U8* p;

	if ( pixelSize == 1)//单通道灰度图
	{
		// grayscale
		for ( y = 0; y < dstH; y++ )
		{
			// Y coordinates
			oy  = (IMP_DOUBLE) y * yFactor - 0.5;
			oy1 = (IMP_S32) oy;
			dy  = oy - (IMP_DOUBLE) oy1;

			for ( x = 0; x < dstW; x++, dst++ )
			{
				// X coordinates
				ox  = (IMP_DOUBLE) x * xFactor - 0.5f;
				ox1 = (IMP_S32) ox;
				dx  = ox - (IMP_DOUBLE) ox1;

				// initial pixel value
				g = 0;

				for ( n = -1; n < 3; n++ )
				{
					// get Y cooefficient
					k1 = BiCubicKernel( dy - (IMP_DOUBLE) n );

					oy2 = oy1 + n;
					if ( oy2 < 0 )
						oy2 = 0;
					if ( oy2 > ymax )
						oy2 = ymax;

					for ( m = -1; m < 3; m++ )
					{
						// get X cooefficient
						k2 = k1 * BiCubicKernel( (IMP_DOUBLE) m - dx );

						ox2 = ox1 + m;
						if ( ox2 < 0 )
							ox2 = 0;
						if ( ox2 > xmax )
							ox2 = xmax;

						g += k2 * src[oy2 * srcW + ox2];
					}
				}
				*dst = (IMP_U8) g;
			}
		}
	}
	else if (pixelSize == 3)//RGB彩色图)
	{
		// RGB
		for ( y = 0; y < dstH; y++ )
		{
			// Y coordinates
			oy  = (IMP_DOUBLE) y * yFactor - 0.5f;
			oy1 = (IMP_S32) oy;
			dy  = oy - (IMP_DOUBLE) oy1;

			for ( x = 0; x < dstW; x++, dst += 3 )
			{
				// X coordinates
				ox  = (IMP_DOUBLE) x * xFactor - 0.5f;
				ox1 = (IMP_S32) ox;
				dx  = ox - (IMP_DOUBLE) ox1;

				// initial pixel value
				r = g = b = 0;

				for ( n = -1; n < 3; n++ )
				{
					// get Y cooefficient
					k1 = BiCubicKernel( dy - (IMP_DOUBLE) n );

					oy2 = oy1 + n;
					if ( oy2 < 0 )
						oy2 = 0;
					if ( oy2 > ymax )
						oy2 = ymax;

					for ( m = -1; m < 3; m++ )
					{
						// get X cooefficient
						k2 = k1 * BiCubicKernel( (IMP_DOUBLE) m - dx );

						ox2 = ox1 + m;
						if ( ox2 < 0 )
							ox2 = 0;
						if ( ox2 > xmax )
							ox2 = xmax;

						// get pixel of original image
						p = src + oy2 * srcW + ox2 * 3;

						b += k2 * p[0];//b
						g += k2 * p[1];//g
						r += k2 * p[2];//r
					}
				}

				dst[0] = (IMP_U8) b;
				dst[1] = (IMP_U8) g;
				dst[2] = (IMP_U8) r;
			}
		}
	}
}
