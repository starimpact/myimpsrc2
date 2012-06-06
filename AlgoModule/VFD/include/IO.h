/** 
 * 文件说明：　人脸识别输入输出处理
 * @brief	         The file for input/output process of Face Recognition
 * @date			 2005/09/30
 * Update history：
 * @date			 2005/12/02 变更　标准化
 *                              Update standardize
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
 *
 *----------------------------------------------------------------------
 */

#ifndef __FR_IO_H__
#define __FR_IO_H__
#include "DetectFaces.h"


typedef unsigned char byte;


typedef struct  {
	int				initFlag;

	DetectParam		detectParam;


} FACEHANDLE;


#endif /* __FR_IO_H__ */



