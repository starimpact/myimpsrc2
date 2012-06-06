

#include "imp_avd_common.h"
#include "imp_avd_freeze.h"



void AVD_freezeAnalysis( AVD_MODULE* avdModule )
{
	IMP_S32 value = avdModule->histYImgDiffNxt2.histAll.bin[IP_HIST_LEVEL-1];  
	IMP_S32 w = avdModule->YImg.s32W;
	IMP_S32 h = avdModule->YImg.s32H;
	IMP_S32 ratio;
	IMP_S32 level = avdModule->Para.stFreezePara.u32AlarmLevel;
	static IMP_S32 freezeFrm;
	if(avdModule->flag_change)
	{
		freezeFrm = 0;
	}
	//value = value*100 /(w * h);
	//printf("cnt=%d,freezeLevel= %d\n",value,level);
#ifdef DEBUG_AVD_SHOW_PRINTF
    printf("cnt=%d\n",value);
#endif
	if (avdModule->prcFrmCnt <= AVD_FREEZE_NO_PROCESS_FRM) 
	{
		ratio = 0;
		freezeFrm = 0;
	}
	else
	{
		if(level == LEVEL5) 
		{
            if (value > AVD_FREEZE_PIXEL_CNT*30)
                freezeFrm = 0;
            else
                freezeFrm ++;
		}
		else if (level == USER_LEVEL)  //用户设置
		{
			if ( value > AVD_FREEZE_PIXEL_CNT ) //用户值
				freezeFrm = 0;
			else
				freezeFrm ++;			
		}
		else //LEVEL 1-4
		{
			if (value > AVD_FREEZE_PIXEL_CNT*(level-1))
				freezeFrm = 0;
			else
				freezeFrm ++;	
		}
	}

	if (freezeFrm >= AVD_FREEZE_MIN_FRM)
		ratio = 100;				
	else
		ratio = 0;					

#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Freeze = %d\n",ratio);
#endif
	avdModule->freezeValue = ratio;
	return;
}


