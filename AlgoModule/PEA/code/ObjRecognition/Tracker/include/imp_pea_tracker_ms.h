
#ifndef _MOTION_MEANSHIFT_H_
#define _MOTION_MEANSHIFT_H_

#include "imp_pea_tracker_common.h"
#include "imp_pea_tracker_para.h"

#ifdef DEBUG_TRACKER_MMS
#include "windows.h"
#endif

#define SHOW_ITER_WEIGHT			0

#define MMS_LEVEL					32
#define MMS_LEVEL_B					3
#define MMS_IMG_SIZE_BIT			1
#define MMS_MAX_SUBDRG				4

#define MMS_FORBIDDEN_BAND			10
#define MMS_FORBIDDEN_FRM			5


typedef struct tagMMS_MOMENTS
{
	IMP_U32 m00;
	IMP_U32 m10;
	IMP_U32 m01;
	IMP_U32 m20;
	IMP_U32 m02;
}MMS_MOMENTS;


typedef struct tagMMS_HIST
{
	IMP_U32 hist[MMS_LEVEL];
	IMP_U32 Volume;
	IMP_U32 cntValid;
	IMP_U32 cnt;
} MMS_HIST;


typedef struct tagMMS_INTERFACE
{

	IMP_U32 tgtID;
	IMP_U8  sn;

	IMP_RECT_S    rg;
	IMP_POINT_S   pt;
	IMP_POINT_S   pre_pt;
	MMS_HIST	  HM;
	MMS_HIST	  HM_new;
	IMP_FLOAT     fMatchCoefficient;
	GRAY_IMAGE_S* image_io_rgn_template;
	GRAY_IMAGE_S* FG_Img;
	IMP_S32		  area_pixel;
	IMP_S32		  flag_FGValid;
	IMP_S32		  FG_frm_cnt;
} MMS_INTERFACE;

typedef struct tagMMS_TRACKER
{

	IMP_U32  iterCnt;
	MMS_HIST  HC;

	IMP_U32   *pu32FrmTimeCur;
	GA_HARDWARE_RS_S *pstHwResource;
	GRAY_IMAGE_S  *testImg;

	IMP_U8* kernelHist;
	IMP_U8* kernelMeanShift;
	IMP_U8* weightImg;

	GRAY_IMAGE_S *gray_last;
	GRAY_IMAGE_S *fg_last;
	GRAY_IMAGE_S *gray_cur;
	GRAY_IMAGE_S *fg_cur;

	IMP_U32 bin_ratio[MMS_LEVEL];
	PEA_DETECTED_REGIONSET_S *pstDRegionSet;
	IMP_U8  sub_drg[MMS_MAX_SUBDRG];
}MMS_TRACKER;


#define Z_IMAGE_ELEM( image, elemtype, row, col )    (((elemtype*)((image)->pu8Data + (image)->s32W*(row)))[(col)])
#define HIST_INDEX(_pData) ((_pData)[0]>>MMS_LEVEL_B)

IMP_VOID MMS_initialize(MMS_TRACKER* mmsTkr);
IMP_VOID MMS_process(MMS_TRACKER* mmsTkr, MMS_INTERFACE * mmsIF, IpTrackerPara *pstPara);
IMP_VOID MMS_terminate(MMS_TRACKER* mmsTkr);

#endif
