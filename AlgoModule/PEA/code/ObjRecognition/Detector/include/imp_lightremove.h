
#ifndef _LIGHTREMOVE_H_
#define _LIGHTREMOVE_H_

#include "imp_pea_common.h"

typedef struct impLightRemovePara_S
{
//	IMP_S32 s32H;
//	IMP_S32 s32W;
	IMP_S32 s32i;
	IMP_FLOAT fthesh1;
	IMP_FLOAT fthesh2;
	IMP_FLOAT fdelay;
}IMP_LightRemovePara_S;

//typedef struct impLightAlgorith_S
//{
//	IMP_FLOAT * apfqueue1[9];
//	IMP_UCHAR * apu8queue2[10];
//	IMP_UCHAR * pu8H;
//	IMP_UCHAR * pu8M;
//	IMP_UCHAR * pu8N;
//	IMP_UCHAR * pu8T;
//	IMP_UCHAR * pu8single;
//	IMP_FLOAT * pfV;
//	IMP_FLOAT * pfVt0;
//	IMP_FLOAT * pfVt1;
//	IMP_UCHAR * pu8frame; 
//}IMP_LightAlgorith_S;


IMP_S32 IMP_GetMemSizeAlgModel( PEA_RESULT_S *pstResult );

IMP_MODULE_HANDLE IMP_CreateLightRemove(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);

IMP_S32 IMP_ConfigLightRemove(IMP_MODULE_HANDLE hModule, IMP_LightRemovePara_S *pstLRP);

IMP_S32 IMP_ProcessLightRemove(IMP_MODULE_HANDLE hModule);

IMP_S32 IMP_ReleaseLightRemove(IMP_MODULE_HANDLE hModule);


#endif


