#include "imp_pea_bva_log.h"

static FILE *pLogFile;

IMP_S32 ipOpenAnalystLogFile()
{
	IMP_S32 s32Ret;
	s32Ret = 0;

	if((pLogFile = fopen( "C:\\analyst.txt", "w" ))==NULL)
		printf("file open error!");

	return s32Ret;
}

IMP_S32 ipCloseAnalystLogFile()
{
	IMP_S32 s32Ret;
	s32Ret = 0;

	fclose( pLogFile );
	pLogFile = NULL;

	return s32Ret;
}

IMP_S32 ipOutputAnalystDataFrame( BEHAVIOR_ANALYSIS_S *pstAnalyst)
{
	IMP_S32 s32Ret = 0;
	return s32Ret;
}
