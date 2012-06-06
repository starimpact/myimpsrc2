
#include "imp_pea_classify.h"
#include "imp_pea_system.h"


IMP_VOID ipParseClassifierParaData(IpClassifierPara *pstPara )
{
	pstPara->s32ClassfierAnimal = IMP_CLASSIFIER_CLASSIFIER_ANIMAL;
	pstPara->s8SvmFuncType = IMP_CLASSIFIER_SVM_FUNC_TYPE;
	pstPara->s32ClassType = IMP_CLASSIFIER_CLASS_TYPE;
	pstPara->s32UseBorderConstrain = IMP_CLASSIFIER_USE_BORDER_CONSTRAIN;
}


static IMP_S32 ipBoundaryConditionsJudgment( IMP_RECT_S *pstRectRg, IMP_RECT_S *pstRectImg, IMP_S32 s32BorderWidth )
{
	IMP_S32 s32Ret = 0;

	s32Ret = ( 0
		|| pstRectImg->s16X1 + s32BorderWidth >= pstRectRg->s16X1
		|| pstRectImg->s16Y1 + s32BorderWidth >= pstRectRg->s16Y1
		|| pstRectImg->s16X2 - s32BorderWidth <= pstRectRg->s16X2
		|| pstRectImg->s16Y2 - s32BorderWidth <= pstRectRg->s16Y2
		);

	return s32Ret;
}

IMP_VOID ipCreateTargetClassifieri( IpTargetClassifier *pstTgtClfier, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{

	memset( pstTgtClfier, 0, sizeof(IpTargetClassifier) );

	pstTgtClfier->pstHwResource = pstHwResource;
	pstTgtClfier->pstResult = pstResult;
}

IMP_VOID ipReleaseTargetClassifieri( IpTargetClassifier *pstTgtClfier )
{
	IpClassifierPara *pstParams = &pstTgtClfier->stPara;
	free_model(pstParams->pstModel,1);
	memset( pstTgtClfier, 0, sizeof(IpTargetClassifier) );
}

IMP_VOID ipConfigTargetClassifieri( IpTargetClassifier *pstTgtClfier, IpClassifierPara *pstPara )
{
	IpClassifierPara *pstParams=&pstTgtClfier->stPara;
	if (pstPara == NULL)
	{
		pstParams->s32ClassType = IMP_TGT_TYPE_HUMAN;
		pstParams->s8SvmFuncType = CLASSIFY;
		pstParams->s32ClassfierAnimal = 0;
		pstParams->s32UseBorderConstrain = 0;
	}
	else
	{
		pstParams->s32ClassType = pstPara->s32ClassType;
		pstParams->s8SvmFuncType = pstPara->s8SvmFuncType;
		pstParams->s32ClassfierAnimal = pstPara->s32ClassfierAnimal;
		pstParams->s32UseBorderConstrain = pstPara->s32UseBorderConstrain;
	}
	pstParams->stTgtPara.stHumanPara.s32MinSize = HUMAN_SIZE_MIN;
	pstParams->stTgtPara.stHumanPara.s32MaxSize = HUMAN_SIZE_MAX;
	pstParams->stTgtPara.stHumanPara.s32MinVelo = HUMAN_VELO_MIN;
	pstParams->stTgtPara.stHumanPara.s32MaxVelo = HUMAN_VELO_MAX;

	pstParams->stTgtPara.stVehiclePara.s32MinSize = VEHICLE_SIZE_MIN;
	pstParams->stTgtPara.stVehiclePara.s32MaxSize = VEHICLE_SIZE_MAX;
	pstParams->stTgtPara.stVehiclePara.s32MinVelo = VEHICLE_VELO_MIN;
	pstParams->stTgtPara.stVehiclePara.s32MaxVelo = VEHICLE_VELO_MAX;


	pstParams->pstModel = read_model(parfil_svm);
}



IMP_VOID ipComputeHuMomentInvariants(GRAY_IMAGE_S *pstImgSrc,IMP_RECT_S *pstRect, IMP_U32 s32Width,IMP_U32 s32Height,IMP_DOUBLE* pdOut,IMP_DOUBLE *pdAngle)
{

	IMP_DOUBLE m10,m01,m00,m11,m20,m02,m21,m12,m30,m03;

	IMP_DOUBLE e11,e20,e02,e30,e03,e21,e12;
	IMP_DOUBLE xb,yb,xd,yd;
	IMP_S32 i,j;
	IMP_U8 *value;

	IMP_DOUBLE a,b,c,square,theta,cs,sn,rotate_a,rotate_c,length,width,inv_m00;
	m10=0;
	m01=0;
	m00=0;
	m11=0;
	m20=0;
	m02=0;
	m21=0;
	m12=0;
	m30=0;
	m03=0;
	xb=0;
	yb=0;
	xd=0;
	yd=0;
	e11=0;
	e20=0;
	e02=0;
	e30=0;
	e03=0;
	e21=0;
	e12=0;




	for (i=pstRect->s16Y1;i<pstRect->s16Y2;i++)
	{

		for (j=pstRect->s16X1;j<pstRect->s16X2;j++)
		{
			value = pstImgSrc->pu8Data + s32Width*i + j;
			if (*value>0)
			{
				m00=m00+(*value);//data[i*COLS+j];
				m10=m10+(*value)*(j+1);
				m01=m01+(*value)*(i+1);
			}
		}
	}


	xb=m10/m00;
	yb=m01/m00;


	for (i=pstRect->s16Y1;i<pstRect->s16Y2;i++)
	{

		for (j=pstRect->s16X1;j<pstRect->s16X2;j++)
		{
			value = pstImgSrc->pu8Data + s32Width*i + j;
			if(*value>0)//data[(i-1)*COL+(j-1)]
			{
				xd=j-xb;
				yd=i-yb;
				m11=m11+xd*yd*(*value);
				m20=m20+xd*xd*(*value);
				m02=m02+yd*yd*(*value);
				m21=m21+xd*xd*yd*(*value);
				m12=m12+xd*yd*yd*(*value);
				m30=m30+xd*xd*xd*(*value);
				m03=m03+yd*yd*yd*(*value);
			}
		}
	}


	e11=m11/(m00*m00);
	e20=m20/(m00*m00);
	e02=m02/(m00*m00);
	e30=m30/pow(m00,2.5);
	e03=m03/pow(m00,2.5);
	e21=m21/pow(m00,2.5);
	e12=m12/pow(m00,2.5);

	*(pdOut)=e20+e02;
	*(pdOut+1)=(e20-e02)*(e20-e02)+e11*e11*4;
	*(pdOut+2)=(e30-3*e12)*(e30-3*e12)+(3*e21-e03)*(3*e21-e03);
	*(pdOut+3)=(e30+e12)*(e30+e12)+(e21+e03)*(e21+e03);
	*(pdOut+4)=(e30-3*e12)*(e30+e12)*((e30+e12)*(e30+e12)-3*(e21+e03)*(e21+e03))+(3*e21-e03)*(e21+e03)*(3*(e30+e12)*(e30+e12)-(e21+e03)*(e21+e03));
	*(pdOut+5)=(e20-e02)*((e30+e12)*(e30+e12)-(e21+e03)*(e21+e03))+e11*(e30+e12)*(e21+e03)*4;
	*(pdOut+6)=(3*e21-e03)*(e30+e12)*((e30+e12)*(e30+e12)-3*(e21+e03)*(e21+e03))+(3*e12-e30)*(e21+e03)*(3*(e30+e12)*(e30+e12)-(e21+e03)*(e21+e03));


	if(*(pdOut)!=0)
	{
		*(pdOut+1)=(*(pdOut+1))/((*(pdOut))*(*(pdOut)));
		*(pdOut+2)=(*(pdOut+2))/((*(pdOut))*(*(pdOut))*(*(pdOut)));
		*(pdOut+3)=(*(pdOut+3))/((*(pdOut))*(*(pdOut))*(*(pdOut)));
		*(pdOut+4)=(*(pdOut+4))/((*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut)));
		*(pdOut+5)=(*(pdOut+5))/((*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut)));
		*(pdOut+6)=(*(pdOut+6))/((*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut))*(*(pdOut)));
    }

	*(pdOut)=*pdOut>0?*pdOut:(-*pdOut);
	*(pdOut+1)=*(pdOut+1)>0?*(pdOut+1):(-*(pdOut+1));
	*(pdOut+2)=*(pdOut+2)>0?*(pdOut+2):(-*(pdOut+2));
	*(pdOut+3)=*(pdOut+3)>0?*(pdOut+3):(-*(pdOut+3));
	*(pdOut+4)=*(pdOut+4)>0?*(pdOut+4):(-*(pdOut+4));
	*(pdOut+5)=*(pdOut+5)>0?*(pdOut+5):(-*(pdOut+5));
	*(pdOut+6)=*(pdOut+6)>0?*(pdOut+6):(-*(pdOut+6));

	*(pdOut)=*(pdOut)!=0?((log10((*(pdOut))))>0?(log10((*(pdOut)))):(-log10((*(pdOut))))):*(pdOut);
	*(pdOut+1)=*(pdOut+1)!=0?((log10((*(pdOut+1))))>0?(log10((*(pdOut+1)))):(-log10((*(pdOut+1))))):*(pdOut+1);
	*(pdOut+2)=*(pdOut+2)!=0?((log10((*(pdOut+2))))>0?(log10((*(pdOut+2)))):(-log10((*(pdOut+2))))):*(pdOut+2);
	*(pdOut+3)=*(pdOut+3)!=0?((log10((*(pdOut+3))))>0?(log10((*(pdOut+3)))):(-log10((*(pdOut+3))))):*(pdOut+3);
	*(pdOut+4)=*(pdOut+4)!=0?((log10((*(pdOut+4))))>0?(log10((*(pdOut+4)))):(-log10((*(pdOut+4))))):*(pdOut+4);
	*(pdOut+5)=*(pdOut+5)!=0?((log10((*(pdOut+5))))>0?(log10((*(pdOut+5)))):(-log10((*(pdOut+5))))):*(pdOut+5);
	*(pdOut+6)=*(pdOut+6)!=0?((log10((*(pdOut+6))))>0?(log10((*(pdOut+6)))):(-log10((*(pdOut+6))))):*(pdOut+6);



	a = m20 / m00; // momentum matrix coefficients
	b = m11 / m00;
	c = m02 / m00;

	inv_m00 = 1. / m00;
	square = sqrt( 4 * b * b + (a - c) * (a - c) );
	theta = atan2( 2 * b, a - c + square ); // ellipse rotation pdAngle
	/* Calculating width & length of figure */
	cs = cos( theta );
	sn = sin( theta );

	rotate_a = cs * cs * m20 + 2 * cs * sn * m11 + sn * sn * m02;
	rotate_c = sn * sn * m20 - 2 * cs * sn * m11 + cs * cs * m02;
	length = sqrt( rotate_a * inv_m00 ) * 4; // the largest
	width = sqrt( rotate_c * inv_m00 ) * 4; // and the smallest ellipse axises.

	// In case, when theta is 0 or 1.57... the Length & Width may be exchanged
	if( length < width )
	{
		IMP_DOUBLE t;
		IMP_SWAP( length, width, t );
		IMP_SWAP( cs, sn, t );
		theta = IMP_PI*0.5 - theta;
	}

	*pdAngle=theta;
}

static IMP_S32 ipGetTargetActualArea(IpTrackedTarget *pstTarget)
{
	IpTargetPosition *pos;
	IMP_S32 i;
	IMP_S32 len;
	IMP_S32 area = 0;
	len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);

	if (len > 5)
		len = 5;
	for (i = 0; i < len; i++)
	{
		pos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -i);
		area += pos->u32AreaActual;
	}
	area /= len;
#if defined(IMP_DEBUG_PRINT)
	printf("area = %d\n", area);
#endif
	return area;
}


static IMP_S32 ipConstrainClassOnSize( IpTrackedTarget *pstTarget,
									CF_TGT_PARA_S *pstPara,
									IMP_S32 indoor,
									IMP_S32 *is_possible_human,
									IMP_S32 *is_possible_vehicle )
{
 	CF_TGT_CONSTRAIN_PARA_S *tpara_h = &pstPara->stHumanPara;
 	CF_TGT_CONSTRAIN_PARA_S *tpara_v = &pstPara->stVehiclePara;
	IMP_S32 area, velocity;
	IpTargetPosition *pos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);

	// 	area = pos->area_actual;
	area = ipGetTargetActualArea(pstTarget);
	velocity = pos->s32Velocity;

	*is_possible_human = ( 1
		&& area >= tpara_h->s32MinSize
		&& area <= tpara_h->s32MaxSize
// 		&& velocity >= tpara_h->velo_min
// 		&& velocity <= tpara_h->velo_max
		);
	*is_possible_vehicle = ( 1
		&& area >= tpara_v->s32MinSize//250*100
		&& area <= tpara_v->s32MaxSize//350*200
// 		&& velocity >= tpara_v->velo_min
// 		&& velocity <= tpara_v->velo_max
		);
	if( indoor )
		*is_possible_vehicle = 0;

// 	*is_possible_human = ( 1
// 		&& area >= tpara_h->size_min
// 		&& area <= tpara_h->size_max
// 		&& velocity >= tpara_h->velo_min
// 		&& velocity <= tpara_h->velo_max
// 		);
// 	*is_possible_vehicle = ( 1
// 		&& area >= tpara_v->size_min
// 		&& area <= tpara_v->size_max
// 		&& velocity >= tpara_v->velo_min
// 		&& velocity <= tpara_v->velo_max
// 		);
// 	if( indoor )
// 		*is_possible_vehicle = 0;
	return *is_possible_human || *is_possible_vehicle ;
}


IMP_S32 ipProcessTargetClassifierInternal( IpTargetClassifier *pstClassifier )
{
	IpTrackedTarget *pstTarget;
	IpTargetPosition *pstPos0;
	IMP_U8 *value1,*value2;
	IMP_S32 i,j,m,n,cnt,c,targetType;
	IpClassFeature *astClassFeature;

	WORDSVM words[15];

	svm_node x[15];
	DOC *doc;

 	IMP_RECT_S *rc;
	IMP_RECT_S pstRectImg;
	IMP_S32 j13;//=blobObjCur.pt.y+blobObjCur.blobHeight/2-blobObjCur.blobHeight/3;
	IMP_S32 j23;//=blobObjCur.pt.y+blobObjCur.blobHeight/2-2*blobObjCur.blobHeight/3;

	IMP_S32 is_possible_human,is_possible_vehicle;

	IMP_FLOAT type;
	IMP_S32 m13,m23,mBG,mFG,mPerimeter;
	PEA_RESULT_S *pstResult = pstClassifier->pstResult;

	PEA_DETECTED_REGIONSET_S *pstRegions = &pstResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgFgOrg = pstRegions->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgMediate = pstRegions->pstImgMediate;

	IMP_S32 s32Width = pstImgFgOrg->s32W;
	IMP_S32 s32Height = pstImgFgOrg->s32H;

	IMP_S32 blobWidth,blobHeight;
	IpClassifierPara *pstParams=&pstClassifier->stPara;
	IMP_S32 s32UseBorderConstrain=pstParams->s32UseBorderConstrain;
	IMP_S32 s32SceneMode = 0;

	pstTarget = pstResult->stTrackedTargetSet.astTargets;
	cnt = pstResult->stTrackedTargetSet.s32UsedTotal;
	astClassFeature=pstResult->stTrackedTargetSet.astClassFeature;

	if (pstParams->pstRule->stEvrmentTgtPara.stScenePara.s32SceneLmt)
	{
		s32SceneMode = pstParams->pstRule->stEvrmentTgtPara.stScenePara.s32SceneMode;
	}

	if (pstParams->s8SvmFuncType==DISABLE)
	{
		return -1;
	}
	IMP_GrayImageClone(pstImgFgOrg,pstImgMediate);
	for( i=0, j=0; i<IMP_MAX_TGT_CNT; i++ )
	{
		if (ipTrackedTargetIsActive(pstTarget))
		{
#if 0		
			{//added by mzhang, just for debug.
				pstTarget->stTargetInfo.s32HumanLikehood = 100;
				pstTarget->stTargetInfo.s32VehicleLikehood = -1;
				pstTarget->stTargetInfo.u32Type=IMP_TGT_TYPE_HUMAN; 
				continue; 
			}
#endif
			{
				targetType = 0;

				if(!targetType)
				{
					pstPos0 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );

					rc=&pstPos0->stRg;

					if( s32UseBorderConstrain )
					{
						pstRectImg.s16X1 = 0;
						pstRectImg.s16Y1 = 0;
						pstRectImg.s16X2 = s32Width - 1;
						pstRectImg.s16Y2 = s32Height - 1;

					}
					if (!s32UseBorderConstrain ||
						s32UseBorderConstrain && !ipBoundaryConditionsJudgment( rc, &pstRectImg, s32UseBorderConstrain )//加入了限定要求并且满足边界条件
						)
					{
						m13=0;
						m23=0;
						mBG=0;
						mFG=0;
						mPerimeter=0;

						j13=rc->s16Y2-(rc->s16Y2-rc->s16Y1)/3;
						j23=rc->s16Y2-2*(rc->s16Y2-rc->s16Y1)/3;

						for (m=rc->s16X1;m<rc->s16X2;m++)
						{
							value1=pstImgFgOrg->pu8Data+ s32Width*j13 + m;
							value2=pstImgFgOrg->pu8Data+ s32Width*j23 + m;
							if(*value1>0)
							{
								m13++;
							}
							if(*value2>0)
							{
								m23++;
							}
						}

						for (m=rc->s16Y1;m<rc->s16Y2;m++)
						{
							for (n=rc->s16X1;n<rc->s16X2;n++)
							{
								value1=pstImgFgOrg->pu8Data+ s32Width*m + n;
								if (*value1==0)
								{
									mBG++;
								}
							}
						}



						for (m=rc->s16Y1;m<rc->s16Y2;m++)
						{

							for (n=rc->s16X1;n<rc->s16X2;n++)
							{
								value1=pstImgMediate->pu8Data+ s32Width*m + n;
								if (*value1>0)
								{
									mPerimeter++;
									*value1=100;
									break;
								}
							}

							for (n=rc->s16X2;n<rc->s16X1;n--)
							{
								value1=pstImgMediate->pu8Data+ s32Width*m + n;
								if (*value1==100)
								{
									break;
								}
								else if (*value1>0)
								{
									mPerimeter++;
									*value1=100;
									break;
								}
							}
						}

						for (n=rc->s16X1;n<rc->s16X2;n++)
						{

							for (m=rc->s16Y1;m<rc->s16Y2;m++)
							{
								value1=pstImgMediate->pu8Data+ s32Width*m + n;
								if (*value1==100)
									break;
								if (*value1>0)
								{
									mPerimeter++;
									*value1=100;
									break;
								}
							}

							for (m=rc->s16Y2;m<rc->s16Y1;m--)
							{
								value1=pstImgMediate->pu8Data+ s32Width*m + n;
								if (*value1==100)
									break;
								if (*value1>0)
								{
									mPerimeter++;
									*value1=100;
									break;
								}
							}
						}
						blobWidth=rc->s16X2-rc->s16X1+1;
						blobHeight=rc->s16Y2-rc->s16Y1+1;

						mFG=blobWidth*blobHeight-mBG;
						astClassFeature->_P=(IMP_DOUBLE)(blobWidth)/blobHeight;
						astClassFeature->_P13=(IMP_DOUBLE)(m13)/blobHeight;
						astClassFeature->_P23=(IMP_DOUBLE)(m23)/blobHeight;

						astClassFeature->_I=(IMP_DOUBLE)(mBG)/(blobWidth*blobHeight);
						astClassFeature->_D=(IMP_DOUBLE)mPerimeter/mFG;


						ipComputeHuMomentInvariants(pstImgFgOrg,rc,s32Width,s32Height,astClassFeature->_Hu,&astClassFeature->_Axis);

						astClassFeature->_Delta=0;

						astClassFeature->label=pstParams->s32ClassType;
						if (pstParams->s8SvmFuncType==CLASSIFY)
						{

							for (i=0;i<=13;i++)
							{
								x[i].index = i+1;
							}

							x[14].index = -1;
							c=-1;
							x[0].value = astClassFeature->_Axis;
							x[1].value = astClassFeature->_D;
							x[2].value = astClassFeature->_Delta;
							x[3].value = astClassFeature->_Hu[0];
							x[4].value = astClassFeature->_Hu[1];
							x[5].value = astClassFeature->_Hu[2];
							x[6].value = astClassFeature->_Hu[3];
							x[7].value = astClassFeature->_Hu[4];
							x[8].value = astClassFeature->_Hu[5];
							x[9].value = astClassFeature->_Hu[6];
							x[10].value = astClassFeature->_I;
							x[11].value = astClassFeature->_P;
							x[12].value = astClassFeature->_P13;
							x[13].value = astClassFeature->_P23;


							// 				(*label)[i]=classFeatureTemp.label;
							// 				(queryid)=0;
							// 				(slackid)=0;
							// 				(costfactor)=1;

							words[0].wnum = 1;
							words[0].weight = (float)astClassFeature->_Axis;
							words[1].wnum = 2;
							words[1].weight = (float)astClassFeature->_D;
							words[2].wnum = 3;
							words[2].weight = (float)astClassFeature->_Delta;

							words[3].wnum = 4;
							words[3].weight = (float)astClassFeature->_Hu[0];
							words[4].wnum = 5;
							words[4].weight = (float)astClassFeature->_Hu[1];
							words[5].wnum = 6;
							words[5].weight = (float)astClassFeature->_Hu[2];
							words[6].wnum = 7;
							words[6].weight = (float)astClassFeature->_Hu[3];
							words[7].wnum = 8;
							words[7].weight = (float)astClassFeature->_Hu[4];
							words[8].wnum = 9;
							words[8].weight = (float)astClassFeature->_Hu[5];
							words[9].wnum = 10;
							words[9].weight = (float)astClassFeature->_Hu[6];

							words[10].wnum = 11;
							words[10].weight = (float)astClassFeature->_I;
							words[11].wnum = 12;
							words[11].weight = (float)astClassFeature->_P;
							words[12].wnum = 13;
							words[12].weight = (float)astClassFeature->_P13;
							words[13].wnum = 14;
							words[13].weight = (float)astClassFeature->_P23;

							words[14].wnum=0;

							//c= (IMP_S32)svm_predict(pstParams->m_model, x);

							doc = create_example(-1,0,0,0.0,create_svector(words,"",1.0));

							type=(float)classify_example(pstParams->pstModel,doc);
							if (type>0 && type <2)
							{
								c=IMP_TGT_TYPE_HUMAN;
								pstTarget->stTargetInfo.s32HumanLikehood++;
								pstTarget->stTargetInfo.s32VehicleLikehood--;

							}
							else if (type>2 && type <4)
							{
								c=IMP_TGT_TYPE_VEHICLE;

								pstTarget->stTargetInfo.s32VehicleLikehood++;
								pstTarget->stTargetInfo.s32HumanLikehood--;
							}

							free_example(doc,1);
						}
				}
			}
			}

			if (pstTarget->stTargetInfo.s32VehicleLikehood>100)
			{
				pstTarget->stTargetInfo.s32VehicleLikehood=100;
			}
			else if(pstTarget->stTargetInfo.s32VehicleLikehood<-1)
			{
				pstTarget->stTargetInfo.s32VehicleLikehood=-1;
			}

			if (pstTarget->stTargetInfo.s32VehicleLikehood>100)
			{
				pstTarget->stTargetInfo.s32VehicleLikehood=100;
			}
			else if(pstTarget->stTargetInfo.s32VehicleLikehood<-1)
			{
				pstTarget->stTargetInfo.s32VehicleLikehood=-1;
			}

			if (pstTarget->stTargetInfo.s32HumanLikehood &&
				pstTarget->stTargetInfo.s32HumanLikehood >= pstTarget->stTargetInfo.s32VehicleLikehood
				)
			{
				pstTarget->stTargetInfo.u32Type=IMP_TGT_TYPE_HUMAN;
				//printf("target Type is HUMAN\n");
			}
			else if (pstTarget->stTargetInfo.s32VehicleLikehood &&
				pstTarget->stTargetInfo.s32VehicleLikehood >= pstTarget->stTargetInfo.s32HumanLikehood
				)
			{
				pstTarget->stTargetInfo.u32Type=IMP_TGT_TYPE_VEHICLE;
				//printf("target Type is VEHICLE\n");
			}
			else
			{
				pstTarget->stTargetInfo.u32Type=IMP_TGT_TYPE_UNKNOWN;
				//printf("target Type is UNKNOWN\n");
			}
		}

		j += pstTarget->s32Used ? 1 : 0;
		if( j>=cnt ) break;
		astClassFeature++;
		pstTarget++;
	}


	return 1;
}


IMP_S32 ipPostProcessTargetClassifieri( IpTargetClassifier *pstTgtClfier )
{
	return 1;
}


IMP_MODULE_HANDLE ipCreateClassifier( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce )
{
	IMP_MODULE_HANDLE hModule=NULL;
	IpModuleClassifier *pModule=NULL;
	IpTargetClassifier *pClassifier=NULL;

	pModule = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpModuleClassifier) );
	pModule->pClassifier = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpTargetClassifier) );
	pClassifier = pModule->pClassifier;
	ipCreateTargetClassifieri( pClassifier, pResult, pHwResouce );

	hModule = (IMP_MODULE_HANDLE)pModule;
	return hModule;
}

IMP_VOID ipReleaseClassifier( IMP_MODULE_HANDLE hModule )
{
	IpModuleClassifier *pModule = (IpModuleClassifier*)hModule;
	IpTargetClassifier *pClassifier = pModule->pClassifier;
	GA_HARDWARE_RS_S *pHwResouce = pClassifier->pstHwResource;
	ipReleaseTargetClassifieri( pClassifier );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pClassifier );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule );
}

IMP_VOID ipConfigClassifier( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstParaInner )
{
	IpModuleClassifier *pModule = (IpModuleClassifier*)hModule;
	IpTargetClassifier *pClassifier = pModule->pClassifier;
	DATA_BUF_S *pDatBuf;
	IpClassifierPara *pstPara = &pClassifier->stPara;


	pstPara->pstRule = pstParaInner->pRule;

 	pDatBuf = &pstParaInner->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_CLASSIFIER];
  	ipParseClassifierParaData(pstPara );
	ipConfigTargetClassifieri( pClassifier, pstPara );
}

IMP_S32 IMP_PEA_ProcessClassifier( IMP_MODULE_HANDLE hModule )
{
	IMP_S32 s32Ret;
	IpModuleClassifier *pModule = (IpModuleClassifier*)hModule;
	IpTargetClassifier *pClassifier = pModule->pClassifier;

	s32Ret = ipProcessTargetClassifierInternal( pClassifier );

	return s32Ret;
}

IMP_S32 ipPostProcessClassifier( IMP_MODULE_HANDLE hModule )
{
	IpModuleClassifier *pModule = (IpModuleClassifier*)hModule;
	IpTargetClassifier *pClassifier = pModule->pClassifier;

	return ipPostProcessTargetClassifieri( pClassifier );
}

