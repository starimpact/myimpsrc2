
/* Haar features calculation */

#include "imp_vfd_haar.h"
#include "imp_ga_image_algo.h"
#include "imp_ga_image_mgr.h"

/* IPP functions for object detection */
icvHaarClassifierInitAlloc_32f_t icvHaarClassifierInitAlloc_32f_p = 0;
icvHaarClassifierFree_32f_t icvHaarClassifierFree_32f_p = 0;
icvApplyHaarClassifier_32s32f_C1R_t icvApplyHaarClassifier_32s32f_C1R_p = 0;
icvRectStdDev_32s32f_C1R_t icvRectStdDev_32s32f_C1R_p = 0;

const int icv_object_win_border = 1;
const float icv_stage_threshold_bias = 0.0001f;

IMP_HaarClassifierCascade* IMP_CreateHaarClassifierCascade( int stage_count )
{
    IMP_HaarClassifierCascade* cascade = 0;
    
    int block_size = sizeof(*cascade) + stage_count*sizeof(*cascade->stage_classifier);
	
    if( stage_count <= 0 )
        printf( "Number of stages should be positive\n" );
	
    cascade = (IMP_HaarClassifierCascade*)malloc( block_size );
    memset( cascade, 0, block_size );
	
    cascade->stage_classifier = (IMP_HaarStageClassifier*)(cascade + 1);
    cascade->flags = IMP_HAAR_MAGIC_VAL;
    cascade->count = stage_count;	
	
    return cascade;
}

void IMP_ReleaseHidHaarClassifierCascade( IMP_HidHaarClassifierCascade** _cascade )
{
    if( _cascade && *_cascade )
    {
        IMP_HidHaarClassifierCascade* cascade = *_cascade;
        if( cascade->ipp_stages && icvHaarClassifierFree_32f_p )
        {
            int i;
            for( i = 0; i < cascade->count; i++ )
            {
                if( cascade->ipp_stages[i] )
                    icvHaarClassifierFree_32f_p( cascade->ipp_stages[i] );
            }
        }
        free( cascade->ipp_stages );
        free( *_cascade );
    }
}


/* create more efficient internal representation of haar classifier cascade */
static IMP_HidHaarClassifierCascade*
IMP_CreateHidHaarClassifierCascade( IMP_HaarClassifierCascade* cascade )
{
    Imp_Rect* ipp_features = 0;
    float *ipp_weights = 0, *ipp_thresholds = 0, *ipp_val1 = 0, *ipp_val2 = 0;
    int* ipp_counts = 0;
	int er = 0;
    IMP_HidHaarClassifierCascade* out = 0;
	
     int i, j, k, l;
    int datasize;
    int total_classifiers = 0;
    int total_nodes = 0;
    char errorstr[100];
    IMP_HidHaarClassifier* haar_classifier_ptr;
    IMP_HidHaarTreeNode* haar_node_ptr;
    Imp_Size orig_window_size;
    int has_tilted_features = 0;
    int max_count = 0;
	
//     if( !IMP_IS_HAAR_CLASSIFIER(cascade) )
//         CV_ERROR( !cascade ? CV_StsNullPtr : CV_StsBadArg, "Invalid classifier pointer" );
// 	
//     if( cascade->hid_cascade )
//         CV_ERROR( CV_StsError, "hid_cascade has been already created" );
// 	
//     if( !cascade->stage_classifier )
//         CV_ERROR( CV_StsNullPtr, "" );
// 	
//     if( cascade->count <= 0 )
//         CV_ERROR( CV_StsOutOfRange, "Negative number of cascade stages" );
	
    orig_window_size = cascade->orig_window_size;
    
    /* check input structure correctness and calculate total memory size needed for
	internal representation of the classifier cascade */
    for( i = 0; i < cascade->count; i++ )
    {
        IMP_HaarStageClassifier* stage_classifier = cascade->stage_classifier + i;
		
        if( !stage_classifier->classifier ||
            stage_classifier->count <= 0 )
        {
//             sprintf( errorstr, "header of the stage classifier #%d is invalid "
// 				"(has null pointers or non-positive classfier count)", i );
//             CV_ERROR( CV_StsError, errorstr );
			er = 1;
        }
		
        max_count = IMP_MAX( max_count, stage_classifier->count );
        total_classifiers += stage_classifier->count;
		
        for( j = 0; j < stage_classifier->count; j++ )
        {
            IMP_HaarClassifier* classifier = stage_classifier->classifier + j;
			
            total_nodes += classifier->count;
            for( l = 0; l < classifier->count; l++ )
            {
                for( k = 0; k < IMP_HAAR_FEATURE_MAX; k++ )
                {
                    if( classifier->haar_feature[l].rect[k].r.width )
                    {
                        Imp_Rect r = classifier->haar_feature[l].rect[k].r;
                        int tilted = classifier->haar_feature[l].tilted;
                        has_tilted_features |= tilted != 0;
                        if( r.width < 0 || r.height < 0 || r.y < 0 ||
                            r.x + r.width > orig_window_size.width
                            ||
                            (!tilted &&
                            (r.x < 0 || r.y + r.height > orig_window_size.height))
                            ||
                            (tilted && (r.x - r.height < 0 ||
                            r.y + r.width + r.height > orig_window_size.height)))
                        {
//                             sprintf( errorstr, "rectangle #%d of the classifier #%d of "
// 								"the stage classifier #%d is not inside "
// 								"the reference (original) cascade window", k, j, i );
//                             CV_ERROR( CV_StsNullPtr, errorstr );
							  er = 2;
                        }
                    }
                }
            }
        }
    }
	
    // this is an upper boundary for the whole hidden cascade size
    datasize = sizeof(IMP_HidHaarClassifierCascade) +
		sizeof(IMP_HidHaarStageClassifier)*cascade->count +
		sizeof(IMP_HidHaarClassifier) * total_classifiers +
		sizeof(IMP_HidHaarTreeNode) * total_nodes +
		sizeof(void*)*(total_nodes + total_classifiers);
	
    out = (IMP_HidHaarClassifierCascade*)malloc( datasize );
    memset( out, 0, sizeof(*out) );
	
    /* init header */
    out->count = cascade->count;
    out->stage_classifier = (IMP_HidHaarStageClassifier*)(out + 1);
    haar_classifier_ptr = (IMP_HidHaarClassifier*)(out->stage_classifier + cascade->count);
    haar_node_ptr = (IMP_HidHaarTreeNode*)(haar_classifier_ptr + total_classifiers);
	
    out->is_stump_based = 1;
    out->has_tilted_features = has_tilted_features;
    out->is_tree = 0;
	
    /* initialize internal representation */
    for( i = 0; i < cascade->count; i++ )
    {
        IMP_HaarStageClassifier* stage_classifier = cascade->stage_classifier + i;
        IMP_HidHaarStageClassifier* hid_stage_classifier = out->stage_classifier + i;
		
        hid_stage_classifier->count = stage_classifier->count;
        hid_stage_classifier->threshold = stage_classifier->threshold - icv_stage_threshold_bias;
        hid_stage_classifier->classifier = haar_classifier_ptr;
        hid_stage_classifier->two_rects = 1;
        haar_classifier_ptr += stage_classifier->count;
		
        hid_stage_classifier->parent = (stage_classifier->parent == -1)
            ? NULL : out->stage_classifier + stage_classifier->parent;
        hid_stage_classifier->next = (stage_classifier->next == -1)
            ? NULL : out->stage_classifier + stage_classifier->next;
        hid_stage_classifier->child = (stage_classifier->child == -1)
            ? NULL : out->stage_classifier + stage_classifier->child;
        
        out->is_tree |= hid_stage_classifier->next != NULL;
		
        for( j = 0; j < stage_classifier->count; j++ )
        {
            IMP_HaarClassifier* classifier = stage_classifier->classifier + j;
            IMP_HidHaarClassifier* hid_classifier = hid_stage_classifier->classifier + j;
            int node_count = classifier->count;
            float* alpha_ptr = (float*)(haar_node_ptr + node_count);
			
            hid_classifier->count = node_count;
            hid_classifier->node = haar_node_ptr;
            hid_classifier->alpha = alpha_ptr;
            
            for( l = 0; l < node_count; l++ )
            {
                IMP_HidHaarTreeNode* node = hid_classifier->node + l;
                IMP_HaarFeature* feature = classifier->haar_feature + l;
                memset( node, -1, sizeof(*node) );
                node->threshold = classifier->threshold[l];
                node->left = classifier->left[l];
                node->right = classifier->right[l];
				
                if( fabs(feature->rect[2].weight) < IMP_DBL_EPSILON ||
                    feature->rect[2].r.width == 0 ||
                    feature->rect[2].r.height == 0 )
                    memset( &(node->feature.rect[2]), 0, sizeof(node->feature.rect[2]) );
                else
                    hid_stage_classifier->two_rects = 0;
            }
			
            memcpy( alpha_ptr, classifier->alpha, (node_count+1)*sizeof(alpha_ptr[0]));
            haar_node_ptr =
                (IMP_HidHaarTreeNode*)IMP_AlignPtr(alpha_ptr+node_count+1, sizeof(void*));
			
            out->is_stump_based &= node_count == 1;
        }
    }
	
    //
    // NOTE: Currently, OpenMP is implemented and IPP modes are incompatible.
    // 
#ifndef _OPENMP
    {
		int can_use_ipp = icvHaarClassifierInitAlloc_32f_p != 0 &&
			icvHaarClassifierFree_32f_p != 0 &&
			icvApplyHaarClassifier_32s32f_C1R_p != 0 &&
			icvRectStdDev_32s32f_C1R_p != 0 &&
			!out->has_tilted_features && !out->is_tree && out->is_stump_based;
		
		if( can_use_ipp )
		{
			int ipp_datasize = cascade->count*sizeof(out->ipp_stages[0]);
			float ipp_weight_scale=(float)(1./((orig_window_size.width-icv_object_win_border*2)*
				(orig_window_size.height-icv_object_win_border*2)));
			
			 out->ipp_stages = (void**)malloc( ipp_datasize );
			memset( out->ipp_stages, 0, ipp_datasize );
			
		    ipp_features = (Imp_Rect*)malloc( max_count*3*sizeof(ipp_features[0]) );
			ipp_weights = (float*)malloc( max_count*3*sizeof(ipp_weights[0]) );
			ipp_thresholds = (float*)malloc( max_count*sizeof(ipp_thresholds[0]) );
			ipp_val1 = (float*)malloc( max_count*sizeof(ipp_val1[0]) );
			ipp_val2 = (float*)malloc( max_count*sizeof(ipp_val2[0]) );
			ipp_counts = (int*)malloc( max_count*sizeof(ipp_counts[0]) );
			
			for( i = 0; i < cascade->count; i++ )
			{
				IMP_HaarStageClassifier* stage_classifier = cascade->stage_classifier + i;
				for( j = 0, k = 0; j < stage_classifier->count; j++ )
				{
					IMP_HaarClassifier* classifier = stage_classifier->classifier + j;
					int rect_count = 2 + (classifier->haar_feature->rect[2].r.width != 0);
					
					ipp_thresholds[j] = classifier->threshold[0];
					ipp_val1[j] = classifier->alpha[0];
					ipp_val2[j] = classifier->alpha[1];
					ipp_counts[j] = rect_count;
					
					for( l = 0; l < rect_count; l++, k++ )
					{
						ipp_features[k] = classifier->haar_feature->rect[l].r;
						//ipp_features[k].y = orig_window_size.height - ipp_features[k].y - ipp_features[k].height;
						ipp_weights[k] = classifier->haar_feature->rect[l].weight*ipp_weight_scale;
					}
				}
				
				if( icvHaarClassifierInitAlloc_32f_p( &out->ipp_stages[i],
					ipp_features, ipp_weights, ipp_thresholds,
					ipp_val1, ipp_val2, ipp_counts, stage_classifier->count ) < 0 )
					break;
			}
			
			if( i < cascade->count )
			{
				for( j = 0; j < i; j++ )
					if( icvHaarClassifierFree_32f_p && out->ipp_stages[i] )
						icvHaarClassifierFree_32f_p( out->ipp_stages[i] );
					free( out->ipp_stages );
			}
		}
    }
#endif
	
    cascade->hid_cascade = out;
    assert( (char*)haar_node_ptr - (char*)out <= datasize );
	
 
    if( er )
        IMP_ReleaseHidHaarClassifierCascade( &out );
	
    free( ipp_features );
    free( ipp_weights );
    free( ipp_thresholds );
    free( ipp_val1 );
    free( ipp_val2 );
    free( ipp_counts );
	
    return out;
}


#define sum_elem_ptr(sum,row,col)  \
((sumtype*)IMP_MAT_ELEM_PTR_FAST((sum),(row),(col),sizeof(sumtype)))

#define sqsum_elem_ptr(sqsum,row,col)  \
((sqsumtype*)IMP_MAT_ELEM_PTR_FAST((sqsum),(row),(col),sizeof(sqsumtype)))

#define calc_sum(rect,offset) \
((rect).p0[offset] - (rect).p1[offset] - (rect).p2[offset] + (rect).p3[offset])


void
IMP_SetImagesForHaarClassifierCascade( IMP_HaarClassifierCascade* _cascade,
									const IMP_Mat* _sum,
									const IMP_Mat* _sqsum,
									const IMP_Mat* _tilted_sum,
									double scale )
{
    
    IMP_Mat sum_stub, *sum = (IMP_Mat*)_sum;
    IMP_Mat sqsum_stub, *sqsum = (IMP_Mat*)_sqsum;
    IMP_Mat tilted_stub, *tilted = (IMP_Mat*)_tilted_sum;
    IMP_HidHaarClassifierCascade* cascade;
    int coi0 = 0, coi1 = 0;
    int i;
    Imp_Rect equ_rect;
    double weight_scale;
	
    if( !IMP_IS_HAAR_CLASSIFIER(_cascade) )
        //CV_ERROR( !_cascade ? CV_StsNullPtr : CV_StsBadArg, "Invalid classifier pointer" );
		printf("Invalid classifier pointer\n");
	
    if( scale <= 0 )
        //CV_ERROR( CV_StsOutOfRange, "Scale must be positive" );
		printf("Scale must be positive\n");
	
	//     CV_CALL( sum = cvGetMat( sum, &sum_stub, &coi0 ));
	//     CV_CALL( sqsum = cvGetMat( sqsum, &sqsum_stub, &coi1 ));
	// 
	//     if( coi0 || coi1 )
	//         CV_ERROR( CV_BadCOI, "COI is not supported" );
	
    if( !IMP_ARE_SIZES_EQ( sum, sqsum ))
 //        CV_ERROR( CV_StsUnmatchedSizes, "All integral images must have the same size" );
		printf("All integral images must have the same size\n");
	
//     if( CV_MAT_TYPE(sqsum->type) != CV_64FC1 ||
//         CV_MAT_TYPE(sum->type) != CV_32SC1 )
//         CV_ERROR( CV_StsUnsupportedFormat,
//         "Only (32s, 64f, 32s) combination of (sum,sqsum,tilted_sum) formats is allowed" );
	
    if( !_cascade->hid_cascade )
         IMP_CreateHidHaarClassifierCascade(_cascade) ;
	
    cascade = _cascade->hid_cascade;
	
    if( cascade->has_tilted_features )
    {
		//         CV_CALL( tilted = cvGetMat( tilted, &tilted_stub, &coi1 ));
		// 
		//         if( CV_MAT_TYPE(tilted->type) != CV_32SC1 )
		//             CV_ERROR( CV_StsUnsupportedFormat,
		//             "Only (32s, 64f, 32s) combination of (sum,sqsum,tilted_sum) formats is allowed" );
		
        if( sum->step != tilted->step )
//             CV_ERROR( CV_StsUnmatchedSizes,
//             "Sum and tilted_sum must have the same stride (step, widthStep)" );
			printf("Sum and tilted_sum must have the same stride (step, widthStep)\n");
		
        if( !IMP_ARE_SIZES_EQ( sum, tilted ))
           // CV_ERROR( CV_StsUnmatchedSizes, "All integral images must have the same size" );
		   printf("All integral images must have the same size\n");
        cascade->tilted = *tilted;
    }
    
    _cascade->scale = scale;
    _cascade->real_window_size.width = IMP_Round( _cascade->orig_window_size.width * scale );
    _cascade->real_window_size.height = IMP_Round( _cascade->orig_window_size.height * scale );
	
    cascade->sum = *sum;
    cascade->sqsum = *sqsum;
    
    equ_rect.x = equ_rect.y = IMP_Round(scale);
    equ_rect.width = IMP_Round((_cascade->orig_window_size.width-2)*scale);
    equ_rect.height = IMP_Round((_cascade->orig_window_size.height-2)*scale);
    weight_scale = 1./(equ_rect.width*equ_rect.height);
    cascade->inv_window_area = weight_scale;
	
    cascade->p0 = sum_elem_ptr(*sum, equ_rect.y, equ_rect.x);
    cascade->p1 = sum_elem_ptr(*sum, equ_rect.y, equ_rect.x + equ_rect.width );
    cascade->p2 = sum_elem_ptr(*sum, equ_rect.y + equ_rect.height, equ_rect.x );
    cascade->p3 = sum_elem_ptr(*sum, equ_rect.y + equ_rect.height,
		equ_rect.x + equ_rect.width );
	
    cascade->pq0 = sqsum_elem_ptr(*sqsum, equ_rect.y, equ_rect.x);
    cascade->pq1 = sqsum_elem_ptr(*sqsum, equ_rect.y, equ_rect.x + equ_rect.width );
    cascade->pq2 = sqsum_elem_ptr(*sqsum, equ_rect.y + equ_rect.height, equ_rect.x );
    cascade->pq3 = sqsum_elem_ptr(*sqsum, equ_rect.y + equ_rect.height,
		equ_rect.x + equ_rect.width );
	
		/* init pointers in haar features according to real window size and
	given image pointers */
    {
// #ifdef _OPENMP
// 		int max_threads = cvGetNumThreads();
// #pragma omp parallel for num_threads(max_threads), schedule(dynamic) 
// #endif // _OPENMP
		for( i = 0; i < _cascade->count; i++ )
		{
			int j, k, l;
			for( j = 0; j < cascade->stage_classifier[i].count; j++ )
			{
				for( l = 0; l < cascade->stage_classifier[i].classifier[j].count; l++ )
				{
					IMP_HaarFeature* feature = 
						&_cascade->stage_classifier[i].classifier[j].haar_feature[l];
						/* IMP_HidHaarClassifier* classifier =
                    cascade->stage_classifier[i].classifier + j; */
					IMP_HidHaarFeature* hidfeature = 
						&cascade->stage_classifier[i].classifier[j].node[l].feature;
					double sum0 = 0, area0 = 0;
					Imp_Rect r[3];
#if IMP_ADJUST_FEATURES
					int base_w = -1, base_h = -1;
					int new_base_w = 0, new_base_h = 0;
					int kx, ky;
					int flagx = 0, flagy = 0;
					int x0 = 0, y0 = 0;
#endif
					int nr;
					
					/* align blocks */
					for( k = 0; k < IMP_HAAR_FEATURE_MAX; k++ )
					{
						if( !hidfeature->rect[k].p0 )
							break;
#if IMP_ADJUST_FEATURES
						r[k] = feature->rect[k].r;
						base_w = (int)IMP_IMIN( (unsigned)base_w, (unsigned)(r[k].width-1) );
						base_w = (int)IMP_IMIN( (unsigned)base_w, (unsigned)(r[k].x - r[0].x-1) );
						base_h = (int)IMP_IMIN( (unsigned)base_h, (unsigned)(r[k].height-1) );
						base_h = (int)IMP_IMIN( (unsigned)base_h, (unsigned)(r[k].y - r[0].y-1) );
#endif
					}
					
					nr = k;
					
#if IMP_ADJUST_FEATURES
					base_w += 1;
					base_h += 1;
					kx = r[0].width / base_w;
					ky = r[0].height / base_h;
					
					if( kx <= 0 )
					{
						flagx = 1;
						new_base_w = IMP_Round( r[0].width * scale ) / kx;
						x0 = IMP_Round( r[0].x * scale );
					}
					
					if( ky <= 0 )
					{
						flagy = 1;
						new_base_h = IMP_Round( r[0].height * scale ) / ky;
						y0 = IMP_Round( r[0].y * scale );
					}
#endif
					
					for( k = 0; k < nr; k++ )
					{
						Imp_Rect tr;
						double correction_ratio;
						
#if IMP_ADJUST_FEATURES
						if( flagx )
						{
							tr.x = (r[k].x - r[0].x) * new_base_w / base_w + x0;
							tr.width = r[k].width * new_base_w / base_w;
						}
						else
#endif
						{
							tr.x = IMP_Round( r[k].x * scale );
							tr.width = IMP_Round( r[k].width * scale );
						}
						
#if IMP_ADJUST_FEATURES
						if( flagy )
						{
							tr.y = (r[k].y - r[0].y) * new_base_h / base_h + y0;
							tr.height = r[k].height * new_base_h / base_h;
						}
						else
#endif
						{
							tr.y = IMP_Round( r[k].y * scale );
							tr.height = IMP_Round( r[k].height * scale );
						}
						
#if IMP_ADJUST_WEIGHTS
						{
							// RAINER START
							const float orig_feature_size =  (float)(feature->rect[k].r.width)*feature->rect[k].r.height; 
							const float orig_norm_size = (float)(_cascade->orig_window_size.width)*(_cascade->orig_window_size.height);
							const float feature_size = float(tr.width*tr.height);
							//const float normSize    = float(equ_rect.width*equ_rect.height);
							float target_ratio = orig_feature_size / orig_norm_size;
							//float isRatio = featureSize / normSize;
							//correctionRatio = targetRatio / isRatio / normSize;
							correction_ratio = target_ratio / feature_size;
							// RAINER END
						}
#else
						correction_ratio = weight_scale * (!feature->tilted ? 1 : 0.5);
#endif
						
						if( !feature->tilted )
						{
							hidfeature->rect[k].p0 = sum_elem_ptr(*sum, tr.y, tr.x);
							hidfeature->rect[k].p1 = sum_elem_ptr(*sum, tr.y, tr.x + tr.width);
							hidfeature->rect[k].p2 = sum_elem_ptr(*sum, tr.y + tr.height, tr.x);
							hidfeature->rect[k].p3 = sum_elem_ptr(*sum, tr.y + tr.height, tr.x + tr.width);
						}
						else
						{
							hidfeature->rect[k].p2 = sum_elem_ptr(*tilted, tr.y + tr.width, tr.x + tr.width);
							hidfeature->rect[k].p3 = sum_elem_ptr(*tilted, tr.y + tr.width + tr.height,
								tr.x + tr.width - tr.height);
							hidfeature->rect[k].p0 = sum_elem_ptr(*tilted, tr.y, tr.x);
							hidfeature->rect[k].p1 = sum_elem_ptr(*tilted, tr.y + tr.height, tr.x - tr.height);
						}
						
						hidfeature->rect[k].weight = (float)(feature->rect[k].weight * correction_ratio);
						
						if( k == 0 )
							area0 = tr.width * tr.height;
						else
							sum0 += hidfeature->rect[k].weight * tr.width * tr.height;
					}
					
					hidfeature->rect[0].weight = (float)(-sum0/area0);
            } /* l */
        } /* j */
    }
    }

}


static
double IMP_EvalHidHaarClassifier( IMP_HidHaarClassifier* classifier,
								double variance_norm_factor,
								size_t p_offset )
{
    int idx = 0;
    do 
    {
        IMP_HidHaarTreeNode* node = classifier->node + idx;
        double t = node->threshold * variance_norm_factor;
		
        double sum = calc_sum(node->feature.rect[0],p_offset) * node->feature.rect[0].weight;
        sum += calc_sum(node->feature.rect[1],p_offset) * node->feature.rect[1].weight;
		
        if( node->feature.rect[2].p0 )
            sum += calc_sum(node->feature.rect[2],p_offset) * node->feature.rect[2].weight;
		
        idx = sum < t ? node->left : node->right;
    }
    while( idx > 0 );
    return classifier->alpha[-idx];
}


int IMP_RunHaarClassifierCascade( IMP_HaarClassifierCascade* _cascade, Imp_Point pt, int start_stage )
{
    int result = -1;
 	
    int p_offset, pq_offset;
    int i, j;
    double mean, variance_norm_factor;
    IMP_HidHaarClassifierCascade* cascade;
	
    if( !IMP_IS_HAAR_CLASSIFIER(_cascade) )
        // CV_ERROR( !_cascade ? CV_StsNullPtr : CV_StsBadArg, "Invalid cascade pointer" );
		printf("Invalid cascade pointer\n");
	
    cascade = _cascade->hid_cascade;
    if( !cascade )
//         CV_ERROR( CV_StsNullPtr, "Hidden cascade has not been created.\n"
// 		"Use IMP_SetImagesForHaarClassifierCascade" );
		printf("Hidden cascade has not been created.\n");
	
    if( pt.x < 0 || pt.y < 0 ||
        pt.x + _cascade->real_window_size.width >= cascade->sum.cols-2 ||
        pt.y + _cascade->real_window_size.height >= cascade->sum.rows-2 )
        return 0;
	
    p_offset = pt.y * (cascade->sum.step/sizeof(sumtype)) + pt.x;
    pq_offset = pt.y * (cascade->sqsum.step/sizeof(sqsumtype)) + pt.x;
    mean = calc_sum(*cascade,p_offset)*cascade->inv_window_area;
    variance_norm_factor = cascade->pq0[pq_offset] - cascade->pq1[pq_offset] -
		cascade->pq2[pq_offset] + cascade->pq3[pq_offset];
    variance_norm_factor = variance_norm_factor*cascade->inv_window_area - mean*mean;
    if( variance_norm_factor >= 0. )
        variance_norm_factor = sqrt(variance_norm_factor);
    else
        variance_norm_factor = 1.;
	
    if( cascade->is_tree )
    {
        IMP_HidHaarStageClassifier* ptr;
        assert( start_stage == 0 );
		
        result = 1;
        ptr = cascade->stage_classifier;
		
        while( ptr )
        {
            double stage_sum = 0;
			
            for( j = 0; j < ptr->count; j++ )
            {
                stage_sum += IMP_EvalHidHaarClassifier( ptr->classifier + j,
                    variance_norm_factor, p_offset );
            }
			
            if( stage_sum >= ptr->threshold )
            {
                ptr = ptr->child;
            }
            else
            {
                while( ptr && ptr->next == NULL ) ptr = ptr->parent;
                if( ptr == NULL )
                {
                    result = 0;
                    return 0;
                }
                ptr = ptr->next;
            }
        }
    }
    else if( cascade->is_stump_based )
    {
        for( i = start_stage; i < cascade->count; i++ )
        {
            double stage_sum = 0;
			
            if( cascade->stage_classifier[i].two_rects )
            {
                for( j = 0; j < cascade->stage_classifier[i].count; j++ )
                {
                    IMP_HidHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                    IMP_HidHaarTreeNode* node = classifier->node;
                    double sum, t = node->threshold*variance_norm_factor, a, b;
					
                    sum = calc_sum(node->feature.rect[0],p_offset) * node->feature.rect[0].weight;
                    sum += calc_sum(node->feature.rect[1],p_offset) * node->feature.rect[1].weight;
					
                    a = classifier->alpha[0];
                    b = classifier->alpha[1];
                    stage_sum += sum < t ? a : b;
                }
            }
            else
            {
                for( j = 0; j < cascade->stage_classifier[i].count; j++ )
                {
                    IMP_HidHaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
                    IMP_HidHaarTreeNode* node = classifier->node;
                    double sum, t = node->threshold*variance_norm_factor, a, b;
					
                    sum = calc_sum(node->feature.rect[0],p_offset) * node->feature.rect[0].weight;
                    sum += calc_sum(node->feature.rect[1],p_offset) * node->feature.rect[1].weight;
					
                    if( node->feature.rect[2].p0 )
                        sum += calc_sum(node->feature.rect[2],p_offset) * node->feature.rect[2].weight;
					
                    a = classifier->alpha[0];
                    b = classifier->alpha[1];
                    stage_sum += sum < t ? a : b;
                }
            }
			
            if( stage_sum < cascade->stage_classifier[i].threshold )
            {
                result = -i;
                return -i;
            }
        }
    }
    else
    {
        for( i = start_stage; i < cascade->count; i++ )
        {
            double stage_sum = 0;
			
            for( j = 0; j < cascade->stage_classifier[i].count; j++ )
            {
                stage_sum += IMP_EvalHidHaarClassifier(
                    cascade->stage_classifier[i].classifier + j,
                    variance_norm_factor, p_offset );
            }
			
            if( stage_sum < cascade->stage_classifier[i].threshold )
            {
                result = -i;
                return 0;
            }
        }
    }
	
    result = 1;
	
	
    return result;
}


int is_equal( const void* _r1, const void* _r2, void*a )
{
    const Imp_Rect* r1 = (const Imp_Rect*)_r1;
    const Imp_Rect* r2 = (const Imp_Rect*)_r2;
    int distance = IMP_Round(r1->width*0.2);
	
    return r2->x <= r1->x + distance &&
		r2->x >= r1->x - distance &&
		r2->y <= r1->y + distance &&
		r2->y >= r1->y - distance &&
		r2->width <= IMP_Round( r1->width * 1.2 ) &&
		IMP_Round( r2->width * 1.2 ) >= r1->width;
}


IMP_Seq* IMP_HaarDetectObjects( VFD_HAAR_MODULE *module, const GRAY_IMAGE_S* _img,
					  IMP_HaarClassifierCascade* cascade,
					  IMP_MemStorage* storage, double scale_factor,
					  int min_neighbors, int flags, Imp_Size min_size )
					  
{
    int split_stage = 2;
	int i,j,*p;
	int _iy;
	IMP_U64 *p1;
	GRAY_IMAGE_S*img = (GRAY_IMAGE_S*)_img;
	GRAY_IMAGE_S *temp = &module->temp;
	IMP_U64 *integralImage = module->integralImage;
	IMP_S32 *sumcanny = module->sumcanny;
	GRAY_IMAGE16_S *stImgInEdgeGrad = &module->stImgInEdgeGrad;		/**< 实时的边缘图像(梯度)*/
	GRAY_IMAGE_S *stImageEdgeMag = &module->stImageEdgeMag;

    IMP_Mat *sum = module->sum;
	IMP_Mat *sum2 = module->sum2;
	IMP_Mat *tilted = module->tilted;
	IMP_Mat *sqsum = module->sqsum;
	IMP_Mat *norm_img = module->norm_img;
	IMP_Mat *img_small = module->img_small;
    IMP_Seq* seq = 0;
    IMP_Seq* seq2 = 0;
    IMP_Seq* idx_seq = 0;
    IMP_Seq* result_seq = 0;
    IMP_MemStorage* temp_storage = 0;
    IMP_AvgComp* comps = 0; 
	
    double factor;
    int npass = 2, coi;
    int do_canny_pruning = flags & IMP_HAAR_DO_CANNY_PRUNING;	    
	int count = 0,count1 = 0;
	temp_storage = IMP_CreateChildMemStorage( storage );
    if( cascade->hid_cascade->has_tilted_features )
        tilted = module->tilted;
	
	seq = IMP_CreateSeq( 0, sizeof(IMP_Seq), sizeof(Imp_Rect), temp_storage );
    seq2 = IMP_CreateSeq( 0, sizeof(IMP_Seq), sizeof(IMP_AvgComp), temp_storage );
    result_seq = IMP_CreateSeq( 0, sizeof(IMP_Seq), sizeof(IMP_AvgComp), storage );
	
    if( min_neighbors == 0 )
        seq = result_seq;
   
	
    if( flags & IMP_HAAR_SCALE_IMAGE )
    {
        Imp_Size win_size0 = cascade->orig_window_size;
        int use_ipp = cascade->hid_cascade->ipp_stages != 0 &&
			icvApplyHaarClassifier_32s32f_C1R_p != 0;
		
        if( use_ipp )
             norm_img = module->norm_img;
		 img_small = module->img_small;
		
        for( factor = 1; ; factor *= scale_factor )
        {
            int positive = 0;
            int x, y;
            Imp_Size win_size = { IMP_Round(win_size0.width*factor),
				IMP_Round(win_size0.height*factor) };
            Imp_Size sz = { IMP_Round( img->s32W/factor ), IMP_Round( img->s32H/factor ) };
            Imp_Size sz1 = { sz.width - win_size0.width, sz.height - win_size0.height };
            Imp_Rect rect1 = { icv_object_win_border, icv_object_win_border,
                win_size0.width - icv_object_win_border*2,
                win_size0.height - icv_object_win_border*2 };
            IMP_Mat img1, sum1, sqsum1, norm1, tilted1, mask1;
            IMP_Mat* _tilted = 0;
			
            if( sz1.width <= 0 || sz1.height <= 0 )
                break;
            if( win_size.width < min_size.width || win_size.height < min_size.height )
                continue;
			
            img1 = IMP_Mat_Construct( sz.height, sz.width, IMP_8UC1, img_small->data.ptr );
            sum1 = IMP_Mat_Construct( sz.height+1, sz.width+1, IMP_32SC1, sum->data.ptr );
            sqsum1 = IMP_Mat_Construct( sz.height+1, sz.width+1, IMP_64FC1, sqsum->data.ptr );
            if( tilted )
            {
                tilted1 = IMP_Mat_Construct( sz.height+1, sz.width+1, IMP_32SC1, tilted->data.ptr );
                _tilted = &tilted1;
            }
            norm1 = IMP_Mat_Construct( sz1.height, sz1.width, IMP_32FC1, norm_img ? norm_img->data.ptr : 0 );
            mask1 = IMP_Mat_Construct( sz1.height, sz1.width, IMP_8UC1, temp->pu8Data );
			
			ipBiLinearResize(img->pu8Data,img1.data.ptr,img->s32H,img->s32W,img1.rows,img1.cols,1);

			ipIntegral(img1.data.ptr,img1.cols,
				integralImage,img1.cols + 1,
				img1.rows,img1.cols);	
			
			for (i = 0; i < sum1.rows; i++ )
			{
				for (j = 0; j< sum1.cols; j++)
				{
					*(sum1.data.i + i*sum1.cols + j) = (int)((*(integralImage + i*sum1.cols + j) >> 36) & 0xFFFFFF);
					*(sqsum1.data.db + i*sum1.cols + j) = ((*(integralImage + i*sum1.cols + j)) & 0xFFFFFFFFF);
				}
			}

			
            if( use_ipp && icvRectStdDev_32s32f_C1R_p( sum1.data.i, sum1.step,
                sqsum1.data.db, sqsum1.step, norm1.data.fl, norm1.step, sz1, rect1 ) < 0 )
                use_ipp = 0;
			
            if( use_ipp )
            {
                positive = mask1.cols*mask1.rows;
                IMP_Set( &mask1, IMP_ScalarAll(255),NULL);
                for( i = 0; i < cascade->count; i++ )
                {
                    if( icvApplyHaarClassifier_32s32f_C1R_p(sum1.data.i, sum1.step,
                        norm1.data.fl, norm1.step, mask1.data.ptr, mask1.step,
                        sz1, &positive, cascade->hid_cascade->stage_classifier[i].threshold,
                        cascade->hid_cascade->ipp_stages[i]) < 0 )
                    {
                        use_ipp = 0;
                        break;
                    }
                    if( positive <= 0 )
                        break;
                }
            }
            
            if( !use_ipp )
            {
                IMP_SetImagesForHaarClassifierCascade( cascade, &sum1, &sqsum1, 0, 1. );
                for( y = 0, positive = 0; y < sz1.height; y++ )
                    for( x = 0; x < sz1.width; x++ )
                    {
                        mask1.data.ptr[mask1.step*y + x] =
                            IMP_RunHaarClassifierCascade( cascade, IMP_Point(x,y), 0 ) > 0;
                        positive += mask1.data.ptr[mask1.step*y + x];
                    }
            }
			
            if( positive > 0 )
            {
                for( y = 0; y < sz1.height; y++ )
                    for( x = 0; x < sz1.width; x++ )
                        if( mask1.data.ptr[mask1.step*y + x] != 0 )
                        {
                            Imp_Rect obj_rect = { IMP_Round(y*factor), IMP_Round(x*factor),
								win_size.width, win_size.height };
                            IMP_SeqPush( seq, &obj_rect );
                        }
            }
        }
    }
    else
    {		
		//integralImage 积分图像和平方积分图像：(W+1)×(H+1), 单通道，64位，其中平方积分图像占用0-35位，积分图像占用36-59位
		ipIntegral(img->pu8Data,img->s32W,
			integralImage,img->s32W + 1,
			img->s32H,img->s32W);	
		
		for (i = 0; i < sum->rows; i++ )
		{
			for (j = 0; j< sum->cols; j++)
			{
				*(sum->data.i + i*sum->cols + j) = (int)((*(integralImage + i*sum->cols + j) >> 36) & 0xFFFFFF);
				*(sqsum->data.db + i*sum->cols + j) = ((*(integralImage + i*sum->cols + j)) & 0xFFFFFFFFF);
			}
		}
		
        if( do_canny_pruning )
        {
			
			ipCanny( img->pu8Data,
				(IMP_S8*)stImgInEdgeGrad->ps16Data,
				stImageEdgeMag->pu8Data,
				temp->pu8Data,
				0, 0, img->s32W, img->s32H, img->s32W, img->s32H, 5 );
			
			ipImageBinary(temp->pu8Data,temp->pu8Data,temp->s32W,temp->s32H,10);
			

			ipIntegral(temp->pu8Data,temp->s32W,
				integralImage,temp->s32W + 1,
				temp->s32H,temp->s32W);	
			
			for (i = 0; i < sum2->rows; i++ )
			{
				for (j = 0; j< sum2->cols; j++)
				{
					*(sum2->data.i + i*sum2->cols + j) = (int)((*(integralImage + i*sum2->cols + j) >> 36) & 0xFFFFFF);
				//	*(sqsum1.data.db + i*sum1.width + j) = ((*(integralImage + i*sum1.width + j)) & 0xFFFFFFFFF);
				}
			}
		
			memcpy(sumcanny,sum2->data.i,((img->s32H + 1) * (img->s32W + 1) * sizeof(IMP_S32)));
        }
		
        if( (unsigned)split_stage >= (unsigned)cascade->count ||
            cascade->hid_cascade->is_tree )
        {
            split_stage = cascade->count;
            npass = 1;
        }
		
        for( factor = 1; factor*cascade->orig_window_size.width < img->s32W - 10 &&
			factor*cascade->orig_window_size.height < img->s32H - 10;
		factor *= scale_factor )
        {
            const double ystep = IMP_MAX( 2, factor );
            Imp_Size win_size = { IMP_Round( cascade->orig_window_size.width * factor ),
				IMP_Round( cascade->orig_window_size.height * factor )};
            Imp_Rect equ_rect = { 0, 0, 0, 0 };
            int *p0 = 0, *p1 = 0, *p2 = 0, *p3 = 0;
            int *pq0 = 0, *pq1 = 0, *pq2 = 0, *pq3 = 0;
            int pass, stage_offset = 0;
            int stop_height = IMP_Round((img->s32H - win_size.height) / ystep);
			
            if( win_size.width < min_size.width || win_size.height < min_size.height )
                continue;
			
            IMP_SetImagesForHaarClassifierCascade( cascade, sum, sqsum, tilted, factor );
	
			IMP_GrayImageClear(temp);
			
            if( do_canny_pruning )
            {
                equ_rect.x = IMP_Round(win_size.width*0.15);
                equ_rect.y = IMP_Round(win_size.height*0.15);
                equ_rect.width = IMP_Round(win_size.width*0.7);
                equ_rect.height = IMP_Round(win_size.height*0.7);
				
				p0 = (int*)(sumcanny + equ_rect.y*(img->s32W + 1)) + equ_rect.x;
				p1 = (int*)(sumcanny+ equ_rect.y*(img->s32W + 1))
					+ equ_rect.x + equ_rect.width;
				p2 = (int*)(sumcanny + (equ_rect.y + equ_rect.height)*(img->s32W + 1)) + equ_rect.x;
				p3 = (int*)(sumcanny + (equ_rect.y + equ_rect.height)*(img->s32W + 1))
					+ equ_rect.x + equ_rect.width;
			
                pq0 = (int*)(sum->data.ptr + equ_rect.y*sum->step) + equ_rect.x;
                pq1 = (int*)(sum->data.ptr + equ_rect.y*sum->step)
					+ equ_rect.x + equ_rect.width;
                pq2 = (int*)(sum->data.ptr + (equ_rect.y + equ_rect.height)*sum->step) + equ_rect.x;
                pq3 = (int*)(sum->data.ptr + (equ_rect.y + equ_rect.height)*sum->step)
					+ equ_rect.x + equ_rect.width;
            }
			
           cascade->hid_cascade->count = split_stage;
		
            for( pass = 0; pass < npass; pass++ )
            {
                for( _iy = 0; _iy < stop_height; _iy++ )
                {
                    
					int iy = IMP_Round(_iy*ystep);
                    int _ix, _xstep = 1;
                    int stop_width = IMP_Round((img->s32W - win_size.width) / ystep);
                    IMP_U8* mask_row = (IMP_U8*)(temp->pu8Data + temp->s32W * iy);
					
                    for( _ix = 0; _ix < stop_width; _ix += _xstep )
                    {
                        int ix = IMP_Round(_ix*ystep); // it really should be ystep
						count++;
                        if( pass == 0 )
                        {
                            int result;
                            _xstep = 2;
							
                            if( do_canny_pruning )
                            {
                                int offset;
                                int s, sq;
								
                                offset = iy*(sum->step/sizeof(p0[0])) + ix;
                                s = p0[offset] - p1[offset] - p2[offset] + p3[offset];
                                sq = pq0[offset] - pq1[offset] - pq2[offset] + pq3[offset];
                                if( s < 100 || sq < 20 )
                                    continue;
                            }
							
                            result = IMP_RunHaarClassifierCascade( cascade, IMP_Point(ix,iy), 0 );
                            if( result > 0 )
                            {
                                if( pass < npass - 1 )
                                    mask_row[ix] = 1;
                                else
                                {
                                    Imp_Rect rect = IMP_Rect(ix,iy,win_size.width,win_size.height);
                                    IMP_SeqPush( seq, &rect );

                                }
                            }
                            if( result < 0 )
                                _xstep = 1;
                        }
                        else if( mask_row[ix] )
                        {
                            int result = IMP_RunHaarClassifierCascade( cascade, IMP_Point(ix,iy),
								stage_offset );
                            if( result > 0 )
                            {
                                if( pass == npass - 1 )
                                {
                                    Imp_Rect rect = IMP_Rect(ix,iy,win_size.width,win_size.height);
                                    IMP_SeqPush( seq, &rect );//////////////////////////////////mark

                                }
                            }
                            else
                                mask_row[ix] = 0;
                        }
                    }
                }
                stage_offset = cascade->hid_cascade->count;
                cascade->hid_cascade->count = cascade->count;
            }
        }
    }
	

    if( min_neighbors != 0 )
    {
        // group retrieved rectangles in order to filter out noise 
        int ncomp = IMP_SeqPartition( seq, 0, &idx_seq, is_equal, 0 );
        comps = (IMP_AvgComp*)malloc( (ncomp+1)*sizeof(comps[0]));
        memset( comps, 0, (ncomp+1)*sizeof(comps[0]));
		
        // count number of neighbors
        for( i = 0; i < seq->total; i++ )
        {
            Imp_Rect r1 = *(Imp_Rect*)IMP_GetSeqElem( seq, i );
            int idx = *(int*)IMP_GetSeqElem( idx_seq, i );
            assert( (unsigned)idx < (unsigned)ncomp );
			
            comps[idx].neighbors++;
			
            comps[idx].rect.x += r1.x;
            comps[idx].rect.y += r1.y;
            comps[idx].rect.width += r1.width;
            comps[idx].rect.height += r1.height;
        }
		
        // calculate average bounding box
        for( i = 0; i < ncomp; i++ )
        {
            int n = comps[i].neighbors;
            if( n >= min_neighbors )
            {
                IMP_AvgComp comp;
                comp.rect.x = (comps[i].rect.x*2 + n)/(2*n);
                comp.rect.y = (comps[i].rect.y*2 + n)/(2*n);
                comp.rect.width = (comps[i].rect.width*2 + n)/(2*n);
                comp.rect.height = (comps[i].rect.height*2 + n)/(2*n);
                comp.neighbors = comps[i].neighbors;
				
                IMP_SeqPush( seq2, &comp );
            }
        }
		
        // filter out small face rectangles inside large face rectangles
        for( i = 0; i < seq2->total; i++ )
        {
            IMP_AvgComp r1 = *(IMP_AvgComp*)IMP_GetSeqElem( seq2, i );
            int j, flag = 1;
			
            for( j = 0; j < seq2->total; j++ )
            {
                IMP_AvgComp r2 = *(IMP_AvgComp*)IMP_GetSeqElem( seq2, j );
                int distance = IMP_Round( r2.rect.width * 0.2 );
				
                if( i != j &&
                    r1.rect.x >= r2.rect.x - distance &&
                    r1.rect.y >= r2.rect.y - distance &&
                    r1.rect.x + r1.rect.width <= r2.rect.x + r2.rect.width + distance &&
                    r1.rect.y + r1.rect.height <= r2.rect.y + r2.rect.height + distance &&
                    (r2.neighbors > IMP_MAX( 3, r1.neighbors ) || r1.neighbors < 3) )
                {
                    flag = 0;
                    break;
                }
            }
			
            if( flag )
            {
                IMP_SeqPush( result_seq, &r1 );
                /// IMP_SeqPush( result_seq, &r1.rect ); 
            }
        }
    }

	
    free( comps );   
	IMP_ReleaseMemStorage( &temp_storage );	

    return result_seq;
}

void
IMP_ReleaseHaarClassifierCascade( IMP_HaarClassifierCascade** _cascade )
{
    if( _cascade && *_cascade )
    {
        int i, j;
        IMP_HaarClassifierCascade* cascade = *_cascade;
		
        for( i = 0; i < cascade->count; i++ )
        {
            for( j = 0; j < cascade->stage_classifier[i].count; j++ )
                free( cascade->stage_classifier[i].classifier[j].haar_feature );
            free( cascade->stage_classifier[i].classifier );
        }
        IMP_ReleaseHidHaarClassifierCascade( &cascade->hid_cascade );
        free( *_cascade );
    }
}


//////////////////////////////////////////////////////////////////////////
/* takes image filename and cascade path from the command line */

#define MAX_CLASSIFIER_SIZE 500*1024
void IMP_WriteHaarClassifierCascade(IMP_HaarClassifierCascade* cascade,char* pPathName)
{
	FILE* fp;
	char *pData = 0;
	char *pOrgData;
	int nDataLen = 0;
	int stage_cnt = cascade->count;
	int classifier_cnt = 0;
	int harr_cnt = 0;
	int i = 0,j =0,k = 0;
	fp = fopen(pPathName,"w");

	pData = (char *)malloc(MAX_CLASSIFIER_SIZE);
	pOrgData = pData;
	pData += sprintf( pData, "flags=%d\n", cascade->flags);
	pData += sprintf( pData, "stage_count=%d\n", cascade->count);
	pData += sprintf( pData, "orig_window_size=(%d,%d)\n", cascade->orig_window_size.width,cascade->orig_window_size.height);
	pData += sprintf( pData, "real_window_size=(%d,%d)\n", cascade->real_window_size.width,cascade->real_window_size.height);
	pData += sprintf( pData, "scale=%f\n", cascade->scale);
	for ( i = 0 ; i < stage_cnt; i++ )
	{	
		pData += sprintf( pData, "stage%d=(%d,%f,%d,%d,%d)\n", 
			              i,cascade->stage_classifier[i].count,
						  cascade->stage_classifier[i].threshold,
						  cascade->stage_classifier[i].next,
						  cascade->stage_classifier[i].child,
						  cascade->stage_classifier[i].parent
						  );
		classifier_cnt = cascade->stage_classifier[i].count;
		for ( j = 0; j < classifier_cnt; j++ ) 
		{
			harr_cnt = cascade->stage_classifier[i].classifier[j].count;
			pData += sprintf( pData, "stage%d_classifier%d_count=%d\n", 
			              i,j,cascade->stage_classifier[i].classifier[j].count
						  );
			for ( k = 0; k < harr_cnt; k++)
			{
				pData += sprintf( pData, "stage%d_classifier%d_haar%d=(%f,%d,%d)\n", 
			              i,j,k,cascade->stage_classifier[i].classifier[j].threshold[k],
						  cascade->stage_classifier[i].classifier[j].left[k],
						  cascade->stage_classifier[i].classifier[j].right[k]					  
						  );
				pData += sprintf( pData, "stage%d_classifier%d_haarfeature%d=(%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f)\n", 
			              i,j,k,cascade->stage_classifier[i].classifier[j].haar_feature[k].tilted,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.x,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.y,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.width,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.height,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].weight,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.x,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.y,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.width,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.height,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].weight,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.x,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.y,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.width,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.height,
						  cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].weight
						  );
			}
			for ( k = 0; k <= harr_cnt; k++)
			{
				pData += sprintf( pData, "stage%d_classifier%d_haar%dalpha=%f\n", 
			              i,j,k,cascade->stage_classifier[i].classifier[j].alpha[k]						  
						  );
			}
		}
	}

	nDataLen = pData - pOrgData;
	fwrite(pOrgData,nDataLen,1,fp);
	fclose(fp); //关闭文件
	free(pOrgData);
}
#define IMP_FIND_LINE_START(data) {data = strstr( data, "\n" ); data += 1;}
void IMP_ReadHaarClassifierCascade(char* pPathName,IMP_HaarClassifierCascade* cascade)
{
	FILE* fp;
	char *pData = 0;
	char *pOrgData;
	int nDataLen = 0;
	int stage_cnt = cascade->count;
	int classifier_cnt = 0;
	int harr_cnt = 0;
	int i = 0,j =0,k = 0;
	fp = fopen(pPathName,"rb");

	pData = (char *)malloc(MAX_CLASSIFIER_SIZE);
	pOrgData = pData;
	nDataLen = fread(pData,1,MAX_CLASSIFIER_SIZE,fp);
	fclose(fp); //关闭文件


	sscanf( pData, "flags=%d", &cascade->flags);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "stage_count=%d", &cascade->count);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "orig_window_size=(%d,%d)", &cascade->orig_window_size.width,&cascade->orig_window_size.height);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "real_window_size=(%d,%d)", &cascade->real_window_size.width,&cascade->real_window_size.height);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "scale=%f", &cascade->scale);
	IMP_FIND_LINE_START(pData);
	for ( i = 0 ; i < stage_cnt; i++ )
	{	
		//sprintf("stage%d",i);
		int m = 0;
		sscanf( pData, "stage%d=(%d,%f,%d,%d,%d)", 
			              &m,&cascade->stage_classifier[i].count,
						  &cascade->stage_classifier[i].threshold,
						  &cascade->stage_classifier[i].next,
						  &cascade->stage_classifier[i].child,
						  &cascade->stage_classifier[i].parent
						  );
		IMP_FIND_LINE_START(pData);
		classifier_cnt = cascade->stage_classifier[i].count;
		for ( j = 0; j < classifier_cnt; j++ ) 
		{
			int n = 0;
			int t = 0;
			harr_cnt = cascade->stage_classifier[i].classifier[j].count;
			sscanf( pData, "stage%d_classifier%d_count=%d", 
			              &m,&n,&cascade->stage_classifier[i].classifier[j].count
						  );
			IMP_FIND_LINE_START(pData);
			for ( k = 0; k < harr_cnt; k++)
			{
				sscanf( pData, "stage%d_classifier%d_haar%d=(%f,%d,%d,%f,%f)", 
			              &m,&n,&t,&cascade->stage_classifier[i].classifier[j].threshold[k],
						  &cascade->stage_classifier[i].classifier[j].left[k],
						  &cascade->stage_classifier[i].classifier[j].right[k]					  
						  );
				IMP_FIND_LINE_START(pData);
				sscanf( pData, "stage%d_classifier%d_haarfeature%d=(%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f)", 
			              &m,&n,&t,&cascade->stage_classifier[i].classifier[j].haar_feature[k].tilted,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.x,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.y,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.width,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.height,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].weight,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.x,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.y,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.width,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.height,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].weight,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.x,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.y,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.width,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.height,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].weight
						  );
				IMP_FIND_LINE_START(pData);
			}
			for ( k = 0; k <= harr_cnt; k++)
			{
				sscanf( pData, "stage%d_classifier%d_haar%dalpha=%f\n", 
			              &m,&n,&t,&cascade->stage_classifier[i].classifier[j].alpha[k]						  
						 );
				IMP_FIND_LINE_START(pData);
			}
		}
	}
	
	free(pOrgData);
}



IMP_HaarClassifierCascade* IMP_CreateHaarClassifier(char* pPathName)
{
	IMP_HaarClassifierCascade* cascade = 0;
	FILE* fp;
	char *pData = 0;
	char *pOrgData;
	int nDataLen = 0;
	int stage_cnt = 0;
	int classifier_cnt = 0;
	int harr_cnt = 0;
	int i = 0,j =0,k = 0;
	int flags = 0;
	int count = 0;
	fp = fopen(pPathName,"rb");

	pData = (char *)malloc(MAX_CLASSIFIER_SIZE);
	pOrgData = pData;
	nDataLen = fread(pData,1,MAX_CLASSIFIER_SIZE,fp);
	fclose(fp); //关闭文件
	


	sscanf( pData, "flags=%d", &flags);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "stage_count=%d", &count);
	
	cascade = IMP_CreateHaarClassifierCascade(count);
	cascade->flags = flags;
	cascade->count = count;
	stage_cnt = cascade->count;
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "orig_window_size=(%d,%d)", &cascade->orig_window_size.width,&cascade->orig_window_size.height);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "real_window_size=(%d,%d)", &cascade->real_window_size.width,&cascade->real_window_size.height);
	IMP_FIND_LINE_START(pData);
	sscanf( pData, "scale=%f", &cascade->scale);
	IMP_FIND_LINE_START(pData);
	for ( i = 0 ; i < stage_cnt; i++ )
	{	
		int m = 0;
		int stage_classifier_count = 0;
		float threshold = 0;
		int next = 0;
		int child = 0;
		int parent = 0;
		sscanf( pData, "stage%d=(%d,%f,%d,%d,%d)", 
			              &m,&stage_classifier_count,
						  &threshold,
						  &next,
						  &child,
						  &parent
						  );
		IMP_FIND_LINE_START(pData);

		cascade->stage_classifier[i].count = stage_classifier_count;
		cascade->stage_classifier[i].classifier =
			(IMP_HaarClassifier*)malloc( stage_classifier_count*sizeof(cascade->stage_classifier[i].classifier[0]));
		
		cascade->stage_classifier[i].threshold = threshold;		      
		cascade->stage_classifier[i].next = next;
		cascade->stage_classifier[i].child = child;
		cascade->stage_classifier[i].parent = parent;
		
		classifier_cnt = cascade->stage_classifier[i].count;
					  
 
		for ( j = 0; j < classifier_cnt; j++ ) 
		{
			int n = 0;
			int t = 0;
			IMP_HaarClassifier* classifier = cascade->stage_classifier[i].classifier + j;
			sscanf( pData, "stage%d_classifier%d_count=%d", 
			              &m,&n,&cascade->stage_classifier[i].classifier[j].count
						  );
			IMP_FIND_LINE_START(pData);
			classifier->haar_feature = (IMP_HaarFeature*) malloc( 
                classifier->count * ( sizeof( *classifier->haar_feature ) +
				sizeof( *classifier->threshold ) +
				sizeof( *classifier->left ) +
				sizeof( *classifier->right ) ) +
                (classifier->count + 1) * sizeof( *classifier->alpha ) );
            classifier->threshold = (float*) (classifier->haar_feature+classifier->count);
            classifier->left = (int*) (classifier->threshold + classifier->count);
            classifier->right = (int*) (classifier->left + classifier->count);
            classifier->alpha = (float*) (classifier->right + classifier->count);
			
			harr_cnt = cascade->stage_classifier[i].classifier[j].count;
			
			for ( k = 0; k < harr_cnt; k++)
			{
				sscanf( pData, "stage%d_classifier%d_haar%d=(%f,%d,%d,%f,%f)", 
			              &m,&n,&t,&cascade->stage_classifier[i].classifier[j].threshold[k],
						  &cascade->stage_classifier[i].classifier[j].left[k],
						  &cascade->stage_classifier[i].classifier[j].right[k]					  
						  );
				IMP_FIND_LINE_START(pData);
				sscanf( pData, "stage%d_classifier%d_haarfeature%d=(%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f)", 
			              &m,&n,&t,&cascade->stage_classifier[i].classifier[j].haar_feature[k].tilted,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.x,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.y,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.width,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].r.height,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[0].weight,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.x,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.y,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.width,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].r.height,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[1].weight,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.x,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.y,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.width,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].r.height,
						  &cascade->stage_classifier[i].classifier[j].haar_feature[k].rect[2].weight
						  );
				IMP_FIND_LINE_START(pData);
			}
			for ( k = 0; k <= harr_cnt; k++)
			{
				sscanf( pData, "stage%d_classifier%d_haar%dalpha=%f\n", 
			              &m,&n,&t,&cascade->stage_classifier[i].classifier[j].alpha[k]						  
						 );
				IMP_FIND_LINE_START(pData);
			}
		}
	}	
	free(pOrgData);
	
	return cascade;
}

 void IMP_ReleaseHaarClassifier(IMP_HaarClassifierCascade** _cascade)
 {
	IMP_ReleaseHaarClassifierCascade(_cascade);
 }

 IMP_HaarClassifierCascade* IMP_CreateHaar(VFD_HAAR_MODULE *module, IMP_MemStorage* storage, char* pPathName)
{

	IMP_S32 s32ImgW = module->s32ImgW;
	IMP_S32 s32ImgH = module->s32ImgH;

	IMP_HaarClassifierCascade* cascade = IMP_CreateHaarClassifier(pPathName);
	
    if( !cascade->hid_cascade )
         IMP_CreateHidHaarClassifierCascade(cascade) ;


	IMP_GrayImageCreate( &module->temp, s32ImgW, s32ImgH, NULL );
	module->integralImage = (IMP_U64 *)malloc((s32ImgW+1) * (s32ImgH+1)*sizeof(IMP_U64));
	memset(module->integralImage,0,(s32ImgW+1) * (s32ImgH+1)*sizeof(IMP_U64));

	module->sumcanny = (int *)malloc( (s32ImgH + 1) * (s32ImgW + 1) * sizeof(IMP_S32));

	IMP_GrayImage16Create( &module->stImgInEdgeGrad, s32ImgW, s32ImgH, NULL );
	IMP_GrayImageCreate( &module->stImageEdgeMag, s32ImgW, s32ImgH, NULL );
	
	
    module->sum = IMP_CreateMat( s32ImgH + 1, s32ImgW + 1, IMP_32SC1 );
    module->sum2 = IMP_CreateMat( s32ImgH + 1, s32ImgW + 1, IMP_32SC1 );
    module->sqsum = IMP_CreateMat( s32ImgH + 1, s32ImgW + 1, IMP_64FC1 );
	module->tilted = IMP_CreateMat( s32ImgH + 1, s32ImgW + 1, IMP_32SC1 );

	module->norm_img = IMP_CreateMat( s32ImgH, s32ImgW, IMP_32FC1 );
    module->img_small = IMP_CreateMat( s32ImgH + 1, s32ImgW + 1, IMP_8UC1 );

	return cascade;
}
void IMP_ReleaseHaar(VFD_HAAR_MODULE *module,IMP_HaarClassifierCascade** _cascade)
{
	free(module->sumcanny);
    IMP_ReleaseMat( &module->norm_img );
    IMP_ReleaseMat( &module->img_small );

	IMP_ReleaseMat( &module->sum );
	IMP_ReleaseMat( &module->sum2 );
    IMP_ReleaseMat( &module->sqsum );
    IMP_ReleaseMat( &module->tilted );
	IMP_GrayImageDestroy(&module->temp,NULL);
	
	
	free(module->integralImage); 
	module->integralImage = NULL;
	
	IMP_GrayImageDestroy(&module->stImageEdgeMag,NULL);
	IMP_GrayImage16Destroy(&module->stImgInEdgeGrad,NULL);	

	IMP_ReleaseHaarClassifier( _cascade );
}
/* End of file. */
