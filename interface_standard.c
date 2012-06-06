//基础数据类型定义
typedef unsigned char IMP_U8;
typedef unsigned char IMP_UCHAR;
typedef unsigned short IMP_U16;
typedef unsigned int IMP_U32;
typedef char IMP_S8;
typedef short IMP_S16;
typedef int IMP_S32;
#ifndef _M_IX86
	typedef unsigned long long IMP_U64;
	typedef long long IMP_S64;
#else
	typedef __int64 IMP_U64;
	typedef __int64 IMP_S64;
#endif
typedef char IMP_CHAR;
typedef char* IMP_PCHAR;
typedef float IMP_FLOAT;
typedef double IMP_DOUBLE;
typedef void IMP_VOID;
typedef unsigned long IMP_SIZE_T;
typedef unsigned long IMP_LENGTH_T;

typedef void *IMP_MODULE_HANDLE;


//系统硬件信息
typedef struct impGA_HARDWARE_RS_S
{
	MEM_MGR_ARRAY_S stMemMgr; //系统内存
	DMA_CHANNLS_S stIDMAChs; //系统DMA
} GA_HARDWARE_RS_S;


//系统信息结构体
typedef struct impPEA_RESULT_S
{
	IMP_S32 s32Width; //图像宽
	IMP_S32 s32Height; //图像高
	IMP_U32 u32FrmTimeCur;
	IMP_U32 u32FrmTimePre;
	GRAY_IMAGE_S stImgInGray; //当前帧图片

	PEA_STATUS_S stSysStatus;
	IMP_S32 s32VideoAbnrmlStatus;

	IMP_OUTPUT stOutput;
	YUV_IMAGE422_S *pstImageYUV;
	EVT_MGR_S stEvtMgr;

	//各个算法模块输出信息
	ImpOutputAlgModel stOutputAlg;
	....

} PEA_RESULT_S;


//////////////////////////////////////////////////////////////////////////////////////////////


//内存操作函数
//内存类型
typedef enum impMEM_BLOCK_TYPE_E
{
	IMP_MEMBLK_TYPE_FAST = 0, //片内
	IMP_MEMBLK_TYPE_SLOW,     //片外
	IMP_MEMBLK_TYPE_MMGR,
	IMP_MEMBLK_NUM = IMP_MEMBLK_TYPE_MMGR
} MEM_BLOCK_TYPE_E;

/***************************************************
name:IMP_MMAlloc
do:审请内存
input:
	pstMgr:系统内存块
	enMemType:IMP_MEMBLK_TYPE_FAST或IMP_MEMBLK_TYPE_SLOW，通常用后者
	sz:需要的内存大小
output:
	不等0:分配到的内存首地址
	NULL:分配内存失败
history:
****************************************************/
IMP_VOID *IMP_MMAlloc( MEM_MGR_ARRAY_S *pstMgr, MEM_BLOCK_TYPE_E enMemType, IMP_S32 sz );


/***************************************************
name:IMP_MMFree
do:释放内存
input:
	pstMgr:系统内存块
	enMemType:IMP_MEMBLK_TYPE_FAST或IMP_MEMBLK_TYPE_SLOW，通常用后者
	sz:需要释放的内存首地址
output:
history:
****************************************************/
IMP_VOID IMP_MMFree( MEM_MGR_ARRAY_S *pstMgr, MEM_BLOCK_TYPE_E enMemType, IMP_VOID *p );


//////////////////////////////////////////////////////////////////////////////////////////////


//算法模块接口标准

//算法参数结构体:定义系统需要传给算法的参数，这个结构体需要公开给系统进行访问配置
typedef struct impAlgPara
{
	...
	...
}ImpAlgPara;


//算法需要输出的结构体
typedef struct impOutputAlgModel
{
	......
	......
}ImpOutputAlgModel
	

//算法模块结构体，这个结构体原则上存在于算法内部，不需要公开给系统。但由于老系统遗留问题，需要公开给系统
typedef struct impAlgModel
{
	//系统成员
    GA_HARDWARE_RS_S  *pHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult; //系统公共数据
	
	ImpOutputAlgModel *pstOutput;
	
	//算法私有自定义成员
	......
	......
}ImpAlgModel;

//算法模块接口分为5个，分别为：getmemsize, create, config, process, release
//算法模块接口前缀用IMP_, 算法内部私有函数前缀imp

/***************************************************
name:IMP_GetMemSizeAlgModel
do:得到算法所需要的内存大小
input:
	pstResult:系统公共数据
output:算法需要的内存大小
history:
****************************************************/
IMP_S32 IMP_GetMemSizeAlgModel( PEA_RESULT_S *pstResult )
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	
 	IMP_S32 size = ......
	
	return size;
}

/***************************************************
name:IMP_CreateAlgModel
do:审请算法内存并始化算法参数
input:
	pstModule:系统内存块
	pstResult:系统公共数据
	pHwResource:系统硬件资源
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_CreateAlgModel( ImpAlgModel *pstModule, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResource )
{
	......
	
	pstModule->pstHwResource = pstHwResource;
	pstModule->pstResult = pstResult;
	
	IpOutputAlgModel *pstOutput = &pstResult->stOutputAlg;
	
	......
}

/***************************************************
name:IMP_ReleaseAlgModel
do:释放算法内存
input:
	pstModule:系统内存块
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ReleaseAlgModel( ImpAlgModel *pstModule );

/***************************************************
name:IMP_ConfigAlgModel
do:配置算法参数
input:
	pstModule:系统内存块
	pstPara:参数配置结构体
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ConfigAlgModel( ImpAlgModel *pstModule, IpAlgPara *pstPara );

/***************************************************
name:IMP_ProcessAlgModel
do:算法主过程
input:
	pstModule:系统内存块
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ProcessAlgModel( ImpAlgModel *pstModule);








