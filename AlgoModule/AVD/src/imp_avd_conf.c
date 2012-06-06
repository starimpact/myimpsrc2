
#include "imp_avd_conf.h"

void AVD_setDefaultUserParameter( IMP_AVD_PARA_S* pstUsrPara )
{

	memset( pstUsrPara,0,sizeof(IMP_AVD_PARA_S));


	pstUsrPara->stTrigModePara.u32IrTrigMode = 0;

	pstUsrPara->stTrigModePara.u32ScnTrigMode = 1;



	pstUsrPara->stBrightAbnmlPara.u32Enable = 1;

	pstUsrPara->stBrightAbnmlPara.u32AlarmLevel = 3;

	pstUsrPara->stBrightAbnmlPara.u32AlarmTime = 1;

	pstUsrPara->stClarityPara.u32Enable = 1;

	pstUsrPara->stClarityPara.u32AlarmLevel = 3;

	pstUsrPara->stClarityPara.u32AlarmTime = 1;

	pstUsrPara->stColorPara.u32Enable = 1;

	pstUsrPara->stColorPara.u32AlarmLevel = 3;

	pstUsrPara->stColorPara.u32AlarmTime = 1;


	pstUsrPara->stNoisePara.u32Enable = 1;

	pstUsrPara->stNoisePara.u32AlarmLevel = 3;

	pstUsrPara->stNoisePara.u32AlarmTime = 1;


	pstUsrPara->stFreezePara.u32Enable = 1;

	pstUsrPara->stFreezePara.u32AlarmLevel = 3;

	pstUsrPara->stFreezePara.u32AlarmTime = 1;


	pstUsrPara->stNosignalPara.u32Enable = 1;

	pstUsrPara->stNosignalPara.u32AlarmLevel = 3;

	pstUsrPara->stNosignalPara.u32AlarmTime = 1;


	pstUsrPara->stChangePara.u32Enable = 1;

	pstUsrPara->stChangePara.u32AlarmLevel = 3;


	pstUsrPara->stInterferePara.u32Enable = 1;

	pstUsrPara->stInterferePara.u32AlarmLevel = 3;

	pstUsrPara->stInterferePara.u32AlarmTime = 1;


	pstUsrPara->stPtzLoseCtlPara.u32Enable = 1;

	pstUsrPara->stPtzLoseCtlPara.u32AlarmLevel = 3;

	pstUsrPara->stPtzLoseCtlPara.u32AlarmTime = 5;


	{

		pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd2 = 60;
		pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd2 = 60;


		pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd2 = 60;
		pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd2 = 60;


		pstUsrPara->stClarityPara.u32UsrNmThrd1 = 30;
		pstUsrPara->stClarityPara.u32UsrNmThrd2 = 60;
		pstUsrPara->stClarityPara.u32UsrIrThrd1 = 30;
		pstUsrPara->stClarityPara.u32UsrIrThrd2 = 60;


		pstUsrPara->stColorPara.u32UsrNmThrd1 = 30;
		pstUsrPara->stColorPara.u32UsrNmThrd2 = 60;
		pstUsrPara->stColorPara.u32UsrIrThrd1 = 30;
		pstUsrPara->stColorPara.u32UsrIrThrd2 = 60;


		pstUsrPara->stNoisePara.u32UsrNmThrd1 = 30;
		pstUsrPara->stNoisePara.u32UsrNmThrd2 = 60;
		pstUsrPara->stNoisePara.u32UsrIrThrd1 = 30;
		pstUsrPara->stNoisePara.u32UsrIrThrd2 = 60;
	}


}


void AVD_checkUserParameter( IMP_AVD_PARA_S* pstUsrPara )
{
		if (pstUsrPara->stBrightAbnmlPara.u32AlarmLevel > 5 || pstUsrPara->stBrightAbnmlPara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stBrightAbnmlPara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stBrightAbnmlPara.u32AlarmTime > 20 || pstUsrPara->stBrightAbnmlPara.u32AlarmTime < 0 )
	{
		pstUsrPara->stBrightAbnmlPara.u32AlarmTime = 1;
	}

	if ( pstUsrPara->stClarityPara.u32AlarmLevel > 5 || pstUsrPara->stClarityPara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stClarityPara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stClarityPara.u32AlarmTime > 20 || pstUsrPara->stClarityPara.u32AlarmTime < 0 )
	{
		pstUsrPara->stClarityPara.u32AlarmTime = 1;
	}


	if ( pstUsrPara->stColorPara.u32AlarmLevel > 5 || pstUsrPara->stColorPara.u32AlarmLevel < 0)
	{
		pstUsrPara->stColorPara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stColorPara.u32AlarmTime > 20 || pstUsrPara->stColorPara.u32AlarmTime < 0 )
	{
		pstUsrPara->stColorPara.u32AlarmTime = 1;
	}


	if ( pstUsrPara->stNoisePara.u32AlarmLevel > 5 || pstUsrPara->stNoisePara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stNoisePara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stNoisePara.u32AlarmTime > 20 || pstUsrPara->stNoisePara.u32AlarmTime < 0 )
	{
		pstUsrPara->stNoisePara.u32AlarmTime = 1;
	}


	if ( pstUsrPara->stFreezePara.u32AlarmLevel > 5 || pstUsrPara->stFreezePara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stFreezePara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stFreezePara.u32AlarmTime > 20 || pstUsrPara->stFreezePara.u32AlarmTime < 0 )
	{
		pstUsrPara->stFreezePara.u32AlarmTime = 1;
	}


	if ( pstUsrPara->stNosignalPara.u32AlarmLevel > 5 || pstUsrPara->stNosignalPara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stNosignalPara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stNosignalPara.u32AlarmTime > 20 || pstUsrPara->stNosignalPara.u32AlarmTime < 0 )
	{
		pstUsrPara->stNosignalPara.u32AlarmTime = 1;
	}


	if ( pstUsrPara->stChangePara.u32AlarmLevel > 5  || pstUsrPara->stChangePara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stChangePara.u32AlarmLevel = 3;
	}


	if (pstUsrPara->stInterferePara.u32AlarmLevel > 5 || pstUsrPara->stInterferePara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stInterferePara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stInterferePara.u32AlarmTime > 20 || pstUsrPara->stInterferePara.u32AlarmTime < 0 )
	{
		pstUsrPara->stInterferePara.u32AlarmTime = 1;
	}


	if (pstUsrPara->stPtzLoseCtlPara.u32AlarmLevel > 5 || pstUsrPara->stPtzLoseCtlPara.u32AlarmLevel < 0 )
	{
		pstUsrPara->stPtzLoseCtlPara.u32AlarmLevel = 3;
	}

	if ( pstUsrPara->stPtzLoseCtlPara.u32AlarmTime > 20 || pstUsrPara->stPtzLoseCtlPara.u32AlarmTime < 0 )
	{
		pstUsrPara->stPtzLoseCtlPara.u32AlarmTime = 5;
	}


	if (pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd2 - pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd1 < 1)
	{
		pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd2 = 60;
	}
	if (pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd2 - pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd1 < 1)
	{
		pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd2 = 60;
	}


	if ( pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd2 - pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd1 < 1)
	{
		pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd2 = 60;
	}
	if(pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd2 - pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd1 < 1)
	{
		pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd1 = 30;
		pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd2 = 60;
	}


	if (pstUsrPara->stClarityPara.u32UsrNmThrd2 - pstUsrPara->stClarityPara.u32UsrNmThrd1 < 1)
	{
		pstUsrPara->stClarityPara.u32UsrNmThrd1 = 30;
		pstUsrPara->stClarityPara.u32UsrNmThrd2 = 60;
	}
	if (pstUsrPara->stClarityPara.u32UsrIrThrd2 - pstUsrPara->stClarityPara.u32UsrIrThrd1 < 1)
	{
		pstUsrPara->stClarityPara.u32UsrIrThrd1 = 30;
		pstUsrPara->stClarityPara.u32UsrIrThrd2 = 60;
	}

	if (pstUsrPara->stColorPara.u32UsrNmThrd2 - pstUsrPara->stColorPara.u32UsrNmThrd1 < 1)
	{
		pstUsrPara->stColorPara.u32UsrNmThrd1 = 30;
		pstUsrPara->stColorPara.u32UsrNmThrd2 = 60;
	}
	if (pstUsrPara->stColorPara.u32UsrIrThrd2 - pstUsrPara->stColorPara.u32UsrIrThrd1 < 1)
	{
		pstUsrPara->stColorPara.u32UsrIrThrd1 = 30;
		pstUsrPara->stColorPara.u32UsrIrThrd2 = 60;
	}


	if (pstUsrPara->stNoisePara.u32UsrNmThrd2 - pstUsrPara->stNoisePara.u32UsrNmThrd1 < 1)
	{
		pstUsrPara->stNoisePara.u32UsrNmThrd1 = 30;
		pstUsrPara->stNoisePara.u32UsrNmThrd2 = 60;
	}
	if (pstUsrPara->stNoisePara.u32UsrIrThrd2 - pstUsrPara->stNoisePara.u32UsrIrThrd1 < 1)
	{
		pstUsrPara->stNoisePara.u32UsrIrThrd1 = 30;
		pstUsrPara->stNoisePara.u32UsrIrThrd2 = 60;
	}

}


void AVD_setAdvancedParameter( IMP_AVD_PARA_S* pstUsrPara, IMP_AVD_ADVAN_PARA_S* pstAdvPara )
{

	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8BrightHThrd[IR_MODE][LEVEL5][TH2] = 30;


	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][LEVEL5][TH2] = 30;

	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8BrightLThrd[IR_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8ClarityThrd[IR_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8ColorThrd[IR_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL1][TH1] = 60;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL1][TH2] = 120;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL2][TH1] = 42;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL2][TH2] = 85;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL3][TH1] = 30;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL3][TH2] = 60;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL4][TH1] = 21;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL4][TH2] = 42;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL5][TH1] = 15;
	pstAdvPara->u8NoiseThrd[IR_MODE][LEVEL5][TH2] = 30;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][USER_LEVEL][TH1] = pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd1;
	pstAdvPara->u8BrightHThrd[NORMAL_MODE][USER_LEVEL][TH2] = pstUsrPara->stBrightAbnmlPara.u32BHUsrNmThrd2;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][USER_LEVEL][TH1] = pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd1;
	pstAdvPara->u8BrightLThrd[NORMAL_MODE][USER_LEVEL][TH2] = pstUsrPara->stBrightAbnmlPara.u32BLUsrNmThrd2;

	pstAdvPara->u8BrightHThrd[IR_MODE][USER_LEVEL][TH1] = pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd1;
	pstAdvPara->u8BrightHThrd[IR_MODE][USER_LEVEL][TH2] = pstUsrPara->stBrightAbnmlPara.u32BHUsrIrThrd2;
	pstAdvPara->u8BrightLThrd[IR_MODE][USER_LEVEL][TH1] = pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd1;
	pstAdvPara->u8BrightLThrd[IR_MODE][USER_LEVEL][TH2] = pstUsrPara->stBrightAbnmlPara.u32BLUsrIrThrd2;


	pstAdvPara->u8ClarityThrd[NORMAL_MODE][USER_LEVEL][TH1] = pstUsrPara->stClarityPara.u32UsrNmThrd1;
	pstAdvPara->u8ClarityThrd[NORMAL_MODE][USER_LEVEL][TH2] = pstUsrPara->stClarityPara.u32UsrNmThrd2;
	pstAdvPara->u8ClarityThrd[IR_MODE][USER_LEVEL][TH1] = pstUsrPara->stClarityPara.u32UsrIrThrd1;
	pstAdvPara->u8ClarityThrd[IR_MODE][USER_LEVEL][TH2] = pstUsrPara->stClarityPara.u32UsrIrThrd2;


	pstAdvPara->u8ColorThrd[NORMAL_MODE][USER_LEVEL][TH1] = pstUsrPara->stColorPara.u32UsrNmThrd1;
	pstAdvPara->u8ColorThrd[NORMAL_MODE][USER_LEVEL][TH2] = pstUsrPara->stColorPara.u32UsrNmThrd2;
	pstAdvPara->u8ColorThrd[IR_MODE][USER_LEVEL][TH1] = pstUsrPara->stColorPara.u32UsrIrThrd1;
	pstAdvPara->u8ColorThrd[IR_MODE][USER_LEVEL][TH2] = pstUsrPara->stColorPara.u32UsrIrThrd2;


	pstAdvPara->u8NoiseThrd[NORMAL_MODE][USER_LEVEL][TH1] = pstUsrPara->stNoisePara.u32UsrNmThrd1;
	pstAdvPara->u8NoiseThrd[NORMAL_MODE][USER_LEVEL][TH2] = pstUsrPara->stNoisePara.u32UsrNmThrd2;
	pstAdvPara->u8NoiseThrd[IR_MODE][USER_LEVEL][TH1] = pstUsrPara->stNoisePara.u32UsrIrThrd1;
	pstAdvPara->u8NoiseThrd[IR_MODE][USER_LEVEL][TH2] = pstUsrPara->stNoisePara.u32UsrIrThrd2;



}
