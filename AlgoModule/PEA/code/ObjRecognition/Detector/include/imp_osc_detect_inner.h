


#ifndef _IMP_OSC_DETECT_INNER_H_
#define _IMP_OSC_DETECT_INNER_H_


#include "imp_osc_detect.h"
#include "imp_pea_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define OSCD_DBG_SHW 0
#define OSCD_DBG_SHW_TIME 0

#define OSCD_SOBJ_ELEM_LEN 128

#define OSCD_SOBJ_SET_CANDIDATE(x) (x |= 0x1)
#define OSCD_SOBJ_IS_CANDIDATE(x) (x & 0x1)
#define OSCD_SOBJ_DEL_CANDIDATE(x) (x = x & (~0x1))

#define OSCD_SOBJ_SET_REALOBJ(x) (x |= 0x2)
#define OSCD_SOBJ_IS_REALOBJ(x) (x & 0x2)
#define OSCD_SOBJ_DEL_REALOBJ(x) (x = x & (~0x2))

#define OSCD_ROBJ_SET_CANDIDATE(x) (x |= 0x4)
#define OSCD_ROBJ_IS_CANDIDATE(x) (x & 0x4)
#define OSCD_ROBJ_DEL_CANDIDATE(x) (x = x & (~0x4))

#define OSCD_ROBJ_SET_REALOBJ(x) (x |= 0x8)
#define OSCD_ROBJ_IS_REALOBJ(x) (x & 0x8)
#define OSCD_ROBJ_DEL_REALOBJ(x) (x = x & (~0x8))

#define OSCD_MAX_TGT_CNT 32


typedef enum impSOBJ_TYPE_S
{
	SOBJT_UKN, //unknown
	SOBJT_RMV, //remove
	SOBJT_STP  //stop
}IMP_SOBJ_TYPE_S;

typedef struct impSTATIC_OBJ_S
{
	IMP_U8 u8Used; //if used
	IMP_U8 u8Sign; //区域标号
	IMP_U32 u32TargetId; //object id
	IMP_U32 u32Life; //object's life
	IMP_U32 u32EvtLife; //event life
	IMP_S32 s32InRgn; //if in a region
	
	IMP_SOBJ_TYPE_S stSOBJType; //
	IMP_SOBJ_TYPE_S astSOBJT[OSCD_SOBJ_ELEM_LEN]; //
	IMP_POINT_S astPos[OSCD_SOBJ_ELEM_LEN]; //position
	IMP_U16 au16MatchScore[OSCD_SOBJ_ELEM_LEN]; //历史上,目标区域匹配分数.
	IMP_U16 au16AreaSize[OSCD_SOBJ_ELEM_LEN]; //历史上,目标区域大小.
	IMP_POINT_S stPos;
	IMP_S32 s32PosLen; //position length
	IMP_RECT_S stRect;  //rectangle
	IMP_U32 u32AreaSize; //object area size in pixel
	IMP_U32 u32MatchScore;
}IMP_STATIC_OBJ_S;


typedef struct impSOBJ_SET_S
{
	IMP_STATIC_OBJ_S aSObjs[OSCD_MAX_TGT_CNT];
}IMP_SOBJ_SET_S;


typedef struct impOSCD_REGION_S
{
	IMP_S32 s32Valid;
	IMP_S8 as8Name[IMP_NAME_BUF_SIZE];
	POLYGON_REGION_S stRgn;
} OSCD_REGION_S;


#define OSCD_R_MAX_NUM (IMP_MAX_OSC_NUM * 8)

//算法数据存放
typedef struct impOSCD_S
{
	//系统成员
    GA_HARDWARE_RS_S  *pstHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult; //系统公共数据
	RULE_S *pstRule; //规则配置信息,主要是需要里面的ＯＳＣ的规则.
	IMP_S32 s32First;
//	IMP_OutputViBe_S *pstOutput;
	
	//可以用户配置的参数
	IMP_S32 s32MinSize; //最小目标大小(像素).
	IMP_S32 s32MaxSize; //最大目标大小(像素).
	IMP_S32 s32AlertTime; //报警时间(帧).
	OSCD_REGION_S astRMVRgns[OSCD_R_MAX_NUM]; //remove detect regions
	
	//算法私有自定义成员
	IMP_U32 u32IDCounter; //object id counter.
	IMP_U32 u32ObjNumber;
	
	GRAY_IMAGE_S stImgFgStatic; //static binary image
	GRAY_IMAGE_S stImgFgStatic2; //static binary image
	GRAY_IMAGE_S stImgSBkg; //background for static detect
	GRAY_IMAGE_S stImgPreGray; //pre gray image
	GRAY_IMAGE_S stImgSObjSign; //static object sign image
	GRAY_IMAGE_S stImgSObjEdge; //static object edge image
	
	GRAY_IMAGE_S stImgRObjSign; //remove object sign image
	GRAY_IMAGE_S stImgRObjEdge; //remove object edge image
	
	GRAY_IMAGE_S stImgBkgEdge;
	
	GRAY_IMAGE_S stImgGrayEdge;
	
	IMP_S32 s32Width;
	IMP_S32 s32Height;
	
	IMP_S32 s32FrmNum;
	
	IMP_U32 u32ObjIDCounter; //generate id
	IMP_SOBJ_SET_S stSObjSet;
	PEA_DETECTED_REGIONSET_S stRgs;
	
	IMP_U16 au16MatchMatrix[OSCD_MAX_TGT_CNT * IMP_MAX_TGT_CNT]; //match matrix between static area sign image and static object sign image
}IMP_OSCD_S;



IMP_S32 impCmp_SArea_SOBJSign(IMP_OSCD_S *pstModule);

IMP_STATIC_OBJ_S *impGetSObjBySign(IMP_STATIC_OBJ_S *pstSObj, IMP_U8 u8Sign);
//detect edges of background and gray image.
IMP_S32 impDetectEdges(IMP_OSCD_S *pstModule);

//去除独立性差的目标
IMP_S32 impCheckIndependence_All(IMP_OSCD_S *pstModule, PEA_DETECTED_REGIONSET_S *pstRgs);

//match real static object with current frame
IMP_S32 impMatchSObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);

//find wanted region by u8Sign
PEA_DETECTED_REGION_S *impGetRgnBySign(PEA_DETECTED_REGION_S *pstDrg, IMP_U8 u8Sign);


//get empty object
IMP_STATIC_OBJ_S *impGetEmptyObj(IMP_STATIC_OBJ_S *pSObjs, IMP_U32 *pu32IDCounter);

IMP_S32 impReplaceObject(IMP_OSCD_S *pstModule, PEA_DETECTED_REGION_S *pstRgn, IMP_STATIC_OBJ_S *pstObj);

//delete object
IMP_S32 impDeleteObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);

//remove detection
IMP_S32 impRMVDetect(IMP_OSCD_S *pstModule);


//normalized cross correlation
IMP_S32 impNCC_U8(IMP_U8 *pu8Data1, IMP_U8 *pu8Data2, IMP_S32 s32DataLen);

IMP_S32 impNCC_U8_(IMP_U8 *pu8Data1, IMP_U8 *pu8Data2, IMP_S32 s32DataLen);

//find wanted static object by u8Sign
IMP_STATIC_OBJ_S *impGetRObjBySign(IMP_STATIC_OBJ_S *pstSObj, IMP_U8 u8Sign);

//check motion type
IMP_SOBJ_TYPE_S impCheckRMotion(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);

//update the static object's position and type
IMP_S32 impUpdateRPosition(IMP_STATIC_OBJ_S *pstObj);

//match static object with current frame, and update
IMP_S32 impMatchRObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);

IMP_S32 impOutPutResult(IMP_OSCD_S *pstModule);

//delete object
IMP_S32 impDeleteRObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);


IMP_S32 impOSCD_Clear(IMP_OSCD_S *pstModule);

//check if has inter-frame motion in the object area
IMP_S32 impCheckIFM(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);


#ifdef __cplusplus
}
#endif

#endif










