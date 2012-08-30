#include "time.h"
#include "stdlib.h"

#if 0
#include "cv.h"
#include "highgui.h"
#endif


#include "imp_osc_detect.h"
#include "imp_osc_detect_inner.h"
#include "imp_pea_regionset_label.h"


#define RMV_CHK_NEED_LEN 16
//remove detection
IMP_S32 impRMVDetect(IMP_OSCD_S *pstModule)
{
	IMP_S32 s32OI, s32PI;
	IMP_S32 s32R;
	IMP_U8 *pu8InGray = 0, *pu8PreGray = 0, *pu8Static2 = 0;
	IMP_U8 *pu8SBkg = 0;
	GA_HARDWARE_RS_S *pstHwResource = pstModule->pstHwResource; //系统硬件资源
	PEA_RESULT_S *pstResult = pstModule->pstResult; //系统公共数据
	
	GRAY_IMAGE_S *pstInGray = &pstResult->stImgInGray;
	IMP_S32 s32Width = pstResult->s32Width, s32Height = pstResult->s32Height;
	
	pu8InGray = pstInGray->pu8Data;
	pu8PreGray = pstModule->stImgPreGray.pu8Data;
	pu8Static2 = pstModule->stImgFgStatic2.pu8Data;
	pu8SBkg = pstModule->stImgSBkg.pu8Data;
	
	s32R = pstResult->s32Noise;
	
	memset(pu8Static2, 0, s32Width * s32Height);
	
	for (s32PI = 0; s32PI < s32Width * s32Height; s32PI++)
	{
		if (abs(pu8InGray[s32PI] - pu8SBkg[s32PI]) > s32R)
		{
			pu8Static2[s32PI] = 1;
		}
	}
	
#if OSCD_DBG_SHW
	ipShowBinImage(s32Width, s32Height, pu8Static2, "pu8Static2");
#endif
	
	for (s32OI = 0; s32OI < OSCD_MAX_TGT_CNT; s32OI++)
	{
	//	IMP_STATIC_OBJ_S *pstSObj = impGetRObjBySign(pstModule->stSObjSet.aSObjs, s32OI);
		IMP_STATIC_OBJ_S *pstSObj = &pstModule->stSObjSet.aSObjs[s32OI];
		if (OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used))
		{
			IMP_S32 s32RMVNum = 0, s32NeedLen = RMV_CHK_NEED_LEN, s32IFM;
			pstSObj->stSOBJType = impCheckRMotion(pstModule, pstSObj);
			s32IFM = impCheckIFM(pstModule, pstSObj);
			//set SOBJT_UKN when there is motion in the RMV object area.
			if (s32IFM > 2)
			{
				pstSObj->stSOBJType = SOBJT_UKN;
			}
			//match static object with current frame, and update
			impMatchRObject(pstModule, pstSObj);
			impUpdateRPosition(pstSObj);
			
			if (pstSObj->s32PosLen > s32NeedLen)
			{
				s32RMVNum = 0;
				for (s32PI = pstSObj->s32PosLen; s32PI > pstSObj->s32PosLen - s32NeedLen; s32PI--)
				{
					if (pstSObj->astSOBJT[s32PI] == SOBJT_RMV)
					{
						s32RMVNum++;
					}
				}

#if OSCD_DBG_SHW				
				printf("\033[33mID_%d_RMV:%d(%d,%d%%)\033[0m\n", pstSObj->u32TargetId, pstSObj->stSOBJType == SOBJT_RMV, pstSObj->s32PosLen, s32RMVNum * 100 / s32NeedLen);
#endif

				if (s32RMVNum * 100 > s32NeedLen * 90)
				{
					OSCD_ROBJ_SET_REALOBJ(pstSObj->u8Used);
				}
			}
		}
	}
	
	for (s32OI = 0; s32OI < OSCD_MAX_TGT_CNT; s32OI++)
	{
	//	IMP_STATIC_OBJ_S *pstSObj = impGetRObjBySign(pstModule->stSObjSet.aSObjs, s32OI);
		IMP_STATIC_OBJ_S *pstSObj = &pstModule->stSObjSet.aSObjs[s32OI];
		if (OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used) && OSCD_ROBJ_IS_REALOBJ(pstSObj->u8Used))
		{
			pstSObj->u32Life++;
		}
	}
	
#if OSCD_DBG_SHW //	
	IMP_S32 s32ObjNum = 0;
	IMP_S32 s32BRI, s32BCI;
	IMP_S32 s32RI, s32CI, s32Oft, s32Oft2;
	IplImage *pstImgMark = cvCreateImage(cvSize(s32Width, s32Height), 8, 3);
	char abyText[100];
	CvFont font;
    cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.35f,0.35f,0,1,CV_AA);
    
	for (s32RI = 0; s32RI < s32Height; s32RI++)
	{
		s32Oft = s32RI * pstImgMark->widthStep;
		s32Oft2 = s32RI * s32Width;
		for (s32CI = 0; s32CI < s32Width; s32CI++)
		{
			pstImgMark->imageData[s32Oft + s32CI * 3 + 0] = pu8InGray[s32Oft2 + s32CI];
			pstImgMark->imageData[s32Oft + s32CI * 3 + 1] = pu8InGray[s32Oft2 + s32CI];
			pstImgMark->imageData[s32Oft + s32CI * 3 + 2] = pu8InGray[s32Oft2 + s32CI];
		}
	}
	for (s32RI = 0; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
	{
	//	IMP_STATIC_OBJ_S *pstSObj = impGetRObjBySign(pstModule->stSObjSet.aSObjs, s32RI);
		IMP_STATIC_OBJ_S *pstSObj = &pstModule->stSObjSet.aSObjs[s32RI];
		if (OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used) && pstSObj->u8Used)
		{
			s32ObjNum++;
		}
		
		if (pstSObj && OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used))
		{
			IMP_RECT_S stRect = pstSObj->stRect;
			cvRectangle(pstImgMark, cvPoint(stRect.s16X1, stRect.s16Y1), cvPoint(stRect.s16X2, stRect.s16Y2), CV_RGB(0, 0, 255), 1, 8, 0);
		}
		
		if (pstSObj && OSCD_ROBJ_IS_REALOBJ(pstSObj->u8Used))
		{
			IMP_RECT_S stRect = pstSObj->stRect;
			cvRectangle(pstImgMark, cvPoint(stRect.s16X1, stRect.s16Y1), cvPoint(stRect.s16X2, stRect.s16Y2), CV_RGB(255, 0, 0), 1, 8, 0);
		}
		
		if (pstSObj && OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used))
		{
			IMP_RECT_S stRect = pstSObj->stRect;
			sprintf(abyText, "%d,(%d,%d)_%d", pstSObj->u8Sign, pstSObj->s32PosLen, pstSObj->u32Life, pstSObj->u32TargetId);
			cvPutText(pstImgMark, abyText, cvPoint((stRect.s16X1 + stRect.s16X2) / 2, (stRect.s16Y1 + stRect.s16Y2) / 2), &font, CV_RGB(0, 255, 0));
		}
	}
	cvShowImage("R_OBJ_INFO", pstImgMark);
	cvReleaseImage(&pstImgMark);
	printf("s32ObjNum:%d\n", s32ObjNum);
#endif //
	
	
	return 0;
}


/*
//check if the current static object is real object
IMP_S32 impCheckRealRObj(IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32PI;
	IMP_POINT_S *pstPos = 0;
	IMP_U16 *pu16MatchScore = 0, *pu16AreaSize = 0;
	IMP_S32 s32PosLen, s32MeanPosX, s32MeanPosY, s32VarPosX, s32VarPosY;
	IMP_S32 s32MatchTime, s32MeanMT, s32MeanAT, s32VarAT;
	IMP_S32 s32FPS = 16, s32PosThrd = 0, s32MSThrd = 0, s32ASThrd = 0;
	
	if (OSCD_ROBJ_IS_REALOBJ(pstObj->u8Used)) return 0;
	
	pstPos = pstObj->astPos;
	pu16MatchScore = pstObj->au16MatchScore;
	pu16AreaSize = pstObj->au16AreaSize;
	s32PosLen = pstObj->s32PosLen;
	s32MatchTime = pstObj->s32PosLen;
	
	//OSCD_SOBJ_ELEM_LEN
	if (s32PosLen < s32FPS) return -1;
	
	//assume the frame rate is 25fps
	s32MeanPosX = 0; s32MeanPosY = 0;
	s32MeanAT = 0;
	s32MeanMT = 0;
	for (s32PI = s32PosLen - 1; s32PI >= s32PosLen - s32FPS; s32PI--)
	{
		s32MeanPosX += pstPos[s32PI].s16X;
		s32MeanPosY += pstPos[s32PI].s16Y;
		s32MeanAT += pu16AreaSize[s32PI];
		s32MeanMT += pu16MatchScore[s32PI];
	}
	s32MeanPosX /= s32FPS;
	s32MeanPosY /= s32FPS;
	s32MeanAT /= s32FPS;
	s32MeanMT /= s32FPS;
	
	s32VarPosX = 0; s32VarPosY = 0;
	s32VarAT = 0;
	for (s32PI = s32PosLen - 1; s32PI >= s32PosLen - s32FPS; s32PI--)
	{
		s32VarPosX += abs(s32MeanPosX - pstPos[s32PI].s16X) << 2;
		s32VarPosY += abs(s32MeanPosY - pstPos[s32PI].s16Y) << 2;
		s32VarAT += abs(s32MeanAT - pu16AreaSize[s32PI]) << 2;
	}
	s32VarPosX /= s32FPS;
	s32VarPosY /= s32FPS;
	s32VarAT /= s32FPS;
	
	s32PosThrd = 2 << 2;
	s32ASThrd = (pstObj->u32AreaSize / 40) << 2;
	printf("s32ASThrd:%d\n", s32ASThrd);
	if (s32VarPosX < s32PosThrd && s32VarPosY < s32PosThrd && s32VarAT < s32ASThrd && s32MeanMT > 80)
	{
		printf("\033[32mREAL_%d: hist:%d (%d,%d,vat:%d,mt:%d)\033[0m\n", pstObj->u32TargetId, pstObj->s32PosLen, s32VarPosX, s32VarPosY, s32VarAT, s32MeanMT);
		pstObj->stSOBJType = SOBJT_STP; //the real object must be stay(stop) type.
		OSCD_SOBJ_SET_REALOBJ(pstObj->u8Used);
	}
	else
	{
		OSCD_SOBJ_DEL_REALOBJ(pstObj->u8Used);
	}
	
	return 0;
}
*/

//find wanted static object by u8Sign
IMP_STATIC_OBJ_S *impGetRObjBySign(IMP_STATIC_OBJ_S *pstSObj, IMP_U8 u8Sign)
{
	IMP_S32 s32PI;
	
	for (s32PI = 0; s32PI < OSCD_MAX_TGT_CNT; s32PI++)
	{
		if (OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used) && u8Sign == pstSObj->u8Sign)
		{
			return pstSObj;
		}
		pstSObj++;
	}
	
	return 0;
}


//update the static object's position and type
IMP_S32 impUpdateRPosition(IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32PI;
	IMP_POINT_S *pstPos = 0;
	IMP_SOBJ_TYPE_S *penType = 0;
	IMP_U16 *pu16MS = 0, *pu16AS = 0;
	
	pstPos = pstObj->astPos;
	penType = pstObj->astSOBJT;
	pu16MS = pstObj->au16MatchScore;
	pu16AS = pstObj->au16AreaSize;
	
	if (pstObj->s32PosLen == OSCD_SOBJ_ELEM_LEN)
	{
		for (s32PI = 0; s32PI < OSCD_SOBJ_ELEM_LEN - 1; s32PI++)
		{
			pstPos[s32PI] = pstPos[s32PI + 1];
			penType[s32PI] = penType[s32PI + 1];
			pu16MS[s32PI] = pu16MS[s32PI + 1];
			pu16AS[s32PI] = pu16AS[s32PI + 1];
		}
		pstPos[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->stPos;
		penType[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->stSOBJType;
		pu16AS[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->u32AreaSize;
		pu16MS[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->u32MatchScore;
	}
	else
	{
		pstPos[pstObj->s32PosLen] = pstObj->stPos;
		penType[pstObj->s32PosLen] = pstObj->stSOBJType;
		pu16MS[pstObj->s32PosLen] = pstObj->u32MatchScore;
		pu16AS[pstObj->s32PosLen] = pstObj->u32AreaSize;
		pstObj->s32PosLen++;
	}
	
	
	return 0;
}


//calc bounding box from points set.
IMP_S32 impGetBoundBox(IMP_POINT_S *pstPnts, IMP_S32 s32PntNum, IMP_RECT_S *pstRect)
{
	IMP_S32 s32PI;
	IMP_POINT_S stPnt1, stPnt2, stPnt;
	
	stPnt1.s16X = 10000;
	stPnt1.s16Y = 10000;
	stPnt2.s16X = 0;
	stPnt2.s16Y = 0;
	
	for (s32PI = 0; s32PI < s32PntNum; s32PI++)
	{
		stPnt = pstPnts[s32PI];
		if (stPnt1.s16X > stPnt.s16X)
		{
			stPnt1.s16X = stPnt.s16X;
		}
		if (stPnt1.s16Y > stPnt.s16Y)
		{
			stPnt1.s16Y = stPnt.s16Y;
		}
		if (stPnt2.s16X < stPnt.s16X)
		{
			stPnt2.s16X = stPnt.s16X;
		}
		if (stPnt2.s16Y < stPnt.s16Y)
		{
			stPnt2.s16Y = stPnt.s16Y;
		}
	}
	
	pstRect->s16X1 = stPnt1.s16X;
	pstRect->s16Y1 = stPnt1.s16Y;
	pstRect->s16X2 = stPnt2.s16X;
	pstRect->s16Y2 = stPnt2.s16Y;
	
	return 0;
}


//crate objects for remove areas in config function
IMP_S32 impCreateRObjects(IMP_OSCD_S *pstModule)
{
	IMP_S32 s32I, s32PI, s32RI, s32CI;
	IMP_S32 s32W, s32H, s32Pos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	POLYGON_REGION_S *pstPolygon = 0;
	IMP_POINT_S stPnt;
	IMP_U8 *pu8RSign = 0, *pu8RObjEdge = 0;
	IMP_U8 *pu8InGray = 0, *pu8InEdge = 0;
	OSCD_REGION_S *pstRMVRgns = 0;

	pstRMVRgns = pstModule->astRMVRgns;
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	pu8RSign = pstModule->stImgRObjSign.pu8Data;
	
	//mark the remove area
	memset(pu8RSign, 0, s32W * s32H);
	for (s32I = 0; s32I < IMP_MAX_OSC_NUM; s32I++)
	{
		OSCD_REGION_S *pstRgn = &pstRMVRgns[s32I];
		if (pstRgn->s32Valid)
		{
			pstPolygon = &pstRgn->stRgn;
			if (pstPolygon->s32Valid)
			{
				int dwI;
			//	printf("pstPolygon->s32PointNum:%d\n", pstPolygon->s32PointNum);
			//	for (dwI = 0; dwI < pstPolygon->s32PointNum; dwI++)
			//	{
			//		printf("(%d,%d)", pstPolygon->astPoint[dwI].s16X, pstPolygon->astPoint[dwI].s16Y);
			//	}
			//	printf("\n");
				for (s32RI = 0; s32RI < s32H; s32RI++)
				{
					for (s32CI = 0; s32CI < s32W; s32CI++)
					{
						stPnt.s16X = s32CI;
						stPnt.s16Y = s32RI;
						if (impInsidePolygon(pstPolygon->astPoint, pstPolygon->s32PointNum, stPnt) == INSIDE)
						{
							s32PI = s32RI * s32W + s32CI;
							pu8RSign[s32PI] = s32I + 1;
						}
					}
				}
			}
		}
	}

#if OSCD_DBG_SHW
	ipShowBinImage(s32W, s32H, pu8RSign, "r_sign");
#endif

	pu8InGray = pstResult->stImgInGray.pu8Data;
	pu8InEdge = pstModule->stImgGrayEdge.pu8Data;
	
	pu8RObjEdge = pstModule->stImgRObjEdge.pu8Data; //RMV object edge image
	
	
	//create RMV object
	for (s32I = 0; s32I < IMP_MAX_OSC_NUM; s32I++)
	{
		OSCD_REGION_S *pstRgn = &pstRMVRgns[s32I];
		IMP_RECT_S stRect;
		RULE_S *pstRule = pstModule->pstRule;
		
		pstPolygon = &pstRgn->stRgn;
		if (pstRgn->s32Valid && pstPolygon->s32Valid)
		{
			IMP_STATIC_OBJ_S *pstObj = impGetEmptyObj(pstModule->stSObjSet.aSObjs, &pstModule->u32IDCounter);
			//create object
			if (pstObj)
			{
				IMP_S32 s32AreaSize = 0;
				impGetBoundBox(pstPolygon->astPoint, pstPolygon->s32PointNum, &stRect);
				for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
				{
					for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
					{
						s32PI = s32RI * s32W + s32CI;
						if (pu8RSign[s32PI] == s32I + 1)
						{
							s32AreaSize++;
							pu8RObjEdge[s32PI] = pu8InEdge[s32PI];
							pu8RObjEdge[s32PI - 1] = pu8InEdge[s32PI - 1];
							pu8RObjEdge[s32PI + 1] = pu8InEdge[s32PI + 1];
						}
					}
				}
				
				OSCD_ROBJ_SET_CANDIDATE(pstObj->u8Used);
				pstObj->u32AreaSize = s32AreaSize;
				pstObj->u8Sign = s32I + 1;
				pstObj->stRect = stRect;
				pstObj->stPos.s16X = (stRect.s16X1 + stRect.s16X2) / 2;
				pstObj->stPos.s16Y = (stRect.s16Y1 + stRect.s16Y2) / 2;
				impUpdateRPosition(pstObj);
			}
		}
	}

#if OSCD_DBG_SHW	
	ipShowGrayImage(s32W, s32H, pu8RObjEdge, "r_edge");
#endif

	return 0;
}



//delete object
IMP_S32 impDeleteRObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8ObjSign = 0, *pu8ObjEdge = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult; 
	
	GRAY_IMAGE_S *pstImgRObjSign = 0, *pstImgRObjEdge = 0;
	
	if (!OSCD_ROBJ_IS_CANDIDATE(pstObj->u8Used)) return -1;
//	printf("\033[31mDEL_%d:%d\033[0m\n", pstObj->u32TargetId, pstObj->s32PosLen);
	
	pstImgRObjSign = &pstModule->stImgRObjSign;
	pstImgRObjEdge = &pstModule->stImgRObjEdge;
	
	s32W = pstImgRObjSign->s32W;
	s32H = pstImgRObjSign->s32H;
	
	pu8ObjSign = pstImgRObjSign->pu8Data;
	pu8ObjEdge = pstImgRObjEdge->pu8Data;
	
	//clean old object area
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				pu8ObjSign[s32Oft] = 0;
				pu8ObjEdge[s32Oft] = 0;
				pu8ObjEdge[s32Oft - 1] = 0;
				pu8ObjEdge[s32Oft + 1] = 0;
			}
		}
	}
	
	memset(pstObj, 0, sizeof(IMP_STATIC_OBJ_S));
	
	return 0;
}


//check if has inter-frame motion in the object area
IMP_S32 impCheckIFM(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_S32 s32W = 0, s32H = 0, s32Oft;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_U8 *pu8Static2 = 0, *pu8ObjSign = 0;
	IMP_U8 u8ObjSign;
	IMP_RECT_S stRect;
	IMP_S32 s32MovePntNum, s32ObjPntNum;
	
	pu8Static2 = pstModule->stImgFgStatic2.pu8Data;
	pu8ObjSign = pstModule->stImgRObjSign.pu8Data;

	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	s32MovePntNum = 0;
	s32ObjPntNum = 0;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				s32ObjPntNum++;
				if (pu8Static2[s32Oft])
				{
					s32MovePntNum++;
				}
			}
		}
	}
	
	s32ObjPntNum += 1;
	
//	printf("CheckIFM_%d:%d\n", pstObj->u32TargetId, s32MovePntNum * 100 / s32ObjPntNum);
	
	
	
	return s32MovePntNum * 100 / s32ObjPntNum;
}


//check motion type
IMP_SOBJ_TYPE_S impCheckRMotion(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8Gray = 0, *pu8Bkg = 0, *pu8ObjSign = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_S32 s32GrayDiff = 0, s32BkgDiff = 0;
	IMP_U16 u16Val;
	IMP_U8 au8Bin[9];
	IMP_S32 as32Oft[9], s32GradX, s32GradY;
	IMP_S32 s32ScoreLost, s32ScoreStay, s32ContourLen;
	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	pu8Gray = pstResult->stImgInGray.pu8Data;
	pu8Bkg = pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	pu8ObjSign = pstModule->stImgRObjSign.pu8Data;
	
	as32Oft[0] = -s32W - 1; as32Oft[1] = -s32W; as32Oft[2] = -s32W + 1;
	as32Oft[3] = -1; as32Oft[4] = 0; as32Oft[5] = 1;
	as32Oft[6] = s32W - 1; as32Oft[7] = s32W; as32Oft[8] = s32W + 1; 
	
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	
	s32ScoreLost = 0;
	s32ScoreStay = 0;
	s32ContourLen = 0;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				au8Bin[0] = pu8ObjSign[s32Oft + as32Oft[0]] > 0;
				au8Bin[1] = pu8ObjSign[s32Oft + as32Oft[1]] > 0;
				au8Bin[2] = pu8ObjSign[s32Oft + as32Oft[2]] > 0;
				au8Bin[3] = pu8ObjSign[s32Oft + as32Oft[3]] > 0;
				au8Bin[4] = pu8ObjSign[s32Oft + as32Oft[4]] > 0;
				au8Bin[5] = pu8ObjSign[s32Oft + as32Oft[5]] > 0;
				au8Bin[6] = pu8ObjSign[s32Oft + as32Oft[6]] > 0;
				au8Bin[7] = pu8ObjSign[s32Oft + as32Oft[7]] > 0;
				au8Bin[8] = pu8ObjSign[s32Oft + as32Oft[8]] > 0;
				
				u16Val = au8Bin[0] + au8Bin[1] + au8Bin[2] + au8Bin[3] + au8Bin[4] + au8Bin[5] + au8Bin[6] + au8Bin[7] + au8Bin[8];
				if (u16Val > 0 && u16Val < 9)
				{
					IMP_S32 s32ValF[2], s32ValB[2], s32POft1, s32POft2;
					IMP_POINT_S stPos1, stPos2;
					
					s32ContourLen++;
				//	printf("hi,");
					s32GradX = au8Bin[2] + au8Bin[5] + au8Bin[8] - (au8Bin[0] + au8Bin[3] + au8Bin[6]); 
					s32GradY = au8Bin[6] + au8Bin[7] + au8Bin[8] - (au8Bin[0] + au8Bin[1] + au8Bin[2]);
					
					//get position along direction
					//impGetPosAlongDir(s32GradX, s32GradY, 2, &stPos1, &stPos2);
					
					stPos1.s16X = 0; stPos1.s16Y = 2;
					stPos2.s16X = 0; stPos2.s16Y = -2;
					//get position along direction
					if (abs(s32GradX) > abs(s32GradY))
					{
						stPos1.s16X = 2; stPos1.s16Y = 0;
						stPos2.s16X = -2; stPos2.s16Y = 0;
					}					
					
					s32POft1 = (stPos1.s16Y + s32RI) * s32W + stPos1.s16X + s32CI;
					s32POft2 = (stPos2.s16Y + s32RI) * s32W + stPos2.s16X + s32CI;
					
					s32ValF[0] = pu8Gray[s32POft1 + as32Oft[0]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[1]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[2]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[3]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[4]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[5]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[6]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[7]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[8]];
					s32ValF[0] /= 9;
					
					s32ValF[1] = pu8Gray[s32POft2 + as32Oft[0]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[1]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[2]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[3]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[4]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[5]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[6]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[7]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[8]];
					s32ValF[1] /= 9;
					
					s32ValB[0] = pu8Bkg[s32POft1 + as32Oft[0]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[1]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[2]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[3]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[4]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[5]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[6]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[7]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[8]];
					s32ValB[0] /= 9;
					
					s32ValB[1] = pu8Bkg[s32POft2 + as32Oft[0]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[1]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[2]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[3]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[4]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[5]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[6]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[7]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[8]];
					s32ValB[1] /= 9;
					
					s32ScoreLost += abs(s32ValB[0] - s32ValB[1]) > abs(s32ValF[0] - s32ValF[1]) * 2;
					s32ScoreStay += abs(s32ValF[0] - s32ValF[1]) > abs(s32ValB[0] - s32ValB[1]) * 2;
				}
			}
		}
	}
	
	if (s32ContourLen == 0) return SOBJT_UKN;

#if OSCD_DBG_SHW
	printf("\033[33mID_%d: Stay:%d(%d%%), Lost:%d(%d%%)\033[0m\n", pstObj->u32TargetId, s32ScoreStay, s32ScoreStay * 100 / s32ContourLen, s32ScoreLost, s32ScoreLost * 100 / s32ContourLen);
#endif

	if (s32ScoreStay * 100 > s32ContourLen * 50 && s32ScoreStay > s32ScoreLost * 3) return SOBJT_STP;
	if (s32ScoreLost * 100 > s32ContourLen * 50 && s32ScoreStay * 3 < s32ScoreLost) return SOBJT_RMV;
//	else return SOBJT_UKN;
	
	return SOBJT_UKN;
}


//match RMV object with current frame, and update
IMP_S32 impMatchRObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI, s32PI;
	IMP_S32 s32R, s32Diff;
	IMP_U8 *pu8Gray = 0, *pu8ObjSign = 0, *pu8ObjEdge = 0, *pu8GrayEdge = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign, u8Flag;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_S32 s32GraySame = 0, s32EdgeSame = 0, s32EdgeNum = 0, as32Oft[9];
	IMP_S32 s32MatchScore, s32GrayScore, s32EdgeScore, s32NCC;
	IMP_U8 au8Data1[9], au8Data2[9];
	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	as32Oft[0] = -s32W - 1; as32Oft[1] = -s32W; as32Oft[2] = -s32W + 1;
	as32Oft[3] = -1; as32Oft[4] = 0; as32Oft[5] = 1;
	as32Oft[6] = s32W - 1; as32Oft[7] = s32W; as32Oft[8] = s32W + 1;
	
	s32R = pstResult->s32Noise;
	pu8Gray = pstResult->stImgInGray.pu8Data;
	pu8GrayEdge = pstModule->stImgGrayEdge.pu8Data;
	pu8ObjSign = pstModule->stImgRObjSign.pu8Data;
	pu8ObjEdge = pstModule->stImgRObjEdge.pu8Data;
	
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	
	s32GraySame = 0;
	s32EdgeSame = 0;
	s32EdgeNum = 1;
	s32NCC = 0;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				if (pu8ObjEdge[s32Oft] > 15 || pu8GrayEdge[s32Oft] > 15)
				{
					s32EdgeNum++;
					au8Data1[0] = pu8ObjEdge[s32Oft + as32Oft[0]];
					au8Data1[1] = pu8ObjEdge[s32Oft + as32Oft[1]];
					au8Data1[2] = pu8ObjEdge[s32Oft + as32Oft[2]];
					au8Data1[3] = pu8ObjEdge[s32Oft + as32Oft[3]];
					au8Data1[4] = pu8ObjEdge[s32Oft + as32Oft[4]];
					au8Data1[5] = pu8ObjEdge[s32Oft + as32Oft[5]];
					au8Data1[6] = pu8ObjEdge[s32Oft + as32Oft[6]];
					au8Data1[7] = pu8ObjEdge[s32Oft + as32Oft[7]];
					au8Data1[8] = pu8ObjEdge[s32Oft + as32Oft[8]];
					
					au8Data2[0] = pu8GrayEdge[s32Oft + as32Oft[0]];
					au8Data2[1] = pu8GrayEdge[s32Oft + as32Oft[1]];
					au8Data2[2] = pu8GrayEdge[s32Oft + as32Oft[2]];
					au8Data2[3] = pu8GrayEdge[s32Oft + as32Oft[3]];
					au8Data2[4] = pu8GrayEdge[s32Oft + as32Oft[4]];
					au8Data2[5] = pu8GrayEdge[s32Oft + as32Oft[5]];
					au8Data2[6] = pu8GrayEdge[s32Oft + as32Oft[6]];
					au8Data2[7] = pu8GrayEdge[s32Oft + as32Oft[7]];
					au8Data2[8] = pu8GrayEdge[s32Oft + as32Oft[8]];
					
					s32NCC += impNCC_U8(au8Data1, au8Data2, 9);
				}
			}
		}
	}


//	s32GrayScore = s32GraySame * 100 / pstObj->u32AreaSize;
//	s32EdgeScore = s32EdgeSame * 100 / s32EdgeNum;
//	s32MatchScore = (s32GrayScore + s32EdgeScore * 2) / 3;
	s32NCC = s32NCC < 0 ? 0 : s32NCC;
	s32MatchScore = s32NCC / s32EdgeNum;
	pstObj->u32MatchScore = s32MatchScore;
//	printf("ID_%d_MatchScore:%d%%, EdgeScore:%d%%, life:%d, PLen:%d\n", pstObj->u32TargetId, s32GraySame * 100 / pstObj->u32AreaSize, s32EdgeSame * 100 / s32EdgeNum, pstObj->u32Life, pstObj->s32PosLen);

#if OSCD_DBG_SHW
	printf("ID_%d_MatchScore_NCC:%d%%\n", pstObj->u32TargetId, s32MatchScore);
#endif

	IMP_S32 s32TestLen = RMV_CHK_NEED_LEN * 4;
	//judge if mark the RMV object
	if (pstObj->s32PosLen > s32TestLen)
	{
		s32MatchScore = 0;
		for (s32PI = pstObj->s32PosLen; s32PI > pstObj->s32PosLen - s32TestLen; s32PI--)
		{
			s32MatchScore += pstObj->au16MatchScore[s32PI];
		}
		
		s32MatchScore /= s32TestLen;
		
//		if (s32MatchScore < 50) impDeleteObject(pstModule, pstObj);
#if OSCD_DBG_SHW
	printf("ID_%d_MatchScore_NCC_2:%d%%\n", pstObj->u32TargetId, s32MatchScore);
#endif
//		if (s32MatchScore < 50) OSCD_ROBJ_SET_REALOBJ(pstObj->u8Used);
		if (s32MatchScore < 40) OSCD_ROBJ_SET_REALOBJ(pstObj->u8Used);
	}
	
	return 0;
}










