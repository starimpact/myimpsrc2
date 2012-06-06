
#include "imp_avd_change.h"


IMP_S32 AVD_changeAnalysis( AVD_MODULE* avdModule )
{
	AVD_HISTOGROUP * pHistGrayImgDiff = &avdModule->histGrayImgDiff;
	IMP_S32 val = pHistGrayImgDiff->histAll.bin[IP_HIST_LEVEL-1];  //二值图的最后一个bin条内存储前景像素点个数
	IMP_S32 w = avdModule->YImg.s32W;
	IMP_S32 h = avdModule->YImg.s32H;
	IMP_S32 changeFrm = 0;
	IMP_S32 ratio = 0;

	if (avdModule->prcFrmCnt <= AVD_CHANGE_NO_PROCESS_FRM) //前N帧 不做处理
	{
		ratio = 0;
		changeFrm = 0;
	}
	else
	{
		if (val * 100 > AVD_CHANGE_RATIO_TH * w * h  )
			changeFrm ++;				// 累积变动帧
		else
			changeFrm = 0;
	}

	if (changeFrm >= AVD_CHANGE_MIN_FRM)
		ratio = 100;					// 报警
	else
		ratio = 0;					// 解除报警

    //printf("val=%d,%d,%d,%d,%d\n",val,w,h,changeFrm,ratio);
	return ratio;
}


#define AVD_IR_NO_PROCESS_FRM 5
#define AVD_IR_MIN_FRM 20
IMP_BOOL AVD_irAnalysis( AVD_MODULE* avdModule )
{
	AVD_HISTOGROUP * pHistUImg = &avdModule->histUImg;
	AVD_HISTOGROUP * pHistVImg = &avdModule->histVImg;
	AVD_HISTOGROUP * pHistRImg = &avdModule->histRImg;
	AVD_HISTOGROUP * pHistGImg = &avdModule->histGImg;
	AVD_HISTOGROUP * pHistBImg = &avdModule->histBImg;

	IMP_S32 UMean = pHistUImg->meanAll;
	IMP_S32 VMean = pHistVImg->meanAll;

	IMP_S32 RMean = pHistRImg->meanAll;
	IMP_S32 GMean = pHistGImg->meanAll;
	IMP_S32 BMean = pHistBImg->meanAll;

	IMP_S32 w = avdModule->imgW;
	IMP_S32 h = avdModule->imgH;
	IMP_S32 diff1,diff2,diff3,i;
	IMP_S32 vr;
	static IMP_S32 irFrm = AVD_IR_MIN_FRM/2;
	IMP_S32 result;

	//R,G,B三均值和方差相近，且U,V的均值在128附近，均值在3个bin条内
	if (avdModule->prcFrmCnt <= AVD_IR_NO_PROCESS_FRM) //前N帧 不做处理
	{
		result = 0;
//		irFrm = AVD_IR_MIN_FRM/2;
		diff1 = 0;
		diff2 = 0;
		diff3 = 0;
		vr = 0;
	}
	else
	{
		diff1 = abs(UMean-128)+abs(VMean-128);
		diff2 = abs(RMean-BMean)+abs(RMean-GMean)+abs(BMean-GMean);
		diff3 = 0;

		for (i=0;i<IP_HIST_LEVEL;i++)
		{
			diff3  = abs(pHistRImg->histAll.bin[i]-pHistGImg->histAll.bin[i]);
			diff3 += abs(pHistRImg->histAll.bin[i]-pHistBImg->histAll.bin[i]);
			diff3 += abs(pHistGImg->histAll.bin[i]-pHistBImg->histAll.bin[i]);
		}

		vr = pHistUImg->stdDevAll + pHistVImg->stdDevAll;

		if ( diff1<=3 && diff2<=14 && diff3<=200 && vr<=2)
		{
			irFrm ++;				// 累积变动帧
		}
		else
		{
			irFrm --;
		}

		if (irFrm > AVD_IR_MIN_FRM)
		{
			irFrm = AVD_IR_MIN_FRM;
		}
		if (irFrm < 0)
		{
			irFrm = 0;
		}
	}

	if (irFrm >= AVD_IR_MIN_FRM/2)
		result = IR_MODE;					// 红外
	else
		result = NORMAL_MODE;				// 可见光

//    printf("IR=%d irFrm=%d diff1=%d diff2=%d diff3=%d vr=%d\n",result,irFrm,diff1,diff2,diff3,vr);
	return result;
}
