
#include "imp_pea_detector.h"

#ifndef _IMP_PEA_WATERMARK_H_
#define _IMP_PEA_WATERMARK_H_


#ifdef __cplusplus
extern "C"
{
#endif


#define X_SPACE 5
#define Y_SPACE 3
#define VLINE_WIDTH 5
#define HLINE_HEIGHT 5
#define Y_PART1_HEIGHT 5
#define Y_PART2_HEIGHT 15
#define CHAR_WIDTH 25
#define CHAR_HEIGHT (Y_PART1_HEIGHT+Y_SPACE+Y_PART2_HEIGHT)


typedef struct
{
	IMP_POINT_S P0;
	IMP_RECT_S I[2];
	IMP_RECT_S M[4];
	IMP_RECT_S P[2];
	IMP_RECT_S O[2];
	IMP_RECT_S W[4];
	IMP_RECT_S E[4];
	IMP_RECT_S R[3];
	IMP_S32 rectNum;
} IpWaterMarkDetectorPara;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource; /**< 资源 */
	PEA_RESULT_S *pstResult;         /**< 处理结果 */
	IpWaterMarkDetectorPara stPara;
	IMP_U8 *ps8LinePixels;
	IMP_S32 s32Confidence;
	IMP_U32 u32Colomn;
	IMP_U32 u32FrmCnt;
	IMP_S32 s32Precf;
}IpWaterMarkDetector;


IMP_VOID ipCreateWaterMarkDetector( IpWaterMarkDetector *module, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );

IMP_VOID ipReleaseWaterMarkDetector( IpWaterMarkDetector *module );

IMP_VOID ipConfigWaterMarkDetector( IpWaterMarkDetector *module, IpWaterMarkDetectorPara *stPara );

IMP_S32 ipProcessWaterMarkDetector( IpWaterMarkDetector *module );

IMP_S32 ipPostProcessWaterMarkDetector( IpWaterMarkDetector *module );


#ifdef __cplusplus
}
#endif

#endif

