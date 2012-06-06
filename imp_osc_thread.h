/**
* \defgroup 结构类型定义
* 算法数据类型定义
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2010
*/
/*@{*/
#ifndef _IMP_OSC_THREAD_H_
#define _IMP_OSC_THREAD_H_
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <pthread.h>

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "mpi_sys.h"
#include "hi_comm_vb.h"
#include "mpi_vb.h"
#include "hi_comm_vi.h"
#include "mpi_vi.h"
#include "vo_open.h"

#include "hi_comm_vo.h"
#include "hi_comm_vi.h"
#include "hi_comm_vpp.h"
#include "hi_comm_venc.h"

#include "mpi_sys.h"
#include "mpi_vo.h"
#include "mpi_vpp.h"
#include "mpi_venc.h"
#include "sample_common.h"

#define MAX_FRM_CNT 256

//#define SHOW_DEBUG_INFO

#define OSC_QCIF

#ifdef OSC_QCIF
#define IMG_WIDTH 176
#define IMG_HEIGHT 144
#else
#define IMG_WIDTH 352
#define IMG_HEIGHT 288
#endif

#include "imp_algo_type.h"
#include "imp_algo_interface.h"
#include "imp_algo_urp_param.h"

#ifdef __cplusplus
extern "C"
{
#endif

int OSC_ALGO_PROCESS();

#ifdef __cplusplus
}
#endif

#endif /*_IMP_OSC_THREAD_H_*/

/*@}*/
