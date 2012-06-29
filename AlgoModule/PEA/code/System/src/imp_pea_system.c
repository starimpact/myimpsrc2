
#include "imp_algo_interface.h"
#include "imp_pea_system.h"

//#define DEBUG_OUTPUT_STRING
#ifdef DEBUG_OUTPUT_STRING
#include <windows.h>
#include <stdio.h>


IMP_VOID WINAPI DebugString(LPCSTR format, ...)
{
	va_list arglist;
	IMP_S8 buffer[1024];
	va_start (arglist,format);
	vsprintf(buffer, format, arglist);
	va_end (arglist);
	strcat(buffer, "\n");
	OutputDebugString (buffer);
}
#endif


IMP_S32 IMP_COM_PacketInteger( IMP_U32 u32Val, IMP_VOID *pData )
{
	IMP_U8 *pCur = pData;

	*((IMP_S32 *)pCur) = sizeof(IMP_S32)+sizeof(IMP_U32);
    pCur += sizeof(IMP_S32);
	*((IMP_U32*)pCur) = u32Val;

	return 1;
}

IMP_S32 IMP_COM_UnPacketInteger( IMP_VOID *pData, IMP_U32 *pu32Val )
{
	IMP_U8 *pCur = pData;
	IMP_S32 nLen;

	nLen = *((IMP_S32*)pCur) - 4;
	pCur += sizeof(IMP_S32);
    if( nLen != sizeof(IMP_U32) )
        return 0;
	*pu32Val = *((IMP_U32 *)pCur);

	return 1;
}



IMP_S32 IMP_COM_PacketDataBuffer( DATA_BUF_S *pstDataBuf, IMP_VOID *pData )
{
	IMP_U8 *pCur = pData;

	*(IMP_S32 *)pCur = sizeof(IMP_S32)+pstDataBuf->s32DatLen;
	pCur += sizeof(IMP_S32);
    memcpy( pCur, pstDataBuf->pu8Buf, pstDataBuf->s32DatLen );

	return 1;
}

IMP_S32 IMP_COM_UnPacketDataBuffer( IMP_VOID *pData, DATA_BUF_S *pstDataBuf )
{
	IMP_U8 *pCur = pData;
	IMP_S32 nLen;

	nLen = *((IMP_U32*)pCur) - 4;
    pCur += sizeof(IMP_S32);
    if( ( nLen > pstDataBuf->s32BufLen ) || ( nLen < 0 ) )
        return 0;
    memcpy( pstDataBuf->pu8Buf, pCur, nLen );
	pstDataBuf->s32DatLen = nLen;

	return 1;
}


//intit some global parameter
IMP_VOID IMP_PEA_ParseProcessorParaData(PEA_SYSTEM_PARA_S *stPara )
{
	stPara->s32OutputDRegion = IMP_PROCESSOR_OUTPUT_DREGION;
	stPara->s32OutputAllTgts = IMP_PROCESSOR_OUTPUT_ALLTGTS;
	stPara->s32OutputPredicted = IMP_PROCESSOR_OUTPUT_PREDICTED;
	stPara->s32PtzTgtEvts = IMP_PROCESSOR_PTZ_TGTEVTS;
	stPara->s32ProcInterval = IMP_PROCESSOR_PROC_INTERVAL;
	stPara->s32TrajectDist = IMP_PROCESSOR_TRAJECT_DIST;
	stPara->s32FrmDura = IMP_PROCESSOR_FRM_DURA;
}


STATUS_E IMP_GetVersion( IMP_HANDLE hModule, IMP_U32 *pu32Version )
{
	IMP_U32 u32Version = ALGO_VERSION;
	u32Version = ( ( u32Version & 0x00FFFFFF ) | ( ALGO_MINOR_VERSION << 24 ) );
	*pu32Version = u32Version;
	return IMP_STATUS_OK;
}

static IMP_U32 ipGetFuncAuthorized();
STATUS_E IMP_GetFuncAuthorized( IMP_HANDLE hModule, IMP_U32 *pu32FuncAuthorized )
{
	*pu32FuncAuthorized = ipGetFuncAuthorized();
	return IMP_STATUS_OK;
}

STATUS_E IMP_GetAlgoLibInfo( IMP_HANDLE hModule, LIB_INFO_S *pstLibInfo )
{
    IMP_GetVersion(hModule, &pstLibInfo->pu32Version);
	IMP_GetFuncAuthorized(hModule,&pstLibInfo->pu32FuncAuthorized);
	return IMP_STATUS_OK;
}

STATUS_E IMP_GetMemReq( IMP_HANDLE hModule, MEM_SET_S *pstMems )
{
	MEM_MGR_REQ_S sNReq;

	sNReq.au32NodNum[IMP_MEMBLK_TYPE_FAST] = FAST_RAM_NODE_NUM;
	sNReq.au32NodNum[IMP_MEMBLK_TYPE_SLOW] = SLOW_RAM_NODE_NUM;
	IMP_MMGetMgrReq( &sNReq );

	pstMems->stMems[IMP_MEMBLK_TYPE_FAST].u32Size = FAST_RAM_MEM_SIZE;
	pstMems->stMems[IMP_MEMBLK_TYPE_FAST].u32Type = IMP_MEM_TYPE_IRAM;
	pstMems->stMems[IMP_MEMBLK_TYPE_FAST].pMem = NULL;

	pstMems->stMems[IMP_MEMBLK_TYPE_SLOW].u32Size = SLOW_RAM_MEM_SIZE;
	pstMems->stMems[IMP_MEMBLK_TYPE_SLOW].u32Type = IMP_MEM_TYPE_ERAM;
	pstMems->stMems[IMP_MEMBLK_TYPE_SLOW].pMem = NULL;

	pstMems->stMems[IMP_MEMBLK_TYPE_MMGR].u32Size = sNReq.u32NodSize;
	pstMems->stMems[IMP_MEMBLK_TYPE_MMGR].u32Type = IMP_MEM_TYPE_ERAM;
	pstMems->stMems[IMP_MEMBLK_TYPE_MMGR].pMem = NULL;

	pstMems->u32MemNum = IMP_MEMBLK_NUM+1;

	return IMP_STATUS_OK;
}

STATUS_E IMP_GetDmaReq( IMP_HANDLE hModule, DMA_CHANNLS_S *pIdmachs )
{
	pIdmachs->s32ChNum = 1;
#ifdef _TIDSP
	pIdmachs->stDMAChRs[0].queueId = 0;
#endif
	return IMP_STATUS_OK;
}

STATUS_E IMP_CheckRule( IMP_HANDLE hModule, RULE_CHECK_S *pRulChk )
{
	STATUS_E enStatus = IMP_STATUS_FALSE;
	IMP_S32 s32Ret = 0;

	if( pRulChk->u32Type == IMP_RULE_SET_TYPE )
		s32Ret = IMP_RULE_CheckRuleValid( &pRulChk->stScnDat, &pRulChk->u32EnaCode );
	if( s32Ret )
		enStatus = IMP_STATUS_OK;

	return enStatus;
}

extern IMP_VOID IMP_RULE_InitParaPea( INNER_PARA_S *pstInnerPara, MEM_MGR_ARRAY_S *pMemMgr );
static IMP_VOID IMP_PEA_ReStartStatus( IMP_HANDLE hModule );

//create PEA_MODULE 
STATUS_E IMP_Create( IMP_HANDLE hModule, MEM_SET_S *pstMems )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	MEM_SET_S *pMReqs = pstMems;
	GA_HARDWARE_RS_S *pHwRes = &pModule->stHwRes;
	MEM_MGR_ARRAY_S *pMemMgr = &pHwRes->stMemMgr;
	MEM_MGR_REQ_S sNReq;
	IMP_S32 s32ImgW, s32ImgH;
	IMP_S32 s32Res = IMP_STATUS_OK;

#ifdef TD_LICENSE_CHECK
	IMP_S32 s32Lic = IMP_CheckLicenseByMac();
    if( s32Lic != IMP_STATUS_OK)
      return s32Lic;
#endif

#ifdef _TIDSP1
	s32Res = SMCO_CRYPT_init();

    if( s32Res == 0 )
	{
#if defined(IMP_DEBUG_PRINT)
	    printf( " SMCO CRYPT init ok!\n" );
#endif
	}
	else
	{
#if defined(IMP_DEBUG_PRINT)
	    printf( " SMCO CRYPT init error!\n" );
#endif
		return IMP_STATUS_FALSE;
	}
#endif
	s32ImgW = pstMems->u32ImgW;
	s32ImgH = pstMems->u32ImgH;
	memset( pModule, 0, sizeof(PEA_MODULE) );

	// init memory
	sNReq.au32NodNum[IMP_MEMBLK_TYPE_FAST] = FAST_RAM_NODE_NUM;
	sNReq.au32NodNum[IMP_MEMBLK_TYPE_SLOW] = SLOW_RAM_NODE_NUM;
	IMP_MMGetMgrReq( &sNReq );
	IMP_ASSERT( sNReq.u32NodSize == pMReqs->stMems[IMP_MEMBLK_TYPE_MMGR].u32Size );
	sNReq.pNodMem = pMReqs->stMems[IMP_MEMBLK_TYPE_MMGR].pMem;
	IMP_MMInit( pMemMgr, &sNReq, pMReqs );

	// init module( rule modeling )
	pModule->pstInnerPara = IMP_MMAlloc( pMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(INNER_PARA_S) );

 	pModule->pstInnerPara->pRule = IMP_RULE_Alloc( s32ImgW, s32ImgH, pMemMgr );
 	IMP_RULE_AdvParaAlloc( &pModule->pstInnerPara->stAdvance, IMP_VPARAADVBUFNUM_MAX,
					IMP_PARA_ADVBUF_BUFLEN, pMemMgr );
	IMP_RULE_InitParaPea( pModule->pstInnerPara, pMemMgr );

	// init parameter
	pModule->pstSysPara = IMP_MMAlloc( pMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(PEA_SYSTEM_PARA_S) );

	memset( pModule->pstSysPara, 0, sizeof(PEA_SYSTEM_PARA_S) );

	// init result
	pModule->pstResult = IMP_MMAlloc( pMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(PEA_RESULT_S) );
		
	IMP_PEA_ResultInit( pModule->pstResult, s32ImgW, s32ImgH, pMemMgr );
	
	pModule->pstResult->s32ModuleSwitch = 1;
	
	// init module( object recognition )
	pModule->hObjRecg = IMP_PEA_CreateObjRecognition( pModule->pstResult, &pModule->stHwRes ); //memory allocation error

	// init module( event analysis )
 	pModule->hEvtAnls = IMP_PEA_BVA_CreateBehaviorAnalysis( pModule->pstResult, &pModule->stHwRes );

	return IMP_STATUS_OK;
}


//release PEA_MODULE
STATUS_E IMP_Release( IMP_HANDLE hModule )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	MEM_MGR_ARRAY_S *pMemMgr = &pModule->stHwRes.stMemMgr;

	// terminate module( event analysis )
 	IMP_PEA_BVA_ReleaseBehaviorAnalysis( pModule->hEvtAnls );

	// terminate module( object recognition )
	IMP_PEA_ReleaseObjRecognition( pModule->hObjRecg );

	// terminate result
	IMP_PEA_ResultTerminate( pModule->pstResult, pMemMgr );
	IMP_MMFree( pMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule->pstResult );

	// terminate parameter
	IMP_MMFree( pMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule->pstSysPara );

	// terminate module( rule modeling )
  	IMP_RULE_AdvParaFree( &pModule->pstInnerPara->stAdvance, IMP_VPARAADVBUFNUM_MAX, pMemMgr );
	IMP_RULE_Free( pModule->pstInnerPara->pRule, pMemMgr );
	IMP_MMFree( pMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule->pstInnerPara );
	
	printf("MemMax:%d\n", pMemMgr->astMemMgrs[IMP_MEMBLK_TYPE_SLOW].s32MemMax);
	
	// terminate memory
	IMP_MMTerminate( pMemMgr );
	
	

	return IMP_STATUS_OK;
}

STATUS_E IMP_InitIdmaChs( IMP_HANDLE hModule, DMA_CHANNLS_S *pIdmachs )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;

	memcpy( &pModule->stHwRes.stIDMAChs, pIdmachs, sizeof(DMA_CHANNLS_S) );

	return IMP_STATUS_OK;
}

static IMP_VOID ipForceInnerParaToSysPara( INNER_PARA_S *pstInnerPara, PEA_SYSTEM_PARA_S *pstSysPara );
STATUS_E IMP_UnpackURPRule( URP_PARA_S *pURPpara, RULE_S *pstRule );




STATUS_E IMP_ConfigArmPeaParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara ,URP_PARA_S *pstURPPara)
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	MEM_MGR_ARRAY_S *pMemMgr = &pModule->stHwRes.stMemMgr;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;
	RULE_S *pRule = (RULE_S*)pstInnerPara->pRule;
	DATA_BUF_S *pDatBuf;
	STATUS_E enRet = IMP_STATUS_OK;
	
	if (pstExtnalPara != NULL)
	{
		if( pstExtnalPara->u32Type & IMP_PARA_TYPE_GLOBAL )
		{
			memcpy( &pstInnerPara->stGlobal, &pstExtnalPara->stGlobal, sizeof(GLOBAL_PARA_S) );
		}

		if( pstExtnalPara->u32Type & IMP_PARA_TYPE_RULE )
		{
			if( !IMP_RULE_UnpackRule( &pstExtnalPara->stRule.stVanaData, pRule ))
			{
				IMP_RULE_DefaultRule( ps8ParRule, pRule, pMemMgr );
				IMP_RULE_Calculate( pRule, 1 );
			}
			IMP_RULE_Calculate( pRule, 1 );
		}
	}


	if ( pstURPPara != NULL )
	{
 //       IMP_OSD2AAI(pstURPPara);
		if( IMP_UnpackURPRule( pstURPPara, pRule )!= IMP_STATUS_OK)
		{
			IMP_RULE_DefaultRule( ps8ParRule, pRule, pMemMgr );
			pRule->stZones.astZone[0].u32Mode = IMP_FUNC_PERIMETER;
			IMP_RULE_Calculate( pRule, 1 );
			enRet = IMP_STATUS_FALSE;
		}
		IMP_RULE_Calculate( pRule, 1 );
	}
	if ( pstExtnalPara == NULL && pstURPPara == NULL )
	{
		IMP_RULE_DefaultRule( ps8ParRule, pRule, pMemMgr );
		pRule->stZones.astZone[0].u32Mode = IMP_FUNC_PERIMETER;
		IMP_RULE_Calculate( pRule, 1 );
	}
	pDatBuf = &pstInnerPara->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_PROCESSOR];
	IMP_PEA_ParseProcessorParaData(pModule->pstSysPara );
	ipForceInnerParaToSysPara( pstInnerPara, pModule->pstSysPara );
	IMP_PEA_ConfigObjRecognition( pModule->hObjRecg, pstInnerPara );
	IMP_PEA_BVA_ConfigBehaviorAnalysis( pModule->hEvtAnls, pstInnerPara );

	return enRet;
}

static STATUS_E IMP_RULE_ReSample(RULE_S *pstRuleSrc, RULE_S *pstRuleDst);
STATUS_E IMP_ConfigParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	MEM_MGR_ARRAY_S *pMemMgr = &pModule->stHwRes.stMemMgr;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;
	RULE_S *pRule = (RULE_S*)pstInnerPara->pRule;
	DATA_BUF_S *pDatBuf;
	STATUS_E enRet = IMP_STATUS_OK;

	if( pstExtnalPara->u32Type & IMP_PARA_TYPE_GLOBAL )
	{
		memcpy( &pstInnerPara->stGlobal, &pstExtnalPara->stGlobal, sizeof(GLOBAL_PARA_S) );
	}

	if( pstExtnalPara->u32Type & IMP_PARA_TYPE_RULE )
	{
		if( pRule->stZones.stImage.s32W == IMP_QCIF_IMG_WIDTH && pRule->stZones.stImage.s32H == IMP_QCIF_PAL_IMG_HEIGHT && pstExtnalPara->stRule.stVanaData.s32DatLen >= 41640 )
		{
            RULE_S *pstTmpRule = IMP_RULE_Alloc( IMP_CIF_IMG_WIDTH, IMP_CIF_PAL_IMG_HEIGHT, NULL );
            if(!IMP_RULE_UnpackRule( &pstExtnalPara->stRule.stVanaData, pstTmpRule ))
            {
                enRet = IMP_STATUS_FALSE;
#if defined(IMP_DEBUG_PRINT)
                 printf("IMP_RULE_ReSample false!\n");
#endif
                return enRet;
            }
            IMP_RULE_ReSample(pstTmpRule,pRule);
#if defined(IMP_DEBUG_PRINT)
            printf("IMP_RULE_ReSample OK!\n");
#endif

            IMP_RULE_Free(pstTmpRule,NULL);
		}
		else
		{
			if(!IMP_RULE_UnpackRule( &pstExtnalPara->stRule.stVanaData, pRule ))
			{
			    IMP_RULE_DefaultRule( ps8ParRule, pRule, pMemMgr );
                pRule->stZones.astZone[0].u32Mode = IMP_FUNC_PERIMETER;
                IMP_RULE_Calculate( pRule, 1 );
                enRet = IMP_STATUS_FALSE;
			}
		}

        IMP_RULE_Calculate( pRule, 1 );

	}

//	if( pstExtnalPara->type & IMP_PARA_TYPE_ADVANCE )
//	{
//	//	ipAdvParaClone( &pstExtnalPara->advance, &pstInnerPara->advance,
//	//		IP_VPARAADVBUFNUM_NICE_MAX, IMP_PARA_ADVBUF_BUFLEN );
//	}

	pDatBuf = &pstInnerPara->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_PROCESSOR];
	IMP_PEA_ParseProcessorParaData(pModule->pstSysPara );
	ipForceInnerParaToSysPara( pstInnerPara, pModule->pstSysPara );

	IMP_PEA_ConfigObjRecognition( pModule->hObjRecg, pstInnerPara );
	IMP_PEA_BVA_ConfigBehaviorAnalysis( pModule->hEvtAnls, pstInnerPara );

	return enRet;
}

//every body know down sample process, but need smooth I think.
IMP_S32 IMP_DownSampleImage( GRAY_IMAGE_S *pstImageSrc, GRAY_IMAGE_S *pstImageDst )
{
	IMP_S32 x, y, w, h;
	IMP_U8 *dat_p, *dat_i;


	w = pstImageSrc->s32W;
	h = pstImageSrc->s32H;

	dat_p = pstImageDst->pu8Data;
	for( y=0; y<h; y+=2 )
	{
		dat_i = pstImageSrc->pu8Data + w*y;
		for( x=0; x<w; x+=2 )
		{
			dat_p[0] = dat_i[x];
			dat_p++;
		}
	}
	return 1;
}


//resample the values of one type image to the values of another type image, such as coordinates.etc...
static STATUS_E IMP_RULE_ReSample(RULE_S *pstRuleSrc, RULE_S *pstRuleDst)
{
	STATUS_E ret = IMP_STATUS_OK;

	IMP_S32 src_img_w = pstRuleSrc->stZones.stImage.s32W;
	IMP_S32 src_img_h = pstRuleSrc->stZones.stImage.s32H;

	IMP_S32 des_img_w = pstRuleDst->stZones.stImage.s32W;
	IMP_S32 des_img_h = pstRuleDst->stZones.stImage.s32H;
	int zone_index = 0;
	int i = 0;
	int j = 0;

	if (src_img_w !=IMP_D1_IMG_WIDTH && src_img_w != IMP_CIF_IMG_WIDTH && src_img_w !=IMP_QCIF_IMG_WIDTH)
	{
		ret = IMP_STATUS_FALSE;
		return ret;
	}
	if (des_img_w !=IMP_D1_IMG_WIDTH && des_img_w != IMP_CIF_IMG_WIDTH && des_img_w !=IMP_QCIF_IMG_WIDTH)
	{
		ret = IMP_STATUS_FALSE;
		return ret;
	}

    memcpy(&pstRuleDst->stZones.astZone[0], &pstRuleSrc->stZones.astZone[0],sizeof(RULE_ZONE_S)*IMP_MAX_NUM_RULE_ZONE);
	for( zone_index=0; zone_index<IMP_MAX_NUM_RULE_ZONE; zone_index++ )
	{
		if( 0
			||	pstRuleSrc->stZones.astZone[zone_index].u32Valid == 0
			||	pstRuleSrc->stZones.astZone[zone_index].u32Enable == 0
		   )
			continue;
		for (i = 0; i < IMP_MAX_TRIPWIRE_CNT;i++)
		{
			int s_x = pstRuleSrc->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPs.s16X;
			int s_y = pstRuleSrc->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPs.s16Y;
			int e_x = pstRuleSrc->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPe.s16X;
			int e_y = pstRuleSrc->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPe.s16Y;

			pstRuleDst->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPs.s16X = (short)(s_x*(des_img_w/(IMP_FLOAT)src_img_w));
			pstRuleDst->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPs.s16Y = (short)(s_y*(des_img_h/(IMP_FLOAT)src_img_h));
			pstRuleDst->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPe.s16X = (short)(e_x*(des_img_w/(IMP_FLOAT)src_img_w));
			pstRuleDst->stZones.astZone[zone_index].stPara.stTripwire.astLines[i].stLine.stPe.s16Y = (short)(e_y*(des_img_h/(IMP_FLOAT)src_img_h));
		}

		for (i = 0; i < IMP_MAX_MTRIPWIRE_CNT;i++)
		{
			for(j = 0; j < 2;j++)
			{
			    int s_x = pstRuleSrc->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPs.s16X;
                int s_y = pstRuleSrc->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPs.s16Y;
                int e_x = pstRuleSrc->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPe.s16X;
                int e_y = pstRuleSrc->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPe.s16Y;

                pstRuleDst->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPs.s16X = (short)(s_x*(des_img_w/(IMP_FLOAT)src_img_w));
                pstRuleDst->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPs.s16Y = (short)(s_y*(des_img_h/(IMP_FLOAT)src_img_h));
                pstRuleDst->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPe.s16X = (short)(e_x*(des_img_w/(IMP_FLOAT)src_img_w));
                pstRuleDst->stZones.astZone[zone_index].stPara.stMTripwire.astLines[i].astLine[j].stPe.s16Y = (short)(e_y*(des_img_h/(IMP_FLOAT)src_img_h));
			}
		}
	}
	IMP_DownSampleImage(&pstRuleSrc->stZones.stImage,&pstRuleDst->stZones.stImage);
	return ret;
}



//transform URP_PARA_S to RULE_S
static STATUS_E IMP_TansURPToInnerData(URP_PARA_S *pURPpara, RULE_S *pRule)
{
	STATUS_E enRet = IMP_STATUS_OK;
	IMP_S32 s32ImgW = pRule->stZones.stImage.s32W;
	IMP_S32 s32ImgH = pRule->stZones.stImage.s32H;
	IMP_S32 s32ZoneIndex = 0;
	IMP_S32 x = 0,y=0;

	Point polygon[8];
	IMP_S32 s32Np = 0;
	IMP_S32 i = 0;
	IMP_S32 n = 0;
	URP_PERIMETER_LIMIT_BOUNDARY_S stPoly;
	IMP_U8 *pu8ZoneData = pRule->stZones.stImage.pu8Data;
	USR_ADV_PARA_S *pstUsrAdvPara = (USR_ADV_PARA_S *)pRule->u8AdvParaData;

      memcpy(&pstUsrAdvPara->stParaUrp,&pURPpara->stAdvancePara,sizeof(URP_ADVANCE_PARA_S));

	for( s32ZoneIndex = 0; s32ZoneIndex < 8; s32ZoneIndex++ )
	{
		pRule->stZones.astZone[s32ZoneIndex].u32Valid = pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Valid;
		pRule->stZones.astZone[s32ZoneIndex].u32Enable = pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Enable;
		pRule->stZones.astZone[s32ZoneIndex].u32Mode = pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Mode;
		if( 0
			||	pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Valid == 0
			||	pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Enable == 0
		   )
			continue;

//		printf("Size: ori:%d urp:%d\n", sizeof(RULE_PARA_OSC_S), sizeof(URP_OSC_RULE_PARA_S));

//#ifdef SUPPORT_OSC
		memcpy(&pRule->stZones.astZone[s32ZoneIndex].stPara.stOsc ,&pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stOscRulePara,sizeof(RULE_PARA_OSC_S));
//#endif

		memcpy(&pRule->stZones.astZone[s32ZoneIndex].stPara.stPerimeter ,&pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara,sizeof(RULE_PARA_PERIMETER_S));
		memcpy(&pRule->stZones.astZone[s32ZoneIndex].stPara.stTripwire,&pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara,sizeof(RULE_PARA_TRIPWIRE_S));
		memcpy(&pRule->stZones.astZone[s32ZoneIndex].stPara.stMTripwire,&pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara,sizeof(RULE_PARA_MTRIPWIRE_S));

		memcpy(&pRule->stZones.astZone[s32ZoneIndex].stPara.stLoiter,&pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stLoiterRulePara,sizeof(RULE_PARA_LOITER_S));


		memcpy(&stPoly,&pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary,sizeof(stPoly));
		pu8ZoneData = pRule->stZones.stImage.pu8Data;
		s32Np = stPoly.s32BoundaryPtNum;

		for (i = 0; i < s32Np;i++)
		{
			polygon[i].x = stPoly.astBoundaryPts[i].s16X;
			polygon[i].y = stPoly.astBoundaryPts[i].s16Y;
		}

		for ( x = 0; x < s32ImgW; x++)
		{
			for ( y = 0; y < s32ImgH; y++)
			{
				IMP_POINT_S stPt;
				Point p;
				stPt.s16X = x;
				stPt.s16Y = y;

				p.x = x;
				p.y = y;
				n = y * s32ImgW + x;

				if(stPoly.s32BoundaryPtNum >2 && !InsidePolygon(polygon,s32Np,p))
				{
					pRule->stZones.stImage.pu8Data[n] |= (1 <<s32ZoneIndex);
				}
				else
				{
					pRule->stZones.stImage.pu8Data[n] &= ~(1<<s32ZoneIndex);
				}
			}
		}
	}
	return enRet;
}


//added by mzhang
static STATUS_E IMP_ReSampleOneRgnRuleDate(URP_POLYGON_REGION_S *pstRgn, IMP_S32 s32SrcImgW, IMP_S32 s32SrcImgH, IMP_S32 s32DstImgW, IMP_S32 s32DstImgH)
{
	STATUS_E enRet = IMP_STATUS_OK;
	int i;
#if PRINT_DBG
	printf("s32Valid:%d\n", pstRgn->s32Valid);
#endif
	if (pstRgn->s32Valid)
	{
		for (i = 0; i < IMP_MAX_BOUNDARY_POINT_NUM;i++)
		{
			URP_IMP_POINT_S *pPnt = &pstRgn->astPoint[i];
			IMP_S32 x = pPnt->s16X;
			IMP_S32 y = pPnt->s16Y;
			pPnt->s16X = (short)(x * (s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pPnt->s16Y = (short)(y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
		}
	}

	return enRet;
}

//do resample operations int the URP_PARA_S based on the two different sizes of images
static STATUS_E IMP_ReSampleRuleDate(URP_PARA_S *pURPpara, RULE_S *pRule)
{
	STATUS_E enRet = IMP_STATUS_OK;

	IMP_S32 s32SrcImgW = pURPpara->stConfigPara.s32ImgW;
	IMP_S32 s32SrcImgH = pURPpara->stConfigPara.s32ImgH;

	IMP_S32 s32DstImgW = pRule->stZones.stImage.s32W;
	IMP_S32 s32DstImgH = pRule->stZones.stImage.s32H;
	IMP_S32 s32ZoneIndex = 0;
	IMP_S32 i = 0;


/*	if (s32SrcImgW !=IMP_D1_IMG_WIDTH && s32SrcImgW != IMP_CIF_IMG_WIDTH && s32SrcImgW !=IMP_QCIF_IMG_WIDTH)
	{
		enRet = IMP_STATUS_FALSE;
		return enRet;
	}*/
	if (s32DstImgW !=IMP_D1_IMG_WIDTH && s32DstImgW != IMP_CIF_IMG_WIDTH && s32DstImgW !=IMP_QCIF_IMG_WIDTH)
	{
		enRet = IMP_STATUS_FALSE;
		return enRet;
	}

	for( s32ZoneIndex=0; s32ZoneIndex<8; s32ZoneIndex++ )
	{
		if( 0
			||	pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Valid == 0
			||	pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Enable == 0
		   )
			continue;

//#ifdef SUPPORT_OSC //bird
		//added by mzhang 2011-11-15 13:25:11
		{
			URP_OSC_SPECL_REGIONS_S *psr =  &(pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stOscRulePara.astSpclRgs[0]);
			IMP_ReSampleOneRgnRuleDate(&psr->stOscRg, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
			IMP_ReSampleOneRgnRuleDate(&psr->astSubRgA, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
			IMP_ReSampleOneRgnRuleDate(&psr->astSubRgA, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
			IMP_ReSampleOneRgnRuleDate(&psr->astSubRgA, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
		}
//#endif //bird

#if PRINT_DBG
printf("Perimeter Boundary Number:%d\n", pURPpara->stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum);
#endif
		for (i = 0; i < IMP_MAX_BOUNDARY_POINT_NUM;i++)
		{
			IMP_S32 x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16X;
			IMP_S32 y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16X = (short)(x * (s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16Y = (short)(y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
		}
		for (i = 0; i < IMP_URP_MAX_TRIPWIRE_CNT;i++)
		{
			IMP_S32 s_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16X;
			IMP_S32 s_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16Y;
			IMP_S32 e_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16X;
			IMP_S32 e_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16X = (short)(s_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16Y = (short)(s_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16X = (short)(e_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16Y = (short)(e_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
		}

		for (i = 0; i < IMP_URP_MAX_MTRIPWIRE_CNT;i++)
		{
			IMP_S32 s_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16X;
			IMP_S32 s_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16Y;
			IMP_S32 e_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16X;
			IMP_S32 e_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16X = (short)(s_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16Y = (short)(s_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16X = (short)(e_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16Y = (short)(e_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));

			s_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16X;
			s_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16Y;
			e_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16X;
			e_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16X = (short)(s_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16Y = (short)(s_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16X = (short)(e_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16Y = (short)(e_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));

		}

	}
	return enRet;
}


//do resample in URP_PARA_S, and transform operations from URP_PARA_S to RULE_S
STATUS_E IMP_UnpackURPRule( URP_PARA_S *pURPpara, RULE_S *pRule )
{
	STATUS_E enRet = IMP_STATUS_OK;
#if PRINT_DBG
printf("Perimeter Boundary Number:%d\n", pURPpara->stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum);
#endif
	if (IMP_ReSampleRuleDate(pURPpara,pRule) != IMP_STATUS_OK)
	{
		enRet = IMP_STATUS_FALSE;
		return enRet;
	}
	if (IMP_TansURPToInnerData(pURPpara,pRule)!= IMP_STATUS_OK)
	{
		enRet = IMP_STATUS_FALSE;
		return enRet;
	}
	return enRet;

}

//if uppackrule is failed, then create a default rule
static STATUS_E IMP_URP_ConfigParameter( IMP_HANDLE hModule, URP_PARA_S *pstURPPara )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	MEM_MGR_ARRAY_S *pMemMgr = &pModule->stHwRes.stMemMgr;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;
	RULE_S *pRule = (RULE_S*)pstInnerPara->pRule;
	STATUS_E enRet = IMP_STATUS_OK;

	if( IMP_UnpackURPRule( pstURPPara, pRule )!= IMP_STATUS_OK)
	{
		IMP_RULE_DefaultRule( ps8ParRule, pRule, pMemMgr );
		pRule->stZones.astZone[0].u32Mode = IMP_FUNC_PERIMETER;
		IMP_RULE_Calculate( pRule, 1 );
		enRet = IMP_STATUS_FALSE;
	}
	IMP_RULE_Calculate( pRule, 1 );

	return enRet;
}

//get INNER_PARA_S from PEA_MODULE
STATUS_E IMP_GetParameter( IMP_HANDLE hModule, INNER_PARA_S **ppstInnerPara )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;

	*ppstInnerPara = pstInnerPara;

	return IMP_STATUS_OK;
}

//Clear something to initialize something
STATUS_E IMP_Start( IMP_HANDLE hModule )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;
	PEA_STATUS_S *pStatus = &pResult->stSysStatus;

	pStatus->u32NotFirstFrm = 0;
	IMP_PEA_ResultClear( pModule->pstResult );
//	ipStartObjRecognition( pModule->hObjRecg );
	IMP_PEA_BVA_StartBehaviorAnalysis( pModule->hEvtAnls );

	return IMP_STATUS_OK;
}

//stopping PEA_MODULE
STATUS_E IMP_Stop( IMP_HANDLE hModule, RESULT_S *pstResult )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;
	PEA_STATUS_S *pStatus = &pResult->stSysStatus;
	IMP_OUTPUT *pOutput = &pResult->stOutput;
	PEA_SYSTEM_PARA_S *pSysPara = pModule->pstSysPara;
	IMP_GA_ResultClear( pOutput );


	IMP_PEA_StopObjRecognition( pModule->hObjRecg );
	IMP_PEA_BVA_StopBehaviorAnalysis( pModule->hEvtAnls );
#ifndef _TIDSP
	memcpy( pstResult, pOutput, sizeof(RESULT_S) );
#endif
	return IMP_STATUS_OK;
}

STATUS_E IMP_GetEnacode( IMP_HANDLE hModule, IMP_U32 *pu32EnaCode )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;
	RULE_S *pstRule = pstInnerPara->pRule;
	*pu32EnaCode = pstRule->u32Enacode;

	return IMP_STATUS_OK;
}
static IMP_S32 IMP_PEA_CheckRuleEnable( PEA_MODULE *pModule );
static IMP_S32 IMP_PEA_CheckCurrentImage( PEA_MODULE *pModule );
static IMP_VOID IMP_PEA_OutputTargets( PEA_MODULE *pModule );


#define DBG_PROCESSIMAGE (1&PRINT_DBG)
//the main procedure of the algorithm
STATUS_E IMP_ProcessImage( IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage )
{
#ifdef TIME_TEST_TD
    //timer
    struct timeval start;
    struct timeval end;
    float timeuse;
    int timestart,timeend;
#endif

	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	PEA_STATUS_S *pstStatus = &pstResult->stSysStatus;
	IMP_OUTPUT *pstOutput = &pstResult->stOutput;
	GRAY_IMAGE_S stImage;
	IMP_U32 u32Time;
	STATUS_E enStatus = IMP_STATUS_OK;

#ifdef USE_TIME_LICENSE_LIMIT
	static IMP_U32 time = 0;
    time++;
    if(time > PEA_TIME_LIMIT)
    {
        return IMP_STATUS_CHECK_LICENSE_TIMEOUT;
    }
#endif

#ifdef TIME_TEST_TD
    gettimeofday(&start,NULL);//start timer
#endif
	IMP_GA_ResultClear( pstOutput );

	IMP_PEA_ReStartStatus( hModule );

	stImage.s32W = pstResult->s32Width;
	stImage.s32H = pstResult->s32Height;
	stImage.pu8Data = pstImage->pu8Y;

	u32Time = pstImage->u32Time;
//	printf("pstImage->u32Time:%d\n", pstImage->u32Time);

	pstStatus->u32SceneStatusPre = pstStatus->u32SceneStatus;
	pstStatus->u32SignalStatusPre = pstStatus->u32SignalStatus;
#define PI_TIME 0
	pstResult->u32FrmTimePre = pstResult->u32FrmTimeCur;
	pstResult->u32FrmTimeCur = u32Time;
	IMP_GrayImageClone( &stImage, &pstResult->stImgInGray );
	pstResult->pstImageYUV = pstImage;
//	ipShowGrayImage(stImage.s32W, stImage.s32H, stImage.pu8Data, "In1");
#if 0
	ipShowGrayImage(stImage.s32W, stImage.s32H, pstResult->stImgInGray.pu8Data, "pstResult->stImgInGray.pu8Data 1");
#endif	

#if PI_TIME
struct timeval t1, t2;
#endif

	if ( IMP_PEA_CheckRuleEnable (pModule ) )
	{
		if( IMP_PEA_CheckCurrentImage( pModule ) )
		{

#if 0
	ipShowGrayImage(stImage.s32W, stImage.s32H, pstResult->stImgInGray.pu8Data, "pstResult->stImgInGray.pu8Data 2");
#endif			

#if PI_TIME
gettimeofday(&t1, NULL);
#endif

			IMP_PEA_ProcessObjRecognition( pModule->hObjRecg );
//ipShowGrayImage(stImage.s32W, stImage.s32H, stImage.pu8Data, "In1");
#if PI_TIME
gettimeofday(&t2, NULL);
printf("ProcessObjRecognition:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif
#if 0
	ipShowGrayImage(stImage.s32W, stImage.s32H, pstResult->stImgInGray.pu8Data, "pstResult->stImgInGray.pu8Data 3");
#endif	
#if PI_TIME
gettimeofday(&t1, NULL);
#endif

 			IMP_PEA_BVA_ProcessBehaviorAnalysis( pModule->hEvtAnls );

#if PI_TIME
gettimeofday(&t2, NULL);
printf("ProcessBehaviorAnalysi:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000); 			
#endif

			IMP_PEA_BVA_PostProcessBehaviorAnalysis( pModule->hEvtAnls );
			IMP_PEA_PostProcessObjRecognition( pModule->hObjRecg );
			IMP_PEA_OutputTargets( pModule );	// 

			
		}
		else
		{
			enStatus = IMP_STATUS_SKIP;
		}
		pstStatus->u32NotFirstFrm = 1;
	}
	else
	{
		enStatus = IMP_STATUS_UNSUPPORT;
	}
#ifdef TIME_TEST_TD
        gettimeofday(&end,NULL);
        timestart=1000000*start.tv_sec+start.tv_usec;
        timeend=1000000*end.tv_sec+end.tv_usec;
#if DBG_PROCESSIMAGE
printf("IMP_ProcessImage used:%d us\n",timeend - timestart);
#endif
#endif
	return enStatus;
}

//copy targets of hModule to pstTargetSet
STATUS_E IMP_GetTargets( IMP_HANDLE hModule, TGT_SET_S *pstTargetSet )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;


	TGT_SET_S *pTgtsSrc = &pResult->stOutput.stTargetSet;

	pstTargetSet->s32TargetNum = pTgtsSrc->s32TargetNum;
	memcpy( &pstTargetSet->astTargets, &pTgtsSrc->astTargets, sizeof(TGT_ITEM_S)*(pTgtsSrc->s32TargetNum) );

	return IMP_STATUS_OK;
}

//copy events of hModule to pstEvtSet
STATUS_E IMP_GetEvents( IMP_HANDLE hModule, EVT_SET_S *pstEvtSet )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;
	EVT_SET_S *pEvtsSrc = &pResult->stOutput.stEventSet;

	pstEvtSet->s32EventNum = pEvtsSrc->s32EventNum;
	memcpy( &pstEvtSet->astEvents, &pEvtsSrc->astEvents, sizeof(EVT_ITEM_S)*(pEvtsSrc->s32EventNum) );

	return IMP_STATUS_OK;
}


STATUS_E IMP_GetCommands( IMP_HANDLE hModule, CMD_SET_S *pstCmdSet )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;
//	CMD_SET_S *pCmdsSrc = &pResult->stOutput.command_set;

//	pstCmdSet->cmd_num = pCmdsSrc->cmd_num;
//	memcpy( &pstCmdSet->commands, &pCmdsSrc->commands, sizeof(IP_CMD_ITEM)*(pCmdsSrc->cmd_num) );

	return IMP_STATUS_OK;
}

//copy results of hModule to pstResult
STATUS_E IMP_GetResults( IMP_HANDLE hModule, RESULT_S *pstResult )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;

	memcpy( pstResult, &pResult->stOutput, sizeof(RESULT_S) );

	return IMP_STATUS_OK;
}


static STATUS_E IMP_GetBGImage( IMP_HANDLE hModule, GRAY_IMAGE_S **ppBgImg );
STATUS_E IMP_DoSpecial( IMP_HANDLE hModule, PEA_OPER_S *pstOper )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	STATUS_E enStatus = IMP_STATUS_FALSE;
	IMP_VOID *pData = pstOper->pData;
	IMP_U32 u32Op = pstOper->u32Type;
	RULE_S *pstRule = pModule->pstInnerPara->pRule;

	switch( u32Op )
	{

	default:
		enStatus = IMP_STATUS_UNSUPPORT;
	    break;
	}

	return enStatus;
}


//get rule of hModule
STATUS_E IMP_GetScene( IMP_HANDLE hModule, IMP_VOID **pRule )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;
	RULE_S *pstRule = (RULE_S*)pstInnerPara->pRule;

	*pRule = pstRule;

	return IMP_STATUS_OK;
}


//get bkg of hModule to ppBgImg
static STATUS_E IMP_GetBGImage( IMP_HANDLE hModule, GRAY_IMAGE_S **ppBgImg )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pResult = pModule->pstResult;

	*ppBgImg = pResult->stDRegionSet.pstImgBgGray;

	return IMP_STATUS_OK;
}


//first stop then start hModule
static IMP_VOID IMP_PEA_ReStartStatus( IMP_HANDLE hModule )
{
	PEA_MODULE *pModule = (PEA_MODULE*)hModule;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	IMP_OUTPUT *pstOutput = &pstResult->stOutput;
	PEA_STATUS_S *pstStatus = &pstResult->stSysStatus;
	PEA_SYSTEM_PARA_S *pstSysPara = pModule->pstSysPara;
	INNER_PARA_S *pstInnerPara = pModule->pstInnerPara;


	{
		if( pstStatus->u32SceneStatus == IMP_SCENE_STATUS_CHANGED )
		{
			IMP_PEA_BVA_StopBehaviorAnalysis( pModule->hEvtAnls );
			IMP_PEA_StopObjRecognition( pModule->hObjRecg );
			IMP_PEA_ResultClear( pModule->pstResult );
			IMP_PEA_StartObjRecognition( pModule->hObjRecg );
			IMP_PEA_BVA_StartBehaviorAnalysis( pModule->hEvtAnls );
		}
	}
}

//check if the rule of pMoudle is enable
static IMP_S32 IMP_PEA_CheckRuleEnable( PEA_MODULE *pModule )
{
	RULE_S *pstRule = (RULE_S *)pModule->pstInnerPara->pRule;

	if (pstRule->u32Enacode !=0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//judge if the current image is valid.
static IMP_S32 IMP_PEA_CheckCurrentImage( PEA_MODULE *pModule )
{
	IMP_S32 s32Ret = 1;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	PEA_STATUS_S *pstStatus = &pstResult->stSysStatus;
	PEA_SYSTEM_PARA_S *pstSysPara = pModule->pstSysPara;

	if( pstStatus->u32NotFirstFrm )
	{
		IMP_S32 s32Interval = 0, s32Invalid = 1;
		if( pstResult->u32FrmTimeCur > pstResult->u32FrmTimePre )
		{
			s32Interval = ( pstResult->u32FrmTimeCur - pstResult->u32FrmTimePre ) * (pstSysPara->s32FrmDura);
			s32Invalid = ( s32Interval < pstSysPara->s32ProcInterval);
		}
		if( s32Invalid )
		{
			pstResult->u32FrmTimeCur = pstResult->u32FrmTimePre;
			s32Ret = 0;
		}
	}
 	else
 	{
 		pstStatus->u32NotFirstFrm = 1;
 	}
	pstStatus->u32FrmCounter++;

	return s32Ret;
}

static IMP_VOID IMP_OutputTargetSet( IpTrackedTargetSet *pstTTargetSet, TGT_SET_S *pstTargetSet,
							IMP_S32 s32OutputAllTgts, IMP_S32 s32OutputPredicted, IMP_S32 s32TrajectDist );
static IMP_VOID IMP_OutputDregionSet( PEA_DETECTED_REGIONSET_S *pstDrgSet, TGT_SET_S *pstTargetSet, IMP_S32 s32Flag );
static IMP_VOID IMP_OutputTargetSetOsc( IpTrackedTargetSet *pstTTargetSet, TGT_SET_S *pstTargetSet,
							IMP_S32 s32OutputAllTgts, IMP_S32 s32OutputPredicted, IMP_S32 s32TrajectDist );							
							
#ifdef USE_WATERMARK_DETECOTR

static IMP_VOID IMP_PEA_OutputWaterMark(PEA_MODULE *pPEA,TGT_SET_S *pstTargetSet)
{
	IMP_S32 k;

	PEA_ModuleObjRecognition *pModuleObjRec = (PEA_ModuleObjRecognition*)(pPEA->hObjRecg);
	IpModuleDetector *pMd = (IpModuleDetector*)pModuleObjRec->hDetector;
	PEA_TARGET_DETECTOR_S *pDetector = pMd->pDetector;
	IpWaterMarkDetectorPara *pstPara=&pDetector->stWaterMarkDetector.stPara;
	TGT_ITEM_S *pstTarget = pstTargetSet->astTargets;
	IMP_POINT_S stPt;

	k = 0;
	memset(pstTargetSet,0,sizeof(TGT_SET_S));
	k = pstTargetSet->s32TargetNum;


	pstTarget[k].u32Id = k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->I[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->I[0].s16X2+pstPara->I[0].s16X1)>>1;
	stPt.s16Y=(pstPara->I[0].s16Y2+pstPara->I[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->I[1], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->I[1].s16X2+pstPara->I[1].s16X1)>>1;
	stPt.s16Y=(pstPara->I[1].s16Y2+pstPara->I[1].s16Y1)>>1;

	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->M[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->M[0].s16X2+pstPara->M[0].s16X1)>>1;
	stPt.s16Y=(pstPara->M[0].s16Y2+pstPara->M[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->M[1], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->M[1].s16X2+pstPara->M[1].s16X1)>>1;
	stPt.s16Y=(pstPara->M[1].s16Y2+pstPara->M[1].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->M[2], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->M[2].s16X2+pstPara->M[2].s16X1)>>1;
	stPt.s16Y=(pstPara->M[2].s16Y2+pstPara->M[2].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->M[3], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->M[3].s16X2+pstPara->M[3].s16X1)>>1;
	stPt.s16Y=(pstPara->M[3].s16Y2+pstPara->M[3].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;


	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->P[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->P[0].s16X2+pstPara->P[0].s16X1)>>1;
	stPt.s16Y=(pstPara->P[0].s16Y2+pstPara->P[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->P[1], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->P[1].s16X2+pstPara->P[1].s16X1)>>1;
	stPt.s16Y=(pstPara->P[1].s16Y2+pstPara->P[1].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->O[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->O[0].s16X2+pstPara->O[0].s16X1)>>1;
	stPt.s16Y=(pstPara->O[0].s16Y2+pstPara->O[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;



	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->W[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->W[0].s16X2+pstPara->W[0].s16X1)>>1;
	stPt.s16Y=(pstPara->W[0].s16Y2+pstPara->W[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->W[1], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->W[1].s16X2+pstPara->W[1].s16X1)>>1;
	stPt.s16Y=(pstPara->W[1].s16Y2+pstPara->W[1].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->W[2], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->W[2].s16X2+pstPara->W[2].s16X1)>>1;
	stPt.s16Y=(pstPara->W[2].s16Y2+pstPara->W[2].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->W[3], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->W[3].s16X2+pstPara->W[3].s16X1)>>1;
	stPt.s16Y=(pstPara->W[3].s16Y2+pstPara->W[3].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;


	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->E[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->E[0].s16X2+pstPara->E[0].s16X1)>>1;
	stPt.s16Y=(pstPara->E[0].s16Y2+pstPara->E[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->E[1], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->E[1].s16X2+pstPara->E[1].s16X1)>>1;
	stPt.s16Y=(pstPara->E[1].s16Y2+pstPara->E[1].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->E[2], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->E[2].s16X2+pstPara->E[2].s16X1)>>1;
	stPt.s16Y=(pstPara->E[2].s16Y2+pstPara->E[2].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->E[3], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->E[3].s16X2+pstPara->E[3].s16X1)>>1;
	stPt.s16Y=(pstPara->E[3].s16Y2+pstPara->E[3].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;


	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->R[0], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->R[0].s16X2+pstPara->R[0].s16X1)>>1;
	stPt.s16Y=(pstPara->R[0].s16Y2+pstPara->R[0].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->R[1], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->R[1].s16X2+pstPara->R[1].s16X1)>>1;
	stPt.s16Y=(pstPara->R[1].s16Y2+pstPara->R[1].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;

	pstTarget[k].u32Id= k;
	pstTarget[k].u32Type = IMP_TGT_TYPE_HUMAN;
	pstTarget[k].u32Event = IMP_TGT_EVENT_PERIMETER ;
	IMP_SET_TGT_STABLE(pstTarget[k].u32Status);
	IMP_SET_TGT_MEASURED(pstTarget[k].u32Status);
	IMP_SET_TGT_MTREND(pstTarget[k].u32Status);
	memcpy( &pstTarget[k].stRect, &pstPara->R[2], sizeof(IMP_RECT_S) );
	stPt.s16X=(pstPara->R[2].s16X2+pstPara->R[2].s16X1)>>1;
	stPt.s16Y=(pstPara->R[2].s16Y2+pstPara->R[2].s16Y1)>>1;
	memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );
	k++;
	pstTargetSet->s32TargetNum = k;
}
#endif


//output targetset of hModule to pResult->stOutput
static IMP_VOID IMP_PEA_OutputTargets( PEA_MODULE *pModule )
{
	PEA_RESULT_S *pResult = pModule->pstResult;
	IMP_OUTPUT *pOutput = &pResult->stOutput;
	PEA_SYSTEM_PARA_S *pSysPara = pModule->pstSysPara;
#ifdef USE_WATERMARK_DETECOTR

    if (pResult->stSysStatus.u32WaterMarkStatus == IMP_WATERMARK_STAT_IMPOWER)
    {
		IMP_PEA_OutputWaterMark(pModule, &pOutput->stTargetSet);
		return;
    }
#endif

	if( pSysPara->s32OutputDRegion )
	{
		if( (pSysPara->s32OutputDRegion&0x10)==0 )
			IMP_OutputDregionSet( &pResult->stDRegionSet, &pOutput->stTargetSet, pSysPara->s32OutputDRegion );

	}
	else
	{
		IMP_OutputTargetSet( &pResult->stTrackedTargetSet, &pOutput->stTargetSet,
						pSysPara->s32OutputAllTgts, pSysPara->s32OutputPredicted, pSysPara->s32TrajectDist );

//#ifdef SUPPORT_OSC
	if (pResult->s32ModuleSwitch & 2)
	{
		IMP_OutputTargetSetOsc( &pResult->stTrackedTargetSetOsc, &pOutput->stTargetSet,
						pSysPara->s32OutputAllTgts,pSysPara->s32OutputPredicted, 0 );
	}
//#endif
	}
}

static IMP_VOID IMP_OutputDregionSet( PEA_DETECTED_REGIONSET_S *pstDrgSet, TGT_SET_S *pstTargetSet, IMP_S32 s32Flag )
{
	IMP_S32	i, k;
	IMP_RECT_S	stRc;
	IMP_POINT_S	stPt;
	TGT_ITEM_S	*pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDrg;

	pstDrg = pstDrgSet->astDrg;
	k = pstTargetSet->s32TargetNum;
	for( i=1; i<IMP_MAX_TGT_CNT-1; i++ )
	{
		if( 0
			|| ( ( IMP_DRG_IS_MOTION(pstDrg[i].u8Used) || IMP_DRG_IS_DIFF(pstDrg[i].u8Used) ) && (s32Flag&0x01) )
			|| ( IMP_DRG_IS_STATIC_L(pstDrg[i].u8Used) && (s32Flag&0x02) )
			|| ( IMP_DRG_IS_STATIC_R(pstDrg[i].u8Used) && (s32Flag&0x04) )
			|| ( IMP_DRG_IS_LIGHT(pstDrg[i].u8Used) && (s32Flag&0x08) )
			)
		{
			stRc.s16X1 = pstDrg[i].stRect.s16X1;
			stRc.s16X2 = pstDrg[i].stRect.s16X2;
			stRc.s16Y1 = pstDrg[i].stRect.s16Y1;
			stRc.s16Y2 = pstDrg[i].stRect.s16Y2;
			stPt.s16X = (stRc.s16X1+stRc.s16X2)>>1;
			stPt.s16Y = (stRc.s16Y1+stRc.s16Y2)>>1;
			if( IMP_IS_RGN_RMOTION(pstDrg[i].u16Status) )
				pstTarget[k].u32Id= 1;
			else if( IMP_IS_RGN_RSTATIC_L(pstDrg[i].u16Status) )
				pstTarget[k].u32Id= 2;
			else if( IMP_IS_RGN_RSTATIC_R(pstDrg[i].u16Status) )
				pstTarget[k].u32Id= 3;
			else
				pstTarget[k].u32Id= 0;
			pstTarget[k].u32Type = 0;
			pstTarget[k].u32Event = 0;
			memcpy( &pstTarget[k].stRect, &stRc, sizeof(IMP_RECT_S) );
			memcpy( &pstTarget[k].stPoint, &stPt, sizeof(IMP_POINT_S) );

			{
				TGT_MOTION_ITEM_S *pstItemData = (TGT_MOTION_ITEM_S*)(pstTarget[k].au8Data);
				pstItemData->s32Velocity = 0;
				pstItemData->s32Direction = 0;
				pstItemData->stTraject.s32Length = 0;
			}

			k++;
		}
	}
	pstTargetSet->s32TargetNum = k;
}


//output valid targets from pstTTargetSet of PEA_MODULE to pstTargetSet
static IMP_VOID IMP_OutputTargetSet( IpTrackedTargetSet *pstTTargetSet, TGT_SET_S *pstTargetSet,
							IMP_S32 s32OutputAllTgts, IMP_S32 s32OutputPredicted, IMP_S32 s32TrajectDist )
{
	IMP_S32 i, j, k;
	TGT_ITEM_S *pstTarget = pstTargetSet->astTargets;
	IpTrackedTarget *pstTTarget = pstTTargetSet->astTargets;
	IpTargetPosition *pstPos;
	IMP_S32 s32TrjDist2 = (s32TrajectDist * s32TrajectDist);

	k = pstTargetSet->s32TargetNum;
	for( i = 0, j = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( !ipTrackedTargetIsActive( &pstTTarget[i] ) )
			continue;

		if( s32TrjDist2 )
		{
			IMP_S32 m, s32Num;
			IMP_S32 x0, x1, y0, y1;
			IpTargetPosition *pstPos0 = NULL;
			s32Num = ipTargetTrajectoryGetLength(&pstTTarget[i].stTrajectory);
			for( m=0; m<s32Num; m++ )
			{
				pstPos = ipTargetTrajectoryGetPosition( &pstTTarget[i].stTrajectory, -m );
				if( IMP_IS_TPOS_KEYTRAJECT(pstPos->s32Used) )
				{
					pstPos0 = pstPos;
					break;
				}
			}
			if( !pstPos0 )
			{
				pstPos = ipTargetTrajectoryGetPosition( &pstTTarget[i].stTrajectory, -(s32Num-1) );
				IMP_SET_TPOS_KEYTRAJECT(pstPos->s32Used);
				pstPos0 = pstPos;
				for( m=s32Num-2; m>=0; m-- )
				{
					pstPos = ipTargetTrajectoryGetPosition( &pstTTarget[i].stTrajectory, -m );
					x0 = pstPos0->stPt.s16X; y0 = pstPos0->stPt.s16Y;
					x1 = pstPos->stPt.s16X; y1 = pstPos->stPt.s16Y;
					if( (x1-x0)*(x1-x0)+(y1-y0)*(y1-y0) >= s32TrjDist2 )
					{
						IMP_SET_TPOS_KEYTRAJECT(pstPos->s32Used);
						pstPos0 = pstPos;
					}
				}
			}
			else
			{
				pstPos = ipTargetTrajectoryGetPosition( &pstTTarget[i].stTrajectory, 0 );
				x0 = pstPos0->stPt.s16X; y0 = pstPos0->stPt.s16Y;
				x1 = pstPos->stPt.s16X; y1 = pstPos->stPt.s16Y;
				if( (x1-x0)*(x1-x0)+(y1-y0)*(y1-y0) >= s32TrjDist2 )
				{
					IMP_SET_TPOS_KEYTRAJECT(pstPos->s32Used);
				}
			}
		}

		if( 1
			&& ( s32OutputPredicted || ipTrackedTargetIsMeasured( &pstTTarget[i] ) )
			&& ( s32OutputAllTgts==1 || ( s32OutputAllTgts==2 && ipTrackedTargetIsActiveMeasuredMtrend(&pstTTarget[i]) ) || pstTTarget[i].stTargetInfo.u32TgtEvent!=IMP_TGT_EVENT_UNKNOWN )
			)
		{
			pstTarget[k].u32Id= pstTTarget[i].u32TargetId;
			pstTarget[k].u32Type = pstTTarget[i].stTargetInfo.u32Type;
			pstTarget[k].u32Event = pstTTarget[i].stTargetInfo.u32TgtEvent;
			pstPos = ipTargetTrajectoryGetPosition(&pstTTarget[i].stTrajectory,0);
			memcpy( &pstTarget[k].stRect, &pstPos->stRg, sizeof(IMP_RECT_S) );
			memcpy( &pstTarget[k].stPoint, &pstPos->stPt, sizeof(IMP_POINT_S) );

			if( !s32TrjDist2 )
			{
				TGT_MOTION_ITEM_S *pstItemData = (TGT_MOTION_ITEM_S*)(pstTarget[k].au8Data);
				pstItemData->s32Velocity = 0;
				pstItemData->s32Direction = 0;
				pstItemData->stTraject.s32Length = 0;
			}
			else
			{
				IMP_S32 m, n, s32Num;
				TGT_MOTION_ITEM_S *pstItemData = (TGT_MOTION_ITEM_S*)(pstTarget[k].au8Data);
				TGT_TRAJECT_S *traject = &pstItemData->stTraject;
				pstItemData->s32Velocity = pstPos->s32Velocity;
				pstItemData->s32Direction = pstPos->s32Direction;
				s32Num = ipTargetTrajectoryGetLength(&pstTTarget[i].stTrajectory);
				for( m = 0, n = 0; m < s32Num && n < IMP_MAX_TRAJECT_LEN; m++ )
				{
					pstPos = ipTargetTrajectoryGetPosition( &pstTTarget[i].stTrajectory, -m );
					if( IMP_IS_TPOS_KEYTRAJECT(pstPos->s32Used) )
					{
						traject->stPoints[n++] = pstPos->stPt;
					}
				}
				traject->s32Length = n;
			}

			k++;
		}

		j += pstTTarget[i].s32Used ? 1 : 0;
		if( j>=pstTTargetSet->s32UsedTotal )
			break;
	}
	pstTargetSet->s32TargetNum = k;
}

//output OSC objects
static IMP_VOID IMP_OutputTargetSetOsc( IpTrackedTargetSet *pstTTargetSet, TGT_SET_S *pstTargetSet,
							IMP_S32 s32OutputAllTgts, IMP_S32 s32OutputPredicted, IMP_S32 s32TrajectDist )
{
	IMP_S32 i, j, k;
	TGT_ITEM_S *pstTarget = pstTargetSet->astTargets;
	IpTrackedTarget *pstTTarget = pstTTargetSet->astTargets;
	IpTargetPosition *pstPos;
	k = pstTargetSet->s32TargetNum;
//	printf("TargetNum:%d\n", k);
	for( i = 0, j = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( !ipTrackedTargetIsActiveMeasured( &pstTTarget[i] ) )
			continue;
		if( 1
			&& ( s32OutputAllTgts==1 || ( s32OutputAllTgts==0  && pstTTarget[i].stTargetInfo.u32TgtEvent==IMP_TGT_EVENT_OSC ))
			)
		{
			pstTarget[k].u32Id= pstTTarget[i].u32TargetId;
			pstTarget[k].u32Type = pstTTarget[i].stTargetInfo.u32Type;
			pstTarget[k].u32Event = pstTTarget[i].stTargetInfo.u32TgtEvent;
			pstPos = ipTargetTrajectoryGetPosition(&pstTTarget[i].stTrajectory,0);
			memcpy( &pstTarget[k].stRect, &pstPos->stRg, sizeof(IMP_RECT_S) );
			memcpy( &pstTarget[k].stPoint, &pstPos->stPt, sizeof(IMP_POINT_S) );
			k++;
		}

		j += pstTTarget[i].s32Used ? 1 : 0;
		if( j>=pstTTargetSet->s32UsedTotal )
			break;
	}
	pstTargetSet->s32TargetNum = k;
}

static IMP_U32 ipGetFuncAuthorized()
{
	IMP_U32 funcode = 0;

#ifdef SUPPORT_PERIMETER
	funcode |= IMP_FUNC_PERIMETER;
#endif

#ifdef SUPPORT_TRIPWIRE
	funcode |= IMP_FUNC_TRIPWIRE;
#endif
#ifdef SUPPORT_MTRIPWIRE
	funcode |= IMP_FUNC_MTRIPWIRE;
#endif
	return funcode;
}

//......
static IMP_VOID ipForceInnerParaToSysPara( INNER_PARA_S *pstInnerPara, PEA_SYSTEM_PARA_S *pstSysPara )
{
	GLOBAL_PARA_S *pstGlobalPara = &pstInnerPara->stGlobal;
	RULE_S *pstRule = pstInnerPara->pRule;
	USR_ADV_PARA_S *pstUsrAdvPara;
	USR_ADV_PARA_SYS_S *pstUsrAdvSysPara;

	pstUsrAdvPara = (USR_ADV_PARA_S*)pstRule->u8AdvParaData;
	pstUsrAdvSysPara = &pstUsrAdvPara->stParaSys;

	if( pstUsrAdvPara->s32UseUsrAdvPara )
	{
		pstSysPara->s32OutputPredicted = pstUsrAdvSysPara->s32OutputPredicted;
		pstSysPara->s32ProcInterval = pstUsrAdvSysPara->s32ProcInterval;
	}

	pstSysPara->s32FrmDura = pstGlobalPara->s32TimeUnit;
//	pstPtzPara = &pstRule->autoptz_para;
//	if( !(pstGlobalPara->func_code & IMP_FUNC_PTZ) )
//		pstPtzPara->enable = PTZMODE_DISABLE;
//	pstSysPara->ptz_tgtevts = pstPtzPara->enable==PTZMODE_AUTO ? pstPtzPara->mode : 0;
}



