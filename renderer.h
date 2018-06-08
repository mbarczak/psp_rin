//---------------------------------------------
// ?G?~?.?O_[?V?I?"O<%??\O_-@/?C?"?^_[?t?F_[?X

#ifndef RENDERER_H
#define RENDERER_H

#define BUF_LEN 160

enum{
	SCR_X1,
	SCR_X15,
	SCR_X2_UNCROPED,
	SCR_X2_FIT,
	SCR_X2_SCANLINE,
	SCR_X2_UTOP,
	SCR_X2_UBOTTOM,
	SCR_FULL,
#ifdef USE_GPU
	SCR_X15_BL,
	SCR_X2_FIT_BL,
	SCR_FULL_BL,
#endif
	SCR_X1_MIRROR,//davex: mirroring modes
	SCR_X15_MIRROR,
	SCR_X2_UNCROPED_MIRROR,
	SCR_X2_FIT_MIRROR,
	SCR_X2_SCANLINE_MIRROR,
	SCR_X2_UTOP_MIRROR,
	SCR_X2_UBOTTOM_MIRROR,
	SCR_FULL_MIRROR,
	SCR_END,
};
extern const char *scr_names[];
extern int pad_state;
extern char render_msg[128];
extern int render_msg_mode;
extern long render_msg_time;

void renderer_init();
void renderer_reset();
void renderer_refresh();
void render_screen(void *buf);
byte renderer_get_time(int type);
word renderer_get_sensor(bool x_y);
void renderer_set_time(int type,byte dat);
void renderer_set_bibrate(bool bibrate);
int renderer_get_timer_state();
void renderer_set_timer_state(int timer);
void renderer_set_msg(const char msg[]);

#endif
