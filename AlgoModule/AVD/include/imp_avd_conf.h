
//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 北京银瀑技术有限公司
///    All rights reserved.
///
/// @file avd_conf.h
/// @brief AVD系列 - 参数模块（塔顶）
///
/// AVD信号缺失检测函数及其相关
///
/// @version 1.0
/// @author 银瀑技术
/// @date 2010.12.26
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#include "imp_avd_para.h"

#ifndef _IMP_AVD_CONF_H_
#define _IMP_AVD_CONF_H_

typedef struct impAVD_ADVAN_PARA_S
{
	IMP_U8 u8NoiseThrd[MODE_CNT][LEVEL_CNT+1][THRSHLD_CNT];		///< 噪声检测双阈值
	IMP_U8 u8BrightHThrd[MODE_CNT][LEVEL_CNT+1][THRSHLD_CNT];	///< 过亮检测双阈值
	IMP_U8 u8BrightLThrd[MODE_CNT][LEVEL_CNT+1][THRSHLD_CNT];	///< 过暗检测双阈值
	IMP_U8 u8ClarityThrd[MODE_CNT][LEVEL_CNT+1][THRSHLD_CNT];	///< 清晰度检测双阈值
	IMP_U8 u8ColorThrd[MODE_CNT][LEVEL_CNT+1][THRSHLD_CNT];		///< 偏色检测双阈值
	
	IMP_U8 u8FreezePara[MODE_CNT][LEVEL_CNT+1][PARA_CNT];		///< 画面冻结检测高级参数
	IMP_U8 u8NosignalPara[MODE_CNT][LEVEL_CNT+1][PARA_CNT];		///< 信号缺失检测高级参数
	IMP_U8 u8ChangePara[MODE_CNT][LEVEL_CNT+1][PARA_CNT];		///< 场景变换检测高级参数
	IMP_U8 u8InterferePara[MODE_CNT][LEVEL_CNT+1][PARA_CNT];	///< 人为干扰检测高级参数
	IMP_U8 u8PtzLoseCtlPara[MODE_CNT][LEVEL_CNT+1][PARA_CNT];	///< PTZ失控检测高级参数
}
IMP_AVD_ADVAN_PARA_S;

void AVD_setDefaultUserParameter( IMP_AVD_PARA_S* pstUsrPara );
void AVD_checkUserParameter( IMP_AVD_PARA_S* pstUsrPara );
void AVD_setAdvancedParameter( IMP_AVD_PARA_S* pstUsrPara, IMP_AVD_ADVAN_PARA_S* pstAdvPara );

#endif /*_IMP_AVD_CONF_H_*/
