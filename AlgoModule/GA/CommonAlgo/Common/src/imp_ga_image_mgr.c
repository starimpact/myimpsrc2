#include "imp_ga_image_mgr.h"
#include "imp_ga_memory_mgr.h"

#if 1

IMP_VOID IMP_CalcMotionDiffImage( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_S32 s32ThetaThld,
				IMP_U8 *pstImgCurData, IMP_U8 *pstImgPreData, IMP_U8 *pstImgDiffData )
{
	IMP_S32 i, pix_num, th, diff;
	const IMP_U8 cst255 = 255;
	const IMP_U8 cst000 = 0;
	IMP_U8 *gray_t, *gray_pre, *gray_diff;
	IMP_S32 gt, gp;

	s32ThetaThld = s32ThetaThld;
	pix_num = s32ImgW*s32ImgH;
	gray_t = pstImgCurData;
	gray_pre = pstImgPreData;
	gray_diff = pstImgDiffData;

	for( i=0; i<pix_num; i++ )
	{
		gt = *gray_t++;
		gp = *gray_pre++;
// 		th = ( s32ThetaThld + (gt>>THETA_INCR_BASE) );
		th = s32ThetaThld;
		diff = abs( gt - gp );
		//diff = abs(diff);
		*gray_diff++ = (diff)>th ? diff : cst000;
	}
}

#else
IMP_VOID IMP_CalcMotionDiffImage( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_S32 s32ThetaThld,
				IMP_U8 *pstImgCurData, IMP_U8 *pstImgPreData, IMP_U8 *pstImgDiffData )
{
	IMP_S32 icol, irow, cols, rows;
	const IMP_U8 cst000 = 0;
	IMP_U32 *p_gray_t4, *p_gray_pre4, *p_gray_diff4;
	IMP_U32 gray_t4, gray_pre4;
	IMP_U8 gray_diff0, gray_diff1, gray_diff2, gray_diff3;
	IMP_S8 gray_deta0, gray_deta1, gray_deta2, gray_deta3;

	s32ThetaThld = s32ThetaThld >> 1;
	cols = s32ImgW << 5;
	rows = s32ImgH >> 5;

	for( irow=0; irow<rows; irow++ )
	{
		p_gray_t4 = (IMP_U32*)(pstImgCurData + irow * cols);
		p_gray_pre4 = (IMP_U32*)(pstImgPreData + irow * cols);
		p_gray_diff4 = (IMP_U32*)(pstImgDiffData + irow * cols);

#ifdef _TIDSP
		#pragma MUST_ITERATE(32,,32);
#endif
		for( icol=0; icol<cols; icol+=8 )
		{
			gray_t4 = *p_gray_t4++;
			gray_pre4 = *p_gray_pre4++;
			gray_deta0 = ( (gray_t4 >> 1) & 0x7F ) - ( (gray_pre4 >>1 ) & 0x7F );
			gray_deta1 = ( (gray_t4 >> 9) & 0x7F ) - ( (gray_pre4 >> 9 ) & 0x7F );
			gray_deta2 = ( (gray_t4 >> 17) & 0x7F ) - ( (gray_pre4 >> 17) & 0x7F );
			gray_deta3 = ( (gray_t4 >> 25) & 0x7F ) - ( (gray_pre4 >> 25) & 0x7F );
			gray_diff0 = abs( gray_deta0 ) >= s32ThetaThld ? (IMP_U8)gray_deta0 : cst000;
			gray_diff1 = abs( gray_deta1 ) >= s32ThetaThld ? (IMP_U8)gray_deta1 : cst000;
			gray_diff2 = abs( gray_deta2 ) >= s32ThetaThld ? (IMP_U8)gray_deta2 : cst000;
			gray_diff3 = abs( gray_deta3 ) >= s32ThetaThld ? (IMP_U8)gray_deta3 : cst000;
			*p_gray_diff4++ = gray_diff0 | (gray_diff1 << 8) | (gray_diff2 << 16) | (gray_diff3 << 24);

			gray_t4 = *p_gray_t4++;
			gray_pre4 = *p_gray_pre4++;
			gray_deta0 = ( (gray_t4 >> 1) & 0x7F ) - ( (gray_pre4 >> 1)	& 0x7F );
			gray_deta1 = ( (gray_t4 >> 9) & 0x7F ) - ( (gray_pre4 >> 9)	& 0x7F );
			gray_deta2 = ( (gray_t4 >> 17) & 0x7F ) - ( (gray_pre4 >> 17)& 0x7F );
			gray_deta3 = ( (gray_t4 >> 25) & 0x7F ) - ( (gray_pre4 >> 25)& 0x7F );
			gray_diff0 = abs( gray_deta0 ) >= s32ThetaThld ? (IMP_U8)gray_deta0 : cst000;
			gray_diff1 = abs( gray_deta1 ) >= s32ThetaThld ? (IMP_U8)gray_deta1 : cst000;
			gray_diff2 = abs( gray_deta2 ) >= s32ThetaThld ? (IMP_U8)gray_deta2 : cst000;
			gray_diff3 = abs( gray_deta3 ) >= s32ThetaThld ? (IMP_U8)gray_deta3 : cst000;
 			*p_gray_diff4++ = gray_diff0 | (gray_diff1 << 8) | (gray_diff2 << 16) | (gray_diff3 << 24);
		}
	}
}

#endif

IMP_VOID IMP_ClearImagePerimeter( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_U8 *pstImgDat )
{
	IMP_U8 *gray_t21, *gray_t22;
	IMP_S32 i;

	memset( pstImgDat, 0, s32ImgW );
	memset( pstImgDat + s32ImgW * (s32ImgH - 1), 0, s32ImgW );

	gray_t21 = (pstImgDat + s32ImgW);
	gray_t22 = (pstImgDat + s32ImgW * 2 - 1);
	for( i = 1; i < s32ImgH - 1; i++ )
	{
		*gray_t21 = 0;
		*gray_t22 = 0;
		gray_t21 += s32ImgW;
		gray_t22 += s32ImgW;
	}
}
IMP_VOID IMP_GrayImageCloneb( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst )
{
	IMP_S32 s32ImgW, s32ImgH;
	IMP_S32 n, i, cnt;
	IMP_U8 *data_s, *data_d;

	s32ImgW = pstDst->s32W;
	s32ImgH = pstDst->s32H;
	cnt = (s32ImgW * s32ImgH) >> 2;
	data_s = pstSrc->pu8Data;
	data_d = pstDst->pu8Data;
	for( n = 0; n < 4; n++)
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(256,,32);
#endif
		for( i=0; i<cnt; i++ )
			data_d[i] = data_s[i] ? 255 : 0;
		data_s += cnt;
		data_d += cnt;
	}
}

IMP_VOID IMP_GrayImageBinary( GRAY_IMAGE_S *pstImg, IMP_S32 s32Th, IMP_S32 s32Value )
{
	IMP_S32 s32ImgW, s32ImgH;
	IMP_S32 n, i, cnt;
	IMP_U8 *data;

	s32ImgW = pstImg->s32W;
	s32ImgH = pstImg->s32H;
	cnt = (s32ImgW * s32ImgH) >> 2;
	data = pstImg->pu8Data;
	for( n = 0; n < 4; n++)
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(256,,32);
#endif
		for( i=0; i<cnt; i++ )
		{
			if( data[i] > s32Th )
				data[i] = s32Value;
			else
				data[i] = 0;
		}
		data += cnt;
	}
}

IMP_VOID IMP_GrayImageBinarys( GRAY_IMAGE_S *pstImg )
{
	IMP_S32 s32ImgW, s32ImgH;
	IMP_S32 n, i, cnt;
	IMP_U8 *data;

	s32ImgW = pstImg->s32W;
	s32ImgH = pstImg->s32H;
	cnt = (s32ImgW * s32ImgH) >> 2;
	data = pstImg->pu8Data;
	for( n = 0; n < 4; n++)
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(256,,32);
#endif
		for( i = 0; i < cnt; i++ )
		{
			if( data[i] )
				data[i] = 0x80;
		}
		data += cnt;
	}
}

IMP_VOID IMP_CheckBinary( GRAY_IMAGE_S *pstImg )
{
	IMP_S32 x, y;
	IMP_S32 s32ImgW = pstImg->s32W;
	IMP_S32 s32ImgH = pstImg->s32H;
	IMP_U8 *data = pstImg->pu8Data;

	for( y = 0; y < s32ImgH; y++ )
	{
		for( x = 0; x < s32ImgW; x++ )
		{
			assert( data[x] == 0 || data[x] == 255 );
		}
		data += s32ImgW;
	}
}

IMP_S32 IMP_GrayImageCompare( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst )
{
	IMP_S32 s32ImgW, s32ImgH;
	IMP_S32 n, i, cnt;
	IMP_U8 *data_s, *data_d;
	IMP_S32 ret=0;

	s32ImgW = pstDst->s32W;
	s32ImgH = pstDst->s32H;
	cnt = (s32ImgW * s32ImgH) >> 2;
	data_s = pstSrc->pu8Data;
	data_d = pstDst->pu8Data;
	for( n = 0; n < 4; n++)
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(256,,32);
#endif
		for( i = 0; i < cnt; i++ )
		{
			ret += ( data_d[i] != data_s[i] );
		}
		data_s += cnt;
		data_d += cnt;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
/// 图像管理, 灰度、RGB、YUV图像创建销毁
//////////////////////////////////////////////////////////////////////////
IMP_VOID IMP_RGBImageCreate( RGB_IMAGE_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *pstMemMgr = (MEM_MGR_ARRAY_S*)pMmgr;
	pstImg->s32H = s32H;
	pstImg->s32W= s32W;
	pstImg->pu8Data = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(PIXEL_S) );
	memset( pstImg->pu8Data, 0, s32W * s32H * sizeof(PIXEL_S) );
}

IMP_VOID IMP_RGBImageDestroy( RGB_IMAGE_S *pstImg, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *pstMemMgr = (MEM_MGR_ARRAY_S*)pMmgr;
	IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8Data );
	memset( pstImg, 0, sizeof(RGB_IMAGE_S) );
}

IMP_VOID IMP_RGBImageClear( RGB_IMAGE_S *pstImg )
{
	memset( pstImg->pu8Data, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(PIXEL_S) );
}

IMP_VOID IMP_RGBImageClone( RGB_IMAGE_S *pstSrc, RGB_IMAGE_S *pstDst )
{
	memcpy( pstDst->pu8Data, pstSrc->pu8Data, (pstDst->s32W) * (pstDst->s32H) * sizeof(PIXEL_S) );
}

IMP_VOID IMP_RGBImageGetPixelVal( RGB_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y, PIXEL_S *pstPixel )
{
	PIXEL_S *pix0;
	pix0 = ((PIXEL_S*)(pstImg->pu8Data)) + s32Y * (pstImg->s32W) + s32X;
	*(PIXEL_S*)(pstPixel) = *pix0;
}

IMP_VOID IMP_RGBImageSetPixelVal( RGB_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y, PIXEL_S *pstPixel )
{
	PIXEL_S *pix0;
	pix0 = ((PIXEL_S*)(pstImg->pu8Data)) + s32Y * (pstImg->s32W) + s32X;
	*pix0 = *(PIXEL_S*)(pstPixel);
}

IMP_VOID IMP_YUVImage422Create( YUV_IMAGE422_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	pstImg->s32H = s32H;
	pstImg->s32W = s32W;
	pstImg->pu8Y = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8) );
	memset( pstImg->pu8Y, 0, s32W * s32H * sizeof(IMP_U8) );
	pstImg->pu8U = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8)/2 );
	memset( pstImg->pu8U, 0, s32W * s32H * sizeof(IMP_U8)/2 );
	pstImg->pu8V = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8)/2 );
	memset( pstImg->pu8V, 0, s32W * s32H * sizeof(IMP_U8)/2 );
}

IMP_VOID IMP_YUVImage422Destroy( YUV_IMAGE422_S *pstImg, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8Y );
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8U );
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8V );
	memset( pstImg, 0, sizeof(YUV_IMAGE422_S) );
}

IMP_VOID IMP_YUVImage422Clear( YUV_IMAGE422_S *pstImg )
{
	memset( pstImg->pu8Y, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) );
	memset( pstImg->pu8U, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) / 2 );
	memset( pstImg->pu8V, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) / 2 );
}

IMP_VOID IMP_YUVImage422Clone( YUV_IMAGE422_S *pstSrc, YUV_IMAGE422_S *pstDst )
{
	memcpy( pstDst->pu8Y, pstSrc->pu8Y, (pstDst->s32W) * (pstSrc->s32H) * sizeof(IMP_U8) );
	memcpy( pstDst->pu8U, pstSrc->pu8U, (pstDst->s32W) * (pstSrc->s32H) * sizeof(IMP_U8) / 2 );
	memcpy( pstDst->pu8V, pstSrc->pu8V, (pstDst->s32W) * (pstSrc->s32H) * sizeof(IMP_U8) / 2 );
}

IMP_VOID IMP_YUVImage420Create( YUV_IMAGE420_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	pstImg->s32H = s32H;
	pstImg->s32W = s32W;
	pstImg->pu8Y = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8) );
	memset( pstImg->pu8Y, 0, s32W * s32H * sizeof(IMP_U8) );
	pstImg->pu8U = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8)/4 );
	memset( pstImg->pu8U, 0, s32W * s32H * sizeof(IMP_U8)/4 );
	pstImg->pu8V = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8)/4 );
	memset( pstImg->pu8V, 0, s32W * s32H * sizeof(IMP_U8)/4 );
	printf("pstImg->s32H = %d pstImg->s32W = %d,\n",pstImg->s32H,pstImg->s32W);
}

IMP_VOID IMP_YUVImage420Destroy( YUV_IMAGE420_S *pstImg, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8Y );
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8U );
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8V );
	memset( pstImg, 0, sizeof(YUV_IMAGE420_S) );
}

IMP_VOID IMP_YUVImage420Clear( YUV_IMAGE420_S *pstImg )
{
	memset( pstImg->pu8Y, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) );
	memset( pstImg->pu8U, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) / 4);
	memset( pstImg->pu8V, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) / 4 );
}

IMP_VOID IMP_YUVImage420Clone( YUV_IMAGE420_S *pstSrc, YUV_IMAGE422_S *pstDst )
{
	memcpy( pstDst->pu8Y, pstSrc->pu8Y, (pstDst->s32W) * (pstSrc->s32H) * sizeof(IMP_U8) );
	memcpy( pstDst->pu8U, pstSrc->pu8U, (pstDst->s32W) * (pstSrc->s32H) * sizeof(IMP_U8) /4);
	memcpy( pstDst->pu8V, pstSrc->pu8V, (pstDst->s32W) * (pstSrc->s32H) * sizeof(IMP_U8) /4 );
}

IMP_VOID IMP_GrayImageCreate( GRAY_IMAGE_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	pstImg->s32H = s32H;
	pstImg->s32W = s32W;
	pstImg->pu8Data = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_U8) );
}

IMP_VOID IMP_GrayImageDestroy( GRAY_IMAGE_S *pstImg, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8Data );
	memset( pstImg, 0, sizeof(GRAY_IMAGE_S) );
}

IMP_VOID IMP_GrayImageClear( GRAY_IMAGE_S *pstImg )
{
	memset( pstImg->pu8Data, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_U8) );
}

IMP_VOID IMP_GrayImageSet( GRAY_IMAGE_S *pstImg, IMP_U8 u8Value )
{
	IMP_S32 w, h, gap;

	w = pstImg->s32W;
	h = pstImg->s32H;
	gap = (h >> 2);

	memset( pstImg->pu8Data + gap * w * 0, u8Value, w * gap * sizeof(IMP_U8) );
	memset( pstImg->pu8Data + gap * w * 1, u8Value, w * gap * sizeof(IMP_U8) );
	memset( pstImg->pu8Data + gap * w * 2, u8Value, w * gap * sizeof(IMP_U8) );
	memset( pstImg->pu8Data + gap * w * 3, u8Value, w * gap * sizeof(IMP_U8) );
}

IMP_VOID IMP_GrayImageClone( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst )
{
	IMP_S32 w, h;

	w = pstDst->s32W;
	h = pstDst->s32H;
	w = (w * h) >> 2;
	h = 4;

	memcpy( pstDst->pu8Data + w * 0, pstSrc->pu8Data + w * 0, w * sizeof(IMP_U8) );
	memcpy( pstDst->pu8Data + w * 1, pstSrc->pu8Data + w * 1, w * sizeof(IMP_U8) );
	memcpy( pstDst->pu8Data + w * 2, pstSrc->pu8Data + w * 2, w * sizeof(IMP_U8) );
	memcpy( pstDst->pu8Data + w * 3, pstSrc->pu8Data + w * 3, w * sizeof(IMP_U8) );
}

IMP_VOID IMP_GrayImageClone2( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst )
{
#ifdef _TIDSP
	IMP_U8 *tmpdat;
	tmpdat = pstDst->pu8Data;
	pstDst->pu8Data = pstSrc->pu8Data;
	pstSrc->pu8Data = tmpdat;
#else
	memcpy( pstDst->pu8Data, pstSrc->pu8Data, (pstDst->s32W) * (pstDst->s32H) * sizeof(IMP_U8) );
#endif
}

IMP_U8 IMP_GrayImageGetPixelVal( GRAY_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y )
{
	IMP_U8 *pix0;
	pix0 = ((IMP_U8*)(pstImg->pu8Data)) + s32Y * (pstImg->s32W) + s32X;
	return *pix0;
}

IMP_VOID IMP_GrayImageSetPixelVal( GRAY_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y, IMP_U8 u8Value )
{
	IMP_U8 *pix0;
	pix0 = ((IMP_U8*)(pstImg->pu8Data)) + s32Y * (pstImg->s32W) + s32X;
	*pix0 = u8Value;
}

IMP_VOID IMP_GrayImage16Create( GRAY_IMAGE16_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	pstImg->s32H = s32H;
	pstImg->s32W = s32W;
	pstImg->ps16Data = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32W * s32H * sizeof(IMP_S16) );
}

IMP_VOID IMP_GrayImage16Destroy( GRAY_IMAGE16_S *pstImg, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->ps16Data );
	memset( pstImg, 0, sizeof(GRAY_IMAGE_S) );
}

IMP_VOID IMP_GrayImage16Clear( GRAY_IMAGE16_S *pstImg )
{
	memset( pstImg->ps16Data, 0, (pstImg->s32H) * (pstImg->s32W) * sizeof(IMP_S16) );
}

IMP_VOID IMP_TmplImageCreate( TMPL_IMAGE_S *pstImg, IMP_S32 s32BufLen, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	pstImg->s32H = 0;
	pstImg->s32W = 0;
	pstImg->s32BufLen = s32BufLen;
	pstImg->pu8Data = IMP_MMAlloc( mem_mgr, IMP_MEMBLK_TYPE_SLOW, s32BufLen * sizeof(IMP_U8) );
}

IMP_VOID IMP_TmplImageDestroy( TMPL_IMAGE_S *pstImg, IMP_VOID *pMmgr )
{
	MEM_MGR_ARRAY_S *mem_mgr = (MEM_MGR_ARRAY_S*)pMmgr;
	IMP_MMFree( mem_mgr, IMP_MEMBLK_TYPE_SLOW, pstImg->pu8Data );
	memset( pstImg, 0, sizeof(TMPL_IMAGE_S) );
}

IMP_VOID IMP_TmplImageClear( TMPL_IMAGE_S *pstImg )
{
	memset( pstImg->pu8Data, 0, (pstImg->s32BufLen)*sizeof(IMP_U8) );
}

IMP_VOID IMP_TmplImageClone( TMPL_IMAGE_S *pstSrc, TMPL_IMAGE_S *pstDst )
{
	pstDst->s32W = pstSrc->s32W;
	pstDst->s32H = pstSrc->s32H;
	memcpy( pstDst->pu8Data, pstSrc->pu8Data, (pstDst->s32BufLen)*sizeof(IMP_U8) );
}



//////////////////////////////////////////////////////////////////////////
/// 历史图
//////////////////////////////////////////////////////////////////////////
IMP_VOID IMP_HistoryImageCreate( HISTORY_IMAGE_S *pstImgHis, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr )
{
	IMP_S32 i;

	for( i = 0; i<IMP_IMGHIS_LEN; i++ )
		IMP_GrayImageCreate( &pstImgHis->stImages[i], s32W, s32H, pMmgr );
}

IMP_VOID IMP_HistoryImageDestroy( HISTORY_IMAGE_S *pstImgHis, IMP_VOID *pMmgr )
{
	IMP_S32 i;

	for( i = 0; i<IMP_IMGHIS_LEN; i++ )
		IMP_GrayImageDestroy( &pstImgHis->stImages[i], pMmgr );
}

IMP_VOID IMP_HistoryImageInit( HISTORY_IMAGE_S *pstImgHis, GRAY_IMAGE_S *pstImgCur, IMP_U32 u32Time )
{
	IMP_S32 i;

	for( i = 0; i<IMP_IMGHIS_LEN; i++ )
	{
		IMP_GrayImageClone( pstImgCur, &pstImgHis->stImages[i] );
		pstImgHis->u32Times[i] = u32Time;
	}
	pstImgHis->u32LastIndex = IMP_IMGHIS_LEN - 1;
}

GRAY_IMAGE_S* IMP_GetHistoryImage( HISTORY_IMAGE_S *pstImgHis, IMP_U32 u32Time )
{
	IMP_S32 i, idx;

	for( i=0; i<IMP_IMGHIS_LEN; i++ )
	{
		idx = ( pstImgHis->u32LastIndex - i + IMP_IMGHIS_LEN ) % IMP_IMGHIS_LEN;
		if( pstImgHis->u32Times[idx] <= u32Time || i == IMP_IMGHIS_LEN - 1 )
        {
            return &pstImgHis->stImages[idx];
        }
	}

	return NULL;
}

IMP_VOID IMP_SetHistoryImage( HISTORY_IMAGE_S *pstImgHis, GRAY_IMAGE_S *pstImgCur, IMP_U32 u32Time )
{
	IMP_S32 idx;

	idx = ( pstImgHis->u32LastIndex + 1 ) % IMP_IMGHIS_LEN;
	pstImgHis->u32LastIndex = idx;
	pstImgHis->u32Times[idx] = u32Time;
	IMP_GrayImageClone( pstImgCur, &pstImgHis->stImages[idx] );
}
