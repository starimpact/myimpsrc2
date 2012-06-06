

#ifndef _IPCLASSIFY_H_
#define _IPCLASSIFY_H_

#include "svm_common.h"
#include "imp_para.h"
#include "imp_pea_common.h"

#ifdef __cplusplus
IMP_EXPORTS "C"
{
#endif



#define CLASS_TYPE IMP_TGT_TYPE_HUMAN


#ifdef IMP_ARM_PEA
static IMP_S8 *parfil_svm		= "trainedModel.svm";
#else
static IMP_S8 *parfil_svm		= "trainedModel.svm";
#endif


typedef enum impSVM_FUNC_TYPE_E
{
	DISABLE = 0,
	CLASSIFY,
	TRAIN
}SVM_FUNC_TYPE_E;

#define HUMAN_SIZE_MIN 1000
#define HUMAN_SIZE_MAX 30000
#define HUMAN_VELO_MIN 0
#define HUMAN_VELO_MAX 1000
#define VEHICLE_SIZE_MIN 30001
#define VEHICLE_SIZE_MAX 1000000
#define VEHICLE_VELO_MIN 0
#define VEHICLE_VELO_MAX 10000

typedef struct impCF_TGT_CONSTRAIN_PARA_S
{
	IMP_S32 s32MinSize;
	IMP_S32 s32MaxSize;
	IMP_S32 s32MinVelo;
	IMP_S32 s32MaxVelo;
}CF_TGT_CONSTRAIN_PARA_S;


typedef struct impCF_TGT_PARA_S
{
	CF_TGT_CONSTRAIN_PARA_S stHumanPara;
	CF_TGT_CONSTRAIN_PARA_S stVehiclePara;
}CF_TGT_PARA_S;


typedef struct
{
	IMP_S32 s32ClassfierAnimal;
	IMP_S32 s32ClassType;
	IMP_S8  s8SvmFuncType;
	IMP_S32 s32UseBorderConstrain;
	CF_TGT_PARA_S stTgtPara;
	MODEL *pstModel;
	RULE_S *pstRule;
} IpClassifierPara;


IMP_VOID ipParseClassifierParaData( IpClassifierPara *pstPara );



#define CTARDAT_BUF_LEN	128

enum
{
	CLASS_TYPE_PBL = 0,
	CLASS_TYPE_SCR
};

enum
{
	FEATC_TYPE_CONT = 0,
	FEATC_TYPE_RGN
};


typedef struct
{
	IMP_U32 ps8Data[CTARDAT_BUF_LEN];
} TARDAT_CLASSIFIER;


typedef struct svm_node
{
	IMP_S32 index;
	IMP_DOUBLE value;
}svm_node;


struct svm_problem
{
	IMP_S32 l;
	IMP_DOUBLE *y;
	struct svm_node **x;
};

typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	IpClassifierPara stPara;
} IpTargetClassifier;


typedef struct
{
	IpTargetClassifier *pClassifier;
} IpModuleClassifier;

IMP_VOID ipCreateTargetClassifieri( IpTargetClassifier *tclassifier, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseTargetClassifieri( IpTargetClassifier *tclassifier );
IMP_VOID ipConfigTargetClassifieri( IpTargetClassifier *tclassifier, IpClassifierPara *pstPara );
IMP_S32 ipProcessTargetClassifierInternal( IpTargetClassifier *pstClassifier );
#ifdef SUPPORT_CHANGE
IMP_S32 ipProcessTargetClassifierInternalChg( IpTargetClassifier *pstClassifier );
#endif
IMP_S32 ipPostProcessTargetClassifieri( IpTargetClassifier *tclassifier );


IMP_EXPORTS IMP_MODULE_HANDLE ipCreateClassifier( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce );

IMP_EXPORTS IMP_VOID ipReleaseClassifier( IMP_MODULE_HANDLE hModule );

IMP_EXPORTS IMP_VOID ipConfigClassifier( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstParaInner );

IMP_EXPORTS IMP_S32 IMP_PEA_ProcessClassifier( IMP_MODULE_HANDLE hModule );

IMP_EXPORTS IMP_S32 ipPostProcessClassifier( IMP_MODULE_HANDLE hModule );

#ifdef __cplusplus
}
#endif

#endif
