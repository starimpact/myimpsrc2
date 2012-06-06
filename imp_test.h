/** 
* \defgroup IMP库使用例程
* IMP库使用例程，见imp_test.h文件
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2010
*/
/*@{*/

#include "imp_algo_type.h"
#include "imp_algo_interface.h"
#include "imp_algo_urp_param.h"

URP_PARA_S stURPpara;
static void IMP_PARA_Config( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH )
{
	EXTERNAL_PARA_S *pstPara = IMP_PARA_Alloc( IMP_PARA_RULE_BUFLEN,
		IMP_PARA_ADVBUF_BUFCNT, IMP_PARA_ADVBUF_BUFLEN, NULL );
	
	memset(&stURPpara,0,sizeof(URP_PARA_S));
	{
		stURPpara.stConfigPara.s32ImgW = 352;
		stURPpara.stConfigPara.s32ImgH = 288;
		stURPpara.stRuleSet.astRule[0].u32Enable = 1;
		stURPpara.stRuleSet.astRule[0].u32Valid = 1;
		stURPpara.stRuleSet.astRule[0].u32Mode |= IMP_FUNC_PERIMETER;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32Mode = IMP_URP_PMODE_INTRUSION;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeHuman = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeVehicle = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.s32DirectionLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.s32ForbiddenDirection = 180;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.s32MinDist = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.s32MinTime = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.s32BoundaryPtNum = 4;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[0].s16X = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[0].s16Y = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[1].s16X = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[1].s16Y = 270;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[2].s16X = 340;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[2].s16Y = 270;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[3].s16X = 340;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.strLimitPara.stBoundary.astBoundaryPts[3].s16Y = 10;
		
		stURPpara.stRuleSet.astRule[1].u32Enable = 1;
		stURPpara.stRuleSet.astRule[1].u32Valid = 1;
		stURPpara.stRuleSet.astRule[1].u32Mode |= IMP_FUNC_TRIPWIRE;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeHuman =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeVehicle =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLimitPara.s32MinDist=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLimitPara.s32MinTime=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32ForbiddenDirection=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32IsDoubleDirection=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32Valid=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16X=232;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16Y=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16X=232;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16Y=270;

	}
	IMP_PARA_Init( pstPara, NULL, NULL, s32ImgW, s32ImgH, NULL );	
	IMP_ConfigArmPeaParameter( hModule, pstPara ,&stURPpara );
	IMP_PARA_Free( pstPara, IMP_PARA_ADVBUF_BUFCNT, NULL );
}


static void IMP_Example()
{
	OBJ_S stObj;
	LIB_INFO_S stLibInfo;
	IMP_HANDLE hIMP = &stObj;
	MEM_SET_S stMems;
	YUV_IMAGE422_S stImage;
	RESULT_S stResult;
	IMP_S32 s32Width, s32Height;

	s32Width = 352; 
	s32Height = 288;
	IMP_YUVImage422Create( &stImage, s32Width, s32Height, NULL );

	// 获取库信息
	IMP_GetAlgoLibInfo( hIMP, &stLibInfo );

	// 获取内存需求
	stMems.u32ImgW = s32Width;
	stMems.u32ImgH = s32Height;
	IMP_GetMemReq( hIMP, &stMems );

	// 初始化模块
	IMP_MemsAlloc( &stMems );
	if(IMP_Create( hIMP, &stMems ) != IMP_STATUS_OK)
	   exit(0);

	// 配置参数
	IMP_PARA_Config(hIMP,s32Width,s32Height);

	// 启动模块
	IMP_Start( hIMP );

	while( 1 )
	{
		// 处理一帧图像
		IMP_ProcessImage( hIMP, &stImage );

		// 获取结果数据
		IMP_GetResults( hIMP, &stResult );
	}

	// 停止模块
	IMP_Stop( hIMP, &stResult );

	// 终止模块
	IMP_Release( hIMP );
	IMP_MemsFree( &stMems );
	IMP_YUVImage422Destroy( &stImage, NULL );
}

/*@}*/

