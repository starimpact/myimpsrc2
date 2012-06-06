
#ifndef _AVD_CANNY_EDGE_H_
#define _AVD_CANNY_EDGE_H_
#include "imp_algo_type.h"
#ifdef __cplusplus
extern "C"
{
#endif


void avd_canny_process( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag, IMP_U8 *p_img_edge,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows, IMP_S32 tgrad );
void avd_canny_calc_gradmag( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows );
void avd_canny_calc_gradient( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows );
void avd_canny_calc_magnitude( IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows );
void avd_canny_non_max_supp( IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag, IMP_U8 *p_img_edge,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows, IMP_S32 tgrad );


#ifdef __cplusplus
}
#endif


#endif

