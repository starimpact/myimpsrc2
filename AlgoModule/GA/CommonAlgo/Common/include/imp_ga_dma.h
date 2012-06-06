

#ifndef _IMP_GA_DMA_H_
#define _IMP_GA_DMA_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum impGA_IDMA_CHANNEL_E
{
	IMP_IDMA_CHANNEL0 = 0,
	IMP_IDMA_CHANNEL1,
	IMP_IDMA_CHANNEL2,
	IMP_IDMA_CHANNEL3,
	IMP_NUM_LOGICAL_CH
}GA_IDMA_CHANNEL_E;


typedef struct impDMA_CHANNLS_S
{
	IMP_S32 s32ChNum;
#ifdef _TIDSP
    IDMA2_ChannelRec stDMAChRs[IMP_NUM_LOGICAL_CH];
#endif
} DMA_CHANNLS_S;

#ifdef __cplusplus
}
#endif

#endif
