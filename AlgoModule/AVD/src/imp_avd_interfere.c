

#include "imp_avd_interfere.h"

#define AVD_INTERFR_BRIGHT_H_TH 30
#define AVD_INTERFR_BRIGHT_L_TH 30
#define AVD_INTERFR_CLARITY_TH_L 15
#define AVD_INTERFR_CLARITY_TH_H 70
#define AVD_INTERFR_MIN_CNT 3
#define AVD_INTERFR_ALARM_CNT 2


void AVD_interfereAnalysis( AVD_MODULE* avdModule, IMP_U32 u32ExtScnChg)
{
	IMP_S32 ratio=0;
	static IMP_S32 startFlag = 0;
	static IMP_S32 alarmStartFlag = 0;
	IMP_S32 flag_change = avdModule->flag_change;

	IMP_S32 brightHValue = avdModule->brightHValue;
	IMP_S32 brightLValue = avdModule->brightLValue;
	IMP_S32 clarityValue = avdModule->clarityValue;
	IMP_S32 level = avdModule->Para.stInterferePara.u32AlarmLevel;
	IMP_S32 clartityTH = AVD_INTERFR_CLARITY_TH_H - (AVD_INTERFR_CLARITY_TH_H-AVD_INTERFR_CLARITY_TH_L)/4*(level-1);

	static IMP_S32 cnt = 0;

;

	if ( (flag_change && !u32ExtScnChg) && clarityValue > clartityTH)   // 场景变换触发分析
	{
		startFlag = AVD_INTERFR_ALARM_CNT;
	}
	else
	{
        startFlag--;
    }
	ratio = 100;

    if(startFlag <= 0)
    {
        startFlag = 0;
        ratio = 0;
    }


	avdModule->interfereValue = ratio;
	return;
}
