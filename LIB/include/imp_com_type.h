
#ifndef _IMP_COM_TYPE_H_
#define _IMP_COM_TYPE_H_

#include "imp_algo_type.h"


#ifdef __cplusplus
extern "C"
{
#endif


#define IMP_BUF_SIZE_CONTROL	(64-32)
#define IMP_BUF_SIZE_EVENT		(64-32)
#define IMP_BUF_SIZE_VPRO		(16*1024-32)
#define IMP_BUF_SIZE_CONFIG		(128*1024-32)

#define IMP_BUFLEN_PACKET	(1024*512)

#define IMP_EVENT_ID_SLAVE_MIN	0x00000000
#define IMP_EVENT_ID_SLAVE_MAX	0xFFFFFFFF

// 状态码
typedef enum _STATUS_E
{
	IMP_STATUS_SUCCESS				= 0x00000000,
	IMP_STATUS_COMMAND				= 0x00010000,
	IMP_STATUS_ERROR				= 0x00080000,
	IMP_STATUS_ERROR_NOT_FOUND		= 0x00080001,
	IMP_STATUS_ERROR_NOT_SUPPORTED	= 0x00080002
} IMP_STATUS_E;

// 操作通用数据
typedef struct impOPER_DATA_S
{
	IMP_U32	u32Size;							// 数据包长度
	IMP_U32	u32Type;							// 类型
	IMP_U32	u32Status;						// 状态
	IMP_U32	u32Channel;						// 通道号
	IMP_U32	u32Time;							// 时间
	IMP_U32	u32Date;							// 日期
	IMP_U32	u32Id;								// 标识
	IMP_U32	u32Level;							// 警报级别
} IMP_OPER_DATA_S;


///////////////////////////////////////////////////////////////////////////////////////////////////
// 控制相关数据类型

// 控制类型
typedef enum impCONTROL_TYPE_E
{
	IMP_CONTROL_TYPE_BASE = 0x1000,

	IMP_CONTROL_TYPE_VDR_Arm					= 0x1000,			// 启动警戒
	IMP_CONTROL_TYPE_VDR_Arm_Rsp				= 0x1001,			// 启动警戒应答

	IMP_CONTROL_TYPE_VDR_Disarm					= 0x1002,			// 解除警戒
	IMP_CONTROL_TYPE_VDR_Disarm_Rsp			    = 0x1003,			// 解除警戒应答

	IMP_CONTROL_TYPE_VDR_ClearAlarm			    = 0x1004,			// 清除警报
	IMP_CONTROL_TYPE_VDR_ClearAlarm_Rsp     	= 0x1005,			// 清除警报应答

	IMP_CONTROL_TYPE_END
} IMP_CONTROL_TYPE_E;

typedef struct
{
	IMP_U32 u32AlgoModule;
}CONTROL_DATA_S;

typedef union impCONTROL_DATA_INTERNAL_U
{
	IMP_U8 au8Buffer[IMP_BUF_SIZE_CONTROL];
	CONTROL_DATA_S stControlData;
} IMP_CONTROL_DATA_INTERNAL_U;

typedef struct impIMP_CONTROL_DATA_S
{
	IMP_OPER_DATA_S stHead;						// 操作头
	IMP_CONTROL_DATA_INTERNAL_U unData;			// 控制数据
} IMP_CONTROL_DATA_S;

// 控制操作相关数据类型
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// 事件相关数据类型

// 事件数据格式

typedef enum impEVENT_TYPE_E
{
	IMP_EVENT_TYPE_BASE = 0x2000,

	IMP_EVENT_TYPE_PEA_EVENT	= 0x2001,		// PEA事件集合
	IMP_EVENT_TYPE_AVD_EVENT	= 0x2002,		// AVD事件集合
	IMP_EVENT_TYPE_VFD_EVENT	= 0x2004,		// VFD事件集合
    IMP_EVENT_TYPE_AAI_EVENT	= 0x2008,		// VFD事件集合

	IMP_EVENT_TYPE_END
} IMP_EVENT_TYPE_E;

typedef union impEVENT_DATA_INTERNAL_U
{
	IMP_U8 au8Buffer[IMP_BUF_SIZE_EVENT];
	EVT_ITEM_S	stDataEvent;						// 事件
} IMP_EVENT_DATA_INTERNAL_U;

typedef struct impEVENT_DATA_S
{
	IMP_OPER_DATA_S stHead;						// 操作头
	IMP_EVENT_DATA_INTERNAL_U unData;				// 事件数据
} IMP_EVENT_DATA_S;

// 事件相关数据类型
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// 视频处理相关数据类型

// 视频处理类型
typedef enum impVPRO_TYPE_E
{
	IMP_VPRO_TYPE_BASE = 0x3000,

	IMP_VPRO_TYPE_PEA_TARGET_SET	= 0x3001,		// 视频分析目标
	IMP_VPRO_TYPE_AVD_TARGET_SET	= 0x3002,		// 视频分析目标
	IMP_VPRO_TYPE_VFD_TARGET_SET	= 0x3004,		// 视频分析目标
    IMP_VPRO_TYPE_AAI_TARGET_SET	= 0x3005,

	IMP_VPRO_TYPE_END
} IMP_VPRO_TYPE_E;

// 视频处理数据格式

typedef union impVPRO_DATA_INTERNAL_U
{
	IMP_U8 au8Buffer[IMP_BUF_SIZE_VPRO];
	TGT_SET_S	stTargetSet;					// 目标集合
} IMP_VPRO_DATA_INTERNAL_U;

typedef struct impVPRO_DATA_S
{
	IMP_OPER_DATA_S stHead;						// 操作头
	IMP_VPRO_DATA_INTERNAL_U unData;				// 视频处理数据
} IMP_VPRO_DATA_S;

// 视频处理相关数据类型
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
// 配置相关数据类型

typedef enum impCONFIG_TYPE_E
{
	IMP_CONFIG_TYPE_BASE = 0x4000,

	IMP_CONFIG_TYPE_SetAlgoModule								= 0x4000,		// 设置算法模块
	IMP_CONFIG_TYPE_GetAlgoModule								= 0x4001,		// 获取算法模块

	IMP_CONFIG_TYPE_SetVideoSourceMode					= 0x4100,		// 设置视频输入制式
	IMP_CONFIG_TYPE_GetVideoSourceMode					= 0x4101,		// 获取视频输入制式
	IMP_CONFIG_TYPE_SetSysTime									= 0x4102,		// 设置系统时间
	IMP_CONFIG_TYPE_GetSysTime									= 0x4103,		// 获取系统时间
	IMP_CONFIG_TYPE_SetSecretCode								= 0x4104,		// 设置系统的加密芯片值
	IMP_CONFIG_TYPE_GetSecretCode								= 0x4105,		// 获取系统的加密芯片值

	IMP_CONFIG_TYPE_SetAlgoLibInfo							= 0x4200,		// 设置算法库信息
	IMP_CONFIG_TYPE_GetAlgoLibInfo							= 0x4201,		// 获取算法库信息

	IMP_CONFIG_TYPE_SetPeaRulePara							= 0x4300,		// 设置PEA视频分析规则
	IMP_CONFIG_TYPE_GetPeaRulePara							= 0x4301, 	// 获取PEA视频分析规则
	IMP_CONFIG_TYPE_SetAvdRulePara							= 0x4302,		// 设置AVD视频分析规则
	IMP_CONFIG_TYPE_GetAvdRulePara							= 0x4303, 	// 获取AVD视频分析规则
	IMP_CONFIG_TYPE_SetVfdRulePara							= 0x4304,		// 设置VFD视频分析规则
	IMP_CONFIG_TYPE_GetVfdRulePara							= 0x4305, 	// 获取VFD视频分析规则
    IMP_CONFIG_TYPE_SetAaiRulePara							= 0x4306,		// 设置VFD视频分析规则
	IMP_CONFIG_TYPE_GetAaiRulePara							= 0x4307,

	IMP_CONFIG_TYPE_SetPeaAdvancePara						= 0x4400,		// 设置PEA视频分析高级参数
	IMP_CONFIG_TYPE_GetPeaAdvancePara						= 0x4401, 	// 获取PEA视频分析高级参数
	IMP_CONFIG_TYPE_SetAvdAdvancePara						= 0x4402,		// 设置AVD视频分析高级参数
	IMP_CONFIG_TYPE_GetAvdAdvancePara						= 0x4403, 	// 获取AVD视频分析高级参数
	IMP_CONFIG_TYPE_SetVfdAdvancePara						= 0x4404,		// 设置VFD视频分析高级参数
	IMP_CONFIG_TYPE_GetVfdAdvancePara						= 0x4405, 	// 获取VFD视频分析高级参数
    IMP_CONFIG_TYPE_SetAaiAdvancePara						= 0x4406,		// 设置VFD视频分析高级参数
	IMP_CONFIG_TYPE_GetAaiAdvancePara						= 0x4407, 	// 获取VFD视频分析高级参数

	IMP_CONFIG_TYPE_END
} IMP_CONFIG_TYPE_E;

// 配置数据格式

typedef union
{
	IMP_U8 au8Buffer[IMP_BUF_SIZE_CONFIG];
} IMP_CONFIG_DATA_INTERNAL_U;

typedef struct  impCONFIG_DATA_S
{
	IMP_OPER_DATA_S stHead;				// 操作头
	IMP_CONFIG_DATA_INTERNAL_U unData;		// 配置数据
} IMP_CONFIG_DATA_S;

typedef enum impVIDEO_STREAM_E
{
	IMP_VIDEO_STREAM_TYPE_BASE = 0x5000,

	IMP_VIDEO_STREAM_TYPE_VENC_H264									= 0x5000,		//
	IMP_VIDEO_STREAM_TYPE_VENC_MJPEG								= 0x5001,		//
	IMP_VIDEO_STREAM_TYPE_VENC_MPEG2								= 0x5002,		//
	IMP_VIDEO_STREAM_TYPE_VENC_MPEG4								= 0x5003		//

}IMP_VIDEO_STREAM_E;


#ifdef __cplusplus
}
#endif


#endif


