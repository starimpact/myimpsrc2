/**
* \defgroup 结构类型定义
* AVD算法配置参数定义
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2011
*/
/*@{*/
#ifndef _IMP_AVD_PARA_H_
#define _IMP_AVD_PARA_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MODE_CNT    2		// 场景模式个数
#define LEVEL_CNT	5		// 灵敏度级别个数，自定义级别为0档，不包括于此
#define THRSHLD_CNT 2		// 得分变化阈值个数
#define PARA_CNT	1		// 检测功能调节参数个数

typedef enum impIR_STATE_E
{
	NORMAL_MODE=0,
	IR_MODE
}IR_STATE_E;			// 红外状态枚举

typedef enum impIR_SWITCH_E
{
	EXT_IR_CHG=0,
	AUTO_IR_CHG
}IR_SWITCH_E;			// “红外切换”开关状态枚举

typedef enum impSCN_CHG_SIGNAL_E
{
	NO_SCN_CHANGE=0,
	SCN_CHANGE
}SCN_CHG_SIGNAL_E;		// “场景切换”控制信号枚举

typedef enum impSCN_CHG_SWITCH_E
{
	EXT_SCN_CHG=0,
	AUTO_SCN_CHG,
	BOTH_SCN_CHG
}SCN_CHG_SWITCH_E;		// “场景切换”开关状态枚举
/*
typedef enum impTRANS_THRSHLDS_E
{
	TH1=0,
	TH2
}impTRANS_THRSHLDS_E;	// 转换阈值枚举

typedef enum impTRANS_PARAS_E
{
	PARA1=0,
}TRANS_PARAS_E;			// 转换参数枚举

typedef enum impSENSITY_E
{
	USER_LEVEL=0,
	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4,
	LEVEL5
}SENSITY_E;			    // 敏感度级别枚举
*/
typedef struct impAVD_BRIGHT_ABMNL_PARA_S
{
	IMP_U32 u32Enable;			// Brightness abormal detection Enable. 0 - disable, 1 - enable. Default 1
	IMP_U32 u32AlarmLevel;		///< 亮度异常检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 亮度异常报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
	IMP_U32 u32BHUsrNmLTP1;	///< 过亮检测可见光模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32BHUsrNmLTP2;	///< 过亮检测可见光模式下用户自定义双阈值第2阈值，默认为60
	IMP_U32 u32BHUsrIrLTP1;		///< 过亮检测红外模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32BHUsrIrLTP2;		///< 过亮检测红外模式下用户自定义双阈值第2阈值，默认为60
	IMP_U32 u32BLUsrNmLTP1;	///< 过暗检测可见光模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32BLUsrNmLTP2;	///< 过暗检测可见光模式下用户自定义双阈值第2阈值，默认为60
	IMP_U32 u32BLUsrIrLTP1;	///< 过暗检测红外模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32BLUsrIrLTP2;	///< 过暗检测红外模式下用户自定义双阈值第2阈值，默认为60
}IMP_AVD_BRIGHT_ABMNL_PARA_S;

typedef struct impAVD_CLARITY_PARA_S
{
	IMP_U32 u32Enable;			// Clarity abormal detection Enable. 0 - disable, 1 - enable. Default 1
	IMP_U32 u32AlarmLevel;		///< 清晰度检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 清晰度检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
	IMP_U32 u32UsrNmLTP1;		///< 清晰度检测可见光模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32UsrNmLTP2;		///< 清晰度检测可见光模式下用户自定义双阈值第2阈值，默认为60
	IMP_U32 u32UsrIrLTP1;		///< 清晰度检测红外模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32UsrIrLTP2;		///< 清晰度检测红外模式下用户自定义双阈值第2阈值，默认为60
}IMP_AVD_CLARITY_PARA_S;

typedef struct impAVD_NOISE_PARA_S
{
	IMP_U32 u32Enable;			// Noise abormal detection Enable. 0 - disable, 1 - enable. Default 1
	IMP_U32 u32AlarmLevel;		///< 噪声检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 噪声检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
	IMP_U32 u32UsrNmLTP1;		///< 噪声检测可见光模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32UsrNmLTP2;		///< 噪声检测可见光模式下用户自定义双阈值第2阈值，默认为60
	IMP_U32 u32UsrIrLTP1;		///< 噪声检测红外模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32UsrIrLTP2;		///< 噪声检测红外模式下用户自定义双阈值第2阈值，默认为60
}IMP_AVD_NOISE_PARA_S;

typedef struct impAVD_COLOR_PARA_S
{
	IMP_U32 u32Enable;			///< 偏色检测功能开关，0为关闭，1为开启，默认为1
	IMP_U32 u32AlarmLevel;		///< 偏色检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 偏色检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
	IMP_U32 u32UsrNmLTP1;		///< 偏色检测可见光模式下用户自定义双阈值第1阈值，默认为30
	IMP_U32 u32UsrNmLTP2;		///< 偏色检测可见光模式下用户自定义双阈值第2阈值，默认为60
}IMP_AVD_COLOR_PARA_S;

typedef struct impAVD_FREEZE_PARA_S
{
	IMP_U32 u32Enable;			///< 画面冻结检测功能开关，0为关闭，1为开启，默认为1
	IMP_U32 u32AlarmLevel;		///< 画面冻结检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 画面冻结检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
}IMP_AVD_FREEZE_PARA_S;

typedef struct impAVD_NOSIGNAL_PARA_S
{
	IMP_U32 u32Enable;			///< 信号缺失检测功能开关，0为关闭，1为开启，默认为1
	IMP_U32 u32AlarmLevel;		///< 信号缺失检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 信号缺失检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
}IMP_AVD_NOSIGNAL_PARA_S;

typedef struct impAVD_CHANGE_PARA_S
{
	IMP_U32 u32Enable;			///< 场景变换检测功能开关，0为关闭，1为开启，默认为1
	IMP_U32 u32AlarmLevel;		///< 场景变换检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
}IMP_AVD_CHANGE_PARA_S;

typedef struct impAVD_INTERFERE_PARA_S
{
	IMP_U32 u32Enable;			///< 人为干扰检测功能开关，0为关闭，1为开启，默认为1
	IMP_U32 u32AlarmLevel;		///< 人为干扰检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< 人为干扰检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
}IMP_AVD_INTERFERE_PARA_S;

typedef struct impAVD_PTZ_LOSE_CTL_PARA_S
{
	IMP_U32 u32Enable;			///< PTZ失控检测功能开关，0为关闭，1为开启，默认为1
	IMP_U32 u32AlarmLevel;		///< PTZ失控检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警，第0档为用户自定义模式，默认档位为3
	IMP_U32 u32AlarmTime;		///< PTZ失控检测报警时间，（0-20秒），数值越大需要的报警时间越长，默认为5秒
}IMP_AVD_PTZ_LOSE_CTL_PARA_S;

typedef struct impAVD_TRIG_MODE_PARA_S
{
	IMP_U32 u32IrTrigMode;		///< 红外切换触发方式，0为手动触发方式，1为自动触发方式，默认为1
	IMP_U32 u32ScnTrigMode;		///< 场景切换触发方式，0为外部触发方式，1为内部自动触发方式，2为共同触发方式，默认为2
}IMP_AVD_TRIG_MODE_PARA_S;

typedef struct impAVD_IR_MODE_PARA_S
{
	IMP_U32 u32BalanceLevel;	///< 可见光-红外自动切换平衡级别（1-5级），1为倾向于被判断为可见光模式，5为倾向于被判断为红外模式，默认为3
}IMP_AVD_IR_MODE_PARA_S;

typedef struct impEXT_TRIGGERS_S
{
	IMP_U32 u32ExtScnChg;		///< 外部场景切换触发，0为不触发切换，1为触发切换
	IMP_U32 u32ExtIrState;		///< 外部红外切换触发，0为切换为可见光状态，1为切换为红外状态
}EXT_TRIGGERS_S;


typedef struct impAVD_PARA_S
{
	IMP_AVD_TRIG_MODE_PARA_S stTrigModePara;			///< 触发方式参数
	IMP_AVD_IR_MODE_PARA_S stIrModePara;				///< 红外模式检测参数
	IMP_AVD_BRIGHT_ABMNL_PARA_S stBrightAbnmlPara;		///< 亮度异常检测参数
	IMP_AVD_CLARITY_PARA_S stClarityPara;				///< 清晰度检测参数
	IMP_AVD_NOISE_PARA_S stNoisePara;					///< 噪声检测参数
	IMP_AVD_COLOR_PARA_S stColorPara;					///< 偏色检测参数
	IMP_AVD_FREEZE_PARA_S stFreezePara;					///< 画面冻结检测参数
	IMP_AVD_NOSIGNAL_PARA_S stNosignalPara;				///< 信号缺失检测参数
	IMP_AVD_CHANGE_PARA_S stChangePara;					///< 场景变换检测参数
	IMP_AVD_INTERFERE_PARA_S stInterferePara;			///< 人为干扰检测参数
	IMP_AVD_PTZ_LOSE_CTL_PARA_S stPtzLoseCtlPara;		///< PTZ失控检测参数
}IMP_AVD_PARA_S;


#ifdef __cplusplus
}
#endif

#endif /*_IMP_AVD_PARA_H_*/

/*@}*/
