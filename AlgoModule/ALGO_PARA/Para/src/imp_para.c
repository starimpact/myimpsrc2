
#include "imp_para.h"

IMP_S8 *ps8ParRule =
"\
enacode: 16\n\
name: 规则\n\
[USRADV_IVPSYS]=0\n\
USE_USRADVPARA=0\n\
OUTPUT_PREDICTED=0\n\
OUTPUT_EVENT=0\n\
PROC_INTERVAL=80\n\
zones start...\n\
zone 0: valid(1)\n\
name: 规则1\n\
enable: 1\n\
level: 3\n\
[ANALYST_TYPE]=0\n\
ANALYST_ATM_ABANDUM=0\n\
ANALYST_ABANDUM=1\n\
ANALYST_OBJSTOLEN=1\n\
ANALYST_NOPARKING=1\n\
ANALYST_PERIMETER=1\n\
ANALYST_TRIPWIRE=0\n\
ANALYST_MTRIPWIRE=0\n\
ANALYST_ABNORMALVEL=0\n\
ANALYST_LOITER=0\n\
[ANALYST_PARAMETER]=0\n\
[ATM_ABANDUM]=0\n\
SCENE_TYPE=0\n\
CAM_TYPE=0\n\
WORK_MODE=0\n\
ATM_REGON1=(1,1,4,(131,84),(141,169),(243,168),(245,82),0,0,(0,0),(0,0),(0,0),(0,0),0,0,(0,0),(0,0),(0,0),(0,0),0,0,(0,0),(0,0),(0,0),(0,0))\n\
ATM_ABANDUM_SIZE_MIN=30\n\
ATM_ABANDUM_SIZE_MAX=10000\n\
ATM_ABANDUM_TIME_MIN=5000\n\
[ABANDUM]=0\n\
ABANDUM_SIZE_MIN=100\n\
ABANDUM_SIZE_MAX=10000\n\
ABANDUM_TIME_MIN=5000\n\
[OBJSTOLEN]=0\n\
OBJSTOLEN_SIZE_MIN=100\n\
OBJSTOLEN_SIZE_MAX=10000\n\
OBJSTOLEN_TIME_MIN=5000\n\
[NOPARKING]=0\n\
NOPARKING_TIME_MIN=5000\n\
[PERIMETER]=0\n\
PERIMETER_TYPE_CONSTRAIN=1\n\
PERIMETER_TYPE_HUMAN=1\n\
PERIMETER_TYPE_VEHICLE=1\n\
PERIMETER_MODE=0\n\
PERIMETER_DIST_MIN=0\n\
PERIMETER_TIME_MIN=0\n\
PERIMETER_DIRECTION_LIMIT=0\n\
PERIMETER_FORBIDDEN_DIRECTION=0\n\
[TRIPWIRE]=0\n\
TRIPWIRE_TYPE_CONSTRAIN=1\n\
TRIPWIRE_TYPE_HUMAN=1\n\
TRIPWIRE_TYPE_VEHICLE=1\n\
TRIPWIRE_VALID=1\n\
TRIPWIRE_BIDIRECTION=0\n\
TRIPWIRE_ABNMLANGLE=225\n\
TRIPWIRE_LINE=((105,232),(232,103))\n\
[MTRIPWIRE]=0\n\
MTRIPWIRE_TYPE_CONSTRAIN=1\n\
MTRIPWIRE_TYPE_HUMAN=1\n\
MTRIPWIRE_TYPE_VEHICLE=1\n\
MTRIPWIRE_VALID=1\n\
MTRIPWIRE_TIME_MIN=1000\n\
MTRIPWIRE_TIME_MAX=5000\n\
MTRIPWIRE_ABNMLANGLE1=225\n\
MTRIPWIRE_ABNMLANGLE2=225\n\
MTRIPWIRE_LINE1=((105,262),(232,133))\n\
MTRIPWIRE_LINE2=((105,232),(232,103))\n\
[ABNORMALVEL]=0\n\
ABNMLVEL_TYPE_CONSTRAIN=1\n\
ABNMLVEL_TYPE_HUMAN=1\n\
ABNMLVEL_TYPE_VEHICLE=1\n\
ABNMLVEL_INTERVAL=(0,500,-1)\n\
[LOITER]=0\n\
LOITER_TYPE_CONSTRAIN=1\n\
LOITER_TIME_MIN=10000\n\
zone end\n\
zone 1: valid(0)\n\
zone end\n\
zone 2: valid(0)\n\
zone end\n\
zone 3: valid(0)\n\
zone end\n\
zone 4: valid(0)\n\
zone end\n\
zone 5: valid(0)\n\
zone end\n\
zone 6: valid(0)\n\
zone end\n\
zone 7: valid(0)\n\
zone end\n\
FDEPTH_ENABLE:1\n\
line num:3\n\
line 0:((160,30),(160,50),200)\n\
line 1:((160,130),(160,150),100)\n\
line 2:((40,30),(40,50),200)\n\
";

//////////////////////////////////////////////////////////////////////////
/// 景深
//////////////////////////////////////////////////////////////////////////

extern CLBR_IALG_S stClbrIAlgCv;

IMP_S32 IMP_FDEPTH_CalcFDepth( FDEPTH_S *pstFDepth )
{
	CLBR_IALG_S *pstClbrIAlg;

	pstClbrIAlg = &stClbrIAlgCv;

	return pstClbrIAlg->IMP_CalcFDepth( pstFDepth );
}

static IMP_VOID IMP_FDEPTH_CalcImageResolution( FDEPTH_S *pstFDepth )
{
	CLBR_IALG_S *pstClbrIAlg;
	GRAY_IMAGE_S *pstImgResolution = &pstFDepth->stImgResolution;
	IMP_S32 s32ImgW = pstImgResolution->s32W;
	IMP_S32 s32ImgH = pstImgResolution->s32H;
	IMP_U8 *pu8DataR = pstImgResolution->pu8Data;

	IMP_S32 s32X, s32Y;
	pstClbrIAlg = &stClbrIAlgCv;


}

IMP_S32 IMP_FDEPTH_Calculate( FDEPTH_S *pstFdepth, IMP_U32 u32Flag )
{
	IMP_S32 s32Ret = IMP_CLBR_SUCCESS;

	s32Ret = IMP_FDEPTH_CalcFDepth( pstFdepth );
	if( s32Ret != IMP_CLBR_SUCCESS )
		return s32Ret;

	IMP_FDEPTH_CalcImageResolution( pstFdepth );

	return s32Ret;
}

IMP_S32 IMP_FDEPTH_GetPointResolution( FDEPTH_S *pstFDepth, IMP_S32 s32X, IMP_S32 s32Y, IMP_DOUBLE *pdResX, IMP_DOUBLE *pdResY )
{
	GRAY_IMAGE_S *pstImgResolution = &pstFDepth->stImgResolution;
	IMP_S32 s32ImgW = pstImgResolution->s32W;
	IMP_U8 *pu8DataR = pstImgResolution->pu8Data;
	IMP_U8 u8ResolveX, u8ResolveY;
	IMP_S32 s32Ret = IMP_CLBR_FAILED;
	IMP_S32 i = 0;
	IMP_S32 s32Index = 0;
	IMP_S32 s32MinDist = 1000;

	*pdResX = 0;
	*pdResY = 0;
	s32X = s32X & ( ~1 );
	u8ResolveX = pu8DataR[s32ImgW * s32Y + s32X];
	u8ResolveY = pu8DataR[s32ImgW * s32Y + s32X + 1];

	if( u8ResolveX )
	{
		*pdResX = u8ResolveX * 0.1;
		*pdResY = u8ResolveY * 0.1;
		s32Ret = IMP_CLBR_SUCCESS;
	}

	return s32Ret;
}

IMP_S32 IMP_FDEPTH_GetLineHeight( FDEPTH_S *pstFDepth, LINE_S *pstLine, IMP_S32 *s32Length )
{
	IMP_S32 s32Ret;
	CLBR_IALG_S *pstClbrIAlg;

	pstClbrIAlg = &stClbrIAlgCv;

	s32Ret = pstClbrIAlg->IMP_GetLineHeight( pstFDepth, pstLine, s32Length );

	return s32Ret;
}


IMP_S32 IMP_FDEPTH_GetRectDimension( FDEPTH_S *pstFDepth , IMP_POINT_S *pstPos, IMP_DOUBLE dArea,
									IMP_DOUBLE dRatio, IMP_S32 *ps32Width, IMP_S32 *ps32Height )
{
	IMP_S32 s32Ret;
	IMP_DOUBLE dResX, dResY;
	IMP_DOUBLE width, height;

	dResX = dResY = 0;
	s32Ret = IMP_FDEPTH_GetPointResolution( pstFDepth, pstPos->s16X, pstPos->s16Y, &dResX, &dResY );
	if( IMP_CLBR_SUCCESS != s32Ret )
		return s32Ret;



	return IMP_CLBR_SUCCESS;
}


IMP_S32 IMP_GetMemSizeRule(IMP_S32 s32ImgW, IMP_S32 s32ImgH)
{
	IMP_S32 s32Size = 0;
	
	s32Size += sizeof(RULE_S);
	s32Size += s32ImgW * s32ImgH;
	s32Size += s32ImgW * s32ImgH;
	
	return s32Size;
}


RULE_S* IMP_RULE_Alloc( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMMgr )
{
	RULE_S *pRule;
	IMP_S32 i;

	pRule = ipMemAlloc( sizeof(RULE_S), pMMgr );
	memset( pRule, 0, sizeof(RULE_S) );

	IMP_GrayImageCreate( &pRule->stZones.stImage, s32ImgW, s32ImgH, pMMgr );
	IMP_GrayImageCreate( &pRule->stFDepth.stImgResolution, s32ImgW, s32ImgH, pMMgr );

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
		IMP_ZONE_Alloc( &pRule->stZones.astZone[i], s32ImgW, s32ImgH, pMMgr );

	return pRule;
}

IMP_VOID IMP_RULE_Free( RULE_S *pRule, IMP_VOID *pMMgr )
{
	IMP_S32 i;

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
		IMP_ZONE_Free( &pRule->stZones.astZone[i], pMMgr );

	IMP_GrayImageDestroy( &pRule->stZones.stImage, pMMgr );
	IMP_GrayImageDestroy( &pRule->stFDepth.stImgResolution, pMMgr );

	memset( pRule, 0, sizeof(RULE_S) );
	ipMemFree( pRule, pMMgr );
}

IMP_VOID IMP_RULE_Clone( RULE_S *pSrc, RULE_S *pDst )
{
	IMP_S32 i;

	memcpy( pDst->as8Name, pSrc->as8Name, sizeof(IMP_S8)*IMP_NAME_BUF_SIZE );

	memcpy( pDst->u8AdvParaData, pSrc->u8AdvParaData, sizeof(IMP_U8)*IMP_ADV_DATA_LEN );

	memcpy( &pDst->stEvrmentTgtPara, &pSrc->stEvrmentTgtPara, sizeof(pSrc->stEvrmentTgtPara) );

	memcpy( &pDst->stFDepth, &pSrc->stFDepth, sizeof(pSrc->stFDepth) );

	IMP_GrayImageClone( &pSrc->stZones.stImage, &pDst->stZones.stImage );

	IMP_GrayImageClone( &pSrc->stFDepth.stImgResolution, &pDst->stFDepth.stImgResolution );

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
		IMP_ZONE_Clone( &pSrc->stZones.astZone[i], &pDst->stZones.astZone[i] );
}

IMP_VOID IMP_RULE_Clear( RULE_S *pRule )
{
	IMP_S32 i;

	memset( pRule->as8Name, 0, sizeof(IMP_S8)*IMP_NAME_BUF_SIZE );

	memset( pRule->u8AdvParaData, 0, sizeof(IMP_U8)*IMP_ADV_DATA_LEN );

	memset( &pRule->stEvrmentTgtPara, 0, sizeof(pRule->stEvrmentTgtPara) );

	memset( &pRule->stFDepth, 0, sizeof(pRule->stFDepth) );

	IMP_GrayImageClear( &pRule->stZones.stImage );
	IMP_GrayImageClear( &pRule->stFDepth.stImgResolution );

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
		IMP_ZONE_Clear( &pRule->stZones.astZone[i] );

}

IMP_S32 IMP_RULE_Calculate( RULE_S *pRule, IMP_U32 u32Flag )
{
	RULE_ZONES_S *pstZones = &pRule->stZones;
	RULE_ZONE_S *pstZone = pstZones->astZone;
	IMP_U32 u32Mode;
	IMP_S32 i;

	u32Mode = 0;
	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		pstZone = &pstZones->astZone[i];
		if( pstZone->u32Valid && pstZone->u32Enable )
			u32Mode |= pstZone->u32Mode;
	}

	pRule->u32Enacode = u32Mode;
	IMP_FDEPTH_Calculate(&pRule->stFDepth,1);
	return 0;
}


IMP_VOID IMP_ZONE_Alloc( RULE_ZONE_S *pZone, IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMMgr )
{
}

IMP_VOID IMP_ZONE_Free( RULE_ZONE_S *pZone, IMP_VOID *pMMgr )
{
}

IMP_VOID IMP_ZONE_Clear( RULE_ZONE_S *pZone )
{
	memset( pZone, 0, sizeof(RULE_ZONE_S) );
}

IMP_VOID IMP_ZONE_Clone( RULE_ZONE_S *pSrc, RULE_ZONE_S *pDst )
{
	memcpy( pDst, pSrc, sizeof(RULE_ZONE_S) );
}


static IMP_S32 IMP_RULE_Write( RULE_S *pRule, IMP_U8 *pData );
static IMP_S32 IMP_RULE_Read( IMP_U8 *pData, RULE_S *pRule );

IMP_S32 IMP_RULE_CheckRuleValid( DATA_BUF_S *pBuf, IMP_U32 *pu32Enacode )
{
	return IMP_RULE_CheckSceneValid( pBuf, IMP_RULE_VERSION, pu32Enacode );
}

IMP_S32 IMP_RULE_PackRule( RULE_S *pRule, DATA_BUF_S *pBuf )
{
	return IMP_RULE_PackAlgp( pRule, pBuf, &IMP_RULE_Write, IMP_RULE_VERSION );
}

IMP_S32 IMP_RULE_UnpackRule( DATA_BUF_S *pBuf, RULE_S *pRule )
{
	return IMP_RULE_UnpackAlgp( pBuf, pRule, &IMP_RULE_Read, IMP_RULE_VERSION );
}

static IMP_S32 IMP_WriteTmplImage( TMPL_IMAGE_S *pImage, IMP_U8 *pData )
{
	IMP_U8 *pCur = pData;
	IMP_S32 s32DatLen = 0;

	*((IMP_S32*)pCur) = pImage->s32W;
	pCur += sizeof(IMP_S32);

	*((IMP_S32*)pCur) = pImage->s32H;
	pCur += sizeof(IMP_S32);

	*((IMP_S32*)pCur) = pImage->s32BufLen;
	pCur += sizeof(IMP_S32);

	memcpy( pCur, pImage->pu8Data, pImage->s32BufLen );
	pCur += pImage->s32BufLen;

	s32DatLen = pCur - pData;
	return s32DatLen;
}

static IMP_S32 IMP_ReadTmplImage( IMP_U8 *pData, TMPL_IMAGE_S *pImage )
{
	IMP_U8 *pCur = pData;
	IMP_S32 s32DatLen = 0;

	pImage->s32W = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	pImage->s32H = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	pImage->s32BufLen = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	memcpy( pImage->pu8Data, pCur, pImage->s32BufLen );
	pCur += pImage->s32BufLen;

	s32DatLen = pCur - pData;
	return s32DatLen;
}

static IMP_S32 IMP_RULE_Write( RULE_S *pRule, IMP_U8 *pData )
{
	IMP_U8 *pCur = pData;
	IMP_S32 s32DatLen = 0;
	RULE_ZONES_S *pZones = &pRule->stZones;
	FDEPTH_S *pFdepth = &pRule->stFDepth;
	ENVIRONMENT_TGT_PARA_S *pevrtgt_para = &pRule->stEvrmentTgtPara;

	IMP_U8 *pUsrAdvData = pRule->u8AdvParaData;
	IMP_S32 nLen, i;

	*((IMP_U32*)pCur) = pRule->u32Enacode;
	pCur += sizeof(IMP_U32);

	nLen = IMP_NAME_BUF_SIZE * sizeof(IMP_S8);
	memcpy( pCur, pRule->as8Name, nLen );
	pCur += nLen;

	nLen = sizeof(RULE_ZONE_S) * IMP_MAX_NUM_RULE_ZONE;
	memcpy( pCur, pZones->astZone, nLen );
	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		RULE_ZONE_S *pZone = (RULE_ZONE_S*)pCur;
	}
	pCur += nLen;

	pCur += IMP_WriteVZImage( &pZones->stImage, pCur );

	memcpy(pCur,pFdepth,sizeof(FDEPTH_S));
	pCur += sizeof(FDEPTH_S);

	memcpy(pCur,pevrtgt_para,sizeof(ENVIRONMENT_TGT_PARA_S));
	pCur += sizeof(ENVIRONMENT_TGT_PARA_S);

	memcpy( pCur, pUsrAdvData, sizeof(IMP_U8) * IMP_ADV_DATA_LEN );
	pCur += sizeof(IMP_U8) * IMP_ADV_DATA_LEN;

	s32DatLen = pCur - pData;
	return s32DatLen;
}

static IMP_S32 IMP_RULE_Read( IMP_U8 *pData, RULE_S *pRule )
{
	IMP_U8 *pCur = pData;
	IMP_S32 s32DatLen = 0;
	RULE_ZONES_S *pZones = &pRule->stZones;
	FDEPTH_S *pFdepth = &pRule->stFDepth;
	ENVIRONMENT_TGT_PARA_S *pevrtgt_para = &pRule->stEvrmentTgtPara;
	IMP_U8 *pUsrAdvData = pRule->u8AdvParaData;
	IMP_S32 nLen;

	pRule->u32Enacode = *((IMP_U32*)pCur);
	pCur += sizeof(IMP_U32);

	nLen = IMP_NAME_BUF_SIZE * sizeof(IMP_S8);
	memcpy( pRule->as8Name, pCur, nLen );
	pCur += nLen;

	nLen = sizeof(RULE_ZONE_S) * IMP_MAX_NUM_RULE_ZONE;
	memcpy( pZones->astZone, pCur, nLen );
	pCur += nLen;

	pCur += IMP_ReadVZImage( pCur, &pZones->stImage );

//	memcpy(pFdepth,pCur,sizeof(FDEPTH_S));
	memcpy(pFdepth,pCur,sizeof(FDEPTH_S)-sizeof(GRAY_IMAGE_S));
	pCur += sizeof(FDEPTH_S);

	memcpy(pevrtgt_para,pCur,sizeof(ENVIRONMENT_TGT_PARA_S));
	pCur += sizeof(ENVIRONMENT_TGT_PARA_S);

	memcpy( pUsrAdvData, pCur, sizeof(IMP_U8) * IMP_ADV_DATA_LEN );
	pCur += sizeof(IMP_U8) * IMP_ADV_DATA_LEN;

	s32DatLen = pCur - pData;
	return s32DatLen;
}


static IMP_S32 IMP_RULE_PacketRuleV( RULE_S *pRule, IMP_S8 *ps8DataIn, IMP_S32 *s32Length );
static IMP_S32 IMP_RULE_ParseRuleV( IMP_S8 *ps8DataIn, IMP_S32 s32Length, RULE_S *pRule );
static IMP_S32 IMP_RULE_PacketRuleZ( RULE_S *pRule, IMP_S8 *ps8DataIn, IMP_S32 *s32Length );
static IMP_S32 IMP_RULE_ParseRuleZ( IMP_S8 *ps8DataIn, IMP_S32 s32Length, RULE_S *pRule );
static IMP_S32 IMP_RULE_DefaultRuleV( IMP_S8 *ps8DataIn, IMP_S32 s32Length, RULE_S *pRule );
static IMP_S32 IMP_RULE_DefaultRuleZ( RULE_S *pRule );

IMP_S32 IMP_RULE_PackRuleBuffer( RULE_S *pRule, DATA_BUF_S *pBufV, DATA_BUF_S *pBufZ )
{
	return IMP_RULE_PackAlgpBuffer( pRule, pBufV, pBufZ, &IMP_RULE_PacketRuleV, &IMP_RULE_PacketRuleZ );
}

IMP_S32 IMP_RULE_UnpackRuleBuffer( DATA_BUF_S *pBufV, DATA_BUF_S *pBufZ, RULE_S *pRule )
{
	return IMP_RULE_UnpackAlgpBuffer( pBufV, pBufZ, pRule, &IMP_RULE_ParseRuleV, &IMP_RULE_ParseRuleZ );
}

IMP_S32 IMP_RULE_PackRuleFile( RULE_S *pRule, IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pMemMgr )
{
	return IMP_RULE_PackAlgpFile( pRule, ps8FilV, ps8FilZ, pMemMgr, &IMP_RULE_PacketRuleV, &IMP_RULE_PacketRuleZ );
}

IMP_S32 IMP_RULE_UnpackRuleFile( IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, RULE_S *pRule, IMP_VOID *pMemMgr )
{
	return IMP_RULE_UnpackAlgpFile( ps8FilV, ps8FilZ, pRule, pMemMgr, &IMP_RULE_ParseRuleV, &IMP_RULE_ParseRuleZ );
}

IMP_S32 IMP_RULE_DefaultRule( IMP_S8 *ps8StrV, RULE_S *pRule, IMP_VOID *pMemMgr )
{
	return IMP_RULE_DefaultAlgp( ps8StrV, pRule, pMemMgr, &IMP_RULE_DefaultRuleV, &IMP_RULE_DefaultRuleZ );
}


static IMP_S32 IMP_RULE_PacketRuleZ( RULE_S *pRule, IMP_S8 *ps8DataIn, IMP_S32 *s32Length )
{
	GRAY_IMAGE_S *pstImgZone = &pRule->stZones.stImage;
	IMP_S32 s32ImgW = pstImgZone->s32W;
	IMP_S32 s32ImgH = pstImgZone->s32H;

	memcpy( ps8DataIn, pstImgZone->pu8Data, s32ImgW * s32ImgH );
	*s32Length = s32ImgW * s32ImgH;

	return 1;
}

static IMP_S32 IMP_RULE_ParseRuleZ( IMP_S8 *ps8DataIn, IMP_S32 s32Length, RULE_S *pRule )
{
	GRAY_IMAGE_S *pstImgZone = &pRule->stZones.stImage;
	IMP_S32 s32ImgW = pstImgZone->s32W;
	IMP_S32 s32ImgH = pstImgZone->s32H;

	memcpy( pstImgZone->pu8Data, ps8DataIn, s32ImgW * s32ImgH );

	return 1;
}

static IMP_S32 IMP_RULE_DefaultRuleZ( RULE_S *pRule )
{
	GRAY_IMAGE_S *pstImgZone = &pRule->stZones.stImage;
	IMP_S32 s32ImgW = pstImgZone->s32W;
	IMP_S32 s32ImgH = pstImgZone->s32H;

	memset( pstImgZone->pu8Data, 255, s32ImgW * s32ImgH );

	return 1;
}

static IMP_S32 IMP_RULE_PacketRuleVZone( RULE_ZONE_S *zone, IMP_S8 *ps8DataIn )
{
	IMP_S32 s32AnalyAbdum,s32AnalyObjStolen, s32AnalyNoParking,
		    s32AnalyNoPerim, s32AnalyTripw, s32AnalyMTripw,s32AnalyAbNrmlVel,s32AnalyLoiter;
	IMP_S8 *ps8Data = ps8DataIn;
	IMP_S32 s32Len = 0;
	IMP_S32 temp = 0;
	IMP_S8 *str1, *str2;
	IMP_S32 i = 0;
	if( zone->as8Name[0] == 0 )
		ps8Data += sprintf( ps8Data, "name: 规则1\n" );
	else
		ps8Data += sprintf( ps8Data, "name: %s\n", zone->as8Name );
	ps8Data += sprintf( ps8Data, "enable: %d\n", zone->u32Enable );
	ps8Data += sprintf( ps8Data, "level: %d\n", zone->u32Level );

	s32AnalyAbdum = 0;
	s32AnalyObjStolen = 0;
	s32AnalyNoParking= 0;
	s32AnalyNoPerim= 0;
	s32AnalyTripw = 0;
	s32AnalyMTripw = 0;
	s32AnalyAbNrmlVel= 0;
	s32AnalyLoiter = 0;

	if( zone->u32Mode & IMP_FUNC_ABANDUM )
		s32AnalyAbdum= 1;
	if( zone->u32Mode & IMP_FUNC_OBJSTOLEN )
		s32AnalyObjStolen= 1;
	if( zone->u32Mode & IMP_FUNC_NOPARKING )
		s32AnalyNoParking= 1;
	if( zone->u32Mode & IMP_FUNC_PERIMETER )
		s32AnalyNoPerim= 1;
	if( zone->u32Mode & IMP_FUNC_TRIPWIRE )
		s32AnalyTripw = 1;
	if( zone->u32Mode & IMP_FUNC_MTRIPWIRE )
		s32AnalyMTripw = 1;
	if( zone->u32Mode & IMP_FUNC_ABNMLVEL )
		s32AnalyAbNrmlVel= 1;
	if( zone->u32Mode & IMP_FUNC_LOITER )
		s32AnalyLoiter = 1;

	IMP_PACKET_PARFILE_START


	IMP_PACKET_PARFILE_ITEM_INT	( "[ANALYST_TYPE]",				temp )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_ABANDUM",			s32AnalyAbdum )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_OBJSTOLEN",			s32AnalyObjStolen )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_NOPARKING",			s32AnalyNoParking )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_PERIMETER",			s32AnalyNoPerim )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_TRIPWIRE",			s32AnalyTripw )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_MTRIPWIRE",			s32AnalyMTripw )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_ABNORMALVEL",		s32AnalyAbNrmlVel )
	IMP_PACKET_PARFILE_ITEM_INT	( "ANALYST_LOITER",				s32AnalyLoiter )

	IMP_PACKET_PARFILE_ITEM_INT	( "[ANALYST_PARAMETER]",		temp )

	IMP_PACKET_PARFILE_ITEM_INT	( "[PERIMETER]",				temp )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_TYPE_CONSTRAIN",	zone->stPara.stPerimeter.s32TypeConstrain )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_TYPE_HUMAN",		zone->stPara.stPerimeter.s32TypeHuman )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_TYPE_VEHICLE",		zone->stPara.stPerimeter.s32TypeVehicle )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_MODE",				zone->stPara.stPerimeter.s32Mode )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_DIST_MIN",			zone->stPara.stPerimeter.stPara.s32DistMin )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_TIME_MIN",			zone->stPara.stPerimeter.stPara.s32TimeMin )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_DIRECTION_LIMIT",	zone->stPara.stPerimeter.stPara.s32DirectionLmt )
	IMP_PACKET_PARFILE_ITEM_INT	( "PERIMETER_FORBIDDEN_DIRECTION",	zone->stPara.stPerimeter.stPara.s32ForbiddenDirection )

	IMP_PACKET_PARFILE_ITEM_INT	( "[IpTripwire]",					temp )
	IMP_PACKET_PARFILE_ITEM_INT	( "TRIPWIRE_TYPE_CONSTRAIN",	zone->stPara.stTripwire.s32TypeConstrain )
	IMP_PACKET_PARFILE_ITEM_INT ( "TRIPWIRE_TYPE_HUMAN",		zone->stPara.stTripwire.s32TypeHuman)
	IMP_PACKET_PARFILE_ITEM_INT ( "TRIPWIRE_TYPE_VEHICLE",		zone->stPara.stTripwire.s32TypeVehicle)
 	IMP_PACKET_PARFILE_ITEM_INT	( "TRIPWIRE_VALID",				zone->stPara.stTripwire.astLines[0].s32Valid)
 	IMP_PACKET_PARFILE_ITEM_INT	( "TRIPWIRE_BIDIRECTION",		zone->stPara.stTripwire.astLines[0].s32Bidirection)
	IMP_PACKET_PARFILE_ITEM_INT ( "TRIPWIRE_ABNMLANGLE",		zone->stPara.stTripwire.astLines[0].s32AbnmlAngle)
 	IMP_PACKET_PARFILE_ITEM_LINE( "TRIPWIRE_LINE",				zone->stPara.stTripwire.astLines[0].stLine)

	IMP_PACKET_PARFILE_END

	return s32Len + (ps8Data - ps8DataIn);
}

static IMP_VOID IMP_RULE_ParseRuleVZone0(RULE_ZONE_S *zone )
{
	IMP_S32 s32AnalyAbdum,s32AnalyObjStolen, s32AnalyNoParking,
		    s32AnalyNoPerim, s32AnalyTripw, s32AnalyMTripw,s32AnalyAbNrmlVel,s32AnalyLoiter;
	IMP_S32 i;

	sprintf(zone->as8Name,IMP_RULE_name1);
	zone->u32Enable = IMP_RULE_enable;
    zone->u32Level = IMP_RULE_level;

    s32AnalyAbdum = IMP_RULE_ANALYST_ABANDUM;
    s32AnalyNoPerim = IMP_RULE_ANALYST_PERIMETER;
    s32AnalyTripw = IMP_RULE_ANALYST_TRIPWIRE;
    s32AnalyAbNrmlVel = IMP_RULE_ANALYST_ABNORMALVEL;

	zone->stPara.stPerimeter.s32TypeConstrain = IMP_RULE_PERIMETER_TYPE_CONSTRAIN;
	zone->stPara.stPerimeter.s32TypeHuman = IMP_RULE_PERIMETER_TYPE_HUMAN;
	zone->stPara.stPerimeter.s32TypeVehicle = IMP_RULE_PERIMETER_TYPE_VEHICLE;
    zone->stPara.stPerimeter.s32Mode = IMP_RULE_PERIMETER_MODE;
	zone->stPara.stPerimeter.stPara.s32DistMin = IMP_RULE_PERIMETER_DIST_MIN;
	zone->stPara.stPerimeter.stPara.s32TimeMin = IMP_RULE_PERIMETER_TIME_MIN;
	zone->stPara.stPerimeter.stPara.s32DirectionLmt = IMP_RULE_PERIMETER_DIRECTION_LIMIT;
	zone->stPara.stPerimeter.stPara.s32ForbiddenDirection = IMP_RULE_PERIMETER_FORBIDDEN_DIRECTION;
	zone->stPara.stTripwire.s32TypeConstrain = IMP_RULE_TRIPWIRE_TYPE_CONSTRAIN;
	zone->stPara.stTripwire.s32TypeHuman = IMP_RULE_TRIPWIRE_TYPE_HUMAN;
	zone->stPara.stTripwire.s32TypeVehicle = IMP_RULE_TRIPWIRE_TYPE_VEHICLE;
	zone->stPara.stTripwire.astLines[0].s32Valid = IMP_RULE_TRIPWIRE_VALID;
	zone->stPara.stTripwire.astLines[0].s32Bidirection = IMP_RULE_TRIPWIRE_BIDIRECTION;
	zone->stPara.stTripwire.astLines[0].s32AbnmlAngle = IMP_RULE_TRIPWIRE_ABNMLANGLE;
	zone->stPara.stTripwire.astLines[0].stLine.stPs.s16X = IMP_RULE_TRIPWIRE_LINE_PTS_X;
	zone->stPara.stTripwire.astLines[0].stLine.stPs.s16Y = IMP_RULE_TRIPWIRE_LINE_PTS_Y;
	zone->stPara.stTripwire.astLines[0].stLine.stPe.s16X = IMP_RULE_TRIPWIRE_LINE_PTE_X;
	zone->stPara.stTripwire.astLines[0].stLine.stPe.s16Y = IMP_RULE_TRIPWIRE_LINE_PTE_Y;

	for( i=1; i<IMP_MAX_TRIPWIRE_CNT; i++ )
	{
		memcpy( &zone->stPara.stTripwire.astLines[i], &zone->stPara.stTripwire.astLines[0], sizeof(TRIPWIRE_S) );
		zone->stPara.stTripwire.astLines[i].s32Valid = 0;
	}
	for( i = 1; i < IMP_MAX_MTRIPWIRE_CNT; i++ )
	{
		memcpy( &zone->stPara.stMTripwire.astLines[i], &zone->stPara.stMTripwire.astLines[0], sizeof(MTRIPWIRE_S) );
		zone->stPara.stMTripwire.astLines[i].s32Valid = 0;
	}

	zone->u32Mode = 0;
	if( s32AnalyAbdum )
		zone->u32Mode |= IMP_FUNC_ABANDUM;
	if( s32AnalyNoPerim )
		zone->u32Mode |= IMP_FUNC_PERIMETER;
	if( s32AnalyAbNrmlVel )
		zone->u32Mode |= IMP_FUNC_ABNMLVEL;
	if( s32AnalyTripw )
		zone->u32Mode |= IMP_FUNC_TRIPWIRE;
}

static IMP_S32 IMP_RULE_PacketRuleV( RULE_S *pRule, IMP_S8 *ps8DataIn, IMP_S32 *s32Length )
{

	USR_ADV_PARA_S *pstUsrAdvPara = (USR_ADV_PARA_S*)pRule->u8AdvParaData;
	IMP_S32 s32Tmp = 0;
	IMP_S8 *ps8Data = ps8DataIn;
	IMP_U32 i;
	IMP_S32 s32Len;
	IMP_S8 *str1, *str2;

	IMP_S32 xs, ys, xe, ye;
	FDEPTH_MEASURE_S *pstMeasure = &pRule->stFDepth.stMeasure;
	FDZ_LINES_S *pstFDLines = &pstMeasure->stFdzLines;

	FDZ_LINE_S *pstLine;

	ps8Data += sprintf( ps8Data, "enacode: %d\n", pRule->u32Enacode );

	if( pRule->as8Name[0] == 0 )
		ps8Data += sprintf( ps8Data, "name: rule1\n" );
	else
		ps8Data += sprintf( ps8Data, "name: %s\n", pRule->as8Name );

	IMP_PACKET_PARFILE_START
	IMP_PACKET_PARFILE_ITEM_INT	( "[USRADV_NICESYS]",			s32Tmp )
	IMP_PACKET_PARFILE_ITEM_INT	( "USE_USRADVPARA",				pstUsrAdvPara->s32UseUsrAdvPara )
	IMP_PACKET_PARFILE_ITEM_INT	( "OUTPUT_PREDICTED",			pstUsrAdvPara->stParaSys.s32OutputPredicted )
	IMP_PACKET_PARFILE_ITEM_INT	( "OUTPUT_EVENT",				pstUsrAdvPara->stParaSys.s32OutputEvents )
	IMP_PACKET_PARFILE_ITEM_INT	( "PROC_INTERVAL",				pstUsrAdvPara->stParaSys.s32ProcInterval )

	IMP_PACKET_PARFILE_END

	ps8Data += s32Len;

	ps8Data += sprintf( ps8Data, "zones start...\n" );

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		RULE_ZONE_S *pstZone = &pRule->stZones.astZone[i];
		ps8Data += sprintf( ps8Data, "zone %d: valid(%d)\n", i, pstZone->u32Valid );
		if( pstZone->u32Valid )
			ps8Data += IMP_RULE_PacketRuleVZone( pstZone, ps8Data );
		ps8Data += sprintf( ps8Data, "zone end\n" );
	}

	ps8Data += sprintf( ps8Data, "FDEPTH_ENABLE:%d\n", pRule->stFDepth.u32Enable);
	ps8Data += sprintf( ps8Data, "line num:%d\n", pstFDLines->u32NumUsed );
	for( i = 0; i < pstFDLines->u32NumUsed; i++ )
	{
		pstLine = &pstFDLines->stLines[i];
		xs = pstLine->stRefLine.stPs.s16X;
		ys = pstLine->stRefLine.stPs.s16Y;
		xe = pstLine->stRefLine.stPe.s16X;
		ye = pstLine->stRefLine.stPe.s16Y;
		ps8Data += sprintf( ps8Data, "line %d:((%d,%d),(%d,%d),%d)\n",
			i, xs, ys, xe, ye, pstLine->s32RefLen );
	}

	*s32Length = ps8Data - ps8DataIn;

	return 1;
}

static IMP_S32 IMP_RULE_ParseRuleV( IMP_S8 *ps8DataIn, IMP_S32 s32Length, RULE_S *pRule )
{
	IMP_S32 i;
	USR_ADV_PARA_S *pstUsrAdvPara = (USR_ADV_PARA_S*)pRule->u8AdvParaData;

	FDEPTH_MEASURE_S *pstMeasure = &pRule->stFDepth.stMeasure;
	FDZ_LINES_S *pstFDLines = &pstMeasure->stFdzLines;

	FDZ_LINE_S *pstLine;
	IMP_S8 as8ZoneValid[8];
	as8ZoneValid[0] = IMP_RULE_ZONE0_VALID;
	as8ZoneValid[1] = IMP_RULE_ZONE1_VALID;
	as8ZoneValid[2] = IMP_RULE_ZONE2_VALID;
	as8ZoneValid[3] = IMP_RULE_ZONE3_VALID;
	as8ZoneValid[4] = IMP_RULE_ZONE4_VALID;
	as8ZoneValid[5] = IMP_RULE_ZONE5_VALID;
	as8ZoneValid[6] = IMP_RULE_ZONE6_VALID;
	as8ZoneValid[7] = IMP_RULE_ZONE7_VALID;

	pRule->u32Enacode = IMP_RULE_enacode;
	sprintf( pRule->as8Name, "规则");
	pstUsrAdvPara->s32UseUsrAdvPara = IMP_RULE_USE_USRADVPARA;
	pstUsrAdvPara->stParaSys.s32OutputPredicted = IMP_RULE_OUTPUT_PREDICTED;
	pstUsrAdvPara->stParaSys.s32OutputEvents = IMP_RULE_OUTPUT_EVENT;
	pstUsrAdvPara->stParaSys.s32ProcInterval = IMP_RULE_PROC_INTERVAL;

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		RULE_ZONE_S *pstZone = &pRule->stZones.astZone[i];

		pstZone->u32Valid = as8ZoneValid[i];
		if( pstZone->u32Valid )
			IMP_RULE_ParseRuleVZone0(pstZone );
	}

	pRule->stFDepth.u32Enable = IMP_RULE_FDEPTH_ENABLE;
	pstFDLines->u32NumUsed = IMP_RULE_FDEPTH_LINE_NUM;

	pstLine = &pstFDLines->stLines[0];
	pstLine->stRefLine.stPs.s16X = IMP_RULE_FDEPTH_LINE1_PTS_X;
	pstLine->stRefLine.stPs.s16Y = IMP_RULE_FDEPTH_LINE1_PTS_Y;
	pstLine->stRefLine.stPe.s16X = IMP_RULE_FDEPTH_LINE1_PTE_X;
	pstLine->stRefLine.stPe.s16Y = IMP_RULE_FDEPTH_LINE1_PTE_Y;
	pstLine->s32RefLen = IMP_RULE_FDEPTH_LINE1_REF_LEN;

	pstLine = &pstFDLines->stLines[1];
	pstLine->stRefLine.stPs.s16X = IMP_RULE_FDEPTH_LINE2_PTS_X;
	pstLine->stRefLine.stPs.s16Y = IMP_RULE_FDEPTH_LINE2_PTS_Y;
	pstLine->stRefLine.stPe.s16X = IMP_RULE_FDEPTH_LINE2_PTE_X;
	pstLine->stRefLine.stPe.s16Y = IMP_RULE_FDEPTH_LINE2_PTE_Y;
	pstLine->s32RefLen = IMP_RULE_FDEPTH_LINE2_REF_LEN;

	pstLine = &pstFDLines->stLines[2];
	pstLine->stRefLine.stPs.s16X = IMP_RULE_FDEPTH_LINE3_PTS_X;
	pstLine->stRefLine.stPs.s16Y = IMP_RULE_FDEPTH_LINE3_PTS_Y;
	pstLine->stRefLine.stPe.s16X = IMP_RULE_FDEPTH_LINE3_PTE_X;
	pstLine->stRefLine.stPe.s16Y = IMP_RULE_FDEPTH_LINE3_PTE_Y;
	pstLine->s32RefLen = IMP_RULE_FDEPTH_LINE3_REF_LEN;

	return 1;
}

static IMP_S32 IMP_RULE_DefaultRuleV( IMP_S8 *ps8DataIn, IMP_S32 s32Length, RULE_S *pRule )
{
	RULE_ZONE_S *pstZone = &pRule->stZones.astZone[0];
	IMP_S32 i;

	IMP_RULE_ParseRuleV( ps8DataIn, s32Length, pRule );

	pstZone->u32Valid = 0;
	for( i = 1; i < IMP_MAX_NUM_RULE_ZONE; i++ )
		IMP_ZONE_Clone( pstZone, pstZone + i );
	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
		sprintf( pstZone[i].as8Name, "规则%d", i + 1 );
	pstZone->u32Valid = 1;

	return 1;
}


IMP_VOID IMP_RULE_InitParaPea( INNER_PARA_S *pstInnerPara, MEM_MGR_ARRAY_S *pMemMgr )
{
	RULE_S *pstRule = (RULE_S*)pstInnerPara->pRule;


	pstInnerPara->stGlobal.s32TimeUnit = 20; //rubish, 40
    pstInnerPara->stGlobal.s32FuncCode = ~0;


	IMP_RULE_DefaultRule( ps8ParRule, pstRule, pMemMgr );

	IMP_RULE_Calculate( pstRule, 1 );

	IMP_RULE_InitParaAdvance( &pstInnerPara->stAdvance, NULL, pMemMgr );
}

IMP_VOID IMP_RULE_InitParaAdvance( ADVANCE_PARA_S *pstAdvPara, STR_ARRAY_PARA_S *pstStrArray, IMP_VOID *pMemMgr )
{
	DATA_BUF_S *pstDatBuf;

	IMP_RULE_AdvParaClear( pstAdvPara, IMP_VPARAADVBUFNUM_MAX );

	if( pstStrArray )
	{
		pstDatBuf = &pstAdvPara->astAdvDats[IMP_VPARAADVBUFNUM_PROCESSOR];
		ipReadFileData( pstStrArray->pStrings[IMP_VPARAADVBUFNUM_PROCESSOR], pstDatBuf );
		pstDatBuf = &pstAdvPara->astAdvDats[IMP_VPARAADVBUFNUM_DETECTOR];
		ipReadFileData( pstStrArray->pStrings[IMP_VPARAADVBUFNUM_DETECTOR], pstDatBuf );
		pstDatBuf = &pstAdvPara->astAdvDats[IMP_VPARAADVBUFNUM_TRACKER];
		ipReadFileData( pstStrArray->pStrings[IMP_VPARAADVBUFNUM_TRACKER], pstDatBuf );
		pstDatBuf = &pstAdvPara->astAdvDats[IMP_VPARAADVBUFNUM_CLASSIFIER];
		ipReadFileData( pstStrArray->pStrings[IMP_VPARAADVBUFNUM_CLASSIFIER], pstDatBuf );
		pstDatBuf = &pstAdvPara->astAdvDats[IMP_VPARAADVBUFNUM_ANALYST];
		ipReadFileData( pstStrArray->pStrings[IMP_VPARAADVBUFNUM_ANALYST], pstDatBuf );
		pstDatBuf = &pstAdvPara->astAdvDats[IMP_VPARAADVBUFNUM_PTZ];
		ipReadFileData( pstStrArray->pStrings[IMP_VPARAADVBUFNUM_PTZ], pstDatBuf );
	}
	else
	{

	}
}

IMP_VOID IMP_RULE_InitParaRule( RULE_PARA_DATA_S *pstAdvPara, STR_ARRAY_PARA_S *pstStrArray,
					   IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMemMgr )
{
	RULE_S *pstRule;

	pstRule = IMP_RULE_Alloc( s32ImgW, s32ImgH, pMemMgr );

	if( pstStrArray )
	{
		IMP_RULE_UnpackRuleFile( pstStrArray->pStrings[IMP_PARA_RULE_SET],
							pstStrArray->pStrings[IMP_PARA_RULE_SET_ZONE],
							pstRule, pMemMgr );
	}
	else
	{
		IMP_RULE_DefaultRule( ps8ParRule, pstRule, pMemMgr );
	}
	IMP_RULE_Calculate( pstRule, 1 );
	IMP_RULE_PackRule( pstRule, &pstAdvPara->stVanaData );

	IMP_RULE_Free( pstRule, pMemMgr );
}

IMP_VOID IMP_PARA_Init( EXTERNAL_PARA_S *pstPara, STR_ARRAY_PARA_S *pstScnFil,
									  STR_ARRAY_PARA_S *pstAdvFil, IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMmgr )
{

	pstPara->u32Type= ~0;

	pstPara->stGlobal.s32TimeUnit = 20; //rubish, 40
    pstPara->stGlobal.s32FuncCode = ~0;


	IMP_RULE_InitParaRule( &pstPara->stRule, pstScnFil, s32ImgW, s32ImgH, pMmgr );

}

IMP_S32 IMP_RULE_CheckSceneValid( DATA_BUF_S *pBuf, IMP_U32 u32Version, IMP_U32 *pu32Enacode )
{
	IMP_S32 s32Ret = 0;
	IMP_U8 *pData = pBuf->pu8Buf;
	IMP_U8 *pCur = pData;
	IMP_S32 nLen;
	IMP_U32 u32VersCur;

	nLen = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	if( nLen > pBuf->s32BufLen )
		return 0;
	if( !ipCheckCRC( pData, sizeof(IMP_S32)+nLen ) )
		return 0;

	u32VersCur = *((IMP_U32*)pCur);
	pCur += sizeof(IMP_U32);
	if( u32VersCur != u32Version )
		return 0;

	*pu32Enacode = *((IMP_U32*)pCur);
	pCur += sizeof(IMP_U32);
	s32Ret = 1;

	return s32Ret;
}

IMP_S32 IMP_RULE_PackAlgp( IMP_VOID *pAlgp, DATA_BUF_S *pBuf, IMP_FUNC_PKGALGP func_pkg, IMP_U32 u32Version )
{
	IMP_S32 s32Ret = 0;
	IMP_U8 *pData = pBuf->pu8Buf;
	IMP_U8 *pCur = pData;
	IMP_S32 nLen;

	pCur += sizeof(IMP_S32);

	*((IMP_U32*)pCur) = u32Version;
	pCur += sizeof(IMP_U32);

	nLen = func_pkg( pAlgp, pCur );

	nLen += sizeof(IMP_U32);
	*((IMP_S32*)pData) = nLen;

	ipGenerateCRC( pData, sizeof(IMP_S32)+nLen );

	pBuf->s32DatLen = sizeof(IMP_S32)+nLen+sizeof(IMP_U32);
	s32Ret = 1;

	return s32Ret;
}

IMP_S32 IMP_RULE_UnpackAlgp( DATA_BUF_S *pBuf, IMP_VOID *pAlgp, IMP_FUNC_UPKALGP func_upk, IMP_U32 u32Version )
{
	IMP_S32 s32Ret = 0;
	IMP_U8 *pData = pBuf->pu8Buf;
	IMP_U8 *pCur = pData;
	IMP_S32 nLen, nLen2;
	IMP_U32 u32VersCur;

	nLen = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	if( nLen > pBuf->s32BufLen )
		return 0;
	if( !ipCheckCRC( pData, sizeof(IMP_S32)+nLen ) )
		return 0;

	u32VersCur = *((IMP_U32*)pCur);
	pCur += sizeof(IMP_U32);
	if( u32VersCur != u32Version )
		return 0;

	nLen2 = func_upk( pCur, pAlgp );
	s32Ret = ((IMP_U32)nLen == nLen2 + sizeof(IMP_U32)) && ((IMP_S32)(sizeof(IMP_S32) + nLen + sizeof(IMP_U32)) == pBuf->s32DatLen);

	return s32Ret;
}

IMP_S32 IMP_RULE_PackAlgpBuffer( IMP_VOID *pAlgp, DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ,
						  IMP_FUNC_PKGALGPV func_pkgv, IMP_FUNC_PKGALGPZ func_pkgz )
{
	IMP_S32 s32Ret = 1;

	if( pstBufV )
		func_pkgv( pAlgp, (IMP_S8*)pstBufV->pu8Buf, &pstBufV->s32DatLen );

	if( pstBufZ )
		func_pkgz( pAlgp, (IMP_S8*)pstBufZ->pu8Buf, &pstBufZ->s32DatLen );

	return s32Ret;
}

IMP_S32 IMP_RULE_UnpackAlgpBuffer( DATA_BUF_S *pBufV, DATA_BUF_S *pBufZ, IMP_VOID *pAlgp,
							IMP_FUNC_UPKALGPV func_upkv, IMP_FUNC_UPKALGPZ func_upkz )
{
	IMP_S32 s32Ret = 1;

	if( pBufV )
		func_upkv( (IMP_S8*)pBufV->pu8Buf, pBufV->s32DatLen, pAlgp );

	if( pBufZ )
		func_upkz( (IMP_S8*)pBufZ->pu8Buf, pBufZ->s32DatLen, pAlgp );

	return s32Ret;
}


IMP_S32 IMP_RULE_PackAlgpFile( IMP_VOID *pAlgp, IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pMemMgr,
						  IMP_FUNC_PKGALGPV func_pkgv, IMP_FUNC_PKGALGPZ func_pkgz )
{
	IMP_S32 s32Ret;
	DATA_BUF_S stBufV, *pstBufV;
	DATA_BUF_S stBufZ, *pstBufZ;

	ipDataBufferAlloc( &stBufV, (1024 * 64), pMemMgr );
	ipDataBufferAlloc( &stBufZ, (1024 * 128), pMemMgr );

	if( ps8FilV )
		pstBufV = &stBufV;
	else
		pstBufV = NULL;
	if( ps8FilZ )
		pstBufZ = &stBufZ;
	else
		pstBufZ = NULL;

	s32Ret = IMP_RULE_PackAlgpBuffer( pAlgp, pstBufV, pstBufZ, func_pkgv, func_pkgz );

	if( pstBufV)
		ipWriteFileData( pstBufV, ps8FilV );
	if( pstBufZ)
		ipWriteFileData( pstBufZ, ps8FilZ );

	ipDataBufferFree( &stBufV, pMemMgr );
	ipDataBufferFree( &stBufZ, pMemMgr );

	return s32Ret;
}

IMP_S32 IMP_RULE_UnpackAlgpFile( IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pAlgp, IMP_VOID *pMemMgr,
							IMP_FUNC_UPKALGPV func_upkv, IMP_FUNC_UPKALGPZ func_upkz )
{
	IMP_S32 s32Ret;
	DATA_BUF_S stBufV, *pstBufV;
	DATA_BUF_S stBufZ, *pstBufZ;

	ipDataBufferAlloc( &stBufV, (1024 * 64), pMemMgr );
	ipDataBufferAlloc( &stBufZ, (1024 * 128), pMemMgr );

	if( ps8FilV )
		pstBufV = &stBufV;
	else
		pstBufV = NULL;
	if( ps8FilZ )
		pstBufZ = &stBufZ;
	else
		pstBufZ = NULL;

	if( pstBufV )
		ipReadFileData( ps8FilV, pstBufV );
	if( pstBufZ )
		ipReadFileData( ps8FilZ, pstBufZ );

	s32Ret = IMP_RULE_UnpackAlgpBuffer( pstBufV, pstBufZ, pAlgp, func_upkv, func_upkz );

	ipDataBufferFree( &stBufV, pMemMgr );
	ipDataBufferFree( &stBufZ, pMemMgr );

	return s32Ret;
}


IMP_S32 IMP_RULE_DefaultAlgp( IMP_S8 *ps8StrV, IMP_VOID *pAlgp, IMP_VOID *pMemMgr, IMP_FUNC_UPKALGPV func_upkv, IMP_FUNC_DFTALGPZ func_dftz )
{
	IMP_S32 s32Ret;

	s32Ret = func_upkv( ps8StrV, strlen(ps8StrV), pAlgp );
	func_dftz( pAlgp );

	return s32Ret;
}



IMP_VOID IMP_MakeRuleFile( SCENE_PACK_OBJ_S *pstObj )
{
	DATA_BUF_S stDatBuf;
	IMP_S32 s32ImgW = pstObj->s32ImgW;
	IMP_S32 s32ImgH = pstObj->s32ImgH;
	IMP_VOID *pMemMgr = pstObj->pMMgr;
	SCENE_FILES_S *pstScnFils = &pstObj->stScnFiles;
	IMP_VOID *pRule;

	ipDataBufferAlloc( &stDatBuf, (1024*128), pMemMgr );
	pRule = pstObj->IMP_Alloc( s32ImgW, s32ImgH, pMemMgr );
	pstObj->IMP_Clear( pRule );

	if( pstScnFils->s8IFlag=='t' &&
		( !fopen(pstScnFils->ps8FileNameVana, "rb") || !fopen(pstScnFils->ps8FileNameZone, "rb") ) )
		pstScnFils->s8IFlag = 'd';
	if( pstScnFils->s8IFlag=='b' && !fopen(pstScnFils->ps8FileNameBin, "rb") )
		pstScnFils->s8IFlag = 'd';

	if( pstScnFils->s8IFlag!='d' && pstScnFils->s8IFlag!='t' && pstScnFils->s8IFlag!='b' )
		pstScnFils->s8IFlag = 'd';
	if( pstScnFils->s8OFlag!='t' && pstScnFils->s8OFlag!='b' )
		pstScnFils->s8OFlag = 'b';

	if( pstScnFils->s8IFlag == 'd' )
	{
		pstObj->IMP_DefaultScene( pstScnFils->ps8StrVana, pRule, pMemMgr );
	}
	else if( pstScnFils->s8IFlag == 't' )
	{
		pstObj->IMP_UnpackSceneFile( pstScnFils->ps8FileNameVana, pstScnFils->ps8FileNameZone, pRule, pMemMgr );
	}
	else if( pstScnFils->s8IFlag == 'b' )
	{
		ipReadFileData( pstScnFils->ps8FileNameBin, &stDatBuf );
		pstObj->IMP_UnpackScene( &stDatBuf, pRule );

	}

	pstObj->IMP_Calculate( pRule, 0 );

	if( pstScnFils->s8OFlag == 't' )
	{
		pstObj->IMP_PackSceneFile( pRule, pstScnFils->ps8FileNameVana, pstScnFils->ps8FileNameZone, pMemMgr );
	}
	else if( pstScnFils->s8OFlag == 'b' )
	{
		pstObj->IMP_PackScene( pRule, &stDatBuf );
		if( pstObj->IMP_UnpackScene( &stDatBuf, pRule ) )
			ipWriteFileData( &stDatBuf, pstScnFils->ps8FileNameBin );
	}

	ipDataBufferFree( &stDatBuf, pMemMgr );
	pstObj->IMP_Free( pRule, pMemMgr );
}

IMP_S32 IMP_WriteVZImage( GRAY_IMAGE_S *pImage, IMP_U8 *pData )
{
	IMP_U8 *pCur = pData;
	IMP_S32 s32DatLen = 0;
	IMP_S32 x, y, w, h;
	IMP_U8 *pu8DatP, *pu8Dati;

	*((IMP_S32*)pCur) = pImage->s32W;
	pCur += sizeof(IMP_S32);

	*((IMP_S32*)pCur) = pImage->s32H;
	pCur += sizeof(IMP_S32);

	w = pImage->s32W;
	h = pImage->s32H;
	pu8DatP = pCur;
	for( y = 0; y < h; y += 2 )
	{
		pu8Dati = pImage->pu8Data + w * y;
		for( x = 0; x < w; x += 2 )
		{
			pu8DatP[0] = pu8Dati[x];
			pu8DatP++;
		}
	}
	pCur += ((w * h) >> 2);

	s32DatLen = pCur - pData;
	return s32DatLen;
}

IMP_S32 IMP_ReadVZImage( IMP_U8 *pData, GRAY_IMAGE_S *pImage )
{
	IMP_U8 *pCur = pData;
	IMP_S32 s32DatLen = 0;
	IMP_S32 x, y, w, h;
	IMP_U8 *pu8DatP, *pu8Dati;

	pImage->s32W = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	pImage->s32H = *((IMP_S32*)pCur);
	pCur += sizeof(IMP_S32);

	w = pImage->s32W;
	h = pImage->s32H;
	pu8DatP = pCur;
	for( y = 0; y < h; y += 2 )
	{
		pu8Dati = pImage->pu8Data + w * y;
		for( x = 0; x < w; x += 2 )
		{
			IMP_U8 val = pu8DatP[0];
			pu8Dati[x] = val;
			pu8Dati[x + 1] = val;
			pu8DatP++;
		}
		memcpy( pu8Dati + w, pu8Dati, w );
	}
	pCur += ((w * h) >> 2);

	s32DatLen = pCur - pData;
	return s32DatLen;
}


static IMP_S32 IMP_RULE_GetLineHeight( FDEPTH_S *pstFDepth, LINE_S *pstLine, IMP_S32 *ps32Len )
{
	IMP_S32 s32Ret = IMP_CLBR_SUCCESS;
	IMP_S32 xs, ys, xe, ye;
	IMP_S32 x, y;
	IMP_DOUBLE dResX, dResY;

	xs = pstLine->stPs.s16X;
	xe = pstLine->stPe.s16X;
	ys = pstLine->stPs.s16Y;
	ye = pstLine->stPe.s16Y;
	{
		x = xe;
		y = ye;
	}
	s32Ret = IMP_FDEPTH_GetPointResolution( pstFDepth, x, y, &dResX, &dResY );
	if( s32Ret != IMP_CLBR_SUCCESS )
		return s32Ret;

	*ps32Len = (IMP_S32)( sqrt( (xe - xs) * (xe - xs) + (ye - ys) * (ye - ys) ) * dResX );

	return s32Ret;
}

static IMP_S32 IMP_RULE_CalcFDepthCoef2D(  FDEPTH_S *pstFDepth )
{
	IMP_U32 i;


	return IMP_CLBR_SUCCESS;
}


static IMP_S32 IMP_RULE_CalcFDepth( FDEPTH_S *pstFDepth )
{
	IMP_S32 s32Ret=IMP_CLBR_SUCCESS;
	s32Ret = IMP_RULE_CalcFDepthCoef2D( pstFDepth );
	return s32Ret;
}

static IMP_S32 IMP_RULE_CalcPointResolution( FDEPTH_S *pstFDepth, IMP_S32 x, IMP_S32 y, IMP_U8 *rx, IMP_U8 *ry )
{
	FDEPTH_COEF_S *pstCoef = &pstFDepth->stParams.stCoef;
	IMP_DOUBLE res;
	IMP_U8 resolution;




	return IMP_CLBR_SUCCESS;
}


CLBR_IALG_S stClbrIAlgCv =
{
	IMP_RULE_CalcFDepth,
	IMP_RULE_CalcPointResolution,
	IMP_RULE_GetLineHeight
};
//
//////////////////////////////////////////////////////////////////////////


IMP_S32 IMP_GetMemSizeRuleAdvPara(IMP_S32 s32BufNum, IMP_S32 s32BufLen)
{
	IMP_S32 s32Size = 0;
	
	s32Size += s32BufNum * s32BufLen;
	
	return s32Size;
}


IMP_VOID IMP_RULE_AdvParaAlloc( ADVANCE_PARA_S *pstAdvPara, IMP_S32 s32BufNum, IMP_S32 s32BufLen, IMP_VOID *pMemMgr )
{
	MEM_MGR_ARRAY_S *pstMemMgr = (MEM_MGR_ARRAY_S*)pMemMgr;
	IMP_S32 i;
	memset( pstAdvPara, 0, sizeof(ADVANCE_PARA_S) );
	for( i = 0; i < s32BufNum; i++ )
		ipDataBufferAlloc( &pstAdvPara->astAdvDats[i], s32BufLen, pstMemMgr );
}

IMP_VOID IMP_RULE_AdvParaFree( ADVANCE_PARA_S *pstAdvPara, IMP_S32 s32Num, IMP_VOID *pMemMgr )
{
	MEM_MGR_ARRAY_S *pstMemMgr = (MEM_MGR_ARRAY_S*)pMemMgr;
	IMP_S32 i;
	for( i = 0; i < s32Num; i++ )
		ipDataBufferFree( &pstAdvPara->astAdvDats[i], pstMemMgr );
	memset( pstAdvPara, 0, sizeof(ADVANCE_PARA_S) );
}

IMP_VOID IMP_RULE_AdvParaClear( ADVANCE_PARA_S *pstAdvPara, IMP_S32 s32Num )
{
	IMP_S32 i;
	for( i = 0; i < s32Num; i++ )
		ipDataBufferClear( &pstAdvPara->astAdvDats[i] );
}

IMP_VOID IMP_RULE_AdvParaClone( ADVANCE_PARA_S *pstSrc, ADVANCE_PARA_S *pstDst, IMP_S32 s32BufNum, IMP_S32 s32BufLen )
{

	IMP_S32 i;

	for( i = 0; i < s32BufNum; i++ )
		ipDataBufferClone( &pstSrc->astAdvDats[i], &pstDst->astAdvDats[i] );
}


EXTERNAL_PARA_S* IMP_PARA_Alloc( IMP_S32 s32VanaBufLen,IMP_S32 s32AdvBufNum, IMP_S32 s32AdvBufLen, IMP_VOID *pMmgr )
{
	EXTERNAL_PARA_S *pstPara = IMP_MMAlloc( pMmgr, IMP_MEMBLK_TYPE_SLOW, sizeof(EXTERNAL_PARA_S) );
	memset( pstPara, 0, sizeof(EXTERNAL_PARA_S) );

	ipDataBufferAlloc( &pstPara->stRule.stVanaData, s32VanaBufLen, pMmgr );
	IMP_RULE_AdvParaAlloc( &pstPara->stAdvance, s32AdvBufNum, s32AdvBufLen, pMmgr );
	return pstPara;
}

IMP_VOID IMP_PARA_Free( EXTERNAL_PARA_S *pstPara, IMP_S32 s32AdvBufNum, IMP_VOID *pMmgr )
{
	IMP_RULE_AdvParaFree( &pstPara->stAdvance, s32AdvBufNum, pMmgr );

	ipDataBufferFree( &pstPara->stRule.stVanaData, pMmgr );
	memset( pstPara, 0, sizeof(EXTERNAL_PARA_S) );
	IMP_MMFree( pMmgr, IMP_MEMBLK_TYPE_SLOW, pstPara );
}
