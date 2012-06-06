
#include "imp_pea_tracker_common.h"

#ifdef DEBUG_TRACKER_TIMER
#include <WTYPES.H>
LARGE_INTEGER   TimerFrequency;
LARGE_INTEGER   TimerStart;
LARGE_INTEGER   TimerEnd;
double TimerDelta;
#endif

#ifdef DEBUG_TRACKER
#include "windows.h"
IMP_S8 imp_info1[10240];
IMP_S8 *info1;
IMP_S8 imp_info2[10240];
IMP_S8 *info2;
IMP_S8 imp_info3[10240];
IMP_S8 *info3;
IMP_S8 imp_info4[10240];
IMP_S8 *info4;
#endif


IMP_S32 ipGetMinimumRegionDistance( const IMP_RECT_S *pstRect1, const IMP_RECT_S *pstRect2, IMP_S32 *s32IntersectArea)
{
	IMP_POINT_S astPt1[4], astPt2[4], stMinPt, stMaxPt;
	IMP_S32 s32MinDist = 10000;


	astPt1[0].s16X = pstRect1->s16X1;
	astPt1[0].s16Y = pstRect1->s16Y1;
	astPt1[1].s16X = pstRect1->s16X1;
	astPt1[1].s16Y = pstRect1->s16Y2;
	astPt1[2].s16X = pstRect1->s16X2;
	astPt1[2].s16Y = pstRect1->s16Y1;
	astPt1[3].s16X = pstRect1->s16X2;
	astPt1[3].s16Y = pstRect1->s16Y2;

	astPt2[0].s16X = pstRect2->s16X1;
	astPt2[0].s16Y = pstRect2->s16Y1;
	astPt2[1].s16X = pstRect2->s16X1;
	astPt2[1].s16Y = pstRect2->s16Y2;
	astPt2[2].s16X = pstRect2->s16X2;
	astPt2[2].s16Y = pstRect2->s16Y1;
	astPt2[3].s16X = pstRect2->s16X2;
	astPt2[3].s16Y = pstRect2->s16Y2;

	stMinPt.s16X = astPt1[0].s16X > astPt2[0].s16X ? astPt1[0].s16X:astPt2[0].s16X;
	stMinPt.s16Y = astPt1[0].s16Y > astPt2[0].s16Y ? astPt1[0].s16Y:astPt2[0].s16Y;
	stMaxPt.s16X = astPt1[3].s16X < astPt2[3].s16X ? astPt1[3].s16X:astPt2[3].s16X;
	stMaxPt.s16Y = astPt1[3].s16Y < astPt2[3].s16Y ? astPt1[3].s16Y:astPt2[3].s16Y;
	if ( stMinPt.s16X > stMaxPt.s16X || stMinPt.s16Y > stMaxPt.s16Y )
	{
		*s32IntersectArea = 0;
		if ( (astPt1[0].s16X > astPt2[0].s16X && astPt1[0].s16X < astPt2[3].s16X) || (astPt2[0].s16X > astPt1[0].s16X && astPt2[0].s16X < astPt1[3].s16X) )//x方向有重叠
		{
			s32MinDist = abs(stMinPt.s16Y - stMaxPt.s16Y);
		}

		else if( (astPt1[0].s16Y > astPt2[0].s16Y && astPt1[0].s16Y < astPt2[3].s16Y) || (astPt2[0].s16Y > astPt1[0].s16Y && astPt2[0].s16Y < astPt1[3].s16Y) )//y方向有重叠
		{
			s32MinDist = abs(stMinPt.s16X - stMaxPt.s16X);
		}
		else
		{
			s32MinDist = ipSqrtInt( (stMaxPt.s16X - stMinPt.s16X) * (stMaxPt.s16X - stMinPt.s16X) + (stMaxPt.s16Y - stMinPt.s16Y) * (stMaxPt.s16Y - stMinPt.s16Y) );
		}

	}
	else
	{
		s32MinDist = 0;
        *s32IntersectArea = (stMaxPt.s16X - stMinPt.s16X + 1) * (stMaxPt.s16Y - stMinPt.s16Y + 1);//计算两矩形的相交象素数
	}
	return s32MinDist;
}



IMP_VOID ipRectPointImgPositionCorrect(IMP_RECT_S *rect, IMP_POINT_S *point, IMP_S32 imgW, IMP_S32 imgH, IMP_S32 mode)
{
	IMP_RECT_S rg = rect[0];
	IMP_POINT_S pt = point[0];

	if (mode)
	{
		IMP_S16 xc,yc;
		xc = (rg.s16X1 + rg.s16X2)>>1;
		yc = (rg.s16Y1 + rg.s16Y2)>>1;


		rg.s16X1 = rg.s16X1 < rg.s16X2 ? rg.s16X1:xc;
		rg.s16X2 = rg.s16X1 < rg.s16X2 ? rg.s16X2:xc;
		rg.s16Y1 = rg.s16Y1 < rg.s16Y2 ? rg.s16Y1:yc;
		rg.s16Y2 = rg.s16Y1 < rg.s16Y2 ? rg.s16Y2:yc;


		rg.s16X1 = rg.s16X1<0 ? 0:rg.s16X1;
		rg.s16X2 = rg.s16X2<0 ? 0:rg.s16X2;
		rg.s16Y1 = rg.s16Y1<0 ? 0:rg.s16Y1;
		rg.s16Y2 = rg.s16Y2<0 ? 0:rg.s16Y2;

		rg.s16X1 = rg.s16X1>imgW-1 ? imgW-1:rg.s16X1;
		rg.s16X2 = rg.s16X2>imgW-1 ? imgW-1:rg.s16X2;
		rg.s16Y1 = rg.s16Y1>imgH-1 ? imgH-1:rg.s16Y1;
		rg.s16Y2 = rg.s16Y2>imgH-1 ? imgH-1:rg.s16Y2;


		pt.s16X = pt.s16X<rg.s16X1 ? rg.s16X1:pt.s16X;
		pt.s16X = pt.s16X>rg.s16X2 ? rg.s16X2:pt.s16X;
		pt.s16Y = pt.s16Y<rg.s16Y1 ? rg.s16Y1:pt.s16Y;
		pt.s16Y = pt.s16Y>rg.s16Y2 ? rg.s16Y2:pt.s16Y;

	}

	IMP_ASSERT(rg.s16X1 <= rg.s16X2);
	IMP_ASSERT(rg.s16Y1 <= rg.s16Y2);

	IMP_ASSERT(rg.s16X1 >= 0);
	IMP_ASSERT(rg.s16Y1 >= 0);
	IMP_ASSERT(rg.s16X2 >= 0);
	IMP_ASSERT(rg.s16Y2 >= 0);
	IMP_ASSERT(rg.s16X1 <= imgW-1);
	IMP_ASSERT(rg.s16Y1 <= imgH-1);
	IMP_ASSERT(rg.s16X2 <= imgW-1);
	IMP_ASSERT(rg.s16Y2 <= imgH-1);

	IMP_ASSERT(rg.s16X1 <= pt.s16X);
	IMP_ASSERT(rg.s16Y1 <= pt.s16Y);
	IMP_ASSERT(rg.s16X2 >= pt.s16X);
	IMP_ASSERT(rg.s16Y2 >= pt.s16Y);

	rect[0]=rg;
	point[0]=pt;

}

void DrawRectGrayImg( GRAY_IMAGE_S *img_in, IMP_RECT_S *rc, IMP_U8 cr )
{
	IMP_U8 *data1, *data2;
	IMP_S32 xs, xe, ys, ye, w, x, y;
	xs = rc->s16X1; ys = rc->s16Y1;
	xe = rc->s16X2; ye = rc->s16Y2;
	w = img_in->s32W;

	data1 = (IMP_U8*)(img_in->pu8Data) + ys*w;
	data2 = (IMP_U8*)(img_in->pu8Data) + ye*w;
	for( x=xs; x<=xe; x++ )
	{
		data1[x] = cr;
		data2[x] = cr;
	}
	data1 = (IMP_U8*)(img_in->pu8Data) + ys*w + xs;
	data2 = (IMP_U8*)(img_in->pu8Data) + ys*w + xe;
	for( y=ys; y<=ye; y++ )
	{
		data1[0] = cr;
		data2[0] = cr;
		data1 += w;
		data2 += w;
	}
}


void DrawCrossGrayImg( GRAY_IMAGE_S *img_in, IMP_POINT_S *pt, IMP_U8 cr, IMP_S32 range)
{
	IMP_U8 *data1,*data2;
	IMP_S32 x0,y0,i,w,h;

	x0 = pt->s16X;
	y0 = pt->s16Y;
	h = img_in->s32H;
	w = img_in->s32W;

	data1 = (IMP_U8*)(img_in->pu8Data) + y0*w;
	for( i=1; i<range && x0+i<w; i++ )
	{
		data1[x0+i] = cr;
	}

	for( i=1; i<range && x0-i>=0; i++ )
	{
		data1[x0-i] = cr;
	}

	data1 = (IMP_U8*)(img_in->pu8Data) + y0*w + x0;
	data2 = data1;
	for( i=1; i<range && y0+i<h; i++ )
	{
		data1[i*w] = cr;
	}
	for( i=1; i<range && y0-i>=0; i++ )
	{
		data2[-i*w] = cr;
	}
}




IMP_VOID ipPointCopy( const IMP_POINT_S *pstSrc, IMP_POINT_S *pstDst )
{
	pstDst->s16X = pstSrc->s16X;
	pstDst->s16Y = pstSrc->s16Y;
}

IMP_VOID ipRectCopy( const IMP_RECT_S *pstSrc, IMP_RECT_S *pstDst )
{
	pstDst->s16X1 = pstSrc->s16X1;
	pstDst->s16Y1 = pstSrc->s16Y1;
	pstDst->s16X2 = pstSrc->s16X2;
	pstDst->s16Y2 = pstSrc->s16Y2;
}



IMP_VOID ipUpdateHist( IMP_U32* hist, IMP_U32* newHist, IMP_FLOAT alpha, IMP_U32 bins)
{
	IMP_U32 i;
	IMP_U32 a = (IMP_U32)(alpha * 100);
	for (i=0;i<bins;i++)
	{
		hist[i] = ( hist[i] * (100-a) + newHist[i] * a + 50 )/100;
	}
}

IMP_VOID ipUpdateInt16_t( IMP_S16* value, IMP_S16* newValue, IMP_FLOAT alpha, IMP_U16 num)
{
	IMP_U16 i;
	IMP_U16 a = (IMP_U16)(alpha * 100);
	for (i=0;i<num;i++)
	{
		value[i] = ( value[i] * (100-a) + newValue[i] * a + 50 )/100;
	}
}

IMP_VOID ipUpdateInt32_t( IMP_S32* value, IMP_S32* newValue, IMP_FLOAT alpha, IMP_U32 num)
{
	IMP_U32 i;
	IMP_U32 a = (IMP_U32)(alpha * 100);
	for (i=0;i<num;i++)
	{
		value[i] = ( value[i] * (100-a) + newValue[i] * a + 50 )/100;
	}
}

IMP_VOID ipUpdateReal( IMP_FLOAT* value, IMP_FLOAT* newValue, IMP_FLOAT alpha, IMP_U32 num)
{
	IMP_U32 i;
	for (i=0;i<num;i++)
	{
		value[i] = value[i] * (1-alpha) + newValue[i] * alpha;
	}
}

IMP_VOID ipUpdateRect( IMP_RECT_S* rect, IMP_RECT_S* newRect, IMP_FLOAT alpha, IMP_S32 imgWidth, IMP_S32 imgHeight)
{
	IMP_S32 w,h,newW,newH;
	IMP_POINT_S offset,offset1,offset2;

	w = rect->s16X2 - rect->s16X1 + 1;
	h = rect->s16Y2 - rect->s16Y1 + 1;
	newW = newRect->s16X2 - newRect->s16X1 + 1;
	newH = newRect->s16Y2 - newRect->s16Y1 + 1;

	ipUpdateInt32_t( &w, &newW, alpha, 1);
	ipUpdateInt32_t( &h, &newH, alpha, 1);
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
	//	info3 += sprintf( info3, "w0:%d, h0:%d | newW:%d, newH:%d | w1:%d, h1:%d | ΔW:%d, ΔH:%d\n",rect->s16X2 - rect->s16X1 + 1,rect->s16Y2 - rect->s16Y1 + 1,newW,newH,w,h,newW-(rect->s16X2 - rect->s16X1 + 1),newH-(rect->s16Y2 - rect->s16Y1 + 1));
#endif
	rect->s16X1 = ( ( newRect->s16X1 + newRect->s16X2) >> 1 ) - ( w >> 1 );
	rect->s16Y1 = ( ( newRect->s16Y1 + newRect->s16Y2) >> 1 ) - ( h >> 1 );
	rect->s16X2 = rect->s16X1 + w - 1;
	rect->s16Y2 = rect->s16Y1 + h - 1;


	offset1.s16X = rect->s16X1<0           ? -rect->s16X1           :0;
	offset2.s16X = rect->s16X2>imgWidth-1  ? imgWidth-1-rect->s16X2 :0;
	offset1.s16Y = rect->s16Y1<0           ? -rect->s16Y1           :0;
	offset2.s16Y = rect->s16Y2>imgHeight-1 ? imgHeight-1-rect->s16Y2:0;

	offset.s16X = offset1.s16X+offset2.s16X;
	offset.s16Y = offset1.s16Y+offset2.s16Y;

	rect->s16X1 += offset.s16X;
	rect->s16X2 += offset.s16X;
	rect->s16Y1 += offset.s16Y;
	rect->s16Y2 += offset.s16Y;

	IMP_ASSERT(rect->s16X1 >= 0 && rect->s16X1 < imgWidth);
	IMP_ASSERT(rect->s16X2 >= 0 && rect->s16X2 < imgWidth);
	IMP_ASSERT(rect->s16Y1 >= 0 && rect->s16Y1 < imgHeight);
	IMP_ASSERT(rect->s16Y2 >= 0 && rect->s16Y2 < imgHeight);

}


IMP_FLOAT ipCalculateHistoCoefficient( const IMP_U32 *pu32CurObj, const IMP_U32 *pu32TrackObj, IMP_S32 s32GrayLevel )
{
	IMP_S32 i, s32ShiftNum;
	IMP_S32 s32SumCurObj = 0, s32SumTrkObj = 0;
	IMP_S32 s32MeanC, s32MeanT, as32SubC[IMP_GRAY_LEVEL], as32SubT[IMP_GRAY_LEVEL];
 	IMP_DOUBLE dSumC = 0, dSumT = 0, dSum = 0;
 	IMP_S32 s32SumCi = 0, s32SumTi = 0, s32Sumi = 0;
 	IMP_S32 s32ShiftC, s32ShiftT;
	IMP_FLOAT fHistoCoef;

	switch (s32GrayLevel)
	{
		case 32:
			s32ShiftNum = 5;
			break;
		case 64:
			s32ShiftNum = 6;
			break;
		case 128:
			s32ShiftNum = 7;
			break;
		case 256:
			s32ShiftNum = 8;
			break;
		default:
			break;
	}
#ifdef _TIDSP
	#pragma MUST_ITERATE(32);
#endif
	for ( i = 0; i < s32GrayLevel; i++ )
	{
		s32SumCurObj += pu32CurObj[i];
		s32SumTrkObj += pu32TrackObj[i];
	}
	s32MeanC = s32SumCurObj >> s32ShiftNum;
	s32MeanT = s32SumTrkObj >> s32ShiftNum;
	s32ShiftC = s32SumCurObj >> 15;
	s32ShiftT = s32SumTrkObj >> 15;

#ifdef _TIDSP
	#pragma MUST_ITERATE(32);
#endif
	for ( i = 0; i < s32GrayLevel; i++ )
	{
		as32SubC[i] = ( (IMP_S32)pu32CurObj[i] - s32MeanC ) >> s32ShiftC;
		as32SubT[i] = ( (IMP_S32)pu32TrackObj[i] - s32MeanT ) >> s32ShiftT;
	}
#ifdef _TIDSP
	#pragma MUST_ITERATE(32);
#endif
	for ( i = 0; i < s32GrayLevel; i++ )
	{
		s32SumCi += as32SubC[i] * as32SubC[i];
		s32SumTi += as32SubT[i] * as32SubT[i];
	}
#ifdef _TIDSP
	#pragma MUST_ITERATE(32);
#endif
	for ( i = 0; i < s32GrayLevel; i++ )
	{
		s32Sumi += as32SubC[i] * as32SubT[i];
	}
	dSum = s32Sumi;
	dSumT = s32SumTi;
	dSumC = s32SumCi;

	fHistoCoef = (IMP_FLOAT)sqrt((dSum * dSum) / (dSumC * dSumT));
	return fHistoCoef;
}


IMP_FLOAT ipCalculateHistoCoefficientRGB( const IMP_U32 *pu32CurObj, const IMP_U32 *pu32TrackObj, IMP_S32 s32RgbLevel )
{
	IMP_S32 i, s32ShiftNum;
	IMP_S32 s32SumCurObj = 0, s32SumTrkObj = 0;
	IMP_S32 s32MeanC, s32MeanT;
 	IMP_FLOAT dSumC = 0, dSumT = 0, dSum = 0;
 	IMP_S32 s32SumCi = 0, s32SumTi = 0, s32Sumi = 0;
	IMP_FLOAT fHistoCoef;

	switch (s32RgbLevel)
	{
		case 512:
			s32ShiftNum = 9;
			break;
		case 4096:
			s32ShiftNum = 12;
			break;
		case 32768:
			s32ShiftNum = 15;
			break;
		default:
			break;
	}
#ifdef _TIDSP
	#pragma MUST_ITERATE(4096);
#endif
	for ( i = 0; i < s32RgbLevel; i++ )
	{
		s32SumCurObj += pu32CurObj[i];
		s32SumTrkObj += pu32TrackObj[i];
	}
	s32MeanC = s32SumCurObj >> s32ShiftNum;
	s32MeanT = s32SumTrkObj >> s32ShiftNum;
//	s32ShiftC = s32SumCurObj >> 15;
//	s32ShiftT = s32SumTrkObj >> 15;


	//RGBhistograms + Bhattachayya
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(4096);
#endif
		for ( i = 0; i < s32RgbLevel; i++ )
		{
			s32SumCi += pu32CurObj[i];
			s32SumTi += pu32TrackObj[i];
		}
#ifdef _TIDSP
	#pragma MUST_ITERATE(4096);
#endif


		for ( i = 0; i < s32RgbLevel; i++ )
		{
			dSum += (IMP_FLOAT)sqrt(pu32CurObj[i] * pu32TrackObj[i]);
		}
		fHistoCoef = (IMP_FLOAT)(dSum / sqrt(s32SumCi * s32SumTi));
	}

//	//RGBhistograms
//	{
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			s32SumCi += pu32CurObj[i];
//			s32SumTi += pu32TrackObj[i];
//			dSum += abs(pu32CurObj[i] - pu32TrackObj[i]);
//		}
//		fHistoCoef = 1/(dSum/sqrt(s32SumCi*s32SumTi));
//	}

//	//RGBhistograms
//	{
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			s32SumCi += pu32CurObj[i];
//			s32SumTi += pu32TrackObj[i];
//			dSum += sqrt((pu32CurObj[i] - pu32TrackObj[i])*(pu32CurObj[i] - pu32TrackObj[i]));
//		}
//		fHistoCoef = 1/(dSum/sqrt(s32SumCi*s32SumTi));
//	}

//	//RGBhistograms + KL Distance
//	{
//		IMP_FLOAT epsilon=0.0000001;
//		IMP_FLOAT rate;
//		IMP_FLOAT Ci,Ti;
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			s32SumCi += pu32CurObj[i]+epsilon;
//			s32SumTi += pu32TrackObj[i]+epsilon;
//		}
//
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			Ti = (pu32TrackObj[i]+epsilon)/s32SumTi;
//			Ci = (pu32CurObj[i]+epsilon)/s32SumCi;
//			rate = Ci/Ti;
//			dSum +=  Ci * log(rate);
//		}
//
//		fHistoCoef = 1/dSum;
//	}


//	//cumulative histograms
//	{
//		IMP_FLOAT CHCi=0.0,CHTi=0.0;
//		IMP_FLOAT CHsumCi=0.0,CHsumTi=0.0;
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			CHCi += pu32CurObj[i];
//			CHTi += pu32TrackObj[i];
//			CHsumCi += CHCi;
//			CHsumTi += CHTi;
//			dSum += sqrt(CHTi * CHCi);
//		}
//		fHistoCoef = dSum/sqrt(CHsumCi*CHsumTi);
//	}

//	//cumulative histograms
//	{
//		IMP_FLOAT CHCi=0.0,CHTi=0.0;
//		IMP_FLOAT CHsumCi=0.0,CHsumTi=0.0;
//
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			CHCi += pu32CurObj[i];
//			CHTi += pu32TrackObj[i];
//			CHsumCi += CHCi;
//			CHsumTi += CHTi;
//			dSum += fabs(CHTi - CHCi);
//		}
//
//		fHistoCoef = 1-dSum/sqrt(CHsumCi*CHsumTi);
//	}


//	//cumulative histograms
//	{
//		IMP_FLOAT CHCi=0.0,CHTi=0.0;
//		IMP_FLOAT CHsumCi=0.0,CHsumTi=0.0;
//
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			CHCi += pu32CurObj[i];
//			CHTi += pu32TrackObj[i];
//			CHsumCi += CHCi;
//			CHsumTi += CHTi;
//			dSum += (CHTi - CHCi)*(CHTi - CHCi);
//		}
//
//		fHistoCoef = 1-sqrt(dSum)/sqrt(CHsumCi*CHsumTi);
//	}

//	//cumulative histograms
//	{
//		IMP_FLOAT CHCi=0.0,CHTi=0.0;
//		IMP_FLOAT CHsumCi=0.0,CHsumTi=0.0;
//		dSum = 0;
//
//		for ( i=0; i<s32RgbLevel; i++ )
//		{
//			CHCi += pu32CurObj[i];
//			CHTi += pu32TrackObj[i];
//			CHsumCi += CHCi;
//			CHsumTi += CHTi;
//			if (fabs(CHTi - CHCi)>dSum)
//				dSum = fabs(CHTi - CHCi);
//		}
//
//		fHistoCoef = 1-dSum/sqrt(CHsumCi*CHsumTi);
//	}

	return fHistoCoef;
}



IMP_S32 ipCalcDiagonalLengthM( IMP_RECT_S *pstRect )
{
	IMP_S32 s32Diagonal;
	IMP_S32 s32TmpX, s32TmpY;

	s32TmpX = pstRect->s16X2 - pstRect->s16X1;
	s32TmpY = pstRect->s16Y2 - pstRect->s16Y1;
	s32Diagonal = ipSqrtInt( s32TmpX * s32TmpX + s32TmpY * s32TmpY );

	return s32Diagonal;
}


