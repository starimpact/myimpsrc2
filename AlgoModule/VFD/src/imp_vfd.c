
#include "imp_vfd.h"
#include "imp_vfd_haar.h"


#ifdef IMP_VFD_HAAR
static char * haarClassifierName = "haar_classifier.dat";
#endif


IMP_HANDLE IMP_VFD_Create(IMP_S32 s32ImgW, IMP_S32 s32ImgH)
{
#ifdef IMP_VFD_HAAR
	VFD_HAAR_MODULE *hVfd  = (VFD_HAAR_MODULE*)malloc(sizeof(VFD_HAAR_MODULE));
	memset(hVfd,0,sizeof(VFD_HAAR_MODULE));
	hVfd->s32ImgH = s32ImgH;
	hVfd->s32ImgW = s32ImgW;
	hVfd->storage  = IMP_CreateMemStorage(0);

	hVfd->Cascade = IMP_CreateHaar(hVfd, hVfd->storage, haarClassifierName);

    if (NULL == hVfd->Cascade)
	{
		printf("VFD initialize error!\n");
		return NULL;
	}

    memset( &hVfd->m_stVfdPara,0,sizeof(IMP_VFD_PARA_S));

    memset( &hVfd->m_stVfdResult,0,sizeof(VFD_RESULT_S));
#else
    IMP_S32 nResult;
	VFD_MODULE *hVfd  = (VFD_MODULE*)malloc(sizeof(VFD_MODULE));
	memset(hVfd,0,sizeof(VFD_MODULE));


    nResult = Initialize(&hVfd->m_pFaceInstance);

    if (nResult != 0)
	{
	    printf("VFD initialize error!\n");
	    return NULL;
	}

    memset( &hVfd->m_stVfdPara,0,sizeof(IMP_VFD_PARA_S));

    memset( &hVfd->m_stVfdResult,0,sizeof(VFD_RESULT_S));
#endif
    return (IMP_HANDLE)hVfd;
}

IMP_VOID IMP_VFD_Release(IMP_HANDLE hModule)
{
#ifdef IMP_VFD_HAAR

	VFD_HAAR_MODULE *hVfd = (VFD_HAAR_MODULE *)hModule;
	IMP_ReleaseMemStorage( &hVfd->storage );

	IMP_ReleaseHaar(hVfd, &hVfd->Cascade);

	free(hVfd);
#else
	VFD_MODULE *hVfd = (VFD_MODULE *)hModule;
	Uninitialize(&hVfd->m_pFaceInstance);
	free(hVfd);
#endif
}

IMP_BOOL IMP_VFD_ConfigParameter(IMP_HANDLE hModule, IMP_VFD_PARA_S *pstVfdParaSrc)
{
#ifdef IMP_VFD_HAAR
	VFD_HAAR_MODULE *hVfd = (VFD_HAAR_MODULE*)hModule;

    if( pstVfdParaSrc == NULL || hVfd == NULL)
        return IMP_FALSE;

    memcpy( &hVfd->m_stVfdPara ,pstVfdParaSrc,sizeof(IMP_VFD_PARA_S ));
#if defined(IMP_DEBUG_PRINT)
    printf("IMP_VFD_ConfigParameter ok\n");
#endif
    return IMP_TRUE;
#else
	VFD_MODULE *hVfd = (VFD_MODULE*)hModule;

    if( pstVfdParaSrc == NULL || hVfd == NULL)
        return IMP_FALSE;

    memcpy( &hVfd->m_stVfdPara ,pstVfdParaSrc,sizeof(IMP_VFD_PARA_S ));
#if defined(IMP_DEBUG_PRINT)
   printf("IMP_VFD_ConfigParameter ok\n");
#endif
    return IMP_TRUE;
#endif
}

static void	VFD_FaceCaptureAnalysis(IMP_HANDLE hModule);
static void	VFD_FaceCamouflageAnalysis(IMP_HANDLE hModule);
static void	VFD_PasswordPeeprAnalysis(IMP_HANDLE hModule);


IMP_BOOL IMP_VFD_Process(IMP_HANDLE hModule, GRAY_IMAGE_S *pstImage)
{
#ifdef IMP_VFD_HAAR
	VFD_RESULT_S *pstVfdResult;
	VFD_HAAR_MODULE *hVfd = (VFD_HAAR_MODULE *)hModule;
	IMP_Seq* faces;
	int i = 0;
	Imp_Size min_size;
	min_size.height = 0;
	min_size.width = 0;
	pstVfdResult = &hVfd->m_stVfdResult;
    /* use the fastest variant */
	faces = IMP_HaarDetectObjects( hModule, pstImage, hVfd->Cascade, hVfd->storage, 1.2, 2, IMP_HAAR_DO_CANNY_PRUNING,min_size );
    /* draw all the rectangles */
	pstVfdResult->stFaceSet.s32FaceNum = faces->total;
    for( i = 0; i < faces->total; i++ )
    {
		Imp_Rect face_rect;
        /* extract the rectanlges only */
		if (i >= 50)
		{
			break;
		}
        face_rect = *(Imp_Rect*)IMP_GetSeqElem( faces, i );
        pstVfdResult->stFaceSet.astFaces[i].centerX = face_rect.x + face_rect.width/2;
		pstVfdResult->stFaceSet.astFaces[i].centerY = face_rect.y + face_rect.height/2;
		pstVfdResult->stFaceSet.astFaces[i].rect.s16X1 = face_rect.x;
		pstVfdResult->stFaceSet.astFaces[i].rect.s16Y1 = face_rect.y;
		pstVfdResult->stFaceSet.astFaces[i].rect.s16X2 = face_rect.x + face_rect.width;
		pstVfdResult->stFaceSet.astFaces[i].rect.s16Y2 = face_rect.y + face_rect.height;
    }
#else
	int i = 0;
	VFD_MODULE *hVfd = (VFD_MODULE*)hModule;
    IMP_S32 nFaces = IMP_MAX_FACE_CNT;
	IMP_U8 *pYRaw8 = pstImage->pu8Data;
	IMP_U32 u32Width = pstImage->s32W;
	IMP_U32 u32Height = pstImage->s32H;
    FACEINFO *io_pFace = (FACEINFO *)&hVfd->m_stVfdResult.stFaceSet.astFaces;
    IMP_U32 nResult;
    hVfd->m_stVfdResult.stFaceSet.s32FaceNum = 0;
    hVfd->m_stVfdResult.stEventSet.s32EventNum = 0;

    nResult = DetectFaces(&hVfd->m_pFaceInstance, pYRaw8, u32Width, u32Height, &nFaces, io_pFace);
	if ( nResult != 0 )
		return IMP_FALSE;
  //  printf("nFaces = %d\n",nFaces);
    hVfd->m_stVfdResult.stFaceSet.s32FaceNum = nFaces;
	/*for (i = 0; i < nFaces; i++)
	{
        printf("centerX=%d,centerY=%d,faceSize=%d,rect.s16X1=%d,rect.s16Y1=%d,rect.s16X2=%d,rect.s16Y2=%d\n",
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].centerX,
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].centerY,
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].faceSize,
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].rect.s16X1,
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].rect.s16Y1,
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].rect.s16X2,
			hVfd->m_stVfdResult.stFaceSet.astFaces[i].rect.s16Y2
			);
	}*/

	//VFD_FaceCaptureAnalysis(hVfd);

	//VFD_FaceCamouflageAnalysis(hVfd);

	//VFD_PasswordPeeprAnalysis(hVfd);
#endif
    return IMP_TRUE;
}
static void	VFD_FaceCaptureAnalysis(IMP_HANDLE hModule)
{
    VFD_MODULE *hVfd = (VFD_MODULE*)hModule;
    VFD_FACE_SET_S *pstFaceSet = &hVfd->m_stVfdResult.stFaceSet;
    EVT_SET_S *pstEvtSet = &hVfd->m_stVfdResult.stEventSet;
    IMP_VFD_PARA_S *pstVfdPara = &hVfd->m_stVfdPara;
    IMP_VFD_FACE_CAPTURE_PARA_S *pstFaceCapturePara = &pstVfdPara->stFaceCapturePara;

	EVT_ITEM_S *evt;

    IMP_U32 type = IMP_EVT_TYPE_AlarmFaceCapture;
    IMP_U32 status;
    static IMP_U32 u32Time = 10;
    static IMP_BOOL bStartFlag = IMP_FALSE;
    if(pstFaceCapturePara->u32Enable)
    {
        if(pstFaceSet->s32FaceNum > 0 )
        {
            if(bStartFlag == IMP_FALSE)
            {
                status = IMP_EVT_STATUS_START;
                evt = IMP_EVT_SET_AddEvent( pstEvtSet, type, 0, 0, status, 0 );
                if( !evt ) return;
            }
            bStartFlag = IMP_TRUE;
            u32Time = 10;

        }
        else
        {
            u32Time--;
            if(u32Time <= 0)
            {
                status = IMP_EVT_STATUS_END;
                evt = IMP_EVT_SET_AddEvent( pstEvtSet, type, 0, 0, status, 0 );
                if( !evt ) return;
                bStartFlag = IMP_FALSE;
                u32Time = 10;
            }
        }
    }
}
int g_bPersonIn = 0;
static void	VFD_FaceCamouflageAnalysis(IMP_HANDLE hModule)
{
    VFD_MODULE *hVfd = (VFD_MODULE*)hModule;
    VFD_FACE_SET_S *pstFaceSet = &hVfd->m_stVfdResult.stFaceSet;
    EVT_SET_S *pstEvtSet = &hVfd->m_stVfdResult.stEventSet;
    IMP_VFD_PARA_S *pstVfdPara = &hVfd->m_stVfdPara;
    IMP_VFD_FACE_CAMOUFLAGE_PARA_S *pstFaceCamouflagePara = &pstVfdPara->stFaceCamouflagePara;

	EVT_ITEM_S *evt;

    IMP_U32 type = IMP_EVT_TYPE_AlarmFaceCamouflage;
    IMP_U32 status;
    static IMP_U32 u32TimeStart = 10;
    static IMP_U32 u32TimeEnd = 10;
    static IMP_BOOL bStartFlag = IMP_TRUE;
    if(pstFaceCamouflagePara->u32Enable)
    {
        if(pstFaceSet->s32FaceNum <=0 &&  g_bPersonIn == 1)
        {
            u32TimeStart--;
            if(u32TimeStart <= 0)
            {
                if(bStartFlag == IMP_FALSE)
                {
                    status = IMP_EVT_STATUS_START;
                    evt = IMP_EVT_SET_AddEvent( pstEvtSet, type, 1, 0, status, 0 );
                    if( !evt ) return;
                    bStartFlag = IMP_TRUE;
                    u32TimeStart = 10;
                    u32TimeEnd = 10;
                }
            }
        }
        else
        {
            u32TimeStart++;
            if(u32TimeStart > 10) u32TimeStart = 10;

            if(bStartFlag == IMP_TRUE)
            {
                u32TimeEnd--;
                if(u32TimeEnd <= 0)
                {
                    status = IMP_EVT_STATUS_END;
                    evt = IMP_EVT_SET_AddEvent( pstEvtSet, type, 1, 0, status, 0 );
                    if( !evt ) return;
                    bStartFlag = IMP_FALSE;
                    u32TimeStart = 10;
                    u32TimeEnd = 10;
                }
            }
        }
    }
}
static IMP_BOOL VFD_PasswordPeeprAnalysisInner(VFD_FACE_SET_S *pstFaceSet)
{
    int nDistanceThreth = 30;
    int i = 0,j = 0;
    int nMinDistance = 100000;
    int nDist = 0;
    int x1,y1,x2,y2;
    if(pstFaceSet->s32FaceNum > 1)
    {
        for( i = 0; i < pstFaceSet->s32FaceNum; i++)
        {
            for(j = 0; j < pstFaceSet->s32FaceNum; i++)
            {
                if(i ==j )
                    continue;
                x1 = pstFaceSet->astFaces[i].centerX;
                x2 = pstFaceSet->astFaces[j].centerX;
                y1 = pstFaceSet->astFaces[i].centerY;
                y2 = pstFaceSet->astFaces[j].centerY;
                nDist = abs(x1-x2)+ abs(y1-y2);
                if(nDist < nMinDistance)
                {
                    nMinDistance = nDist;
                }
            }
        }
        if(nMinDistance < nDistanceThreth)
        {
            return IMP_TRUE;
        }
        else
        {
            return IMP_FALSE;
        }
    }
    else
    {
        return IMP_FALSE;
    }
}
static void	VFD_PasswordPeeprAnalysis(IMP_HANDLE hModule)
{
    VFD_MODULE *hVfd = (VFD_MODULE*)hModule;
    VFD_FACE_SET_S *pstFaceSet = &hVfd->m_stVfdResult.stFaceSet;
    EVT_SET_S *pstEvtSet = &hVfd->m_stVfdResult.stEventSet;
    IMP_VFD_PARA_S *pstVfdPara = &hVfd->m_stVfdPara;
    IMP_VFD_PASSWORD_PEEP_PARA_S *pstPasswordPeepPara = &pstVfdPara->stPasswordPeepPara;

	EVT_ITEM_S *evt;

    IMP_U32 type = IMP_EVT_TYPE_AlarmPasswordPeep;
    IMP_U32 status;
    static IMP_U32 u32Time = 10;
    static IMP_BOOL bStartFlag = IMP_FALSE;
    IMP_U32 bPasswordPeepFlag = IMP_FALSE;
    if(pstPasswordPeepPara->u32Enable)
    {
        bPasswordPeepFlag = VFD_PasswordPeeprAnalysisInner(pstFaceSet);
        if(bPasswordPeepFlag )
        {
            if(bStartFlag == IMP_FALSE)
            {
                status = IMP_EVT_STATUS_START;
                evt = IMP_EVT_SET_AddEvent( pstEvtSet, type, 2, 0, status, 0 );
                if( !evt ) return;
            }
            bStartFlag = IMP_TRUE;
            u32Time = 10;
        }
        else
        {
            u32Time--;
            if(u32Time <= 0)
            {
                status = IMP_EVT_STATUS_END;
                evt = IMP_EVT_SET_AddEvent( pstEvtSet, type, 2, 0, status, 0 );
                if( !evt ) return;
                bStartFlag = IMP_FALSE;
                u32Time = 10;
            }
        }
    }
}

IMP_VOID IMP_VFD_GetResult(IMP_HANDLE hModule, VFD_RESULT_S *pstVfdResult)
{
#ifdef IMP_VFD_HAAR
	VFD_HAAR_MODULE *hVfd = (VFD_HAAR_MODULE*)hModule;
    memcpy(pstVfdResult , &hVfd->m_stVfdResult,sizeof(VFD_RESULT_S));
#else
	VFD_MODULE *hVfd = (VFD_MODULE*)hModule;
    memcpy(pstVfdResult , &hVfd->m_stVfdResult,sizeof(VFD_RESULT_S));
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


