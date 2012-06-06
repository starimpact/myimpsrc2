
#include "imp_avd_common.h"
#include "imp_avd_bright.h"


void AVD_brightAnalysis( AVD_MODULE* avdModule )
{
	IMP_S32 ratio1,ratio2;
//	IMP_S32 ratio3; //百分比
	IMP_S32 highVolume = 0;
	IMP_S32 lowVolume = 0;
	IMP_S32 allVolume;
	IMP_S32 highThrd;
	IMP_S32 lowThrd;
	IMP_S32 i;
//	IMP_S32 j;
	AVD_HISTOGROUP* histImg = &avdModule->histGrayImg;
	IMP_S32* bin;
//	IMP_S32* warnValue = avdModule->warnValue;
//	IMP_S32 sum;
	IMP_FLOAT para1,para2;
//	IMP_FLOAT para3;
	IMP_S32 mean,para_mean_l,para_mean_h;
	IMP_U8 level = avdModule->Para.stBrightAbnmlPara.u32AlarmLevel;
	IMP_U8 mode = avdModule->IR_mode;

	if(mode == IR_MODE)
	{
		para1 = 0.0f; // 衡量端头数量的百分比
		para2 = 0.99f; // 衡量整幅均值的偏斜程度
		para_mean_l = AVD_BRIGHT_MEAN_L_TH_IR;
		para_mean_h = AVD_BRIGHT_MEAN_H_TH_IR;
		highThrd = AVD_BRIGHT_HT_IR;
		lowThrd = AVD_BRIGHT_LT_IR;
	}
	else
	{
		para1 = 0.3f; // 衡量端头数量的百分比
		para2 = 0.7f; // 衡量整幅均值的偏斜程度
		para_mean_l = AVD_BRIGHT_MEAN_L_TH;
		para_mean_h = AVD_BRIGHT_MEAN_H_TH;
		highThrd = AVD_BRIGHT_HT;
		lowThrd = AVD_BRIGHT_LT;
	}


	bin = histImg->histAll.bin;
	allVolume = histImg->histAll.volume;
	highThrd = max(highThrd,0);
	for (i=highThrd;i<IP_HIST_LEVEL;i++)
	{
		highVolume += bin[i];
	}
	ratio1 = highVolume*100/allVolume;


	mean = histImg->meanAll;
	ratio2 = (mean-para_mean_h)*100/(abs(para_mean_h-128)+0.000001f);
	ratio2 = max(ratio2,0);


	avdModule->brightHValue = ratio1*para1 + ratio2*para2;
	
//	avdModule->brightHValue = ratio1*para1 + ratio2*para2 + ratio3*para3;
//	AVD_WarningShowInBlocks(avdModule, WARNING_BRIGHT_HIGH);
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("ratio1=%d,ratio2=%d\n",ratio1,ratio2);
	printf("BrightH = %d\n",avdModule->brightHValue);
#endif
	avdModule->brightHValue = AVD_2ThTrans(	avdModule->Adv_Para.u8BrightHThrd[mode][level][TH1],
											avdModule->Adv_Para.u8BrightHThrd[mode][level][TH2],
											avdModule->brightHValue);
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("BrightH'= %d\n",avdModule->brightHValue);
#endif

	bin = histImg->histAll.bin;
	allVolume = histImg->histAll.volume;
	lowThrd = min(lowThrd,IP_HIST_LEVEL);
	for (i=0;i<lowThrd;i++)
	{
		lowVolume += bin[i];
	}
	ratio1 = lowVolume*100/allVolume;


	mean = histImg->meanAll;
 	ratio2 = (para_mean_l-mean)*100/(abs(128-para_mean_l)+0.000001f);
 	ratio2 = max(ratio2,0);


	avdModule->brightLValue = ratio1*para1 + ratio2*para2;
//	avdModule->brightLValue = ratio1*para1 + ratio2*para2 + ratio3*para3;
//	AVD_WarningShowInBlocks(avdModule, WARNING_BRIGHT_LOW);
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("ratio1=%d,ratio2=%d\n",ratio1,ratio2);
	printf("BrightL = %d\n",avdModule->brightLValue);
#endif

	avdModule->brightLValue = AVD_2ThTrans(	avdModule->Adv_Para.u8BrightLThrd[mode][level][TH1],
											avdModule->Adv_Para.u8BrightLThrd[mode][level][TH2],
											avdModule->brightLValue);
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("BrightL'= %d\n",avdModule->brightLValue);
#endif
	return;
}


