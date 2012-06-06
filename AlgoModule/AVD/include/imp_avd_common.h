#ifndef __AVD_COMMON__
#define __AVD_COMMON__

//#include <cv.h>
//#include <cxcore.h>
//#include "windows.h"

#define DEBUG_AVD_SHOW_GRAY
//#define DEBUG_AVD_SHOW_EDGE
//#define DEBUG_AVD_SHOW_RGB
//#define DEBUG_AVD_SHOW_FRM_DIFF
//#define DEBUG_AVD_SHOW_FRM_LAST
#define DEBUG_AVD_SHOW_DIFF_NXT
//#define DEBUG_AVD_SHOW_YUV
//#define DEBUG_AVD_SHOW_WARNING

//#define DEBUG_AVD_SHOW_PRINTF
//#define DEBUG_AVD_ANALOG_ALARM_INFO
//#define DEBUG_AVD_DIGITAL_ALARM_INFO

//#define TIME_TEST
#ifdef TIME_TEST
    #include <sys/time.h>
#endif

#include "stdio.h"
#include "imp_algo_type.h"
//#include "imp_avd_para.h"
#include "imp_avd_conf.h"
#include "imp_avd_canny.h"
#include "imp_ga_image_mgr.h"


//#define SHOW_DEBUG_HISTOGRAM_INFO
//#define printf(...)
#define FALSE 0
#define TRUE 1

#define IMP_QCIF_PAL_IMG_WIDTH      176
#define IMP_QCIF_PAL_IMG_HEIGHT     144
#define IMP_QCIF_NTSC_IMG_WIDTH     176
#define IMP_QCIF_NTSC_IMG_HEIGHT    120
#define IMP_QCIF_PAL_UVIMG_WIDTH    176
#define IMP_QCIF_PAL_UVIMG_HEIGHT   36
#define IMP_QCIF_NTSC_UVIMG_WIDTH   176
#define IMP_QCIF_NTSC_UVIMG_HEIGHT  30


#define IP_HIST_LEVEL		32			
#define IP_HIST_LEVEL_B		3			
	

#define IP_FUNCTIONS        6		



typedef struct tagIpHistogram
{
	IMP_S32 bin[IP_HIST_LEVEL];    
	IMP_S32 max;                  
	IMP_S32 volume;	             
}IpHistogram;



typedef struct tagAVD_HISTOGRAM_GROUP
{
//	IpHistogram histPart[IP_BLOCK_CNT];		
	IpHistogram histAll;					

//	IMP_S32 meanPart[IP_BLOCK_CNT];		
	IMP_S32 meanAll;					

//	IMP_S32 stdDevPart[IP_BLOCK_CNT];	
	IMP_S32 stdDevAll;					

}AVD_HISTOGROUP;


typedef struct tagAVD_SCORE_S
{
	IMP_S32 s32Value;			
	IMP_U32 u32Type;			
	IMP_U32 u32AlarmTh;			
	IMP_U32 u32Score;			
	IMP_U32 u32FrmCnt;			
	IMP_U32 u32Ratio;			
	IMP_U8  bFlagStart;			
}AVD_SCORE_S;

typedef struct tagAVD_Upper_ANALYSIS_S
{
	AVD_SCORE_S stChangeScore;
	AVD_SCORE_S stBrightHScore;
	AVD_SCORE_S	stBrightLScore;
	AVD_SCORE_S stClarityScore;
	AVD_SCORE_S stColorScore;
	AVD_SCORE_S stNoiseScore;
	AVD_SCORE_S stNoSignalScore;
	AVD_SCORE_S stFreezeScore;
	AVD_SCORE_S stPtzScore;
	AVD_SCORE_S stInterfereScore;
}AVD_Upper_ANALYSIS_S;



typedef struct tagAVD_MODULE
{
	IMP_S32 imgW;							
	IMP_S32 imgH;							
	IMP_U8  PMode;                          
	IMP_U8  IR_mode;						
	IMP_U8  IR_mode_old;					
	IMP_U8  IR_change;						

    IMP_AVD_PARA_S Para;					
	IMP_AVD_ADVAN_PARA_S Adv_Para;			
	AVD_RESULT_S stAvdResult;				
	AVD_RESULT_S stAvdResultOld;			
	AVD_Upper_ANALYSIS_S stAvdUpperAnalysis;

	RGB_IMAGE_S RGBImg;					
	GRAY_IMAGE_S YImg;					
	GRAY_IMAGE_S UImg;					
	GRAY_IMAGE_S VImg;					

	GRAY_IMAGE_S YImgLast;				
	GRAY_IMAGE_S UImgLast;				
	GRAY_IMAGE_S VImgLast;				

	GRAY_IMAGE_S RImg;					
	GRAY_IMAGE_S GImg;					
	GRAY_IMAGE_S BImg;					
	RGB_IMAGE_S  ClrCastImg;			

	GRAY_IMAGE_S EdgeImgMag;			
	GRAY_IMAGE16_S EdgeImgGrad;			
	GRAY_IMAGE_S EdgeImg;				

	GRAY_IMAGE_S RefYImg;				
	GRAY_IMAGE_S RefUImg;				
	GRAY_IMAGE_S RefVImg;				

	GRAY_IMAGE_S YImgDiffNxt;			
	GRAY_IMAGE_S UImgDiffNxt;			
	GRAY_IMAGE_S VImgDiffNxt;			

	GRAY_IMAGE_S YImgDiffNxt2;			

	GRAY_IMAGE_S GrayImgDiff;			
//	GRAY_IMAGE_S GrayImgDiff2;			
#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	GRAY_IMAGE_S GrayImgDiff3;			
#endif
	GRAY_IMAGE_S UImgDiff;				
	GRAY_IMAGE_S VImgDiff;				
 
	AVD_HISTOGROUP histGrayImg;			
	AVD_HISTOGROUP histGrayImgSmth;		
	AVD_HISTOGROUP histGrayImgSmthEdge;	

	AVD_HISTOGROUP histYImgDiffNxt;		
	AVD_HISTOGROUP histUImgDiffNxt;		
	AVD_HISTOGROUP histVImgDiffNxt;		
	AVD_HISTOGROUP histYImgDiffNxt2;	

	AVD_HISTOGROUP histGrayImgBG;		
	AVD_HISTOGROUP histGrayImgFG;		
	AVD_HISTOGROUP histGrayImgDiff;		

#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	AVD_HISTOGROUP histGrayImgDiff3;	
#endif
	AVD_HISTOGROUP histEdgeImg;			

	AVD_HISTOGROUP histBImg;			
	AVD_HISTOGROUP histGImg;			
	AVD_HISTOGROUP histRImg;			

	AVD_HISTOGROUP histUImg;			
	AVD_HISTOGROUP histVImg;			
	AVD_HISTOGROUP histUImgDiff;		
	AVD_HISTOGROUP histVImgDiff;		

	IMP_S32 flag_change;				
	IMP_S32 smthScale;					

	IMP_S32 signalValue;				
	IMP_S32 clarityValue;				
	IMP_S32 brightHValue;				
	IMP_S32 brightLValue;				
	IMP_S32 colorValue;					
	IMP_S32 noiseValue;					
	IMP_S32 ptzValue;					
	IMP_S32 freezeValue;				
	IMP_S32 interfereValue;				

	IMP_S32 flag_RefImg;				

//	IMP_S32 warnValue[IP_BLOCK_CNT];	
	IMP_S32 prcFrmCnt;					

	IMP_U8  clrLine;					

}AVD_MODULE;

IMP_HANDLE AVD_Initialize(IMP_S32 width ,IMP_S32 height);
IMP_VOID AVD_Terminate(AVD_MODULE* avdModule);
IMP_VOID AVD_Transform(AVD_MODULE* avdModule,IMP_U32 u32ExtScnChg);
IMP_S32 AVD_2ThTrans(IMP_S32 thL, IMP_S32 thH, IMP_S32 input);

//void ipCalHist(GRAY_IMAGE_S* pImg, AVD_HISTOGROUP* pHistGroup, IMP_S32 flag_0);
void ipCalHist2(GRAY_IMAGE_S* pImg, AVD_HISTOGROUP* pHistGroup, IMP_S32 flag_0);
void AVD_rgb2yuv444( IMP_U8 *rgbDat, IMP_U8 *yDat, IMP_U8 *uDat, IMP_U8 *vDat, IMP_U16 width, IMP_U16 heigth);
void ipSplit( IMP_U8 *rgbDat, IMP_U8 *bDat, IMP_U8 *gDat, IMP_U8 *rDat, IMP_U16 width, IMP_U16 height);
void ipGray2RGB( IMP_U8 *grayDat, IMP_U8 *rgbDat, IMP_U16 width, IMP_U16 height);

#endif
