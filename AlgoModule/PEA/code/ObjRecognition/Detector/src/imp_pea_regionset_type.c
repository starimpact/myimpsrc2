#include "imp_pea_regionset_label.h"
#include "imp_pea_regionset_type.h"


#ifndef _TIDSP
#define DEBUG_TYPE_EDGE
#endif

static IMP_VOID ipLabelRegionSetTypeHelper( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_VOID ipLabelRegionSetTypeMotion( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	pstPara->s32ChangeOnly = 0;
	ipLabelRegionSetTypeHelper( pstPara, pstRgs );
}

IMP_VOID ipLabelRegionSetTypeChange( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	pstPara->s32ChangeOnly = 1;
	ipLabelRegionSetTypeHelper( pstPara, pstRgs );
}



static IMP_S32 ipDetermineRegionMotionType( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );
static IMP_S32 ipDetermineRegionStaticType( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );
static IMP_VOID ipLabelRegionSetTypeHelper( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	PEA_DETECTED_REGION_S *pstDrg;
	IMP_S32 i, s32Cnt;
	IMP_S32 s32IsStatic, s32IsLeft, s32IsRemoved, s32StaticType;

	IMP_ITERATE_RGS_START
	IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	{
		
		if( IMP_DRG_IS_DIFF(pstDrg->u8Used) || IMP_DRG_IS_LIGHT(pstDrg->u8Used) )
			continue;

 		s32IsStatic = 0;
		s32IsLeft = 0;
		s32IsRemoved = 0;

		//determine if it is static object
		s32IsStatic = ipDetermineRegionMotionType( pstPara, pstRgs, i );
	//	if (pstDrg->s32AreaPixel > 500) printf("%d:%d\n", pstDrg->s32AreaPixel, s32IsStatic);
		s32IsLeft = 0; s32IsRemoved = 0;
		if( s32IsStatic || pstPara->s32MotionIsLeft )
		{	
			//determine static type of object
 			s32StaticType = ipDetermineRegionStaticType( pstPara, pstRgs, i );
			if( s32StaticType == 1 ) s32IsLeft = 1;
			else if( s32StaticType == 2 )	s32IsRemoved = 1;
		}

		if( s32IsStatic )
		{
			if( s32IsLeft )
			{
				IMP_DRG_SET_STATIC_L(pstDrg->u8Used);
			//	printf("IMP_DRG_SET_STATIC_L:%x\n", pstDrg->u8Used);
			//	if (pstDrg->s32AreaPixel > 500) printf("left detect:%d.\n", pstDrg->s32AreaPixel);
			}
			else if( s32IsRemoved )
			{
				IMP_DRG_SET_STATIC_R(pstDrg->u8Used);
			//	if (pstDrg->s32AreaPixel > 500) printf("remove detect:%d.\n", pstDrg->s32AreaPixel);
			}
			else
				IMP_DRG_SET_LIGHT(pstDrg->u8Used);
		}
		else
		{
			if( s32IsLeft || !pstPara->s32MotionIsLeft)
				IMP_DRG_SET_MOTION(pstDrg->u8Used);
			else
				IMP_DRG_SET_LIGHT(pstDrg->u8Used);
		}

	}
	IMP_ITERATE_RGS_END

#if 1
	{
		IMP_S32 s32Width = pstRgs->pstImgFgRgn->s32W;
		IMP_S32 s32Height = pstRgs->pstImgFgRgn->s32H;
		int ri;
		PEA_DETECTED_REGION_S *pstDrg; //[IMP_MAX_TGT_CNT];
		
		for (ri = 0; ri < IMP_MAX_TGT_CNT; ri++)
		{
			pstDrg = &pstRgs->astDrg[ri];
		//	printf("fuck!!!%x\n", pstDrg->u8Used);
			if (IMP_DRG_IS_STATIC_L(pstDrg->u8Used))
			{
			//	printf("fuck!!!\n");
	//			pstRgs->pstImgBgGray->pu8Data[pstDrg->stRect.s16Y1 * s32Width + pstDrg->stRect.s16X1] = 255;
			}
		}
	//	ipShowGrayImage(s32Width, s32Height, pstRgs->pstImgFrmDiff->pu8Data, "pstImgFrmDiff123");
	}
#endif
}

static IMP_S32 ipDetermineRegionMotionType( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S	*pstDrg = pstRgs->astDrg + s32Index;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgFrmDiff = pstRgs->pstImgFrmDiff;
	IMP_S32 s32ThRgTmDifgRatio = pstPara->s32ThRgTmDifgRatio;
	IMP_S32 s32ThRgTmWidhRatio = pstPara->s32ThRgTmWidhRatio;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S32 s32Width, s32Height;
	IMP_S32 xx, yy, s32AreaPixel, s32AreaDiff, s32Sn, s32ThStatic, s32IsStatic;
	IMP_U8 *pu8DataDiff, *pu8DataFg;

	if( pstPara->s32ChangeOnly )
		return 1;

	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Width = ru_x - lb_x + 1;
	s32Height = ru_y - lb_y + 1;
	s32AreaPixel = pstDrg->s32AreaPixel;
	s32Sn = pstDrg->u8Sign;

	s32AreaDiff = 0;
	pu8DataDiff = pstImgFrmDiff->pu8Data + (lb_y) * s32ImgW;
	pu8DataFg = pstImgFgRgn->pu8Data + (lb_y) * s32ImgW;
	for( yy = lb_y; yy <= ru_y; yy++ )
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(3, ,1);
#endif
		for( xx = lb_x; xx <= ru_x; xx++ )
		{
			if( pu8DataFg[xx] == s32Sn && pu8DataDiff[xx] )
				s32AreaDiff += 1;
		}
		pu8DataDiff += s32ImgW;
		pu8DataFg += s32ImgW;
	}
	s32ThStatic = s32AreaPixel * s32ThRgTmDifgRatio;
	if( s32ThStatic > (s32Width+s32Height) * s32ThRgTmWidhRatio )
		s32ThStatic = (s32Width+s32Height) * s32ThRgTmWidhRatio;
 	s32IsStatic = ( s32AreaDiff * 100 <= s32ThStatic );
// 	printf("s32AreaDiff:%d, s32ThStatic:%d   ", s32AreaDiff, s32ThStatic);

	return s32IsStatic;
}

static IMP_S32 ipDetermineRegionStaticTypeMethod( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );
static IMP_S32 ipDetermineRegionStaticTypeGray( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );
static IMP_S32 ipDetermineRegionStaticTypeEdge( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );
static IMP_S32 ipDetermineRegionStaticType( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	IMP_S32 s32Method, s32Type;

	s32Method = ipDetermineRegionStaticTypeMethod( pstPara, pstRgs, s32Index );
	s32Method = 1;
	if( s32Method == 1 )
 		s32Type = ipDetermineRegionStaticTypeGray( pstPara, pstRgs, s32Index );
	else if( s32Method == 2 )
		s32Type = ipDetermineRegionStaticTypeEdge( pstPara, pstRgs, s32Index );
	else
		s32Type = 0;

	return s32Type;
}
#ifdef IMP_ARM_PEA
static IMP_S32 ipDetermineRegionStaticTypeMethod( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S	*pstDrg = pstRgs->astDrg + s32Index;
	//	IP_GRAYIMAGE *pstImgFgRgn = pstRgs->pstImgFgRgn;
	//	IP_GRAYIMAGE *pstImgBgEdge = pstRgs->pstImgBgEdge;
	IMP_S32 s32ThRgTsSizeMin = pstPara->s32ThRgTsSizeMin;
	// 	IMP_S32 s32ThRgTsSizeMax = pstPara->s32ThRgTsSizeMax;
	// 	IMP_S32 s32ThRgTsEdgeMin = pstPara->s32ThRgTsEdgeMin;
	// 	IMP_S32 s32ThRgTsEdgeMax = pstPara->s32ThRgTsEdgeMax;
	// 	IMP_S32 s32ThRgTsWidhRatio = pstPara->s32ThRgTsWidhRatio;
	// 	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	// 	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	// 	IMP_S32 s32Width, s32Height;
	// 	IMP_S32 xx, yy, s32Sn;
	// 	IMP_S32 s32EdgeNum, s32ThEdgeMin, s32ThEdgeMax;
	// 	uint8_t *pu8DataBg, *pu8DataFg;
	IMP_S32 s32AreaPixel;
	IMP_S32 s32Ret = 0;

	s32AreaPixel = pstDrg->s32AreaPixel;
	if( s32AreaPixel < s32ThRgTsSizeMin )
	{
		if( pstPara->s32ChangeOnly )
			s32Ret = 0;
		else
			s32Ret = 1;
	}
	else
	{
		if( pstPara->s32ChangeOnly )
			s32Ret = 2;
		else
			s32Ret = 1;
		// 		if( s32ThRgTsEdgeMin==0 && s32ThRgTsEdgeMax==100 )
		// 		{
		// 			s32Ret = 2;
		// 		}
		// 		else
		// 		{
		// 			lb_x = pstDrg->stRect.s16X1;
		// 			ru_x = pstDrg->stRect.s16X2;
		// 			lb_y = pstDrg->stRect.s16Y1;
		// 			ru_y = pstDrg->stRect.s16Y2;
		// 			s32Width = ru_x-lb_x+1;
		// 			s32Height = ru_y-lb_y+1;
		// 			s32Sn = pstDrg->u8Sign;
		//
		// 			s32ThEdgeMax = s32AreaPixel * s32ThRgTsEdgeMax;
		// 			s32ThEdgeMin = s32AreaPixel * s32ThRgTsEdgeMin;
		// 			if( s32ThEdgeMin > (s32Width+s32Height)*s32ThRgTsWidhRatio )
		// 				s32ThEdgeMin = (s32Width+s32Height)*s32ThRgTsWidhRatio;
		//
		// 			s32EdgeNum = 0;
		// 			for( yy=lb_y; yy<=ru_y; yy++ )
		// 			{
		// 				pu8DataFg = pstImgFgRgn->pu8Data + yy*s32ImgW;
		// 				pu8DataBg = pstImgBgEdge->pu8Data + yy*s32ImgW;
		//
		// 				for( xx=lb_x; xx<=ru_x; xx++ )
		// 				{
		// 					if( pu8DataFg[xx]==s32Sn && pu8DataBg[xx] )
		// 						++s32EdgeNum;
		// 				}
		// 			}
		//
		// 			if( s32EdgeNum*100 >= s32ThEdgeMin && s32EdgeNum*100 <= s32ThEdgeMax )
		// 				s32Ret = 2;
		// 			else
		// 				s32Ret = 1;
		// 		}
	}

	return s32Ret;
}
#else
static IMP_S32 ipDetermineRegionStaticTypeMethod( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S	*pstDrg = pstRgs->astDrg + s32Index;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgBgEdge = pstRgs->pstImgBgEdge;
	IMP_S32 s32ThRgTsSizeMin = pstPara->s32ThRgTsSizeMin;
 	IMP_S32 s32ThRgTsSizeMax = pstPara->s32ThRgTsSizeMax;
 	IMP_S32 s32ThRgTsEdgeMin = pstPara->s32ThRgTsEdgeMin;
 	IMP_S32 s32ThRgTsEdgeMax = pstPara->s32ThRgTsEdgeMax;
 	IMP_S32 s32ThRgTsWidhRatio = pstPara->s32ThRgTsWidhRatio;
 	IMP_S32 lb_x, lb_y, ru_x, ru_y;


 	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
 	IMP_S32 s32Width, s32Height;
 	IMP_S32 xx, yy, s32Sn;
 	IMP_S32 s32EdgeNum, s32ThEdgeMin, s32ThEdgeMax;
 	IMP_U8 *pu8DataBg, *pu8DataFg;
	IMP_S32 s32AreaPixel;
	IMP_S32 s32Ret = 0;

	s32AreaPixel = pstDrg->s32AreaPixel;
	if( s32AreaPixel < s32ThRgTsSizeMin )
	{
		if( pstPara->s32ChangeOnly )
			s32Ret = 0;
		else
			s32Ret = 1;
	}
	else
	{
		if( pstPara->s32ChangeOnly )
			s32Ret = 2;
		else
			s32Ret = 1;
		///from here
 		if( s32ThRgTsEdgeMin == 0 && s32ThRgTsEdgeMax == 100 )
		 		{
		 			s32Ret = 2;
		 		}
		 		else
		 		{
		 			lb_x = pstDrg->stRect.s16X1;
		 			ru_x = pstDrg->stRect.s16X2;
		 			lb_y = pstDrg->stRect.s16Y1;
		 			ru_y = pstDrg->stRect.s16Y2;
		 			s32Width = ru_x - lb_x + 1;
		 			s32Height = ru_y - lb_y + 1;
		 			s32Sn = pstDrg->u8Sign;

		 			s32ThEdgeMax = s32AreaPixel * s32ThRgTsEdgeMax;
		 			s32ThEdgeMin = s32AreaPixel * s32ThRgTsEdgeMin;
		 			if( s32ThEdgeMin > (s32Width + s32Height) * s32ThRgTsWidhRatio )
		 				s32ThEdgeMin = (s32Width + s32Height) * s32ThRgTsWidhRatio;

		 			s32EdgeNum = 0;
		 			for( yy = lb_y; yy <= ru_y; yy++ )
		 			{
		 				pu8DataFg = pstImgFgRgn->pu8Data + yy * s32ImgW;
		 				pu8DataBg = pstImgBgEdge->pu8Data + yy * s32ImgW;

		 				for( xx = lb_x; xx <= ru_x; xx++ )
		 				{
		 					if( pu8DataFg[xx] == s32Sn && pu8DataBg[xx] )
		 						++s32EdgeNum;
		 				}
		 			}

		 			if( s32EdgeNum * 100 >= s32ThEdgeMin && s32EdgeNum * 100 <= s32ThEdgeMax )
		 				s32Ret = 2;
		 			else
		 				s32Ret = 1;
		 		}
				////end here
	}

	return s32Ret;
}
#endif
static IMP_S32 ipDetermineRegionStaticTypeMethod1( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S	*pstDrg = pstRgs->astDrg + s32Index;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgBgEdge = pstRgs->pstImgBgEdge;
	GRAY_IMAGE_S *pstImgContour = pstRgs->pstImgContour;
	IMP_S32 s32ThRgTsSizeMin = pstPara->s32ThRgTsSizeMin;
 	IMP_S32 s32ThRgTsSizeMax = pstPara->s32ThRgTsSizeMax;
 	IMP_S32 s32ThRgTsEdgeMin = pstPara->s32ThRgTsEdgeMin;
 	IMP_S32 s32ThRgTsEdgeMax = pstPara->s32ThRgTsEdgeMax;
 	IMP_S32 s32ThRgTsWidhRatio = pstPara->s32ThRgTsWidhRatio;
 	IMP_S32 lb_x, lb_y, ru_x, ru_y;
 	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
 	IMP_S32 s32Width, s32Height;
 	IMP_S32 xx, yy, s32Sn;
 	IMP_S32 s32EdgeNum, s32ThEdgeMin, s32ThEdgeMax;
 	IMP_U8 *pu8DataBg, *pu8DataFg, *pu8DataCnter;
	IMP_S32 s32AreaPixel;
	IMP_S32 s32Ret = 0;
	IMP_S32 s32StartX = 0,s32StartY = 0;//[352];
	IMP_S32 s32EndX = 0, s32EndY = 0;//[352];
//	memset(s32StartY,0,sizeof(s32StartY));
//	memset(s32EndY,0,sizeof(s32EndY));
	s32AreaPixel = pstDrg->s32AreaPixel;
	if( s32AreaPixel < s32ThRgTsSizeMin )
	{
		if( pstPara->s32ChangeOnly )
			s32Ret = 0;
		else
			s32Ret = 1;
	}
	else
	{
		if( pstPara->s32ChangeOnly )
			s32Ret = 2;
		else
			s32Ret = 1;
		///from here
		{
		 	lb_x = pstDrg->stRect.s16X1;
		 	ru_x = pstDrg->stRect.s16X2;
		 	lb_y = pstDrg->stRect.s16Y1;
		 	ru_y = pstDrg->stRect.s16Y2;
		 	s32Width = ru_x - lb_x + 1;
		 	s32Height = ru_y - lb_y + 1;
		 	s32Sn = pstDrg->u8Sign;

		 	s32ThEdgeMax = s32AreaPixel * s32ThRgTsEdgeMax;
		 	s32ThEdgeMin = s32AreaPixel * s32ThRgTsEdgeMin;
		 	if( s32ThEdgeMin > (s32Width + s32Height) * s32ThRgTsWidhRatio )
		 		s32ThEdgeMin = (s32Width + s32Height) * s32ThRgTsWidhRatio;

		 	s32EdgeNum = 0;
		 	for( yy = lb_y; yy <= ru_y; yy++ )
		 	{
		 		pu8DataFg = pstImgFgRgn->pu8Data + yy * s32ImgW;
		 		pu8DataBg = pstImgBgEdge->pu8Data + yy * s32ImgW;
				pu8DataCnter = pstImgContour->pu8Data + yy * s32ImgW;

				s32StartX = 0;
				s32EndX = 0;
		 		for( xx = lb_x; xx <= ru_x; xx++ )
		 		{
		 			if( pu8DataFg[xx] == s32Sn )
					{
						if(s32StartX == 0)
						{
							s32StartX = xx;
						}
						s32EndX = xx;
					}
		 		}
				if (s32StartX != 0)
				{
					pu8DataCnter[s32StartX - 1] = 255;
					pu8DataCnter[s32StartX] = 255;
					pu8DataCnter[s32EndX - 1] = 255;
					pu8DataCnter[s32EndX] = 255;
				}
				if ((pu8DataBg[s32StartX] || pu8DataBg[s32StartX - 1] || pu8DataBg[s32StartX + 1]) && s32StartX != 0)
					++s32EdgeNum;
				if ((pu8DataBg[s32EndX] || pu8DataBg[s32EndX - 1] || pu8DataBg[s32EndX + 1]) && s32EndX != 0)
					++s32EdgeNum;
		 	}
			pu8DataFg = pstImgFgRgn->pu8Data;
			pu8DataBg = pstImgBgEdge->pu8Data;
			pu8DataCnter = pstImgContour->pu8Data;
			for( xx = lb_x; xx <= ru_x; xx++ )
		 	{
//		 		pu8DataFg = pstImgFgRgn->pu8Data + yy*s32ImgW;
//		 		pu8DataBg = pstImgBgEdge->pu8Data + yy*s32ImgW;

				s32StartY = 0;
				s32EndY = 0;
				for( yy = lb_y; yy <= ru_y; yy++ )
		 		{
		 			if( pu8DataFg[yy * s32ImgW + xx] == s32Sn )
					{
						if(s32StartY == 0)
						{
							s32StartY = yy;
						}
						s32EndY = yy;
					}
		 		}
				if (s32StartY != 0)
				{
					pu8DataCnter[(s32StartY - 1) * s32ImgW + xx] = 255;
					pu8DataCnter[s32StartY * s32ImgW + xx] = 255;
					pu8DataCnter[(s32EndY - 1) * s32ImgW + xx] = 255;
					pu8DataCnter[s32EndY * s32ImgW + xx] = 255;
				}
				if ((pu8DataBg[s32StartY * s32ImgW + xx] || pu8DataBg[(s32StartY - 1) * s32ImgW + xx] || pu8DataBg[(s32StartY + 1) * s32ImgW + xx]) && s32StartY != 0)
					++s32EdgeNum;
 				if ((pu8DataBg[s32EndY * s32ImgW + xx] || pu8DataBg[(s32EndY - 1) * s32ImgW + xx] || pu8DataBg[(s32EndY + 1) * s32ImgW + xx]) && s32EndY != 0)
					++s32EdgeNum;
		 	}

//		 	if( s32EdgeNum*100 >= s32ThEdgeMin && s32EdgeNum*100 <= s32ThEdgeMax )
//		 		s32Ret = 2;
		 	if (s32EdgeNum * 100 > 50 * (s32Width + s32Height) * 2)
			{
				s32Ret = 1;
		 	}
			else
			{
				s32Ret = 2;
			}
		}
				////end here
	}

	return s32Ret;
}
static IMP_S32 ipDetermineRegionStaticTypeGray( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S	*pstDrg = pstRgs->astDrg + s32Index;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgInGray = pstRgs->pstImgInGray;
	GRAY_IMAGE_S *pstImgBgGray = pstRgs->pstImgBgGray;
	GRAY_IMAGE_S *pstImgInEdgeMag = pstRgs->pstImgInEdgeMag;
	GRAY_IMAGE_S *pstImgBgEdgeMag = pstRgs->pstImgBgEdgeMag;
	IMP_S32 s32ImgH = pstRgs->pstImgFgRgn->s32H;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_U8 *pu8DataBg, *pu8DataIn, *pu8DataFg;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 xs, ys, xe, ye;
	IMP_S32 s32Width, s32Height;
	IMP_S32 xx, yy, s32Sn;
	IMP_S32 s32GrayInSelf, s32GrayBgSelf, s32CntSelf;
	IMP_S32 s32GrayInEnvi, s32CntInEnvi;
	IMP_S32 s32GrayBgEnvi, s32CntBgEnvi;
	IMP_S32 s32GrayInSSum, s32GrayBgSSum;
	IMP_S32 s32GrayInESum, s32GrayBgESum;
	IMP_S32 s32DiffIn, s32DiffBg;
	IMP_S32 s32StaticType;

	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Width = ru_x - lb_x + 1;
	s32Height = ru_y - lb_y + 1;
	s32Sn = pstDrg->u8Sign;

	s32GrayInSSum = 0;
	s32GrayBgSSum = 0;
	s32GrayInESum = 0;
	s32GrayBgESum = 0;
	s32GrayInEnvi = 0;
	s32GrayBgEnvi = 0;
	s32CntInEnvi = 0;
	s32CntBgEnvi = 0;
	s32GrayInSelf = 0;
	s32GrayBgSelf = 0;
	s32CntSelf = 0;
//printf("s32ThRgTsInBgSize:%d\n", pstPara->s32ThRgTsInBgSize);
	if( pstDrg->s32AreaPixel <= pstPara->s32ThRgTsInBgSize )
	{
		IMP_S32 in, bg, fg;
		IMP_S32 ofs, area;

		ofs = 0;
		do
		{
			ofs += 2;
			area = (s32Width + ofs * 2) * (s32Height + ofs * 2);
		} while( area < pstDrg->s32AreaPixel * 2 );
		xs = lb_x - ofs;
		xe = ru_x + ofs;
		ys = lb_y - ofs;
		ye = ru_y + ofs;
		if( xs < 0 ) xs = 0;
		if( ys < 0 ) ys = 0;
		if( xe > (s32ImgW - 1) ) xe = s32ImgW - 1;
		if( ye > (s32ImgH - 1) ) ye = s32ImgH - 1;

		pu8DataFg = pstImgFgRgn->pu8Data + ys * s32ImgW;
		pu8DataIn = pstImgInGray->pu8Data + ys * s32ImgW;
		pu8DataBg = pstImgBgGray->pu8Data + ys * s32ImgW;
		for( yy = ys; yy <= ye; yy++ )
		{
			for( xx = xs; xx <= xe; xx++ )
			{
				fg = pu8DataFg[xx];
				in = pu8DataIn[xx];
				bg = pu8DataBg[xx];
				if( fg == s32Sn )
				{
					s32GrayInSSum += in;
					s32GrayBgSSum += bg;
					s32CntSelf++;
				}
				{
					s32GrayBgESum += bg;
					s32CntBgEnvi++;
				}

			}
			pu8DataFg += s32ImgW;
			pu8DataIn += s32ImgW;
			pu8DataBg += s32ImgW;
		}

		if( s32CntSelf )
		{
			s32GrayInSelf = (s32GrayInSSum + (s32CntSelf >> 1)) / s32CntSelf;
			s32GrayBgSelf = (s32GrayBgSSum + (s32CntSelf >> 1)) / s32CntSelf;
		}
		if( s32CntBgEnvi )
			s32GrayBgEnvi = (s32GrayBgESum + (s32CntBgEnvi >> 1)) / s32CntBgEnvi;

		s32DiffIn = abs( s32GrayInSelf - s32GrayBgEnvi );
		s32DiffBg = abs( s32GrayBgSelf - s32GrayBgEnvi );
	}
	else
	{
		IMP_S32 fg1, fg2, fg3, fg4;
		IMP_S32 fg5, fg6, fg7, fg8;
		IMP_S32 in2, in3;
		IMP_S32 bg2, bg3;
		IMP_S32 isedge2, isedge3;

		xs = lb_x;
		xe = ru_x;
		ys = lb_y;
		ye = ru_y;

		if( (xe - xs + 1) & 1 ) xe += 1;
		if( (ye - ys + 1) & 1 ) ye += 1;
		if( xs < 1 ) xs = 1;
		if( ys < 1 ) ys = 1;
		if( xe > (s32ImgW - 2) ) xe = s32ImgW - 2;
		if( ye > (s32ImgH - 2) ) ye = s32ImgH - 2;

		pu8DataFg = pstImgFgRgn->pu8Data + ys * s32ImgW;
		pu8DataIn = pstImgInEdgeMag->pu8Data + ys * s32ImgW;
		pu8DataBg = pstImgBgEdgeMag->pu8Data + ys * s32ImgW;
		for( yy = ys; yy <= ye; yy += 2 )
		{
			for( xx = xs; xx <= xe; xx += 2 )
			{
				fg1 = pu8DataFg[xx - 1];
				fg2 = pu8DataFg[xx];
				fg3 = pu8DataFg[xx + 1];
				fg4 = pu8DataFg[xx + 2];
				fg5 = pu8DataFg[xx - s32ImgW];
				fg6 = pu8DataFg[xx + s32ImgW];
				fg7 = pu8DataFg[xx - s32ImgW + 1];
				fg8 = pu8DataFg[xx + s32ImgW + 1];
				in2 = pu8DataIn[xx];
				in3 = pu8DataIn[xx + 1];
				bg2 = pu8DataBg[xx];
				bg3 = pu8DataBg[xx + 1];
				isedge2 = ( (fg1 != s32Sn) + (fg3 != s32Sn) + (fg5 != s32Sn) + (fg6 != s32Sn) );
				isedge3 = ( (fg2 != s32Sn) + (fg4 != s32Sn) + (fg7 != s32Sn) + (fg8 != s32Sn) );

				fg2 = (fg2 == s32Sn);
				if( fg2 && isedge2 )
				{
					s32GrayInSSum += in2;
					s32GrayBgSSum += bg2;
					s32CntSelf++;
				}
				fg3 = (fg3 == s32Sn);
				if( fg3 && isedge3 )
				{
					s32GrayInSSum += in3;
					s32GrayBgSSum += bg3;
					s32CntSelf++;
				}
			}
			pu8DataFg += s32ImgW * 2;
			pu8DataIn += s32ImgW * 2;
			pu8DataBg += s32ImgW * 2;
		}

		if( s32CntSelf )
		{
			s32GrayInSelf = (s32GrayInSSum + (s32CntSelf >> 1)) / s32CntSelf;
			s32GrayBgSelf = (s32GrayBgSSum + (s32CntSelf >> 1)) / s32CntSelf;
		}

		pu8DataFg = pstImgFgRgn->pu8Data + (ys + 1) * s32ImgW;
		pu8DataIn = pstImgInEdgeMag->pu8Data + (ys + 1) * s32ImgW;
		pu8DataBg = pstImgBgEdgeMag->pu8Data + (ys + 1) * s32ImgW;
		for( yy = ys + 1; yy <= ye; yy += 2 )
		{
			for( xx = xs; xx <= xe; xx += 2 )
			{
				fg1 = pu8DataFg[xx - 1];
				fg2 = pu8DataFg[xx];
				fg3 = pu8DataFg[xx + 1];
				fg4 = pu8DataFg[xx + 2];
				fg5 = pu8DataFg[xx - s32ImgW];
				fg6 = pu8DataFg[xx + s32ImgW];
				fg7 = pu8DataFg[xx - s32ImgW + 1];
				fg8 = pu8DataFg[xx + s32ImgW + 1];
				in2 = pu8DataIn[xx];
				in3 = pu8DataIn[xx + 1];
				bg2 = pu8DataBg[xx];
				bg3 = pu8DataBg[xx + 1];
				isedge2 = ( (fg1 != s32Sn) + (fg3 != s32Sn) + (fg5 != s32Sn) + (fg6 != s32Sn) );
				isedge3 = ( (fg2 != s32Sn) + (fg4 != s32Sn) + (fg7 != s32Sn) + (fg8 != s32Sn) );

				fg2 = (fg2 == s32Sn);
				if( fg2 && isedge2 )
				{
					s32GrayInSSum += in2;
					s32GrayBgSSum += bg2;
					s32CntSelf++;
				}
				fg3 = (fg3 == s32Sn);
				if( fg3 && isedge3 )
				{
					s32GrayInSSum += in3;
					s32GrayBgSSum += bg3;
					s32CntSelf++;
				}

				if( fg2 && !isedge2 )
				{
					if( in2<=s32GrayInSelf )
					{
						s32GrayInESum += in2;
						s32CntInEnvi++;
					}
					if( bg2<=s32GrayBgSelf )
					{
						s32GrayBgESum += bg2;
						s32CntBgEnvi++;
					}
				}
				if( fg3 && !isedge3 )
				{
					if( in3<=s32GrayInSelf )
					{
						s32GrayInESum += in3;
						s32CntInEnvi++;
					}
					if( bg3<=s32GrayBgSelf )
					{
						s32GrayBgESum += bg3;
						s32CntBgEnvi++;
					}
				}
			}
			pu8DataFg += s32ImgW * 2;
			pu8DataIn += s32ImgW * 2;
			pu8DataBg += s32ImgW * 2;
		}

		if( s32CntSelf )
		{
			s32GrayInSelf = (s32GrayInSSum + (s32CntSelf >> 1)) / s32CntSelf;
			s32GrayBgSelf = (s32GrayBgSSum + (s32CntSelf >> 1)) / s32CntSelf;
		}
		if( s32CntInEnvi )
			s32GrayInEnvi = (s32GrayInEnvi + (s32CntInEnvi >> 1)) / s32CntInEnvi;
		if( s32CntBgEnvi )
			s32GrayBgEnvi = (s32GrayBgEnvi + (s32CntBgEnvi >> 1)) / s32CntBgEnvi;

		s32DiffIn = ( s32GrayInSelf - s32GrayInEnvi );
		s32DiffBg = ( s32GrayBgSelf - s32GrayBgEnvi );
	}

	s32StaticType = 0;
	if( s32DiffIn > (s32DiffBg + pstPara->s32ThRgTsInBgDiff) )
		s32StaticType = 1;
	else
		s32StaticType = 2;

	
	return s32StaticType;
}

static IMP_S32 ipDetermineRegionStaticTypeEdge( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S	*pstDrg = pstRgs->astDrg + s32Index;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgInEdge = pstRgs->pstImgInEdge;
	GRAY_IMAGE_S *pstImgBgEdge = pstRgs->pstImgBgEdge;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
//	IMP_S32 s32ImgH = pstRgs->pstImgFgRgn->s32Height;
	IMP_S32 s32ThRgTsStrengthRatioL = pstPara->s32ThRgTsStrengthRatioL;
	IMP_S32 s32ThRgTsShapeRatioL = pstPara->s32ThRgTsShapeRatioL;
	IMP_S32 s32ThRgTsEdgeRatioL = pstPara->s32ThRgTsEdgeRatioL;
	IMP_S32 s32ThRgTsStrengthRatioR = pstPara->s32ThRgTsStrengthRatioR;
	IMP_S32 s32ThRgTsShapeRatioR = pstPara->s32ThRgTsShapeRatioR;
	IMP_S32 s32ThRgTsEdgeRatioR = pstPara->s32ThRgTsEdgeRatioR;
	IMP_S32 s32InFirst, s32InLast, s32BgFirst, s32BgLast;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 s32Width, s32Height;
	IMP_S32 xx, yy, s32Sn;
	IMP_U8 *pu8DataIn, *pu8DataBg, *pu8DataFg;
	IMP_S32 fg, in, bg;
	IMP_S32 s32LNumIn, s32RNumBg;
	IMP_S32 s32LNumFg, s32RNumFg;
	IMP_S32 s32StaticType;
	IMP_S32 s32IsLeft, s32IsRemoved;

	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Width = ru_x - lb_x + 1;
	s32Height = ru_y - lb_y + 1;
	s32Sn = pstDrg->u8Sign;

	s32LNumIn = 0;
	s32RNumBg = 0;
	s32LNumFg = 0;
	s32RNumFg = 0;


	pu8DataFg = pstImgFgRgn->pu8Data + lb_y * s32ImgW;
	pu8DataIn = pstImgInEdge->pu8Data + lb_y * s32ImgW;
	pu8DataBg = pstImgBgEdge->pu8Data + lb_y * s32ImgW;
	for( yy = lb_y; yy <= ru_y; yy++ )
	{
		s32InFirst = 0; s32BgFirst = 0;
		s32InLast = 0; s32BgLast = 0;
// 		fg_first = 0; fg_last = 0;

		for( xx = lb_x; xx <= ru_x; xx++ )
		{
			fg = pu8DataFg[xx];
			in = pu8DataIn[xx];
			bg = pu8DataBg[xx];

			fg = (fg==s32Sn);

// 			if( fg	)
			{
#ifdef DEBUG_TYPE_EDGE
				pu8DataIn[xx] &= 0xFE;
				pu8DataBg[xx] &= 0xFE;
#endif

				if( fg && in )
				{
					if( !s32InFirst )
						s32InFirst = xx;
					s32InLast = xx;
				}
				if( fg && bg )
				{
					if( !s32BgFirst )
						s32BgFirst = xx;
					s32BgLast = xx;
				}

// 				if( !fg_first )
// 					fg_first = xx;
// 				fg_last = xx;
			}
		}

		if( abs(s32InFirst - s32BgFirst) > 1 )
		{
			if( s32InFirst && s32InFirst < s32BgFirst )
				s32BgFirst = 0;
			else if( s32BgFirst && s32InFirst > s32BgFirst )
				s32InFirst = 0;
		}
		if( abs(s32InLast - s32BgLast) > 1 )
		{
			if( s32InLast && s32InLast > s32BgLast )
				s32BgLast = 0;
			else if( s32BgLast && s32InLast < s32BgLast )
				s32InLast = 0;
		}

// 		if( s32InFirst && abs(s32InFirst-fg_first)>4 )
// 			s32InFirst = 0;
// 		if( s32InLast && abs(s32InLast-fg_last)>4 )
// 			s32InLast = 0;
// 		if( s32BgFirst && abs(s32BgFirst-fg_first)>4 )
// 			s32BgFirst = 0;
// 		if( s32BgLast && abs(s32BgLast-fg_last)>4 )
// 			s32BgLast = 0;

		if( s32InFirst != s32InLast )
		{
			if( s32InFirst )
				pu8DataIn[s32InFirst] |= 0x01;
			if( s32InLast )
				pu8DataIn[s32InLast] |= 0x01;
		}
		if( s32BgFirst != s32BgLast )
		{
			if( s32BgFirst )
				pu8DataBg[s32BgFirst] |= 0x01;
			if( s32BgLast )
				pu8DataBg[s32BgLast] |= 0x01;
		}

		pu8DataFg += s32ImgW;
		pu8DataIn += s32ImgW;
		pu8DataBg += s32ImgW;
	}

	for( xx = lb_x; xx <= ru_x; xx++ )
	{
 		IMP_S32 ofs;

		pu8DataFg = pstImgFgRgn->pu8Data + xx;
		pu8DataIn = pstImgInEdge->pu8Data + xx;
		pu8DataBg = pstImgBgEdge->pu8Data + xx;

		s32InFirst = 0; s32BgFirst = 0;
		s32InLast = 0; s32BgLast = 0;
// 		fg_first = 0; fg_last = 0;

 		ofs = lb_y * s32ImgW;
		for( yy = lb_y; yy <= ru_y; yy++ )
		{
			fg = pu8DataFg[ofs];
			in = pu8DataIn[ofs];
			bg = pu8DataBg[ofs];

			fg = (fg == s32Sn);

// 			if( fg	)
			{
				if( fg && in )
				{
					if( !s32InFirst )
						s32InFirst = yy;
					s32InLast = yy;
				}
				if( fg && bg )
				{
					if( !s32BgFirst )
						s32BgFirst = yy;
					s32BgLast = yy;
				}

// 				if( !fg_first )
// 					fg_first = yy;
// 				fg_last = yy;
			}

			ofs += s32ImgW;
		}

		if( abs(s32InFirst - s32BgFirst) > 1 )
		{
			if( s32InFirst && s32InFirst < s32BgFirst )
				s32BgFirst = 0;
			else if( s32BgFirst && s32InFirst > s32BgFirst )
				s32InFirst = 0;
		}
		if( abs(s32InLast - s32BgLast) > 1 )
		{
			if( s32InLast && s32InLast > s32BgLast )
				s32BgLast = 0;
			else if( s32BgLast && s32InLast < s32BgLast )
				s32InLast = 0;
		}

// 		if( s32InFirst && abs(s32InFirst-fg_first)>4 )
// 			s32InFirst = 0;
// 		if( s32InLast && abs(s32InLast-fg_last)>4 )
// 			s32InLast = 0;
// 		if( s32BgFirst && abs(s32BgFirst-fg_first)>4 )
// 			s32BgFirst = 0;
// 		if( s32BgLast && abs(s32BgLast-fg_last)>4 )
// 			s32BgLast = 0;

		if( s32InFirst != s32InLast )
		{
			if( s32InFirst )
				pu8DataIn[s32InFirst * s32ImgW] |= 0x01;
			if( s32InLast )
				pu8DataIn[s32InLast * s32ImgW] |= 0x01;
		}
		if( s32BgFirst != s32BgLast )
		{
			if( s32BgFirst )
				pu8DataBg[s32BgFirst * s32ImgW] |= 0x01;
			if( s32BgLast )
				pu8DataBg[s32BgLast * s32ImgW] |= 0x01;
		}
	}

	pu8DataFg = pstImgFgRgn->pu8Data + lb_y * s32ImgW;
	pu8DataIn = pstImgInEdge->pu8Data + lb_y * s32ImgW;
	pu8DataBg = pstImgBgEdge->pu8Data + lb_y * s32ImgW;
	for( yy = lb_y; yy <= ru_y; yy++ )
	{
		for( xx = lb_x; xx <= ru_x; xx++ )
		{
			IMP_S32 in1, in2, imat;
			IMP_S32 bg1, bg2, bmat;
			IMP_S32 ine, bge;
			fg = pu8DataFg[xx];
			in = pu8DataIn[xx];
			in1 = pu8DataIn[xx - 1];
			in2 = pu8DataIn[xx + 1];
			bg = pu8DataBg[xx];
			bg1 = pu8DataBg[xx - 1];
			bg2 = pu8DataBg[xx + 1];

			fg = (fg == s32Sn);
			ine = (in & 0x01);
			bge = (bg & 0x01);

			imat = ( bg1 || bg || bg2 );
			bmat = ( in1 || in || in2 );
// 			imat = ( bg );
// 			bmat = ( in );

// 			if( fg )
			{
				if( fg && ine )
				{
					s32LNumIn++;
					if( imat )
						s32LNumFg++;
				}
				if( fg && bge )
				{
					s32RNumBg++;
					if( bmat )
						s32RNumFg++;
				}

#ifndef DEBUG_TYPE_EDGE
				if( fg )
				{
					pu8DataIn[xx] = (in & 0xFE);
					pu8DataBg[xx] = (bg & 0xFE);
				}
#endif
			}
		}

		pu8DataFg += s32ImgW;
		pu8DataIn += s32ImgW;
		pu8DataBg += s32ImgW;
	}
	s32LNumFg = s32LNumIn - s32LNumFg;
	s32RNumFg = s32RNumBg - s32RNumFg;

	// 区分移走和遗留区域
	s32IsLeft		= 1
				&& ( s32LNumIn * 100 > s32RNumBg * s32ThRgTsStrengthRatioL )
				&& ( s32LNumIn * 100  >= (s32Width + s32Height) * s32ThRgTsShapeRatioL )
				&& ( s32LNumFg * 100  >= s32LNumIn * s32ThRgTsEdgeRatioL )
				;
	s32IsRemoved	= 1
				&& ( s32RNumBg * 100 > s32LNumIn * s32ThRgTsStrengthRatioR )
				&& ( s32RNumBg * 100 >= (s32Width + s32Height) * s32ThRgTsShapeRatioR )
				&& ( s32RNumFg * 100 >= s32RNumBg * s32ThRgTsEdgeRatioR )
				;

	s32StaticType = 0;
	if( s32IsLeft )
		s32StaticType = 1;
	else if( s32IsRemoved )
		s32StaticType = 2;

	return s32StaticType;
}

IMP_VOID ipLabelRegionSetTypePost( IpRGETypePara *pstPara, IpTrackedTargetSet *pstTTargetSet )
{
	IMP_S32  i, s32Index;
	IpTrackedTarget *pstTarget;
	PEA_DETECTED_REGION_S	*pstDrg;
	PEA_DETECTED_REGIONSET_S	*pstRgs;
	IpTargetPosition *pstPos;
	IMP_U32 u32Status;
	IMP_S32 s32IsLeft, s32IsRemoved, s32StaticType;

	pstPara->s32ChangeOnly = 1;
   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTTargetSet->astTargets[i];
		if( !( ipTrackedTargetIsActiveMeasured( pstTarget ) && IMP_IS_TGT_PSTATOBJ( pstTarget->stTargetInfo.u32Status ) ) )
			continue;

		pstRgs = pstTarget->stTargetImage.pstDRegions;
		s32Index = pstTarget->stTargetImage.s32DrgIndex;
		pstDrg = &pstRgs->astDrg[s32Index];

		if( IMP_IS_RGN_RMOTION(pstDrg->u16Status) )
			continue;

 		s32StaticType = ipDetermineRegionStaticType( pstPara, pstRgs, s32Index );
		s32IsLeft = 0; s32IsRemoved = 0;
		if( s32StaticType == 1 ) s32IsLeft = 1;
		else if( s32StaticType == 2 )	s32IsRemoved = 1;

		if( s32IsLeft )
			IMP_DRG_SET_STATIC_L(pstDrg->u8Used);
		else if( s32IsRemoved )
			IMP_DRG_SET_STATIC_R(pstDrg->u8Used);
		else
			IMP_DRG_SET_LIGHT(pstDrg->u8Used);

		u32Status = pstDrg->u16Status;
		if( IMP_DRG_IS_STATIC_L(pstDrg->u8Used) )
			IMP_SET_RGN_RSTATIC_L( u32Status );
		else if( IMP_DRG_IS_STATIC_R(pstDrg->u8Used) )
			IMP_SET_RGN_RSTATIC_R( u32Status );
		else if( IMP_DRG_IS_LIGHT(pstDrg->u8Used) )
			IMP_SET_RGN_RLIGHT( u32Status );

		if( u32Status != pstDrg->u16Status )
		{
			pstDrg->u16Status = u32Status;
			pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
			pstPos->u32DrgType = u32Status;
		}
	}
}
