
#include "imp_com_type.h"

IMP_U32 gu32AlgoModule = IMP_NONE_AGLO_MODULE;

void IMP_DataBufferAlloc( DATA_BUF_S *pstDataBuf, IMP_S32 s32BufLen )
{
	memset( pstDataBuf, 0, sizeof(DATA_BUF_S) );
	pstDataBuf->s32BufLen = s32BufLen;
	pstDataBuf->s32DatLen = 0;
	pstDataBuf->pu8Buf = malloc( pstDataBuf->s32BufLen );
	memset( pstDataBuf->pu8Buf, 0, pstDataBuf->s32BufLen );
}

void IMP_DataBufferFree( DATA_BUF_S *pstDataBuf )
{
	free( pstDataBuf->pu8Buf );
	memset( pstDataBuf, 0, sizeof(DATA_BUF_S) );
}
extern void IMP_InitVideoAnalysis();
extern void IMP_ConfigAlgoPara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp );
extern IMP_S32 ConfigSmartKey( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp );
extern IMP_S32 SetVideoSourceMode( IMP_U32 u32Channel, IMP_U32 mode );
extern IMP_U32 GetVideoSourceMode( IMP_U32 u32Channel );

// Definition of the structure describing the state of the thread.
typedef struct
{
	IMP_S32 thrID;
	DATA_BUF_S stCmdDataBuf, stRspDataBuf;
} IMP_ControlThread;

IMP_ControlThread stControlThread;

#define BUFLEN_PACKET (1024*512)

void IMP_ControlThreadInit()
{
	memset( &stControlThread, 0, sizeof(IMP_ControlThread) );

	IMP_DataBufferAlloc( &stControlThread.stCmdDataBuf, BUFLEN_PACKET );
	IMP_DataBufferAlloc( &stControlThread.stRspDataBuf, BUFLEN_PACKET );
}

//  ======== thrControlStartup ========
void IMP_ControlThreadStart()
{
}

//  ======== IMP_ControlThreadProcess ========
//  Main function of Control task.
static IMP_S32 ProcessCommandPacket( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
extern void IMP_NET_SendData( DATA_BUF_S *pstOutBuffer );
int g_RspFd;
void IMP_NET_SendRspData( DATA_BUF_S *pstOutBuffer ,int sockfd)
{
   	int sendLen = 0;
   	sendLen = send(sockfd, pstOutBuffer->pu8Buf, pstOutBuffer->s32DatLen,0);
   	printf("IMP_NET_SendRspData sendLen = %d\n",sendLen);
}
void IMP_ControlThreadProcess(IMP_OPER_DATA_S *pstCmd)
{
	IMP_S32 ret = 0;
	DATA_BUF_S *pstBufferCmd = &stControlThread.stCmdDataBuf;
	DATA_BUF_S *pstBufferRsp = &stControlThread.stRspDataBuf;
	//IMP_OPER_DATA_S *pstCmdHead = pstCmd;//(IMP_OPER_DATA_S*)pstBufferCmd->pu8Buf;
	//IMP_OPER_DATA_S *pstCmdHead = (IMP_OPER_DATA_S*)pstBufferCmd->pu8Buf;
	IMP_OPER_DATA_S *pstCmdHead = pstCmd;
	IMP_OPER_DATA_S *pstRspHead = (IMP_OPER_DATA_S*)pstBufferRsp->pu8Buf;
//    memcpy(pstCmdHead,pstCmd,sizeof(IMP_OPER_DATA_S));
	//usleep(1000);

//	IMP_InitVideoAnalysis();

//    while( IMP_TRUE )
    {
		//pstBufferCmd->s32DatLen = pstBufferCmd->s32BufLen;
		//ret = ioCmdPkg_Receive( pstBufferCmd->buf, &pstBufferCmd->s32DatLen, -1 );
		// 网络接收
	//	if( ret )
		{

			ret = ProcessCommandPacket( pstCmdHead, pstRspHead );

			if( ret )
			{
				pstBufferRsp->s32DatLen = pstRspHead->u32Size;
				printf("pstBufferRsp->s32DatLen = %d\n",pstBufferRsp->s32DatLen);
				IMP_NET_SendRspData(pstBufferRsp,g_RspFd);
				//IMP_NET_SendData(pstBufferRsp);
				//ioCmdPkg_Send( pstBufferRsp->buf, pstBufferRsp->s32DatLen, -1 );
				//网络发送
			}
		}
    }
}

static void ProcessControlCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
void ProcessUnknownCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );

static IMP_S32 ProcessCommandPacket( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_U32 S32CmdType = pstCmdHead->u32Type;
	IMP_U32 cmdclass = ( S32CmdType & 0xF000 );
	IMP_S32 ret=1;
   // printf("S32CmdType = %d\n",S32CmdType);
	if( cmdclass == IMP_CONTROL_TYPE_BASE )
	{
		//MBX_post( mbxProcess, pstCmdHead, 0 );
		// 控制相关函数
		ProcessControlCommand(pstCmdHead, pstRspHead);
	}
	else if( cmdclass == IMP_CONFIG_TYPE_BASE )
	{
		ProcessConfigCommand(pstCmdHead, pstRspHead);
	}
	else
	{
		ProcessUnknownCommand( pstCmdHead, pstRspHead );
	}

	return ret;
}
static void ProcessConfigCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
void IMP_FillCmdHeadData( IMP_OPER_DATA_S *pstHead, IMP_U32 u32Size, IMP_U32 u32Type, IMP_U32 u32Status,
					IMP_U32 u32Channel, IMP_U32 u32Time, IMP_U32 u32Date, IMP_U32 u32Id, IMP_U32 u32Level )
{
	pstHead->u32Size = u32Size;
	pstHead->u32Type = u32Type;
	pstHead->u32Status = u32Status;
	pstHead->u32Channel = u32Channel;
	pstHead->u32Time = u32Time;
	pstHead->u32Date = u32Date;
	pstHead->u32Id = u32Id;
	pstHead->u32Level = u32Level;
}

static void ProcessControlCommandArm( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessControlCommandDisArm( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessControlCommandClearAlarm( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
void ProcessControlCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
		IMP_S32 S32CmdType = pstCmdHead->u32Type;
		if( S32CmdType == IMP_CONTROL_TYPE_VDR_Arm )
		{
			ProcessControlCommandArm( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONTROL_TYPE_VDR_Disarm )
		{
			ProcessControlCommandDisArm( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONTROL_TYPE_VDR_ClearAlarm )
		{
			ProcessControlCommandClearAlarm( pstCmdHead, pstRspHead );
		}
		else
		{
			ProcessUnknownCommand( pstCmdHead, pstRspHead );
		}
}

extern IMP_S32 IMP_PEA_ArmAlgoModule( IMP_S32 s32ChanNum);
extern IMP_S32 IMP_AVD_ArmAlgoModule( IMP_S32 s32ChanNum);
extern IMP_S32 IMP_VFD_ArmAlgoModule( IMP_S32 s32ChanNum);
static IMP_S32 IMP_ArmAlgoModule( IMP_S32 s32ChanNum, ALGO_MODULE_E enAlgoModule)
{
	IMP_S32 ret;
	printf("enAlgoModule = %d,gu32AlgoModule = %d\n",enAlgoModule,gu32AlgoModule);
	enAlgoModule = IMP_PEA_AGLO_MODULE | IMP_AVD_AGLO_MODULE | IMP_VFD_AGLO_MODULE | IMP_AAI_AGLO_MODULE;
    gu32AlgoModule = IMP_PEA_AGLO_MODULE | IMP_AVD_AGLO_MODULE | IMP_VFD_AGLO_MODULE | IMP_AAI_AGLO_MODULE;

	if((enAlgoModule & IMP_PEA_AGLO_MODULE ) && (gu32AlgoModule& IMP_PEA_AGLO_MODULE))
		ret = IMP_PEA_ArmAlgoModule(s32ChanNum);

	if((enAlgoModule & IMP_AVD_AGLO_MODULE ) && (gu32AlgoModule& IMP_AVD_AGLO_MODULE))
		ret = IMP_AVD_ArmAlgoModule(s32ChanNum);

	if((enAlgoModule & IMP_VFD_AGLO_MODULE ) && (gu32AlgoModule& IMP_VFD_AGLO_MODULE))
		ret = IMP_VFD_ArmAlgoModule(s32ChanNum);

    if((enAlgoModule & IMP_AAI_AGLO_MODULE ) && (gu32AlgoModule& IMP_AAI_AGLO_MODULE))
		ret = IMP_VFD_ArmAlgoModule(s32ChanNum);

	return ret;
}

static void ProcessControlCommandArm( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONTROL_DATA_S *pstCmd = (IMP_CONTROL_DATA_S*)pstCmdHead;
	IMP_CONTROL_DATA_S *pstRsp = (IMP_CONTROL_DATA_S*)pstRspHead;
	IMP_S32 s32ChanNum = pstCmdHead->u32Channel;
	IMP_U32 u32AlgoModule = pstCmd->unData.stControlData.u32AlgoModule;
/*	VideoChanData *vchanData = &thrProcess.vchanDatas[s32ChanNum];
	DATABUFFER *outBuffer = &thrProcess.outBuffer;
	VAEVT_ITEM evt;*/
	IMP_S32 u32Status;
	IMP_S32 ret=0;

//	if( !vchanData->sceneArmed )
	{
		ret = IMP_ArmAlgoModule( s32ChanNum,u32AlgoModule );
	/*	if( ret )
		{
			vchanData->sceneArmed = TRUE;
			sEVTMGR_ConstructEvent( &thrProcess.evtMgr, &evt, VAEVT_TYPE_Armed, VAALARM_LEVEL_NOT, VAEVT_STATUS_NOSTA );
			SendResultEvent( &evt, s32ChanNum, outBuffer );
		}*/
	}

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( &pstRsp->stHead, sizeof(IMP_OPER_DATA_S), pstCmd->stHead.u32Type, u32Status,
					pstCmd->stHead.u32Channel, 0, 0, pstCmd->stHead.u32Id, 0 );
}

extern IMP_S32 IMP_PEA_DisArmAlgoModule( IMP_S32 s32ChanNum);
extern IMP_S32 IMP_AVD_DisArmAlgoModule( IMP_S32 s32ChanNum);
extern IMP_S32 IMP_VFD_DisArmAlgoModule( IMP_S32 s32ChanNum);
static IMP_S32 IMP_DisArmAlgoModule( IMP_S32 s32ChanNum, ALGO_MODULE_E enAlgoModule)
{
		IMP_S32 ret,ret1,ret2,ret3,ret4;
	if(gu32AlgoModule & IMP_PEA_AGLO_MODULE )
		ret1 = IMP_PEA_DisArmAlgoModule(s32ChanNum);

	if(gu32AlgoModule & IMP_AVD_AGLO_MODULE )
		ret2 = IMP_AVD_DisArmAlgoModule(s32ChanNum);

	if(gu32AlgoModule & IMP_VFD_AGLO_MODULE )
		ret3 = IMP_VFD_DisArmAlgoModule(s32ChanNum);

    if(gu32AlgoModule & IMP_AAI_AGLO_MODULE )
		ret4 = IMP_AAI_DisArmAlgoModule(s32ChanNum);



	ret = ret1 && ret2 && ret3 && ret4;
	return ret;
}
static void ProcessControlCommandDisArm( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONTROL_DATA_S *pstCmd = (IMP_CONTROL_DATA_S*)pstCmdHead;
	IMP_CONTROL_DATA_S *pstRsp = (IMP_CONTROL_DATA_S*)pstRspHead;
	IMP_S32 s32ChanNum = pstCmdHead->u32Channel;
/*	DATABUFFER *outBuffer = &thrProcess.outBuffer;
	VAEVT_ITEM evt;*/
	IMP_S32 u32Status;
	IMP_S32 ret=0;
	//VideoChanData *vchanData = &thrProcess.vchanDatas[s32ChanNum];

	IMP_DisArmAlgoModule( s32ChanNum ,1);
	ret = 1;

	/*sEVTMGR_ConstructEvent( &thrProcess.evtMgr, &evt, VAEVT_TYPE_Disarmed, VAALARM_LEVEL_NOT, VAEVT_STATUS_NOSTA );
	SendResultEvent( &evt, s32ChanNum, outBuffer );*/

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( &pstRsp->stHead, sizeof(IMP_OPER_DATA_S), pstCmd->stHead.u32Type, u32Status,
					pstCmd->stHead.u32Channel, 0, 0, pstCmd->stHead.u32Id, 0 );
}

extern IMP_S32 IMP_PEA_ClearAlarmAlgoModule( IMP_S32 s32ChanNum);
extern IMP_S32 IMP_AVD_ClearAlarmAlgoModule( IMP_S32 s32ChanNum);
extern IMP_S32 IMP_VFD_ClearAlarmAlgoModule( IMP_S32 s32ChanNum);
static IMP_S32 IMP_ClearAlarmAlgoModule( IMP_S32 s32ChanNum, ALGO_MODULE_E enAlgoModule)
{
	IMP_S32 ret,ret1,ret2,ret3,ret4;
	if(enAlgoModule & IMP_PEA_AGLO_MODULE )
		ret1 = IMP_PEA_ClearAlarmAlgoModule(s32ChanNum);

	if(enAlgoModule & IMP_AVD_AGLO_MODULE )
		ret2 = IMP_AVD_ClearAlarmAlgoModule(s32ChanNum);

	if(enAlgoModule & IMP_VFD_AGLO_MODULE )
		ret3 = IMP_VFD_ClearAlarmAlgoModule(s32ChanNum);

    if(enAlgoModule & IMP_VFD_AGLO_MODULE )
		ret4 = IMP_VFD_ClearAlarmAlgoModule(s32ChanNum);

	ret = ret1 && ret2 && ret3 && ret4;

	return ret;
}
static void ProcessControlCommandClearAlarm( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONTROL_DATA_S *pstCmd = (IMP_CONTROL_DATA_S*)pstCmdHead;
	IMP_CONTROL_DATA_S *pstRsp = (IMP_CONTROL_DATA_S*)pstRspHead;
	IMP_S32 s32ChanNum = pstCmdHead->u32Channel;
/*	VideoChanData *vchanData = &thrProcess.vchanDatas[s32ChanNum];
	DATABUFFER *outBuffer = &thrProcess.outBuffer;
	VAEVT_ITEM evt;*/
	IMP_S32 u32Status;
	IMP_S32 ret=0;

/*if( vchanData->sceneArmed )
	{
		ret = 1;
		sEVTMGR_ConstructEvent( &thrProcess.evtMgr, &evt, VAEVT_TYPE_AlarmCleared, VAALARM_LEVEL_NOT, VAEVT_STATUS_NOSTA );
		SendResultEvent( &evt, s32ChanNum, outBuffer );
	}*/
	//
	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( &pstRsp->stHead, sizeof(IMP_OPER_DATA_S), pstCmd->stHead.u32Type, u32Status,
					pstCmd->stHead.u32Channel, 0, 0, pstCmd->stHead.u32Id, 0 );
}



static void ProcessConfigCommandConfigAlgoRulePara( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandConfigAlgoAdvancePara( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandConfigSystemPara( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandConfigAlgoLibInfo( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandConfigAlgoModule( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );

void ProcessConfigCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
		IMP_S32 S32CmdType = pstCmdHead->u32Type;

		if(( S32CmdType & 0xFF00 ) == 0x4000 )
		{
			ProcessConfigCommandConfigAlgoModule( pstCmdHead, pstRspHead);
		}
		else if(( S32CmdType & 0xFF00 ) == 0x4100 )
		{
			ProcessConfigCommandConfigAlgoLibInfo( pstCmdHead, pstRspHead);
		}
		else if(( S32CmdType & 0xFF00 ) == 0x4200 )
		{
			ProcessConfigCommandConfigSystemPara( pstCmdHead, pstRspHead);
		}
		else if( ( S32CmdType & 0xFF00 ) == 0x4300 )
		{
			ProcessConfigCommandConfigAlgoRulePara( pstCmdHead, pstRspHead );
		}
		else if( ( S32CmdType & 0xFF00 ) == 0x4400 )
		{
			ProcessConfigCommandConfigAlgoAdvancePara( pstCmdHead, pstRspHead );
		}
		else
		{
			ProcessUnknownCommand( pstCmdHead, pstRspHead );
		}
}

static void ProcessConfigCommandSetAlgoModule( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandGetAlgoModule( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandConfigAlgoModule( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
		IMP_S32 S32CmdType = pstCmdHead->u32Type;
		if( S32CmdType == IMP_CONFIG_TYPE_SetAlgoModule )
		{
			ProcessConfigCommandSetAlgoModule( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONFIG_TYPE_GetAlgoModule )
		{
			ProcessConfigCommandGetAlgoModule( pstCmdHead, pstRspHead );
		}
		else
		{
			ProcessUnknownCommand( pstCmdHead, pstRspHead );
		}
}

static void ProcessConfigCommandConfigAlgoLibInfo( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{

}

static void ProcessConfigCommandSetAlgoModule( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;
	IMP_U32 u32Channel, s32ChanNum, mode;
	IMP_S32 ret;

	u32Channel = pstCmd->stHead.u32Channel;

	gu32AlgoModule = *(IMP_U32*)(pstCmd->unData.au8Buffer);

	ret = 1;

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;

	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}
static void ProcessConfigCommandGetAlgoModule( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;
	IMP_S32 ret;

	IMP_U32 u32Channel = pstCmd->stHead.u32Channel;

	memcpy( pstRsp->unData.au8Buffer,&gu32AlgoModule,sizeof(gu32AlgoModule));

	ret = 1;

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;

	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S)+sizeof(IMP_U32), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}


static void ProcessConfigCommandSetVideoMode( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandGetVideoMode( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandSetSysTime( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandGetSysTime( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandGetSecretCode( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead );
static void ProcessConfigCommandConfigSystemPara( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
		IMP_S32 S32CmdType = pstCmdHead->u32Type;
		if( S32CmdType == IMP_CONFIG_TYPE_GetVideoSourceMode )
		{
			ProcessConfigCommandGetVideoMode( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONFIG_TYPE_SetVideoSourceMode )
		{
			ProcessConfigCommandSetVideoMode( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONFIG_TYPE_SetSysTime )
		{
			ProcessConfigCommandSetSysTime( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONFIG_TYPE_GetSysTime )
		{
			ProcessConfigCommandGetSysTime( pstCmdHead, pstRspHead );
		}
		else if( S32CmdType == IMP_CONFIG_TYPE_GetSecretCode)
		{
			ProcessConfigCommandGetSecretCode( pstCmdHead, pstRspHead);
		}
		else
		{
			ProcessUnknownCommand( pstCmdHead, pstRspHead );
		}
}

void ProcessUnknownCommand( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;

	u32Status = IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}

static void ProcessConfigCommandSetVideoMode( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;
	IMP_U32 u32Channel, s32ChanNum, mode;
	IMP_S32 ret;

	u32Channel = pstCmd->stHead.u32Channel;
	s32ChanNum = ( u32Channel & 0xFF );
	mode = *(IMP_U32*)(pstCmd->unData.au8Buffer);
//	ret = SetVideoSourceMode( s32ChanNum, mode );

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}

static void ProcessConfigCommandGetVideoMode( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;
	IMP_U32 u32Channel, s32ChanNum, mode;

	u32Channel = pstCmd->stHead.u32Channel;
	s32ChanNum = ( u32Channel & 0xFF );
//	mode = GetVideoSourceMode( s32ChanNum );
	*((IMP_U32*)pstRsp->unData.au8Buffer) = mode;

	u32Status = IMP_STATUS_SUCCESS;
	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S)+sizeof(IMP_U32), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}

static void ProcessConfigCommandSetSysTime( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;
//	ZSYSTIME *systime;
	IMP_S32 ret;

//	systime = (ZSYSTIME*)(pstCmd->unData.au8Buffer);
//	ret = setSysTime( systime );

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}

static void ProcessConfigCommandGetSysTime( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;
//	ZSYSTIME systime;
	IMP_S32 ret;

//	ret = getSysTime( &systime );
//	*((ZSYSTIME*)pstRsp->unData.au8Buffer) = systime;

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S), pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}

static void ProcessConfigCommandGetSecretCode( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;
	IMP_U32 u32Status;

//	getID( pstRsp->unData.au8Buffer );

	u32Status = IMP_STATUS_SUCCESS;
	IMP_FillCmdHeadData( pstRspHead, sizeof(IMP_OPER_DATA_S)+16, pstCmdHead->u32Type, u32Status,
					pstCmdHead->u32Channel, 0, 0, pstCmdHead->u32Id, 0 );
}

static  IMP_U32 ProcessConfigCommandSetPeaAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp );
static  IMP_U32 ProcessConfigCommandGetPeaAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen);

static  IMP_U32 ProcessConfigCommandSetAvdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp );
static  IMP_U32 ProcessConfigCommandGetAvdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen);

static  IMP_U32 ProcessConfigCommandSetVfdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp );
static  IMP_U32 ProcessConfigCommandGetVfdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen);

static  IMP_U32 ProcessConfigCommandSetAaiAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp );
static  IMP_U32 ProcessConfigCommandGetAaiAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen);
static void ProcessConfigCommandConfigAlgoRulePara( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead )
{
	IMP_CONFIG_DATA_S *pstCmd = (IMP_CONFIG_DATA_S*)pstCmdHead;
	IMP_CONFIG_DATA_S *pstRsp = (IMP_CONFIG_DATA_S*)pstRspHead;

	IMP_U8 *data;
	IMP_U32 datlen;
	IMP_S32 ret=0;
	IMP_U32 u32Status;
	IMP_U32 s32ChanNum = pstCmd->stHead.u32Channel;
	printf("ProcessConfigCommandConfigAlgoRulePara start %d\n",pstCmd->stHead.u32Type);
	if( pstCmd->stHead.u32Type & 1 )	// get
	{
		data = pstRsp->unData.au8Buffer;
		datlen = 0;
	}
	else						// set
	{
		data = pstCmd->unData.au8Buffer;
		datlen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	}
	switch( pstCmd->stHead.u32Type )
	{
	    case IMP_CONFIG_TYPE_SetPeaRulePara:						// 设置PEA规则参数
				ret = ProcessConfigCommandSetPeaAlgoRulePara( pstCmd, pstRsp );
			break;

			case IMP_CONFIG_TYPE_GetPeaRulePara:						// 获取PEA规则参数
				ret = ProcessConfigCommandGetPeaAlgoRulePara( pstCmd, pstRsp ,&datlen);
			break;

			case IMP_CONFIG_TYPE_SetAvdRulePara:						// 设置AVD规则参数
				ret = ProcessConfigCommandSetAvdAlgoRulePara( pstCmd, pstRsp);
			break;

			case IMP_CONFIG_TYPE_GetAvdRulePara:						// 获取AVD规则参数
				ret = ProcessConfigCommandGetAvdAlgoRulePara( pstCmd, pstRsp ,&datlen);
			break;

			case IMP_CONFIG_TYPE_SetVfdRulePara:						// 设置VFD规则参数
				ret = ProcessConfigCommandSetVfdAlgoRulePara( pstCmd, pstRsp );
			break;

			case IMP_CONFIG_TYPE_GetVfdRulePara:						// 获取VFD规则参数
				ret = ProcessConfigCommandGetVfdAlgoRulePara( pstCmd, pstRsp ,&datlen);
			break;

            case IMP_CONFIG_TYPE_SetAaiRulePara:						// 设置VFD规则参数
				ret = ProcessConfigCommandSetAaiAlgoRulePara( pstCmd, pstRsp );
			break;

			case IMP_CONFIG_TYPE_GetAaiRulePara:						// 获取VFD规则参数
				ret = ProcessConfigCommandGetAaiAlgoRulePara( pstCmd, pstRsp ,&datlen);
			break;
			default:
			ret = 0;
	        break;
	 }

  if( !ret || !(pstCmd->stHead.u32Type & 1) )
		datlen = 0;

	u32Status = ret ? IMP_STATUS_SUCCESS : IMP_STATUS_ERROR;
	IMP_FillCmdHeadData( &pstRsp->stHead, sizeof(IMP_OPER_DATA_S)+datlen, pstCmd->stHead.u32Type, u32Status,
					pstCmd->stHead.u32Channel, 0, 0, pstCmd->stHead.u32Id, 0 );

}
#define MAX_RULE_DATA_SIZE 64*1024
static  IMP_U32 ProcessConfigCommandSetPeaAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp )
{

	char *pFileRuleData	= "pea_rule.dat";
	int i = 0;
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstCmd->unData.au8Buffer;

    printf("ProcessConfigCommandSetPeaAlgoRulePara start!\n");
    printf("u32DataLen = %d\n",u32DataLen);
	FILE* pFile = fopen(pFileRuleData, "w");

	fwrite(pu8Data,1,u32DataLen,pFile);

	fclose(pFile);
	printf("ProcessConfigCommandSetPeaAlgoRulePara ok!\n");
	return IMP_TRUE;
}
static  IMP_U32 ProcessConfigCommandGetPeaAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen)
{
	char *pFileRuleData	= "pea_rule.dat";
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstRsp->unData.au8Buffer;
	DATA_BUF_S stData;
  stData.s32BufLen = MAX_RULE_DATA_SIZE;
  stData.pu8Buf = (IMP_U8*)malloc(MAX_RULE_DATA_SIZE);

	FILE* pFile = fopen(pFileRuleData, "rb");
	if (!pFile)
  {
  	free(stData.pu8Buf);
  	return IMP_FALSE;
  }
	stData.s32DatLen = fread(stData.pu8Buf, 1, MAX_RULE_DATA_SIZE, pFile);

	if (!stData.s32DatLen)
  {
      fclose(pFile);
      free(stData.pu8Buf);
      return IMP_FALSE;
  }
	*pu32DataLen = stData.s32DatLen;
	memcpy(pu8Data,stData.pu8Buf,stData.s32DatLen);

	fclose(pFile);
	free(stData.pu8Buf);

	return IMP_TRUE;
}
static  IMP_U32 ProcessConfigCommandSetAvdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp )
{
	char *pFileRuleData	= "avd_rule.dat";
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstCmd->unData.au8Buffer;
     printf("ProcessConfigCommandSetVfdAlgoRulePara u32DataLen = %d\n",u32DataLen);
	FILE* pFile = fopen(pFileRuleData, "w");
    if (!pFile)
    {
        return IMP_FALSE;
    }
	fwrite(pu8Data,1,u32DataLen,pFile);
	fclose(pFile);
	return IMP_TRUE;
}
static  IMP_U32 ProcessConfigCommandGetAvdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen)
{
	char *pFileRuleData	= "avd_rule.dat";
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);

	IMP_U8 *pu8Data = &pstRsp->unData.au8Buffer;
	DATA_BUF_S stData;

     printf("ProcessConfigCommandGetAvdAlgoRulePara u32DataLen start\n");
    stData.s32BufLen = MAX_RULE_DATA_SIZE;
    stData.pu8Buf = (IMP_U8*)malloc(MAX_RULE_DATA_SIZE);

	FILE* pFile = fopen(pFileRuleData, "rb");
	if (!pFile)
  {
  	free(stData.pu8Buf);
  	return IMP_FALSE;
  }
	stData.s32DatLen = fread(stData.pu8Buf, 1, MAX_RULE_DATA_SIZE, pFile);
  if (!stData.s32DatLen)
  {
      fclose(pFile);
      free(stData.pu8Buf);
      return IMP_FALSE;
  }
	*pu32DataLen = stData.s32DatLen;
	memcpy(pu8Data,stData.pu8Buf,stData.s32DatLen);

	fclose(pFile);
	free(stData.pu8Buf);
    printf("ProcessConfigCommandGetAvdAlgoRulePara u32DataLen ok %d \n",*pu32DataLen);
	return IMP_TRUE;
}
static  IMP_U32 ProcessConfigCommandSetVfdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp )
{
	char *pFileRuleData	= "vfd_rule.dat";

	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstCmd->unData.au8Buffer;
    printf("ProcessConfigCommandSetVfdAlgoRulePara u32DataLen = %d\n",u32DataLen);
	FILE* pFile = fopen(pFileRuleData, "w");
    if (!pFile)
    {
        return IMP_FALSE;
    }

	fwrite(pu8Data,1,u32DataLen,pFile);
	fclose(pFile);
	return IMP_TRUE;
}
static  IMP_U32 ProcessConfigCommandGetVfdAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen)
{

	char *pFileRuleData	= "vfd_rule.dat";
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstRsp->unData.au8Buffer;
	DATA_BUF_S stData;
    stData.s32BufLen = MAX_RULE_DATA_SIZE;
    stData.pu8Buf = (IMP_U8*)malloc(MAX_RULE_DATA_SIZE);

	FILE* pFile = fopen(pFileRuleData, "rb");
	if (!pFile)
    {
        free(stData.pu8Buf);
        return IMP_FALSE;
    }
	stData.s32DatLen = fread(stData.pu8Buf, 1, MAX_RULE_DATA_SIZE, pFile);
	if (!stData.s32DatLen)
  {
      fclose(pFile);
      free(stData.pu8Buf);
      return IMP_FALSE;
  }
	*pu32DataLen = stData.s32DatLen;
	memcpy(pu8Data,stData.pu8Buf,stData.s32DatLen);

	fclose(pFile);
	free(stData.pu8Buf);

	return IMP_TRUE;
}

static  IMP_U32 ProcessConfigCommandSetAaiAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp )
{

	char *pFileRuleData	= "aai_rule.dat";
	int i = 0;
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstCmd->unData.au8Buffer;

	FILE* pFile = fopen(pFileRuleData, "w");
    if (!pFile)
    {
        return IMP_FALSE;
    }
	fwrite(pu8Data,1,u32DataLen,pFile);

	fclose(pFile);
 printf("ProcessConfigCommandSetAaiAlgoRulePara ok!\n");
	return IMP_TRUE;
}

static  IMP_U32 ProcessConfigCommandGetAaiAlgoRulePara( IMP_CONFIG_DATA_S *pstCmd, IMP_CONFIG_DATA_S *pstRsp ,IMP_U32 *pu32DataLen)
{
	char *pFileRuleData	= "aai_rule.dat";
	IMP_U32 u32DataLen = pstCmd->stHead.u32Size - sizeof(IMP_OPER_DATA_S);
	IMP_U8 *pu8Data = &pstRsp->unData.au8Buffer;
	DATA_BUF_S stData;
    stData.s32BufLen = MAX_RULE_DATA_SIZE;
    stData.pu8Buf = (IMP_U8*)malloc(MAX_RULE_DATA_SIZE);

	FILE* pFile = fopen(pFileRuleData, "rb");
	if (!pFile)
    {
        free(stData.pu8Buf);
        return IMP_FALSE;
    }
	stData.s32DatLen = fread(stData.pu8Buf, 1, MAX_RULE_DATA_SIZE, pFile);

    if (!stData.s32DatLen)
    {
        fclose(pFile);
        free(stData.pu8Buf);
        return IMP_FALSE;
    }
	*pu32DataLen = stData.s32DatLen;
	memcpy(pu8Data,stData.pu8Buf,stData.s32DatLen);

	fclose(pFile);
	free(stData.pu8Buf);

	return IMP_TRUE;
}

static  void ProcessConfigCommandConfigAlgoAdvancePara( IMP_OPER_DATA_S *pstCmdHead, IMP_OPER_DATA_S *pstRspHead)
{
    return IMP_TRUE;
}
