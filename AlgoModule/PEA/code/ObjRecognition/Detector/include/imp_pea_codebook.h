#include "imp_pea_detector.h"

#ifndef _IMP_PEA_CODEBOOK_H_
#define _IMP_PEA_CODEBOOK_H_


#define DEFAULT_LI_BGFG_MOG_INTERFRAME            3

#define DEFAULT_LI_BGFG_MOG_WINDOW_SIZE           200

#define DEFAULT_LI_BGFG_MOG_MOTION_THRESHOLD	  3

#define DEFAULT_LI_BGFG_MOG_SUBTRACTION_THRESHOLD_H 15

#define DEFAULT_LI_BGFG_MOG_SUBTRACTION_THRESHOLD_M 7
#define DEFAULT_LI_BGFG_MOG_SUBTRACTION_THRESHOLD_L 5

#define DEFAULT_LI_BGFG_MOG_TIME_UPDATE_LENGTH    3

#define DEFAULT_LI_BGFG_MOG_SCALE_OF_UPDATING     0.3
#define DEFAULT_LI_BFFG_MOG_GRADIENT_POINT_SCALE 0.1

#define DEFAULT_LI_BGFG_MOG_LUMINANCE_LOW_THRESHOLD   70

#define DEFAULT_LI_BGFG_MOG_LUMINANCE_HIGH_THRESHOLD  160

#define DEFAULT_LI_BGFG_MOG_T                     3


typedef struct IpCodebookBgModelPara
{
	IMP_S32 s32ThresholdCur;
	IMP_S32 s32ThresholdMin;
	IMP_S32 s32ThresholdMax;
    IMP_S32 s32ThresholdUdpm;
	IMP_S32 s32ThresholdNoiseRatio;
    IMP_S32 s32ThresholdUp2DwRatio;
	IMP_DOUBLE dGradientPointScale;
	IMP_DOUBLE dLumiThreshold;
	IMP_U8  u8UpdateSpeedMax;
    IMP_U8  u8LearnRate;


	IMP_S32 s32TAdd;
	IMP_S32 s32TCatch;

}IpCodebookBgModelPara;


#define CB_BOUNDS 10
#define MIN_MOD 10
#define MAX_MOD 10


#define MAX_CODEWORDS_LEN 2

#define UPDATE_SPEED     100 //5

#define LEARN_RATE       2//2


#define CHANNELS 1
#define DEFAULT_LEARN_RATIO 0.2
#define MAX_LEARN_RATIO 2.5
#define MIN_LEARN_RATIO 0.01

#define TWO_LAYER_CACHE

#define	TCHANGE   150

#define	TADD      300

#define TDEL      500

typedef struct ce
{
	IMP_U8 learnHigh;
	IMP_U8 learnLow;
	IMP_U8 max;
	IMP_U8 min;

	IMP_FLOAT learnRatio;
	IMP_S32 t_last_update;
} code_element;



typedef struct code_book
{
	code_element cb[MAX_CODEWORDS_LEN];
} codeBook;
typedef struct IpCodeBookModel
{
	codeBook codeBookBasic;
	codeBook codeBookCache;
}IpCodeBookModel;


typedef struct impPEA_MULTIMODEL_BGMODELING_S
{
    GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
    IpCodebookBgModelPara   stPara;
	IMP_S32 s32Frames;
	IpCodeBookModel *cbModel;
	IMP_S32 tBasic;
	IMP_S32 tCache;
	IMP_S32 s32Th;
	IMP_S32 s32TAdd;
	IMP_S32 s32TDel;

	GRAY_IMAGE_S      stImgBg;
	GRAY_IMAGE_S      stImgFg;
	GRAY_IMAGE_S      stImgFgCb;
	GRAY_IMAGE_S      stImgFgCbCache;

	GRAY_IMAGE_S      stImgD;
	GRAY_IMAGE_S      stImgDPre;
	GRAY_IMAGE_S      stImgGradReAppearFq;
	GRAY_IMAGE_S      stImgReAppearMap;
	HISTORY_IMAGE_S	  stImgHist;
	IMP_S32           s32FrmCnt;
	IMP_S32			  s32UdpmTimePre;
	IMP_S32           s32NoiseArea;
	IMP_S32           s32NoiseAreaAvg;
	IMP_S32           s32NoiseAdjustThCnt;
}IMP_PEA_MULTIMODEL_BGMODELING_S;


IMP_VOID ipCreateCodebookBgModel( IMP_PEA_MULTIMODEL_BGMODELING_S *pModule, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );

IMP_VOID ipReleaseCodebookBgModel( IMP_PEA_MULTIMODEL_BGMODELING_S *pModule );

IMP_VOID ipConfigCodebookBgModel( IMP_PEA_MULTIMODEL_BGMODELING_S *pModule, IpCodebookBgModelPara *pstPara );

IMP_S32 ipProcessMultiModelBgModeling( IMP_PEA_MULTIMODEL_BGMODELING_S *pModule);

IMP_VOID ipPostProcessCodebookBgModel( IMP_PEA_MULTIMODEL_BGMODELING_S *pModule );

#endif


