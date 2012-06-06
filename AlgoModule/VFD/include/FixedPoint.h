/*****************************************************************************
 ** 
 * 文件说明：固定小数定义
 * @date	2005/09/30
 * @date	2005/12/02 变更　脸部识别和检出模块的合并及标准化
 *                     Update Face recognition and detect module merged and standardize
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
 *
 *----------------------------------------------------------------------
 */
#ifndef __FIXEDPOINT_H__
#define __FIXEDPOINT_H__


/*  2^1       2 */
/*  2^10   1024 */
/*  2^12   4096 */
/*  2^13   8192 */
/*  2^14  16384 */

/* #define FXP_UNIT_SCALE        ( 4096 ) */ /* <- 1.0 */
#define FXP_UNIT_SCALE        ( 1024 )  /* <- 1.0 */

#define FXP_UNIT_POS            ( 16 )  /* <- 1.0 */
#define FXP_UNIT_POS_HALF       (  8 )

#define FXP_UNIT_LINEAR       ( 1024 )


/* 以下与三角函数相关 */
#define FXP_UNIT_ANGLE        ( 4096 )  /* <- pi 弧度 */
#define FXP_UNIT_TANGENT       ( 256 )  /* <- 1.0 x与y的比 */
#define FXP_UNIT_TRIANGULAR   ( 4096 )  /* <- 1.0 sin,cos的输出 */
#define MYTABLE_ANGLE_SHIFT     ( 2 )   /* <- 移位 pi/4=256．（pi=4096 <--> pi=1024）*/





#endif /*__FIXEDPOINT_H__*/


/* End of file */
