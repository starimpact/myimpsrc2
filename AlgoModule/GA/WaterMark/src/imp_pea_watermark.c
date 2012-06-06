
#include "imp_pea_detector.h"



void ipCreateWaterMarkDetector( IpWaterMarkDetector *module, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	IMP_S32 s32ImgHeight, s32ImgWidth;
	MEM_MGR_ARRAY_S *pstMemMgr;
	memset( module, 0, sizeof(IpWaterMarkDetector) );
	module->pstHwResource = pstHwResource;
	module->pstResult = pstResult;
	module->u32FrmCnt=0;
	module->s32Confidence=0;
	module->s32Precf=-1;
	pstMemMgr = &pstHwResource->stMemMgr;
	s32ImgHeight = pstResult->s32Height;
	s32ImgWidth = pstResult->s32Width;
	module->ps8LinePixels = (IMP_U8*)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW,(s32ImgWidth)*sizeof(IMP_U8));
	//ÇåÁã
	memset(module->ps8LinePixels, 0 ,sizeof((s32ImgWidth)*sizeof(IMP_U8)));

}
void ipReleaseWaterMarkDetector( IpWaterMarkDetector *module )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &module->pstHwResource->stMemMgr;
	//ÊÍ·ÅÄÚ´æ
	IMP_U8 *ps8LinePixels = module->ps8LinePixels;
	IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, ps8LinePixels);
	ps8LinePixels = NULL;
	memset( module, 0, sizeof(IpWaterMarkDetector) );
}
void ipConfigWaterMarkDetector( IpWaterMarkDetector *module, IpWaterMarkDetectorPara *stPara )
{
	PEA_RESULT_S *pstResult = module->pstResult;
	IpTrackedTargetSet *pstTargetSet=&pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget=pstTargetSet->astTargets;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pstResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgInGray = pstRgs->pstImgInGray;

	IMP_S32 s32ImgWidth = pstImgInGray->s32W;
	IMP_S32 s32ImgHeight = pstImgInGray->s32H;

	IMP_S32 x_space;

	if (stPara==NULL)
	{

		if (s32ImgWidth >= 352)
		{
			module->stPara.P0.s16X=100;
			module->stPara.P0.s16Y=100;
			x_space = X_SPACE;
		}
		else if(s32ImgWidth <= 176)
		{
			module->stPara.P0.s16X=1;
			module->stPara.P0.s16Y=50;
			x_space = X_SPACE - 4;
		}


		module->stPara.I[0].s16X1=module->stPara.P0.s16X;
		module->stPara.I[0].s16Y1=module->stPara.P0.s16Y;
		module->stPara.I[0].s16X2=module->stPara.P0.s16X+VLINE_WIDTH;
		module->stPara.I[0].s16Y2=module->stPara.P0.s16Y+Y_PART1_HEIGHT;

		module->stPara.I[1].s16X1=module->stPara.P0.s16X;
		module->stPara.I[1].s16Y1=module->stPara.I[0].s16Y2+Y_SPACE;
		module->stPara.I[1].s16X2=module->stPara.I[1].s16X1+VLINE_WIDTH;
		module->stPara.I[1].s16Y2=module->stPara.I[1].s16Y1+Y_PART2_HEIGHT;


		module->stPara.M[0].s16X1=module->stPara.I[0].s16X2+x_space;
		module->stPara.M[0].s16Y1=module->stPara.P0.s16Y;
		module->stPara.M[0].s16X2=module->stPara.M[0].s16X1+VLINE_WIDTH;
		module->stPara.M[0].s16Y2=module->stPara.M[0].s16Y1+CHAR_HEIGHT;

		module->stPara.M[1].s16X1=module->stPara.I[0].s16X2+x_space;
		module->stPara.M[1].s16Y1=module->stPara.P0.s16Y;
		module->stPara.M[1].s16X2=module->stPara.M[1].s16X1+CHAR_WIDTH;
		module->stPara.M[1].s16Y2=module->stPara.M[1].s16Y1+Y_PART1_HEIGHT;

		module->stPara.M[2].s16X1=module->stPara.M[0].s16X2+CHAR_WIDTH/3;
		module->stPara.M[2].s16Y1=module->stPara.M[0].s16Y1;
		module->stPara.M[2].s16X2=module->stPara.M[2].s16X1+VLINE_WIDTH;
		module->stPara.M[2].s16Y2=module->stPara.M[0].s16Y2;

		module->stPara.M[3].s16X1=module->stPara.M[2].s16X2+CHAR_WIDTH/3;
		module->stPara.M[3].s16Y1=module->stPara.M[2].s16Y1;
		module->stPara.M[3].s16X2=module->stPara.M[3].s16X1+VLINE_WIDTH;
		module->stPara.M[3].s16Y2=module->stPara.M[2].s16Y2;

		module->stPara.P[0].s16X1=module->stPara.M[3].s16X2+x_space;
		module->stPara.P[0].s16Y1=module->stPara.M[3].s16Y1;
		module->stPara.P[0].s16X2=module->stPara.P[0].s16X1+VLINE_WIDTH;
		module->stPara.P[0].s16Y2=module->stPara.P[0].s16Y1+CHAR_HEIGHT;

		module->stPara.P[1].s16X1=module->stPara.M[3].s16X2+x_space;
		module->stPara.P[1].s16Y1=module->stPara.M[3].s16Y1;
		module->stPara.P[1].s16X2=module->stPara.P[0].s16X1+CHAR_WIDTH;
		module->stPara.P[1].s16Y2=module->stPara.P[0].s16Y1+CHAR_HEIGHT/2;

		module->stPara.O[0].s16X1=module->stPara.P[1].s16X2+x_space;
		module->stPara.O[0].s16Y1=module->stPara.P[1].s16Y1;
		module->stPara.O[0].s16X2=module->stPara.O[0].s16X1+CHAR_WIDTH;
		module->stPara.O[0].s16Y2=module->stPara.O[0].s16Y1+CHAR_HEIGHT;

		module->stPara.W[0].s16X1=module->stPara.O[0].s16X2+x_space;
		module->stPara.W[0].s16Y1=module->stPara.O[0].s16Y1;
		module->stPara.W[0].s16X2=module->stPara.W[0].s16X1+VLINE_WIDTH;
		module->stPara.W[0].s16Y2=module->stPara.W[0].s16Y1+CHAR_HEIGHT;

		module->stPara.W[1].s16X1=module->stPara.W[0].s16X2+CHAR_WIDTH/3;
		module->stPara.W[1].s16Y1=module->stPara.W[0].s16Y1;
		module->stPara.W[1].s16X2=module->stPara.W[1].s16X1+VLINE_WIDTH;
		module->stPara.W[1].s16Y2=module->stPara.W[1].s16Y1+CHAR_HEIGHT;

		module->stPara.W[2].s16X1=module->stPara.W[1].s16X2+CHAR_WIDTH/3;
		module->stPara.W[2].s16Y1=module->stPara.W[1].s16Y1;
		module->stPara.W[2].s16X2=module->stPara.W[2].s16X1+VLINE_WIDTH;
		module->stPara.W[2].s16Y2=module->stPara.W[2].s16Y1+CHAR_HEIGHT;

		module->stPara.W[3].s16X1=module->stPara.W[0].s16X1;
		module->stPara.W[3].s16Y1=module->stPara.W[0].s16Y2;
		module->stPara.W[3].s16X2=module->stPara.W[3].s16X1+CHAR_WIDTH+VLINE_WIDTH;
		module->stPara.W[3].s16Y2=module->stPara.W[3].s16Y1;

		module->stPara.E[0].s16X1=module->stPara.W[3].s16X2+x_space;
		module->stPara.E[0].s16Y1=module->stPara.W[0].s16Y1;
		module->stPara.E[0].s16X2=module->stPara.E[0].s16X1+CHAR_WIDTH;
		module->stPara.E[0].s16Y2=module->stPara.E[0].s16Y1+HLINE_HEIGHT;

		module->stPara.E[1].s16X1=module->stPara.E[0].s16X1;
		module->stPara.E[1].s16Y1=module->stPara.E[0].s16Y2+CHAR_HEIGHT/3-HLINE_HEIGHT/2-1;
		module->stPara.E[1].s16X2=module->stPara.E[1].s16X1+CHAR_WIDTH;
		module->stPara.E[1].s16Y2=module->stPara.E[1].s16Y1+HLINE_HEIGHT;

		module->stPara.E[2].s16X1=module->stPara.E[1].s16X1;
		module->stPara.E[2].s16Y1=module->stPara.E[1].s16Y2+CHAR_HEIGHT/3-HLINE_HEIGHT/2-1;
		module->stPara.E[2].s16X2=module->stPara.E[2].s16X1+CHAR_WIDTH;
		module->stPara.E[2].s16Y2=module->stPara.E[2].s16Y1+HLINE_HEIGHT;

		module->stPara.E[3].s16X1=module->stPara.E[0].s16X1;
		module->stPara.E[3].s16Y1=module->stPara.E[0].s16Y1;
		module->stPara.E[3].s16X2=module->stPara.E[3].s16X1+VLINE_WIDTH;
		module->stPara.E[3].s16Y2=module->stPara.E[3].s16Y1+CHAR_HEIGHT;

		module->stPara.R[0].s16X1=module->stPara.E[2].s16X2+x_space;
		module->stPara.R[0].s16Y1=module->stPara.E[0].s16Y1;
		module->stPara.R[0].s16X2=module->stPara.R[0].s16X1+CHAR_WIDTH;
		module->stPara.R[0].s16Y2=module->stPara.R[0].s16Y1+CHAR_HEIGHT/2;

		module->stPara.R[1].s16X1=module->stPara.R[0].s16X1;
		module->stPara.R[1].s16Y1=module->stPara.R[0].s16Y1;
		module->stPara.R[1].s16X2=module->stPara.R[1].s16X1+VLINE_WIDTH;
		module->stPara.R[1].s16Y2=module->stPara.R[1].s16Y1+CHAR_HEIGHT;

		module->stPara.R[2].s16X1=module->stPara.R[1].s16X1+CHAR_WIDTH-VLINE_WIDTH;
		module->stPara.R[2].s16Y1=module->stPara.R[1].s16Y1;
		module->stPara.R[2].s16X2=module->stPara.R[2].s16X1+VLINE_WIDTH;
		module->stPara.R[2].s16Y2=module->stPara.R[2].s16Y1+CHAR_HEIGHT;

		module->stPara.rectNum=20;


	}
	else
	{
		memcpy( &module->stPara, stPara, sizeof(IpWaterMarkDetectorPara) );
	}

}


IMP_S32 ipProcessWaterMarkDetector( IpWaterMarkDetector *module)
{

	IMP_S32 cnt,x,cf;
	PEA_RESULT_S *pstResult = module->pstResult;
	IpTrackedTargetSet *pstTargetSet=&pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget=pstTargetSet->astTargets;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pstResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgInGray = pstRgs->pstImgInGray;
	GRAY_IMAGE_S *pstImgInEdgeMag = pstRgs->pstImgInEdgeMag;
	IMP_U8 *ps8LinePixels = module->ps8LinePixels;
	IMP_U8 *data;
	IMP_S8 s8DataCur,s8DataPre;
	IMP_S32 s32Tmp;
	IMP_S32 s32PosCur,s32PosPre;
	IMP_S32 s32ImgWidth = pstImgInGray->s32W;
	IMP_S32 s32ImgHeight = pstImgInGray->s32H;
	IMP_U8 u8MinDis,u8MaxDis;

	IMP_S32 row=module->u32FrmCnt%s32ImgHeight;


	module->u32FrmCnt++;

	cnt = pstTargetSet->s32UsedTotal;
	data = pstImgInGray->pu8Data+row*s32ImgWidth;


	if (s32ImgWidth >= 352)
	{
		u8MinDis = 3;
		u8MaxDis = 7;
	}
	else if(s32ImgWidth <= 176)
	{
		u8MinDis = 3;
		u8MaxDis = 4;
	}
	cf=0;

	for( x = 1; x < s32ImgWidth ; x++ )
	{
		s32Tmp = data[x] - data[x - 1];

		if (s32Tmp > 50 )
		{
			ps8LinePixels[x - 1] = 2;
		}
		else if(s32Tmp < -50 )
		{
			ps8LinePixels[x - 1] = 1;
		}
		else /*if(ps8LinePixels[x - 1] > -50 && ps8LinePixels[x - 1] < 50)*/
		{
			ps8LinePixels[x - 1] = 0;
		}
	}

	s8DataPre = 0;
	s8DataCur = 0;
	s32PosPre = 0;
	s32PosCur = 0;
	for( x = 0; x < s32ImgWidth ; x++ )
	{
		s8DataCur = ps8LinePixels[x];
		if (s8DataCur != 0)
		{
			s32PosCur = x;
			if ( x == 0)
			{
				continue;
			}
			else
			{
				if (s32PosPre == 0 )
				{
					s8DataPre = s8DataCur;
					s32PosPre = s32PosCur;
					continue;
				}
				else
				{
					if (s32PosCur - s32PosPre <= u8MinDis)
					{
						s32Tmp = s8DataPre * s8DataCur;
// 						if (s32Tmp > 0)
// 						{
// 							cf++;
// 						}
// 						else
						if(s32Tmp == 2)
						{
							cf--;
						}
					}
					else if (s32PosCur - s32PosPre >= u8MaxDis)
					{
						s32Tmp = s8DataPre * s8DataCur;
						if (s32Tmp == 2)
						{
							cf++;
						}
						else if(s32Tmp == 1 || s32Tmp == 4 )
						{
							cf--;
						}
					}

					s32PosPre = s32PosCur;
					s8DataPre = s8DataCur;
				}
			}
		}
	}


	if (cf < 0)
	{
		cf = 0;
	}

	if (module->s32Precf < 0)
	{
		module->s32Precf = cf;
		return IMP_TRUE;
	}

	if (s32ImgWidth >= 352)
	{
		if ((cf >= (s32ImgWidth >> 5)) && (cf <= (s32ImgWidth >> 3)) && (abs(module->s32Precf - cf) < 8))
		{
			module->s32Confidence++;
		}
		else
		{
			module->s32Confidence--;
		}
	}
	else if(s32ImgWidth <= 176)
	{
		if ((cf >= (s32ImgWidth >> 4)) && (cf <= (s32ImgWidth >> 2)) && (abs(module->s32Precf - cf) < 4))
		{
			module->s32Confidence++;
		}
		else
		{
			module->s32Confidence--;
		}
	}

	module->s32Precf = cf;


	if (module->s32Confidence > 100)
	{
		module->s32Confidence = 100;
	}
	else if (module->s32Confidence < 0)
	{
		module->s32Confidence = 0;
	}


	if (module->s32Confidence > 80)
	{
		pstResult->stSysStatus.u32WaterMarkStatus = IMP_WATERMARK_STAT_IMPOWER;
	}
	else
	{
		pstResult->stSysStatus.u32WaterMarkStatus = IMP_WATERMARK_STAT_UNKNOWN;
	}

	return IMP_TRUE;

}

IMP_S32 ipPostProcessWaterMarkDetector( IpWaterMarkDetector *module )
{

	return 1;
}
