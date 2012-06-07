#ifndef _IMP_ALGO_COMMAND_H_
#define _IMP_ALGO_COMMAND_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** 命令最大个数 */
#define IMP_MAX_CMD_CNT			4

typedef enum impCMD_TYPE_E
{
	IMP_CMD_TYPE_UNKNOWN	= 0x00000000	/**< 未知类型 */
}CMD_TYPE_E;

/** 命令缓存个数 */
#define IMP_BUFLEN_CMD_ITEM		64
/** 命令缓存数据长度 */
#define IMP_BUFLEN_CMD_DATA		(IMP_BUFLEN_CMD_ITEM-sizeof(IMP_U32) * 1)

/** 命令项结构 */
typedef struct impCMD_ITEM_S
{
	IMP_U32	u32Type;						/**< 命令类型 */
	IMP_U8	au8Data[IMP_BUFLEN_CMD_DATA];	/**< 命令数据*/
} CMD_ITEM_S;

/** 命令集结构 */
typedef struct impCMD_SET_S 
{
	IMP_S32	    s32CmdNum;					/**< 命令数目 */
	CMD_ITEM_S	astCmds[IMP_MAX_CMD_CNT];	/**< 命令数据 */
} CMD_SET_S;

#ifdef __cplusplus
}
#endif


#endif /*_IMP_ALGO_COMMAND_H_*/
/*@}*/

/*@}*/
