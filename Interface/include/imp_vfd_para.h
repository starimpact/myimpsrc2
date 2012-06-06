/**
* \defgroup 结构类型定义
* VFD算法配置参数定义
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2011
*/
/*@{*/
#ifndef _IMP_VFD_PARA_H_
#define _IMP_VFD_PARA_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct impVFD_FACE_CAPTURE_PARA_S
{
	IMP_U32 u32Enable;			///< 人脸抓拍功能开关，0为关闭，1为开启
	IMP_U32 u32AlarmTime;		///< 人脸抓拍报警时间，（0-20秒），数值越大需要的报警时间越长
}IMP_VFD_FACE_CAPTURE_PARA_S;

typedef struct impVFD_FACE_CAMOUFLAGE_PARA_S
{
	IMP_U32 u32Enable;			///< 人脸伪装功能开关，0为关闭，1为开启
	IMP_U32 u32AlarmLevel;		///< 人脸伪装报警灵敏度，1-5档，数值越大越灵敏越易报警
	IMP_U32 u32AlarmTime;		///< 人脸伪装报警时间，（0-20秒），数值越大需要的报警时间越长
}IMP_VFD_FACE_CAMOUFLAGE_PARA_S;

typedef struct impVFD_PASSWORD_PEEP_PARA_S
{
	IMP_U32 u32Enable;			///< 密码偷窥功能开关，0为关闭，1为开启
	IMP_U32 u32AlarmLevel;		///< 密码偷窥报警灵敏度，1-5档，数值越大越灵敏越易报警
	IMP_U32 u32AlarmTime;		///< 密码偷窥报警时间，（0-20秒），数值越大需要的报警时间越长
}IMP_VFD_PASSWORD_PEEP_PARA_S;

typedef struct impVFD_SCENE_MODE_PARA_S
{
	IMP_U32 IR_mode;				///< 模式切换，0为可见光模式，1为红外模式
}IMP_VFD_SCENE_MODE_PARA_S;

typedef struct impVFD_PARA_S
{
	IMP_VFD_SCENE_MODE_PARA_S stTrigModePara;				///< 场景模式参数
	IMP_VFD_FACE_CAPTURE_PARA_S stFaceCapturePara;			///< 人脸抓拍参数
	IMP_VFD_FACE_CAMOUFLAGE_PARA_S stFaceCamouflagePara;	///< 人脸伪装参数
	IMP_VFD_PASSWORD_PEEP_PARA_S stPasswordPeepPara;		///< 密码偷窥参数
}IMP_VFD_PARA_S;

#ifdef __cplusplus
}
#endif

#endif /*_IMP_VFD_PARA_H_*/

/*@}*/
