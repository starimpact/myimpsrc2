#include "imp_draw_osd.h"


IMP_VOID IMP_DrawRect( VIDEO_FRAME_S *pVBuf, IMP_RECT_S *rc,PIXEL_S *cr,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo );
IMP_VOID IMP_DrawLine( VIDEO_FRAME_S *pVBuf, IMP_POINT_S *pt1, IMP_POINT_S *pt2,PIXEL_S *cr ,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo);



void IMP_CopySmallImage2Video(VIDEO_FRAME_S *pVBuf, GRAY_IMAGE_S *pstSmall, IMP_S32 s32X, IMP_S32 s32Y)
{	
	IMP_U32 w, h;
	HI_U32 phy_addr,size;
	IMP_U8 *pVBufVirt_Y = 0, *pVBufVirt_VU = 0, *Dst = 0, *Sor = 0;
	if(pVBuf->enPixelFormat == PIXEL_FORMAT_YUV_SEMIPLANAR_420 )
	{	
		size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
	}
	else
		{
			printf("Frame->format is not yuv_420\n");
			return;
		}
	phy_addr = pVBuf->u32PhyAddr[0];
    pVBufVirt_Y = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
//	printf("pVBufVirt_Y:%x\n", pVBufVirt_Y);
	pVBufVirt_VU = pVBufVirt_Y+pVBuf->u32Width*pVBuf->u32Height;
	if (NULL == pVBufVirt_Y)
	{
		return;
	}
	Sor = pstSmall->pu8Data;
	Dst = pVBufVirt_Y + pVBuf->u32Width*s32Y +s32X;
//	printf("ww:%d; s32X:%d, s32Y:%d\n", pVBuf->u32Width, s32X, s32Y);
//	printf("Dst:%x, Sor:%x \n", Dst, Sor);
	for(h = 0;(h<pstSmall->s32H) && (h<pVBuf->u32Height-s32Y);h++)
	{
//		printf("h:%d ", h);
		if(pstSmall->s32W<pVBuf->u32Width-s32X)
		{
//			printf("Dst:%x, Sor:%x ", Dst, Sor);
			memcpy(Dst,Sor,pstSmall->s32W);
		}
		else
		{
			memcpy(Dst,Sor,pVBuf->u32Width-s32X);
		}
		Dst+=pVBuf->u32Width;
		Sor+=pstSmall->s32W;
//		printf("\n");
	}
	
//	return;
	Dst = pVBufVirt_VU+pVBuf->u32Width*s32Y/2 +s32X;
	for(h=0;(h<pstSmall->s32H/2) && (h<pVBuf->u32Height/2-s32Y/2);h++)
	{
		if(pstSmall->s32W<pVBuf->u32Width-s32X)
		{
			memset(Dst,128,pstSmall->s32W);
		}
		else
		{
			memset(Dst,128,pVBuf->u32Width-s32X);
		}
		Dst+=pVBuf->u32Width;
		
	}
	
	HI_MPI_SYS_Munmap(pVBufVirt_Y, size);
	
	return ;
}


IMP_VOID IMP_DrawPeaResult(VIDEO_FRAME_S *pVBuf, RESULT_S *pstResult,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo)
{
    TGT_SET_S	*pstTargetSet = &pstResult->stTargetSet;
    IMP_S32 u32TargetNum = pstTargetSet->s32TargetNum;
    TGT_ITEM_S *pstTarget;
    IMP_RECT_S *rc;
    PIXEL_S *pstRectCr = &cr_tbl[0];
    PIXEL_S *pstLineCr = &cr_tbl[1];
    IMP_S32 i = 0;
    for( i = 0;i < u32TargetNum;i++)
    {
        pstTarget = &pstTargetSet->astTargets[i];
        if((pstTarget->u32Event &IMP_TGT_EVENT_PERIMETER )|| ( pstTarget->u32Event&IMP_TGT_EVENT_TRIPWIRE ) )
        {
            TGT_MOTION_ITEM_S  *item_data = (TGT_MOTION_ITEM_S *)(pstTarget->au8Data);
            TGT_TRAJECT_S *traject = &item_data->stTraject;
            IMP_S32 num = 0;
            IMP_S32 k = 0;
            IMP_POINT_S *pt1,*pt2;
            num = traject->s32Length;
            pt1 = &traject->stPoints[0];
            for( k=1; k<num; k++ )
            {
                pt2 = &traject->stPoints[k];
                IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,enFormatSrc,enFormatAlgo );
                pt1 = pt2;
            }
            rc = &pstTarget->stRect;
            IMP_DrawRect(pVBuf,rc,pstRectCr, enFormatSrc,enFormatAlgo);
        }
    }
}



IMP_VOID IMP_DrawOSCResult(VIDEO_FRAME_S *pVBuf, RESULT_S *result,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo)
{
    static int total = 0;
    int i,j,k,zone,num1,tmp,width ,height;
    PIXEL_S *pstRectCr = &cr_tbl[0];
    PIXEL_S *pstRectCr2 = &cr_tbl[2];
    PIXEL_S *pstLineCr = &cr_tbl[1];
 //   printf("result->stEventSet.s32EventNum:%d", result->stEventSet.s32EventNum);
//	return 0;
    for(i=0;i<result->stEventSet.s32EventNum;i++)
    {
//        if(result->stEventSet.astEvents[i].u32Status==IMP_EVT_STATUS_START)
        {
        //    printf("[%d][%d]Event Type:%x,   ", ++total, i, result->stEventSet.astEvents[i].u32Type);
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmOsc)
            {
                EVT_DATA_TARGET_S *pOSC = 0;
                zone=result->stEventSet.astEvents[i].u32Zone;
                pOSC=(EVT_DATA_TARGET_S*)(result->stEventSet.astEvents[i].au8Data);
            //    printf("Hello OSC!!!\n");
                IMP_DrawRect(pVBuf,&pOSC->stRect,pstRectCr, enFormatSrc,enFormatAlgo);
            }
            
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmOsc_R)
            {
                EVT_DATA_TARGET_S *pOSC = 0;
                zone=result->stEventSet.astEvents[i].u32Zone;
                pOSC=(EVT_DATA_TARGET_S*)(result->stEventSet.astEvents[i].au8Data);
            //    printf("Hello OSC!!!\n");
                IMP_DrawRect(pVBuf,&pOSC->stRect,pstRectCr2, enFormatSrc,enFormatAlgo);
            }
          //  printf("\n");
        }
    }
 //   printf("\n");
 #if 0
 {
     TGT_SET_S	*pstTargetSet = &result->stTargetSet;
    IMP_S32 u32TargetNum = pstTargetSet->s32TargetNum;
    TGT_ITEM_S *pstTarget;
    IMP_RECT_S *rc;
    PIXEL_S *pstRectCr = &cr_tbl[0];
    PIXEL_S *pstLineCr = &cr_tbl[1];
    IMP_S32 i = 0;
    for( i = 0;i < u32TargetNum;i++)
    {
        pstTarget = &pstTargetSet->astTargets[i];
       {
            TGT_MOTION_ITEM_S  *item_data = (TGT_MOTION_ITEM_S *)(pstTarget->au8Data);
            TGT_TRAJECT_S *traject = &item_data->stTraject;
            IMP_S32 num = 0;
            IMP_S32 k = 0;
            IMP_POINT_S *pt1,*pt2;
            num = traject->s32Length;
            pt1 = &traject->stPoints[0];
            for( k=1; k<num; k++ )
            {
                pt2 = &traject->stPoints[k];
                IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,enFormatSrc,enFormatAlgo );
                pt1 = pt2;
            }
            rc = &pstTarget->stRect;
            IMP_DrawRect(pVBuf,rc,pstRectCr, enFormatSrc,enFormatAlgo);
        }
    }
 }
 #endif
}

IMP_VOID IMP_DrawVfdResult(VIDEO_FRAME_S *pVBuf, VFD_RESULT_S *pVfdResult,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo)
{
    IMP_RECT_S rc;
    PIXEL_S *pstRectCr = &cr_tbl[0];
    PIXEL_S *pstLineCr = &cr_tbl[1];
    IMP_S32 i = 0;
    int nLeftTopX, nLeftTopY, nRightBottomX, nRightBottomY;
    FACE_ITEM_S *pFaceInfo = &pVfdResult->stFaceSet.astFaces[0];
    int nFaces = pVfdResult->stFaceSet.s32FaceNum;
    //printf("nFaces = %d\n",nFaces);
    //printf("MY nFaces = %d\n",nFaces);
    for( i = 0;i < nFaces;i++)
    {
        int dSize  = pFaceInfo[i].faceSize  * (100 + 20) / 100;   // 20%
        int dUpperH = dSize * (50 + 40) / 100;     // 80%
		int dLowerH = dSize;     // 100%
		int dHeight = dUpperH + dLowerH; // Upper + Lower

        int nLimitWidth = 352;
        int nLimitHeight = 288;
		int dWidth  = dHeight * 3 / 4;    // width:height   12:16
		int dwCenterX = pFaceInfo[i].centerX;
		int dwCenterY = pFaceInfo[i].centerY;
		/* Left */
		nLeftTopX  = dwCenterX - dWidth / 2;

		if (nLeftTopX < 0)
			nLeftTopX = 0;
		if (nLeftTopX >= nLimitWidth)
			nLeftTopX = nLimitWidth-1;
		/* Right */
		nRightBottomX = dwCenterX + dWidth / 2;

		if ( nRightBottomX >= nLimitWidth )
			nRightBottomX = nLimitWidth-1;
		if (nRightBottomX < 0)
			nRightBottomX = 0;
		/* Top */
		nLeftTopY   = dwCenterY - dUpperH;
		if (nLeftTopY < 0)
			nLeftTopY = 0;
		if (nLeftTopY > nLimitHeight)
			nLeftTopY = nLimitHeight-1;
		/* Bottom */
		nRightBottomY= dwCenterY + dLowerH;
		if ( nRightBottomY >= nLimitHeight )
			nRightBottomY = nLimitHeight-1;
		if (nRightBottomY < 0)
			nRightBottomY = 0;

		if( nRightBottomX - nLeftTopX <= 0 )
			continue;

		if( nRightBottomY - nLeftTopY <= 0 )
			continue;

        rc.s16X1 = nLeftTopX;
        rc.s16Y1 = nLeftTopY;
        rc.s16X2 = nRightBottomX;
        rc.s16Y2 = nRightBottomY;
        //printf("MY ++++LeftTop=(%d,%d), RightBottom=(%d,%d)\n", nLeftTopX, nLeftTopY,nRightBottomX, nRightBottomY);
        IMP_DrawRect(pVBuf,&rc,pstRectCr, enFormatSrc,enFormatAlgo);
    }
}
IMP_VOID IMP_DrawRect( VIDEO_FRAME_S *pVBuf, IMP_RECT_S *rc,PIXEL_S *cr ,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo)
{
    IMP_U32 w, h;
    IMP_S32 xs, xe, ys, ye, x, y;
    IMP_S32 index = 0;
    char * pVBufVirt_Y;
    char * pVBufVirt_C;
    char * pMemContent;
    char * dataY1;
    char * dataY2;
    char * dataU1;
    char * dataU2;
    char * dataV1;
    char * dataV2;

    char * dataY3;
    char * dataY4;
    char * dataU3;
    char * dataU4;
    char * dataV3;
    char * dataV4;

    unsigned char TmpBuff[1024];
    HI_U32 phy_addr,size;
	HI_CHAR *pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    HI_U32 u32UvHeight;
    int step = 1;


    if (enFormatAlgo == IMP_QCIF)
	{
		if(rc->s16X1 < 0 || rc->s16X1 > 176 || rc->s16Y1 < 0 || rc->s16Y1 > 144 || rc->s16X2 < 0 || rc->s16X2 > 176 || rc->s16Y2 < 0 || rc->s16Y2 > 144)
		{
			return;
		}
    }
	else if (enFormatAlgo == IMP_CIF)
	{
		if(rc->s16X1 < 0 || rc->s16X1 > 352 || rc->s16Y1 < 0 || rc->s16Y1 > 288 || rc->s16X2 < 0 || rc->s16X2 > 352 || rc->s16Y2 < 0 || rc->s16Y2 > 288)
		{
			return;
		}
	}
	else if (enFormatAlgo == IMP_D1)
	{
		if(rc->s16X1 < 0 || rc->s16X1 > 704 || rc->s16Y1 < 0 || rc->s16Y1 > 576 || rc->s16X2 < 0 || rc->s16X2 > 704 || rc->s16Y2 < 0 || rc->s16Y2 > 576)
		{
			return;
		}
	}

    if( pVBuf->u32Height == 120 || pVBuf->u32Height == 240 || pVBuf->u32Height == 480 )
    {
		if (enFormatAlgo == IMP_QCIF)
        {
			if(rc->s16Y1 == 120)
			{
				rc->s16Y1 = 119;
			}
			if(rc->s16Y2 == 120)
			{
				rc->s16Y2 = 119;
			}
			if(rc->s16X1 < 0 || rc->s16X1 > 176 || rc->s16Y1 < 0 || rc->s16Y1 > 119 || rc->s16X2 < 0 || rc->s16X2 > 176 || rc->s16Y2 < 0 || rc->s16Y2 > 119)
			{

				return;
			}
        }
		else if (enFormatAlgo == IMP_CIF)
        {
			if(rc->s16Y1 == 240)
			{
				rc->s16Y1 = 239;
			}
			if(rc->s16Y2 == 240)
			{
				rc->s16Y2 = 239;
			}
			if(rc->s16X1 < 0 || rc->s16X1 > 352 || rc->s16Y1 < 0 || rc->s16Y1 > 239 || rc->s16X2 < 0 || rc->s16X2 > 352 || rc->s16Y2 < 0 || rc->s16Y2 > 239)
			{
				return;
			}
        }
		else if (enFormatAlgo == IMP_D1)
        {
			if(rc->s16Y1 == 480)
			{
				rc->s16Y1 = 479;
			}
			if(rc->s16Y2 == 480)
			{
				rc->s16Y2 = 479;
			}
			if(rc->s16X1 < 0 || rc->s16X1 > 704 || rc->s16Y1 < 0 || rc->s16Y1 > 479 || rc->s16X2 < 0 || rc->s16X2 > 704 || rc->s16Y2 < 0 || rc->s16Y2 > 479)
			{
				return;
			}
        }
    }

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
        u32UvHeight = pVBuf->u32Height/2;
        step = 2;
    }
    else
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u32PhyAddr[0];

    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }

	pVBufVirt_Y = pUserPageAddr[0];
	pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

    if(enFormatSrc == enFormatAlgo)
    {
        xs = rc->s16X1; ys = rc->s16Y1;
        xe = rc->s16X2; ye = rc->s16Y2;
    }
    else if(enFormatSrc == IMP_D1 && enFormatAlgo == IMP_QCIF)
    {
        xs = rc->s16X1 * 4; ys = rc->s16Y1 * 4;
        xe = rc->s16X2 * 4; ye = rc->s16Y2 * 4;
    }
    else if((enFormatSrc == IMP_D1 && enFormatAlgo == IMP_CIF) || (enFormatSrc == IMP_CIF && enFormatAlgo == IMP_QCIF))
    {
        xs = rc->s16X1 * 2; ys = rc->s16Y1 * 2;
        xe = rc->s16X2 * 2; ye = rc->s16Y2 * 2;
    }

    dataY1 = pVBufVirt_Y + ys*pVBuf->u32Stride[0];
    dataY2 = pVBufVirt_Y + ye*pVBuf->u32Stride[0];
    dataY3 = pVBufVirt_Y + (ys+1)*pVBuf->u32Stride[0];
    dataY4 = pVBufVirt_Y + (ye-1)*pVBuf->u32Stride[0];

    dataU1 = pVBufVirt_C + (ys/step )*pVBuf->u32Stride[1]+1;
    dataU2 = pVBufVirt_C + (ye/step )*pVBuf->u32Stride[1]+1;
    dataU3 = pVBufVirt_C + (ys/step  + 1)*pVBuf->u32Stride[1]+1;
    dataU4 = pVBufVirt_C + (ye/step  - 1)*pVBuf->u32Stride[1]+1;

    dataV1 = pVBufVirt_C + (ys/step )*pVBuf->u32Stride[1];
    dataV2 = pVBufVirt_C + (ye/step )*pVBuf->u32Stride[1];
    dataV3 = pVBufVirt_C + (ys/step  + 1)*pVBuf->u32Stride[1];
    dataV4 = pVBufVirt_C + (ye/step  - 1)*pVBuf->u32Stride[1];

    for( x=xs; x<=xe; x++ )
	{
		dataY1[x] = cr->u8B;
		dataY2[x] = cr->u8B;
		dataY3[x] = cr->u8B;
		dataY4[x] = cr->u8B;

	}
    for( x=xs; x<=xe; x+=2 )
    {

        dataU1[x] = cr->u8G;
		dataU2[x] = cr->u8G;
        dataU3[x] = cr->u8G;
		dataU4[x] = cr->u8G;

        dataV1[x] = cr->u8R;
		dataV2[x] = cr->u8R;
        dataV3[x] = cr->u8R;
		dataV4[x] = cr->u8R;
	}

    dataY1 = pVBufVirt_Y + ys*pVBuf->u32Stride[0] + xs;
    dataY2 = pVBufVirt_Y + ys*pVBuf->u32Stride[0] +  xe;

    dataU1 = pVBufVirt_C + (ys/step )*pVBuf->u32Stride[1] + xs+1;
    dataU2 = pVBufVirt_C + (ys/step )*pVBuf->u32Stride[1] +  xe+1;

    dataV1 = pVBufVirt_C + (ys/step )*pVBuf->u32Stride[1] + xs;
    dataV2 = pVBufVirt_C + (ys/step )*pVBuf->u32Stride[1] +  xe;

	for( y=ys; y<=ye; y++ )
	{
		dataY1[0] = cr->u8B;
		dataY2[0] = cr->u8B;

		dataY1 +=pVBuf->u32Stride[0];
		dataY2 +=pVBuf->u32Stride[0];
	}
    for( y=ys; y<=ye; y+=step )
	{
        dataU1[0] = cr->u8G;
		dataU2[0] = cr->u8G;

        dataV1[0] = cr->u8R;
		dataV2[0] = cr->u8R;


        dataU1 +=pVBuf->u32Stride[1];
		dataU2 +=pVBuf->u32Stride[1];

        dataV1 +=pVBuf->u32Stride[1];
		dataV2 +=pVBuf->u32Stride[1];
	}

	HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
}
IMP_VOID IMP_DrawLine( VIDEO_FRAME_S *pVBuf, IMP_POINT_S *pt1, IMP_POINT_S *pt2,PIXEL_S *cr ,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo)
{
    IMP_U32 w, h;
    IMP_S32 xs = pt1->s16X;
    IMP_S32 ys = pt1->s16Y;
	IMP_S32 xe = pt2->s16X;
	IMP_S32 ye = pt2->s16Y;
    IMP_S32 index = 0;
    char * pVBufVirt_Y;
    char * pVBufVirt_C;
    char * pMemContent;
    char * dataY1;
    char * dataY2;
    char * dataU1;
    char * dataU2;
    char * dataV1;
    char * dataV2;
    unsigned char TmpBuff[1024];
    HI_U32 phy_addr,size;
	HI_CHAR *pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    HI_U32 u32UvHeight;
    int step = 1;
    if (enFormatAlgo == IMP_QCIF)
	{
		if(pt1->s16X < 0 || pt1->s16X > 176 || pt1->s16Y < 0 || pt1->s16Y > 144 || pt2->s16X < 0 || pt2->s16X > 176 || pt2->s16Y < 0 || pt2->s16Y > 144)
		{
			return;
		}
    }
	else if (enFormatAlgo == IMP_CIF)
	{
		if(pt1->s16X < 0 || pt1->s16X > 352 || pt1->s16Y < 0 || pt1->s16Y > 288 || pt2->s16X < 0 || pt2->s16X > 352 || pt2->s16Y < 0 || pt2->s16Y > 288)
		{
			return;
		}
	}
	else if (enFormatAlgo == IMP_D1)
	{
		if(pt1->s16X < 0 || pt1->s16X > 704 || pt1->s16Y < 0 || pt1->s16Y > 576 || pt2->s16X < 0 || pt2->s16X > 704 || pt2->s16Y < 0 || pt2->s16Y > 576)
		{
			return;
		}
	}

    if( pVBuf->u32Height == 120 || pVBuf->u32Height == 240 || pVBuf->u32Height == 480 )
    {
        if (enFormatAlgo == IMP_QCIF)
		{
			if(ys == 120)
			{
				ys = 119;
			}
			if(ye == 120)
			{
				ye = 119;
			}
			if(xs < 0 || xs > 175 || ys < 0 || ys > 119 || xe < 0 || xe > 175 || ye < 0 || ye > 119)
			{
				return;
			}
		}
		else if (enFormatAlgo == IMP_CIF)
		{
			if(ys == 240)
			{
				ys = 239;
			}
			if(ye == 240)
			{
				ye = 239;
			}
			if(xs < 0 || xs > 351 || ys < 0 || ys > 239 || xe < 0 || xe > 351 || ye < 0 || ye > 239)
			{
				return;
			}
		}
        else if (enFormatAlgo == IMP_D1)
		{
			if(ys == 480)
			{
				ys = 479;
			}
			if(ye == 480)
			{
				ye = 479;
			}
			if(xs < 0 || xs > 703 || ys < 0 || ys > 479 || xe < 0 || xe > 703 || ye < 0 || ye > 479	)
			{
				return;
			}
		}
    }
    else
    {
        if (enFormatAlgo == IMP_QCIF)
		{
			if(xs < 0 || xs > 175 || ys < 0 || ys > 143 || xe < 0 || xe > 175 || ye < 0 || ye > 143)
			{
				return;
			}
		}
		else if (enFormatAlgo == IMP_CIF)
		{
			if(xs < 0 || xs > 351 || ys < 0 || ys > 287 || xe < 0 || xe > 351 || ye < 0 || ye > 287)
			{
				return;
			}
		}
		else if (enFormatAlgo == IMP_D1)
		{
			if(xs < 0 || xs > 703 || ys < 0 || ys > 575 || xe < 0 || xe > 703 || ye < 0 || ye > 575)
			{
				return;
			}
		}
    }

	IMP_S32 x1 = xs;
	IMP_S32 y1 = ys;
	IMP_S32 x = x1;                       // Start x off at the first pixel
	IMP_S32 y = y1;                       // Start y off at the first pixel
	IMP_S32 x2 = xe;
	IMP_S32 y2 = ye;

	IMP_S32 xinc1,xinc2,yinc1,yinc2;      // Increasing values
	IMP_S32 den, num, numadd,numpixels;
	IMP_S32 deltax = abs(x2 - x1);        // The difference between the x's
	IMP_S32 deltay = abs(y2 - y1);        // The difference between the y's
    IMP_S32 curpixel = 0;
    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
        u32UvHeight = pVBuf->u32Height/2;
        step = 2;
    }
    else
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u32PhyAddr[0];

    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }

    if(enFormatSrc == enFormatAlgo)
    {
        xs = pt1->s16X;
        ys = pt1->s16Y;
        xe = pt2->s16X;
        ye = pt2->s16Y;
    }
    else if(enFormatSrc == IMP_D1 && enFormatAlgo == IMP_QCIF)
    {
        xs = pt1->s16X * 4;
        ys = pt1->s16Y * 4;
        xe = pt2->s16X * 4;
        ye = pt2->s16Y * 4;
    }
    else if((enFormatSrc == IMP_D1 && enFormatAlgo == IMP_CIF) || (enFormatSrc == IMP_CIF && enFormatAlgo == IMP_QCIF))
    {
        xs = pt1->s16X * 2;
        ys = pt1->s16Y * 2;
        xe = pt2->s16X * 2;
        ye = pt2->s16Y * 2;
    }
    x1 = xs;
    y1 = ys;
    x = x1;                       // Start x off at the first pixel
    y = y1;                       // Start y off at the first pixel
    x2 = xe;
    y2 = ye;
    deltax = abs(x2 - x1);        // The difference between the x's
    deltay = abs(y2 - y1);

	pVBufVirt_Y = pUserPageAddr[0];
	pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

    // Get Increasing Values
	if (x2 >= x1)
	{                         // The x-values are increasing
		xinc1 = 1;
		xinc2 = 1;
	}
	else
	{                         // The x-values are decreasing
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)
	{                         // The y-values are increasing
		yinc1 = 1;
		yinc2 = 1;
	}
	else
	{                         // The y-values are decreasing
		yinc1 = -1;
		yinc2 = -1;
	}

	// Actually draw the line
	if (deltax >= deltay)         // There is at least one x-value for every y-value
	{
		xinc1 = 0;                  // Don't change the x when numerator >= denominator
		yinc2 = 0;                  // Don't change the y for every iteration
		den = deltax;
		num = deltax >> 1;
		numadd = deltay;
		numpixels = deltax;         // There are more x-values than y-values
	}
	else                          // There is at least one y-value for every x-value
	{
		xinc2 = 0;                  // Don't change the x for every iteration
		yinc1 = 0;                  // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay >> 1;
		numadd = deltax;
		numpixels = deltay;         // There are more y-values than x-values
	}

	{
	    for ( curpixel = 0; curpixel <= numpixels; curpixel++)
        {
            // Draw the current pixel
            pVBufVirt_Y[y*pVBuf->u32Stride[0]+x] = cr->u8B;
            pVBufVirt_Y[(y+1)*pVBuf->u32Stride[0]+x] = cr->u8B;

            if (x%2)
            {
                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+ x + 1] = cr->u8G;
                pVBufVirt_C[((y/step) + 1)*pVBuf->u32Stride[1]+ x + 1] = cr->u8G;

                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+x] = cr->u8R;
                pVBufVirt_C[((y/step) + 1)*pVBuf->u32Stride[1]+x] = cr->u8R;
            }
            else
            {
                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+x] = cr->u8G;
                pVBufVirt_C[((y/step) +1)*pVBuf->u32Stride[1]+x] = cr->u8G;

                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+(x+1)] = cr->u8R;
                pVBufVirt_C[((y/step)+1)*pVBuf->u32Stride[1]+(x+1)] = cr->u8R;
            }
            num += numadd;              // Increase the numerator by the top of the fraction
            if (num >= den)             // Check if numerator >= denominator
            {
                num -= den;               // Calculate the new numerator value
                x += xinc1;               // Change the x as appropriate
                y += yinc1;               // Change the y as appropriate
            }
            x += xinc2;                 // Change the x as appropriate
            y += yinc2;                 // Change the y as appropriate
        }
	}

	HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
}

IMP_VOID IMP_DrawLine2( VIDEO_FRAME_S *pVBuf, IMP_POINT_S *pt1, IMP_POINT_S *pt2,PIXEL_S *cr ,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo)
{
    IMP_U32 w, h;
    IMP_S32 xs = pt1->s16X;
    IMP_S32 ys = pt1->s16Y;
	IMP_S32 xe = pt2->s16X;
	IMP_S32 ye = pt2->s16Y;
    IMP_S32 index = 0;
    char * pVBufVirt_Y;
    char * pVBufVirt_C;
    char * pMemContent;
    char * dataY1;
    char * dataY2;
    char * dataU1;
    char * dataU2;
    char * dataV1;
    char * dataV2;
    unsigned char TmpBuff[1024];
    HI_U32 phy_addr,size;
	HI_CHAR *pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    HI_U32 u32UvHeight;
    int step = 1;
    if (enFormatAlgo == IMP_QCIF)
	{
		if(pt1->s16X < 0 || pt1->s16X > 176 || pt1->s16Y < 0 || pt1->s16Y > 144 || pt2->s16X < 0 || pt2->s16X > 176 || pt2->s16Y < 0 || pt2->s16Y > 144)
		{
			return;
		}
    }
	else if (enFormatAlgo == IMP_CIF)
	{
		if(pt1->s16X < 0 || pt1->s16X > 352 || pt1->s16Y < 0 || pt1->s16Y > 288 || pt2->s16X < 0 || pt2->s16X > 352 || pt2->s16Y < 0 || pt2->s16Y > 288)
		{
			return;
		}
	}
	else if (enFormatAlgo == IMP_D1)
	{
		if(pt1->s16X < 0 || pt1->s16X > 704 || pt1->s16Y < 0 || pt1->s16Y > 576 || pt2->s16X < 0 || pt2->s16X > 704 || pt2->s16Y < 0 || pt2->s16Y > 576)
		{
			return;
		}
	}

    if( pVBuf->u32Height == 120 || pVBuf->u32Height == 240 || pVBuf->u32Height == 480 )
    {
        if (enFormatAlgo == IMP_QCIF)
		{
			if(ys == 120)
			{
				ys = 119;
			}
			if(ye == 120)
			{
				ye = 119;
			}
			if(xs < 0 || xs > 175 || ys < 0 || ys > 119 || xe < 0 || xe > 175 || ye < 0 || ye > 119)
			{
				return;
			}
		}
		else if (enFormatAlgo == IMP_CIF)
		{
			if(ys == 240)
			{
				ys = 239;
			}
			if(ye == 240)
			{
				ye = 239;
			}
			if(xs < 0 || xs > 351 || ys < 0 || ys > 239 || xe < 0 || xe > 351 || ye < 0 || ye > 239)
			{
				return;
			}
		}
        else if (enFormatAlgo == IMP_D1)
		{
			if(ys == 480)
			{
				ys = 479;
			}
			if(ye == 480)
			{
				ye = 479;
			}
			if(xs < 0 || xs > 703 || ys < 0 || ys > 479 || xe < 0 || xe > 703 || ye < 0 || ye > 479	)
			{
				return;
			}
		}
    }
    else
    {
        if (enFormatAlgo == IMP_QCIF)
		{
			if(xs < 0 || xs > 175 || ys < 0 || ys > 143 || xe < 0 || xe > 175 || ye < 0 || ye > 143)
			{
				return;
			}
		}
		else if (enFormatAlgo == IMP_CIF)
		{
			if(xs < 0 || xs > 351 || ys < 0 || ys > 287 || xe < 0 || xe > 351 || ye < 0 || ye > 287)
			{
				return;
			}
		}
		else if (enFormatAlgo == IMP_D1)
		{
			if(xs < 0 || xs > 703 || ys < 0 || ys > 575 || xe < 0 || xe > 703 || ye < 0 || ye > 575)
			{
				return;
			}
		}
    }

	IMP_S32 x1 = xs;
	IMP_S32 y1 = ys;
	IMP_S32 x = x1;                       // Start x off at the first pixel
	IMP_S32 y = y1;                       // Start y off at the first pixel
	IMP_S32 x2 = xe;
	IMP_S32 y2 = ye;

	IMP_S32 xinc1,xinc2,yinc1,yinc2;      // Increasing values
	IMP_S32 den, num, numadd,numpixels;
	IMP_S32 deltax = abs(x2 - x1);        // The difference between the x's
	IMP_S32 deltay = abs(y2 - y1);        // The difference between the y's
    IMP_S32 curpixel = 0;
    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
        u32UvHeight = pVBuf->u32Height/2;
        step = 2;
    }
    else
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u32PhyAddr[0];

    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }

    if(enFormatSrc == enFormatAlgo)
    {
        xs = pt1->s16X;
        ys = pt1->s16Y;
        xe = pt2->s16X;
        ye = pt2->s16Y;
    }
    else if(enFormatSrc == IMP_D1 && enFormatAlgo == IMP_QCIF)
    {
        xs = (IMP_S32)(pt1->s16X * 4 );
        ys = (IMP_S32)(pt1->s16Y * 4);
        xe = (IMP_S32)(pt2->s16X * 4);
        ye = (IMP_S32)(pt2->s16Y * 4);
    }
	else if(enFormatSrc == IMP_HD1 && enFormatAlgo == IMP_QCIF)
    {
        xs = (IMP_S32)(pt1->s16X * 4.0909);
        ys = (IMP_S32)(pt1->s16Y * 4.0909);
        xe = (IMP_S32)(pt2->s16X * 4.0909);
        ye = (IMP_S32)(pt2->s16Y * 4.0909);
    }

    else if((enFormatSrc == IMP_D1 && enFormatAlgo == IMP_CIF) || (enFormatSrc == IMP_CIF && enFormatAlgo == IMP_QCIF))
    {
        xs = pt1->s16X * 2;
        ys = pt1->s16Y * 2;
        xe = pt2->s16X * 2;
        ye = pt2->s16Y * 2;
    }
    x1 = xs;
    y1 = ys;
    x = x1;                       // Start x off at the first pixel
    y = y1;                       // Start y off at the first pixel
    x2 = xe;
    y2 = ye;
    deltax = abs(x2 - x1);        // The difference between the x's
    deltay = abs(y2 - y1);

	pVBufVirt_Y = pUserPageAddr[0];
	pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

    // Get Increasing Values
	if (x2 >= x1)
	{                         // The x-values are increasing
		xinc1 = 1;
		xinc2 = 1;
	}
	else
	{                         // The x-values are decreasing
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)
	{                         // The y-values are increasing
		yinc1 = 1;
		yinc2 = 1;
	}
	else
	{                         // The y-values are decreasing
		yinc1 = -1;
		yinc2 = -1;
	}

	// Actually draw the line
	if (deltax >= deltay)         // There is at least one x-value for every y-value
	{
		xinc1 = 0;                  // Don't change the x when numerator >= denominator
		yinc2 = 0;                  // Don't change the y for every iteration
		den = deltax;
		num = deltax >> 1;
		numadd = deltay;
		numpixels = deltax;         // There are more x-values than y-values
	}
	else                          // There is at least one y-value for every x-value
	{
		xinc2 = 0;                  // Don't change the x for every iteration
		yinc1 = 0;                  // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay >> 1;
		numadd = deltax;
		numpixels = deltay;         // There are more y-values than x-values
	}

	{
	    for ( curpixel = 0; curpixel <= numpixels; curpixel++)
        {
            // Draw the current pixel
            pVBufVirt_Y[y*pVBuf->u32Stride[0]+x] = cr->u8B;
            pVBufVirt_Y[(y+1)*pVBuf->u32Stride[0]+x] = cr->u8B;

            if (x%2)
            {
                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+ x + 1] = cr->u8G;
                pVBufVirt_C[((y/step) + 1)*pVBuf->u32Stride[1]+ x + 1] = cr->u8G;

                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+x] = cr->u8R;
                pVBufVirt_C[((y/step) + 1)*pVBuf->u32Stride[1]+x] = cr->u8R;
            }
            else
            {
                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+x] = cr->u8G;
                pVBufVirt_C[((y/step) +1)*pVBuf->u32Stride[1]+x] = cr->u8G;

                pVBufVirt_C[(y/step)*pVBuf->u32Stride[1]+(x+1)] = cr->u8R;
                pVBufVirt_C[((y/step)+1)*pVBuf->u32Stride[1]+(x+1)] = cr->u8R;
            }
            num += numadd;              // Increase the numerator by the top of the fraction
            if (num >= den)             // Check if numerator >= denominator
            {
                num -= den;               // Calculate the new numerator value
                x += xinc1;               // Change the x as appropriate
                y += yinc1;               // Change the y as appropriate
            }
            x += xinc2;                 // Change the x as appropriate
            y += yinc2;                 // Change the y as appropriate
        }
	}

	HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
}

#define ARROW_LENGTH 8
#define IMP_PI 3.1415926
static IMP_VOID IMP_DrawArrow( VIDEO_FRAME_S *pVBuf,IMP_POINT_S *pt_s, IMP_POINT_S *pt_e, PIXEL_S *pstLineCr)
{

	double atan = atan2((double)(pt_e->s16Y - pt_s->s16Y), (double)(pt_e->s16X - pt_s->s16X));
	double angle = atan - IMP_PI/6;

	IMP_POINT_S stPt;
	//left arrow
	stPt.s16X = pt_e->s16X - (long)(ARROW_LENGTH * cos(angle));
	stPt.s16Y = pt_e->s16Y - (long)(ARROW_LENGTH * sin(angle));

	IMP_DrawLine( pVBuf, pt_e, &stPt, pstLineCr ,IMP_D1,IMP_QCIF );

	//right arrow
	angle = atan + IMP_PI/6;
	stPt.s16X = pt_e->s16X - (long)(ARROW_LENGTH * cos(angle));
	stPt.s16Y = pt_e->s16Y - (long)(ARROW_LENGTH * sin(angle));

	IMP_DrawLine( pVBuf, pt_e, &stPt, pstLineCr ,IMP_D1,IMP_QCIF );
}

#define DRAG_RANGE	8

#define TRIPWIRE_LINE_LENGTH	30

void IMP_DrawTripwireLine( VIDEO_FRAME_S *pVBuf,IMP_POINT_S *pt_s, IMP_POINT_S *pt_e, PIXEL_S *pstLineCr )
{
	if ((pt_s->s16X == pt_e->s16X) && (pt_s->s16Y == pt_e->s16Y))
		return;

	double atan = atan2((double)(pt_e->s16Y - pt_s->s16Y), (double)(pt_e->s16X - pt_s->s16X));

	IMP_POINT_S mid;
	mid.s16X = (pt_s->s16X + pt_e->s16X)/2;
	mid.s16Y = (pt_s->s16Y + pt_e->s16Y)/2;

	IMP_POINT_S stPt;
	stPt.s16X = mid.s16X + (short)(TRIPWIRE_LINE_LENGTH * sin(atan));
	stPt.s16Y = mid.s16Y - (short)(TRIPWIRE_LINE_LENGTH * cos(atan));

	IMP_DrawLine( pVBuf, &mid, &stPt, pstLineCr ,IMP_D1,IMP_QCIF );

	//arrow
	IMP_DrawArrow(pVBuf, &mid, &stPt, pstLineCr);

	IMP_DrawLine( pVBuf, pt_s, pt_e, pstLineCr ,IMP_D1,IMP_QCIF );
}

//added by mzhang
void IMP_DrawATMOneRule(VIDEO_FRAME_S *pVBuf, PIXEL_S *pstLineCr, URP_POLYGON_REGION_S *pregion)
{
	int i;
	int s32BoundaryPtNum = 0;
	IMP_POINT_S *pt1 = 0, *pt2 = 0;

	if (pregion->s32Valid)
	{
		s32BoundaryPtNum = pregion->s32PointNum;
//		printf("s32BoundaryPtNum:%d\n", s32BoundaryPtNum);
		for (i = 0; i <  s32BoundaryPtNum -1 ;i++)
		{
			pt1 = &pregion->astPoint[i];
			pt2 = &pregion->astPoint[i+1];
	//	printf("%d, %d; %d, %d\n", pt1->s16X, pt1->s16Y, pt2->s16X, pt2->s16Y);
			IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,IMP_D1,IMP_QCIF );
	//	IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,IMP_D1,IMP_CIF );
		}
		pt1 = &pregion->astPoint[0];
		pt2 = &pregion->astPoint[s32BoundaryPtNum-1];
		IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,IMP_D1,IMP_QCIF );
	//IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,IMP_D1,IMP_CIF );
	}
}


void IMP_DrawPeaRule( VIDEO_FRAME_S *pVBuf, URP_PARA_S *pstURPpara )
{
    IMP_POINT_S *pt1, *pt2, *pt3, *pt4;
    int s32RuleIndex;
    int i;
    PIXEL_S *pstLineCr = &cr_tbl[0];
    int s32BoundaryPtNum;
 //   printf("\n");
    for( s32RuleIndex=0; s32RuleIndex<8; s32RuleIndex++ )
	{
		if( 0
			||	pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Valid == 0
			||	pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Enable == 0
		   )
			continue;

		//added by mzhang
      //  printf("%x, ", pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Mode);

        
		if ((pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Mode & IMP_URP_FUNC_OSC)
         //   || (pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Mode & IMP_URP_FUNC_OSC)
		)
		{
		 //   printf("Hello ATM or OSC\n");
			URP_OSC_SPECL_REGIONS_S *psr = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stOscRulePara.astSpclRgs[0];
			IMP_DrawATMOneRule(pVBuf, pstLineCr, &psr->stOscRg);
//printf("DRAW:stSpclRgA:%d %d\n", psr->stSpclRgA.s32Valid, psr->stSpclRgA.s32PointNum);
			IMP_DrawATMOneRule(pVBuf, pstLineCr, &psr->astSubRgA);
//printf("stSpclRgA:%d\n", psr->stSpclRgA.s32Valid);
			IMP_DrawATMOneRule(pVBuf, pstLineCr, &psr->astSubRgB);
			IMP_DrawATMOneRule(pVBuf, pstLineCr, &psr->astSubRgC);
        }        

		if (pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Mode & IMP_URP_FUNC_PERIMETER)
		{
//
			s32BoundaryPtNum = pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum;
//printf("Hello draw 2:%d\n", s32BoundaryPtNum);
			for (i = 0; i <  s32BoundaryPtNum -1 ;i++)
			{
				pt1 = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i];
				pt2 = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i+1];
		//		printf("%d, %d; %d, %d\n", pt1->s16X, pt1->s16Y, pt2->s16X, pt2->s16Y);
				IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,IMP_D1,IMP_QCIF );
			}
			pt1 = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0];
			pt2 = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[s32BoundaryPtNum-1];
			IMP_DrawLine( pVBuf, pt1, pt2, pstLineCr ,IMP_D1,IMP_QCIF );
        }

		if (pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Mode & IMP_URP_FUNC_TRIPWIRE)
		{
//printf("Hello draw 3\n");
			for (i = 0; i < IMP_URP_MAX_TRIPWIRE_CNT;i++)
			{
				if(pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stTripwireRulePara.astLines[i].s32Valid)
				{
				    pt1 = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt;
                    pt2 = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt;

                    IMP_DrawTripwireLine( pVBuf, pt1, pt2, pstLineCr  );

                    if (pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stTripwireRulePara.astLines[i].s32IsDoubleDirection)
                    {
                        IMP_DrawTripwireLine( pVBuf, pt2 ,pt1, pstLineCr );
                    }
				}
			}
		}
	}
}


IMP_VOID IMP_TDE_DrawPeaResult( TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstYSurface,TDE2_SURFACE_S* pstUVSurface, RESULT_S *result,VIDEO_FORMAT_E enFormatSrc,VIDEO_FORMAT_E enFormatAlgo )
{
    TGT_SET_S *pstTargetSet = &result->stTargetSet;
    EVT_SET_S *pstEventSet = &result->stEventSet;
    IMP_U32 u32TargetNum = pstTargetSet->s32TargetNum;

    TGT_ITEM_S *pstTarget;
    IMP_RECT_S *rc;
    IMP_RECT_S rc1;
    PIXEL_S *pstCr = &cr_tbl[1];

    IMP_FLOAT fYScaleX;
    IMP_FLOAT fYScaleY;
    IMP_FLOAT fUVScaleX;
    IMP_FLOAT fUVScaleY;

    IMP_U32 i = 0;
    IMP_U32 j = 0;
    IMP_U32 u32PixWidth = 2;

    IMP_U32 s32Ret = 0;

    TDE2_RECT_S pstDstRect;
    TDE2_OPT_S pstOpt;
    TDE2_FILLCOLOR_S stYFillColor;
    TDE2_FILLCOLOR_S stUVFillColor;

    stYFillColor.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stUVFillColor.enColorFmt = TDE2_COLOR_FMT_ARGB1555;

    if(enFormatSrc == enFormatAlgo)
    {
        fYScaleX = 0.5;
        fYScaleY = 1.0;
        fUVScaleX = 0.5;
        fUVScaleY = 0.5;
    }
    else if(enFormatSrc == IMP_D1 && enFormatAlgo == IMP_QCIF)
    {
        fYScaleX = 4*0.5;
        fYScaleY = 4*1.0;
        fUVScaleX = 4*0.5;
        fUVScaleY = 4*0.5;
    }
    else if((enFormatSrc == IMP_D1 && enFormatAlgo == IMP_CIF) || (enFormatSrc == IMP_CIF && enFormatAlgo == IMP_QCIF))
    {
        fYScaleX = 2*0.5;
        fYScaleY = 2*1.0;
        fUVScaleX = 2*0.5;
        fUVScaleY = 2*0.5;
    }

    memset(&pstOpt,0,sizeof(pstOpt));
    pstOpt.bDeflicker = HI_TRUE;

#if 1
    pstDstRect.s32Xpos =0;
    pstDstRect.s32Ypos =0;
    pstDstRect.u32Width = 2;
    pstDstRect.u32Height = 2;
    stYFillColor.u32FillColor = 0;
    HI_TDE2_SolidDraw( s32Handle,NULL,NULL,pstYSurface,&pstDstRect,&stYFillColor,&pstOpt);
#endif
    for(i=0;i<pstEventSet->s32EventNum;i++)
    {
        if(pstEventSet->astEvents[i].u32Status==IMP_EVT_STATUS_START || pstEventSet->astEvents[i].u32Status== IMP_EVT_STATUS_PROCEDURE)
        {
            if(pstEventSet->astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {
                pstCr = &cr_tbl[0];
            }
            if(pstEventSet->astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                pstCr = &cr_tbl[0];
            }
        }
    }
    for( i = 0;i < u32TargetNum;i++)
    {
        pstTarget = &pstTargetSet->astTargets[i];
        {
            rc = &pstTarget->stRect;
            if(pstTarget->u32Event&IMP_TGT_EVENT_PERIMETER || pstTarget->u32Event&IMP_TGT_EVENT_TRIPWIRE)
            {
                rc1.s16X1 = (short)((rc->s16X1)*fYScaleX);
                rc1.s16Y1 = (short)((rc->s16Y1)*fYScaleY);
                rc1.s16X2 = (short)((rc->s16X2)*fYScaleX);
                rc1.s16Y2 = (short)((rc->s16Y2)*fYScaleY);

                for( j = 0; j < 4;j++)
                {
                    if(j == 0)
                    {
                        pstDstRect.s32Xpos = rc1.s16X1;
                        pstDstRect.s32Ypos =rc1.s16Y1;
                        pstDstRect.u32Width = u32PixWidth;
                        pstDstRect.u32Height = rc1.s16Y2 - rc1.s16Y1;
                    }
                    else if(j == 1)
                    {
                        pstDstRect.s32Xpos = rc1.s16X1;
                        pstDstRect.s32Ypos =rc1.s16Y1;
                        pstDstRect.u32Width = rc1.s16X2 - rc1.s16X1;
                        pstDstRect.u32Height = u32PixWidth;
                    }
                    else if(j == 2)
                    {
                        pstDstRect.s32Xpos = rc1.s16X1;
                        pstDstRect.s32Ypos =rc1.s16Y2;
                        pstDstRect.u32Width = rc1.s16X2 - rc1.s16X1;
                        pstDstRect.u32Height = u32PixWidth;
                    }
                    else if(j == 3)
                    {
                        pstDstRect.s32Xpos = rc1.s16X2;
                        pstDstRect.s32Ypos =rc1.s16Y1;
                        pstDstRect.u32Width = u32PixWidth;
                        pstDstRect.u32Height = rc1.s16Y2 - rc1.s16Y1+u32PixWidth;
                    }
                    stYFillColor.u32FillColor = pstCr->u8B;
                    HI_TDE2_SolidDraw( s32Handle,NULL,NULL,pstYSurface,&pstDstRect,&stYFillColor,&pstOpt);
                }

                rc1.s16X1 = (short)((rc->s16X1)*fUVScaleX);
                rc1.s16Y1 = (short)((rc->s16Y1)*fUVScaleY);
                rc1.s16X2 = (short)((rc->s16X2)*fUVScaleX);
                rc1.s16Y2 = (short)((rc->s16Y2)*fUVScaleY);

                for( j = 0; j < 4;j++)
                {
                    if(j == 0)
                    {
                        pstDstRect.s32Xpos = rc1.s16X1;
                        pstDstRect.s32Ypos =rc1.s16Y1;
                        pstDstRect.u32Width = u32PixWidth;
                        pstDstRect.u32Height = rc1.s16Y2 - rc1.s16Y1;
                    }
                    else if(j == 1)
                    {
                        pstDstRect.s32Xpos = rc1.s16X1;
                        pstDstRect.s32Ypos =rc1.s16Y1;
                        pstDstRect.u32Width = rc1.s16X2 - rc1.s16X1;
                        pstDstRect.u32Height = u32PixWidth;
                    }
                    else if(j == 2)
                    {
                        pstDstRect.s32Xpos = rc1.s16X1;
                        pstDstRect.s32Ypos =rc1.s16Y2;
                        pstDstRect.u32Width = rc1.s16X2 - rc1.s16X1;
                        pstDstRect.u32Height = u32PixWidth;
                    }
                    else if(j == 3)
                    {
                        pstDstRect.s32Xpos = rc1.s16X2;
                        pstDstRect.s32Ypos =rc1.s16Y1;
                        pstDstRect.u32Width = u32PixWidth;
                        pstDstRect.u32Height = rc1.s16Y2 - rc1.s16Y1+u32PixWidth;
                    }
                    stUVFillColor.u32FillColor = pstCr->u8R;
                    HI_TDE2_SolidDraw( s32Handle,NULL,NULL,pstUVSurface,&pstDstRect,&stUVFillColor,&pstOpt);
                }
            }
        }
    }
}
