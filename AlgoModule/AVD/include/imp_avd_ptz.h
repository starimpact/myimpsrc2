
//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 北京银瀑技术有限公司
///    All rights reserved.
///
/// @file avd_ptz.h
/// @brief AVD系列 - PTZ失控检测模块（塔身）
///
/// AVD PTZ失控检测函数及其相关
///
/// @version 1.0
/// @author 银瀑技术
/// @date 2010.12.23
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////

#include "imp_avd_common.h"


#define AVD_PTZ_CNT_HIGH 20
#define AVD_PTZ_CNT_LOW 5
#define AVD_PTZ_CHG_CNT 6

void AVD_ptzAnalysis( AVD_MODULE* avdModule ,IMP_U32 u32ExtScnChg);