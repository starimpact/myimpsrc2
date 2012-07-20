#include "imp_pea_bva_common.h"

IMP_S32 ipZoneObjectAvailable( IpTrackedTarget *pstTarget, GRAY_IMAGE_S *pstImage, IMP_S32 s32ZoneIndex )
{
	IMP_S32 s32Ret;
	IMP_U8  u8PixelVal;
	IpTargetPosition  *pstPos;

	pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	u8PixelVal = IMP_GrayImageGetPixelVal( pstImage, pstPos->stPt.s16X, pstPos->stPt.s16Y );
	s32Ret = ( u8PixelVal>>s32ZoneIndex & 0x01 );

	return s32Ret;
}
IMP_S32 ipIsAcoordantAngleRange( IMP_S32 s32Angle, IMP_S32 s32Direction, IMP_S32 s32Tolerance )
{
	IMP_S32 s32AngleDiff;

//	s32AngleDiff = s32Direction - s32Angle;

//	if (s32AngleDiff < 0)
//		s32AngleDiff += 360;
//	else if (s32AngleDiff > 360)
//		s32AngleDiff -= 360;

//	if (s32AngleDiff > 180)
//		s32AngleDiff = 360 - s32AngleDiff;

//	return (s32AngleDiff <= s32Tolerance);
	
	s32AngleDiff = abs(s32Direction - s32Angle);
	
	if (s32AngleDiff > 180) s32AngleDiff = 360 - s32AngleDiff;
	
	return (s32AngleDiff <= s32Tolerance);
}

IMP_S32 ipIsAccordantStatic( IpTrackedTarget *pstTarget )
{
	IpTargetFeatureVal *pstFeatureVal = (IpTargetFeatureVal*)pstTarget->stTargetFeat.au8Data;
	return pstFeatureVal->u32StaticTimeStart;
}

IMP_S32 ipIsAccordantStaticTime( IpTrackedTarget *pstTarget )
{
	IpTargetFeatureVal *pstFeatureVal = (IpTargetFeatureVal*)pstTarget->stTargetFeat.au8Data;
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
	return (pstPos->u32Time - pstFeatureVal->u32StaticTimeStart);
}

IMP_S32 ipGetObjectDirectionByTrajectory( IpTrackedTarget *pstTarget,IMP_S32 *s32Direction )
{
	IMP_FLOAT fDetaX = 0;
	IMP_FLOAT fDetaY = 0;
	IMP_S32 s32Quadrant = 0;
	IMP_FLOAT fMovingAngle = 0;
	IpTargetPosition *pstPos;
	IMP_S32 s32Len, s32Trail;
	IMP_S32 i;
	IMP_S32 s32Ret = 0;

	IMP_S32 s32X0 = 0;
	IMP_S32 s32Y0 = 0;
	IMP_S32 s32X1 = 0;
	IMP_S32 s32Y1 = 0;

	s32Len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	s32Trail = s32Len - 16;
	if (s32Trail <= 0)
	{
		*s32Direction = 0;
		return 1;
	}


	for (i = 0; i < 7; i++)
	{
		pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		s32X1+=pstPos->stPt.s16X;
		s32Y1+=pstPos->stPt.s16Y;
	}

	s32X1 /= 8;
	s32Y1 /= 8;
	for (i = 8; i < 15; i++)
	{
		pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		s32X0+=pstPos->stPt.s16X;
		s32Y0+=pstPos->stPt.s16Y;
	}

	s32X0 /= 8;
	s32Y0 /= 8;


	fDetaX = (IMP_FLOAT)(s32X1 - s32X0);
	fDetaY = (IMP_FLOAT)(s32Y1 - s32Y0);

	if ( fDetaX >= 0 )
	{
		if ( fDetaY >= 0 )
			s32Quadrant = 1;
		else
			s32Quadrant = 4;
	}
	else
	{
		if ( fDetaY >= 0 )
			s32Quadrant = 2;
		else
			s32Quadrant = 3;
	}
	if(fabs(fDetaX) > 0.000001)
		fMovingAngle = (IMP_FLOAT)atan(fDetaY/fDetaX);
	else
	{
		fMovingAngle = 0;

	}
	switch (s32Quadrant)
	{
	case 1:
		break;
	case 2:
	case 3:
		fMovingAngle += (IMP_FLOAT)IMP_PI;
		break;
	case 4:
		fMovingAngle += (IMP_FLOAT)IMP_PI * 2;
		break;
	default:
		break;
	}
	if ( fMovingAngle < 0 )
		fMovingAngle += (IMP_FLOAT)IMP_PI * 2;
	*s32Direction = (IMP_S32)(fMovingAngle * 180 / IMP_PI);

	return 1;
}


static IMP_S32 ipGetDirection(IMP_S32 s32Angle);
IMP_S32 ipGetObjectDirection( IpTrackedTarget *pstTarget,IMP_S32 s32PosIndex,IMP_S32 *s32Direction )
{
	IMP_S32 s32MeanDir = 0;
	IpTargetPosition *pstPos;
	IMP_S32 s32Len,s32Trail;
	IMP_S32 i;
	IMP_S32 s32Ret = 0;
	IMP_S32 s32AnalysLen;
	IMP_S32 s32MaxValue = -1, s32MinValue = 360;
	IMP_S32 s32Left = 0, s32Right = 0,s32Middle = 0;
	IMP_S32 s32MeanLeft = 0,s32MeanRight = 0, s32MeanMiddle = 0;

	s32Len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	s32Trail = s32Len - 8;
	if (s32Trail <= 0)
	{
		*s32Direction = 0;
		return 1;
	}
// 	s32Trail = s32Trail<0 ? s32Len:s32Trail;
	s32Trail = s32Trail > s32PosIndex + 8 ? s32PosIndex + 8 : s32Trail;

	{
		for (i = s32PosIndex; i < s32Trail + 1; i++)
		{
			pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
			s32MeanDir += pstPos->s32Direction;
			if (s32MaxValue < pstPos->s32Direction)
			{
				s32MaxValue = pstPos->s32Direction;
			}
			if (s32MinValue>pstPos->s32Direction)
			{
				s32MinValue = pstPos->s32Direction;
			}
			s32Ret = ipGetDirection(pstPos->s32Direction);
			if (s32Ret == 0)
			{
				s32Left++;
				s32MeanLeft += pstPos->s32Direction;
			}
			if (s32Ret == 1)
			{
				s32Middle++;
				s32MeanMiddle += pstPos->s32Direction;
			}
			if (s32Ret == 2)
			{
				s32Right++;
				s32MeanRight += pstPos->s32Direction;
			}
		}

		s32AnalysLen = s32Trail-s32PosIndex + 1;
		if(s32MinValue != 360 && s32MaxValue != -1 && s32AnalysLen > 3)
		{
			s32MeanDir -= s32MaxValue;
			s32MeanDir -= s32MinValue;
			s32AnalysLen -= 2;
		}
		if(s32AnalysLen != 0 )
			s32MeanDir /= s32AnalysLen;
		else
			s32MeanDir = 90;
	}

	if (s32Left > s32Right && s32Left > s32Middle)
	{
		s32Ret = 0;
		*s32Direction = s32MeanLeft / s32Left;
		return s32Ret;
	}
	else if(s32Right > s32Left && s32Right > s32Middle)
	{
		s32Ret = 2;
		*s32Direction = s32MeanRight / s32Right;
		return s32Ret;
	}
	else if(s32Middle > s32Left && s32Middle > s32Right)
	{
		s32Ret = 1;
		*s32Direction = s32MeanMiddle / s32Middle;
		return s32Ret;
	}
	else
		s32Ret = ipGetDirection(s32MeanDir);

	*s32Direction = s32MeanDir;
	return s32Ret;
}
static IMP_S32 ipGetDirection(IMP_S32 s32Angle)
{
	IMP_S32 s32Ret;
// 	if ( (s32Angle>=292&&s32Angle<=359) || (s32Angle>=0&&s32Angle<=67) )//用下底的右边作为目标的位置点
	if ( (s32Angle>=315&&s32Angle<=359) || (s32Angle>=0&&s32Angle<=45) )//用下底的右边作为目标的位置点
		s32Ret = 2;
// 	else if ( (s32Angle>=68&&s32Angle<=112) || (s32Angle>=247&&s32Angle<=291) )//用下底线的中间点作为目标的位置点
	else if ( (s32Angle>=44&&s32Angle<=135) || (s32Angle>=225&&s32Angle<=314) )//用下底线的中间点作为目标的位置点
		s32Ret = 1;
// 	else if (s32Angle>=113&&s32Angle<=246)//用下底线的左边点作为目标的位置点
	else if (s32Angle>=136&&s32Angle<=224)//用下底线的左边点作为目标的位置点
		s32Ret = 0;
	return s32Ret;
}

IMP_S32 ipGetObjectActualArea( IpTrackedTarget *pstTarget )
{
	IpTargetPosition *pstPos;
	IMP_S32 i;
	IMP_S32 s32Len;
	IMP_S32 s32Area = 0;
	s32Len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	if (s32Len > 5)
		s32Len = 5;
	for (i = 0; i < s32Len; i++)
	{
		pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		s32Area += pstPos->u32AreaActual;
	}
	s32Area /= s32Len;

	return s32Area;
}
static IMP_S32 ipGetRectInRegionPointNum(GRAY_IMAGE_S *pstImage,IpTargetPosition  *pstPos,IMP_S32 s32ZoneIndex);
static IMP_S32 ipComputeTargetInZoneRatio(IpTrackedTarget *pstTarget, GRAY_IMAGE_S *pstImage, IMP_S32 s32ZoneIndex);
IMP_S32 ipZoneObjectEffective( IpTrackedTarget *pstTarget, GRAY_IMAGE_S *pstImage, IMP_S32 s32ZoneIndex,IMP_S32 s32UsingBottom,IMP_S32 s32DelicacyFlag,IMP_S32 s32AreaRatio )
{
	IMP_S32 s32Ret = 0 ;
	IpTargetPosition  *pstPos;
	IMP_S32 s32Ratio = 0;
	IMP_S32 s32PointNum = 0;
	IMP_U8 u8PixelVal = 0;
	IMP_S32 s32Tmp = 0,s32TmpX = 0;
	IMP_S32 s32Width = pstImage->s32W;

	if(s32DelicacyFlag)
	{
		s32Ratio = ipComputeTargetInZoneRatio(pstTarget,pstImage,s32ZoneIndex);

		if (s32Ratio >= s32AreaRatio)
		{
			s32Ret = IMP_TRUE;
		}
		else
			s32Ret = IMP_FALSE;
	}
	else if(!s32UsingBottom)
	{
		s32Ret = ipZoneObjectAvailable(pstTarget, pstImage, s32ZoneIndex);
	}
	else if(s32UsingBottom)
	{
		pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
		if (pstTarget->stTargetInfo.u32Type == IMP_TGT_TYPE_VEHICLE)
		{
			s32PointNum = ipComputePointsInZone(pstTarget,pstPos,pstImage,s32ZoneIndex);
			s32Ratio = (s32PointNum * 100) >> 3;
			if (s32AreaRatio == 100 && s32Ratio == 100)
				s32Ret = IMP_TRUE;
			else if (s32AreaRatio != 100 && s32Ratio >= s32AreaRatio)
				s32Ret = IMP_TRUE;
			else
				s32Ret = IMP_FALSE;
		}
		else if ( pstTarget->stTargetInfo.u32Type == IMP_TGT_TYPE_HUMAN )
		{
			u8PixelVal = pstImage->pu8Data[pstPos->stRg.s16Y2 * s32Width + pstPos->stRg.s16X1];
			s32Tmp += u8PixelVal>>s32ZoneIndex & 0x01;

			u8PixelVal = pstImage->pu8Data[pstPos->stRg.s16Y2 * s32Width + pstPos->stRg.s16X2];
			s32Tmp += u8PixelVal>>s32ZoneIndex & 0x01;

			if (s32AreaRatio == 100 && s32Tmp == 2)
				s32Ret = IMP_TRUE;
			else if (s32AreaRatio != 100 && s32Tmp != 0)
				s32Ret = IMP_TRUE;
			else
				s32Ret = IMP_FALSE;
		}
		else
		{
// 			s32Ret = ipZoneObjectAvailable(pstTarget, pstImage, s32ZoneIndex);
			s32TmpX = (pstPos->stRg.s16X1 + pstPos->stRg.s16X2) >> 1;
			u8PixelVal = pstImage->pu8Data[pstPos->stRg.s16Y2 * s32Width + s32TmpX];
			s32Ret = u8PixelVal >> s32ZoneIndex & 0x01;
		}
	}
	return s32Ret;
}
IMP_S32 ipGetRectInRegionPointNum(GRAY_IMAGE_S *pstImage,IpTargetPosition  *pstPos,IMP_S32 s32ZoneIndex)
{
	IMP_S32 s32Width;
	IMP_S32 x,y;
	IMP_S32 s32Num = 0;
	IMP_U8  u8PixelVal;
	s32Width = pstImage->s32W;

	y = pstPos->stRg.s16Y1;	x = pstPos->stRg.s16X1;
	u8PixelVal = pstImage->pu8Data[y * s32Width + x];
	s32Num += u8PixelVal>>s32ZoneIndex & 0x01;

	y = pstPos->stRg.s16Y1;	x = pstPos->stRg.s16X2;
	u8PixelVal = pstImage->pu8Data[y * s32Width + x];
	s32Num += u8PixelVal>>s32ZoneIndex & 0x01;

	y = pstPos->stRg.s16Y2;	x = pstPos->stRg.s16X1;
	u8PixelVal = pstImage->pu8Data[y * s32Width + x];
	s32Num += u8PixelVal>>s32ZoneIndex & 0x01;

	y = pstPos->stRg.s16Y2;	x = pstPos->stRg.s16X2;
	u8PixelVal = pstImage->pu8Data[y * s32Width + x];
	s32Num += u8PixelVal>>s32ZoneIndex & 0x01;

	return s32Num;
}

IMP_S32 ipComputePolygonOfTarget(IpTrackedTarget *pstTarget,IpTargetPosition *pstPos,IpEightPolygon *pstPolygon)
{
	IMP_U8 *pImage = NULL;
	IMP_S32 w = 0,h = 0;
	IMP_S32 s32Sn, s32Index;
	IMP_S32 i = 0;
	IMP_POINT_S ul_point,ur_point,dl_point,dr_point,lu_point,ld_point,ru_point,rd_point;
	IMP_S32 s32TmpX, s32TmpY;

	s32Index = pstTarget->stTargetImage.s32DrgIndex;
	if(pstTarget->stTargetImage.pstDRegions!=NULL)
		pImage = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->pu8Data;
	else
		return 0;
	w = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->s32W;
	h = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->s32H;
	s32Sn=pstTarget->stTargetImage.pstDRegions->astDrg[s32Index].u8Sign;


	ul_point.s16X = pstPos->stRg.s16X2;	ul_point.s16Y = pstPos->stRg.s16Y1;
	ur_point.s16X = pstPos->stRg.s16X1;	ur_point.s16Y = pstPos->stRg.s16Y1;
	dl_point.s16X = pstPos->stRg.s16X2;  dl_point.s16Y = pstPos->stRg.s16Y2;
	dr_point.s16X = pstPos->stRg.s16X1;  dr_point.s16Y = pstPos->stRg.s16Y2;
	for (i = pstPos->stRg.s16X1; i < pstPos->stRg.s16X2; i++)
	{
		s32TmpY = pstPos->stRg.s16Y1;
		if ( pImage[s32TmpY * w + i] == s32Sn && i < ul_point.s16X )
			ul_point.s16X = i;
		if ( pImage[s32TmpY * w + i] == s32Sn && i > ur_point.s16X )
			ur_point.s16X = i;

		s32TmpY = pstPos->stRg.s16Y2;
		if ( pImage[s32TmpY * w + i] == s32Sn && i <dl_point.s16X )
			dl_point.s16X = i;
		if ( pImage[s32TmpY * w + i] == s32Sn && i > dr_point.s16X )
			dr_point.s16X = i;
	}

	s32TmpX = pstPos->stRg.s16X1;
	lu_point.s16X = pstPos->stRg.s16X1;	lu_point.s16Y = pstPos->stRg.s16Y2;
	ld_point.s16X = pstPos->stRg.s16X1;	ld_point.s16Y = pstPos->stRg.s16Y1;
	ru_point.s16X = pstPos->stRg.s16X2;  ru_point.s16Y = pstPos->stRg.s16Y2;
	rd_point.s16X = pstPos->stRg.s16X2;  rd_point.s16Y = pstPos->stRg.s16Y1;
	for (i = pstPos->stRg.s16Y1; i < pstPos->stRg.s16Y2; i++)
	{
		s32TmpX = pstPos->stRg.s16X1;
		if ( pImage[i * w + s32TmpX] == s32Sn && i < lu_point.s16Y )
			lu_point.s16Y = i;
		if ( pImage[i * w + s32TmpX] == s32Sn && i > ld_point.s16Y )
			ld_point.s16Y = i;

		s32TmpX = pstPos->stRg.s16X2;
		if ( pImage[i * w + s32TmpX] == s32Sn && i < ru_point.s16Y )
			ru_point.s16Y = i;
		if ( pImage[i * w + s32TmpX] == s32Sn && i > rd_point.s16Y )
			rd_point.s16Y = i;
	}

	memcpy(&pstPolygon->ul_point,&ul_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->ur_point,&ur_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->dl_point,&dl_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->dr_point,&dr_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->lu_point,&lu_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->ld_point,&ld_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->ru_point,&ru_point,sizeof(IMP_POINT_S));
	memcpy(&pstPolygon->rd_point,&rd_point,sizeof(IMP_POINT_S));
	return 1;
}
IMP_S32 ipComputePointsInZone(IpTrackedTarget *pstTarget,IpTargetPosition *pstPos,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex)
{
	IMP_S32 s32Ret = 0;
	IMP_U8  u8PixelVal;
	IpEightPolygon pstPolygon;
	IMP_S32 s32Width = pstImage->s32W;

	if( !ipComputePolygonOfTarget(pstTarget,pstPos,&pstPolygon) )
		return 0;

	u8PixelVal = pstImage->pu8Data[pstPolygon.ul_point.s16Y * s32Width + pstPolygon.ul_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;
	u8PixelVal = pstImage->pu8Data[pstPolygon.ur_point.s16Y * s32Width + pstPolygon.ur_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;

	u8PixelVal = pstImage->pu8Data[pstPolygon.dl_point.s16Y * s32Width + pstPolygon.dl_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;
	u8PixelVal = pstImage->pu8Data[pstPolygon.dr_point.s16Y * s32Width + pstPolygon.dr_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;

	u8PixelVal = pstImage->pu8Data[pstPolygon.lu_point.s16Y * s32Width + pstPolygon.lu_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;
	u8PixelVal = pstImage->pu8Data[pstPolygon.ld_point.s16Y * s32Width + pstPolygon.ld_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;

	u8PixelVal = pstImage->pu8Data[pstPolygon.ru_point.s16Y * s32Width + pstPolygon.ru_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;
	u8PixelVal = pstImage->pu8Data[pstPolygon.rd_point.s16Y * s32Width + pstPolygon.rd_point.s16X];
	s32Ret += u8PixelVal>>s32ZoneIndex & 0x01;
	return s32Ret;
}

static IMP_S32 ipComputeTargetInZoneRatio(IpTrackedTarget *pstTarget, GRAY_IMAGE_S *pstImage, IMP_S32 s32ZoneIndex)
{
	IMP_U8  u8PixelVal;
	IpTargetPosition  *pstPos;
	IMP_S32 s32TgtPixArea=0;
	IMP_S32	s32AvailNum = 0;
	IMP_S32 x, y;
	IMP_S32 s32RgIndex=0;
	IMP_S32 s32ObjSn = 0;
	IMP_S32 s32Ratio = 0;
	IMP_S32 s32Sn = 0;
	IMP_S32 s32RetTmp = 0;
	IMP_S32 w = 0, h = 0;
	IMP_U8 *pu8DatCtr, *pu8DatCtrSn;

	pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	s32TgtPixArea = pstPos->u32AreaPixel;
	w = pstImage->s32W;
	h = pstImage->s32H;


	s32RetTmp = ipGetRectInRegionPointNum(pstImage,pstPos,s32ZoneIndex);
	if (!s32RetTmp)
	{
		return -1;
	}
	if (TRACKED_TARGET_is_meanshifted(pstTarget))
	{
		s32Ratio = 100;
		return s32Ratio;
	}

	s32RgIndex = pstTarget->stTargetImage.s32DrgIndex;
	if (NULL == pstTarget->stTargetImage.pstDRegions)
	{
		return 0;
	}
	s32ObjSn = pstTarget->stTargetImage.pstDRegions->astDrg[s32RgIndex].u8Sign;

	for (y=pstPos->stRg.s16Y1; y<pstPos->stRg.s16Y2; y++)
	{
		pu8DatCtr = pstImage->pu8Data + w*y + pstPos->stRg.s16X1;
		pu8DatCtrSn = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->pu8Data + w * y + pstPos->stRg.s16X1;
		for (x=pstPos->stRg.s16X1; x<pstPos->stRg.s16X2; x++)
		{
			u8PixelVal = pu8DatCtr[0];
			s32Sn = pu8DatCtrSn[0];
			s32RetTmp = ( u8PixelVal>>s32ZoneIndex & 0x01 && (s32Sn == s32ObjSn));
			s32AvailNum =  (s32RetTmp == 0) ? s32AvailNum : (s32AvailNum + 1);
			pu8DatCtr++;
			pu8DatCtrSn++;
		}
	}
	s32Ratio = (IMP_S32)(((IMP_FLOAT)s32AvailNum/s32TgtPixArea)*100);
	return s32Ratio;
}

IMP_S32 ipIsObjectInZone(IpCycleByteArray *pstCycleArray,IMP_S32 s32ZoneIndex,IMP_S32 s32PosIndex)
{
	IMP_U8 s32ZoneValue = ipByteCArrayGetElem(pstCycleArray,s32PosIndex);
	return (s32ZoneValue >> s32ZoneIndex) & 0x01;
}

IMP_VOID ipSetBitValue(IMP_S32 s32BitIndex,IMP_U8 *pu8ByteValue)
{
	IMP_U8 u8TmpValue = *pu8ByteValue;
	IMP_U8 s32Tmp;
	s32Tmp = (u8TmpValue >> s32BitIndex) | 0x01;
	s32Tmp = s32Tmp << s32BitIndex;
	*pu8ByteValue = u8TmpValue | s32Tmp;
}

