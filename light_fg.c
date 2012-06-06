#define SCENE_CHANGE_RADIO 15
#define SCENE_CHANGE_CNT 8
static int32_t cal_light_fg_ratio( DETECTED_REGION_SET *rgs );

if(1)
    {
        static scene_chg_cnt = 0;
        fg_ratio = cal_light_fg_ratio(rgs);
        
        if(fg_ratio > SCENE_CHANGE_RADIO )
        {

            scene_chg_cnt++;
        }
        else
        {
            scene_chg_cnt = 0;
        }
        if(scene_chg_cnt > SCENE_CHANGE_CNT)
        {
            module->detdata->bg_inited[0] = 0;
            scene_chg_cnt = 0;
			module->countFrames = 0;
        }
    }
static int32_t cal_light_fg_ratio( DETECTED_REGION_SET *rgs )
{
	int32_t i, cnt;
	DETECTED_REGION	*drg;
	IP_GRAYIMAGE *image_fg_rgn = rgs->image_fg_rgn;
	int32_t img_w = rgs->image_fg_rgn->width;
	int32_t img_h = rgs->image_fg_rgn->height;
	int32_t xx, yy, sn;
	int32_t lb_x, lb_y, ru_x, ru_y;
	int32_t do_diff;
	uint8_t *diff, *fg;
	int32_t fg_num = 0;
	int32_t fg_ratio = 0;
//	rc->lb_x = 1024; rc->lb_y = 1024;
//	rc->ru_x = 0; rc->ru_y = 0;

	ITERATE_RGS_START
	if( DRG_IS_COMPLETE(drg->used) && DRG_IS_LIGHT(rgs->drg[i].used))// && ( DRG_IS_HIGH_BRIGHT(drg->used) || DRG_IS_HIGH_DARK(drg->used) ) )
	{
		{
			lb_x = drg->lb_x;
			ru_x = drg->ru_x;
			lb_y = drg->lb_y;
			ru_y = drg->ru_y;
			sn = drg->sn;
			fg = image_fg_rgn->data + lb_y * img_w;
			for( yy=lb_y; yy<=ru_y; yy++ )
			{
				for( xx=lb_x; xx<=ru_x; xx++ )
				{
					if( fg[xx]==sn )
					{
						fg_num++;
					}
				}
				fg += img_w;
			}
		}
	}
	ITERATE_RGS_END
	fg_ratio = (int32_t)((fg_num * 100) / (real)(img_w*img_h) + 0.5);
	return fg_ratio;

}