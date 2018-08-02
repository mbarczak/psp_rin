//
// Created by Papiex on 18.07.2018.
//

#include "menu_rewind.h"
#include "tools.h"
#include "rewind.h"

#define MAX_MENU_ENTRY_LENGTH 200

static int rin_menu_rewind_get_config_decrease_row(const int MAX_ROW, int sel);
static int rin_menu_rewind_get_config_increase_row(const int MAX_ROW, int sel);
static void rin_menu_rewind_get_config_save_value(SETTING* localSettings);
static void rin_menu_rewind_get_config_increase_value(SETTING *local, short longStep);
static void rin_menu_rewind_get_config_decrease_value(SETTING *local, short longStep);
static void rin_menu_rewind_get_config_show_current(long sel,int* crs_count,SETTING* local);
static void rin_menu_rewind_get_config_toogle_max(SETTING *local);
static void change_value(u32 *baseValue, int lowerBound, int upperBound, int step, int direction);
static void rin_frame_rewind_use_max();
static void rin_frame_rewind_no_max();
static void rin_frame_rewind_change_mode();
static void rin_frame_rewind_disabled();
static void change_selected_value(SETTING *local, int direction, short longStep);
static void print_rewind_memory_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel);
static void print_rewind_states_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel);
static void print_rewind_states_help(unsigned long *x, unsigned long *y, const SETTING *local, const long sel);
static void rin_frame_rewind(SETTING *local, long sel);

/*
 *
 * 0 : Limit mode : memory amount
 * 1 : Preferred memory amount designed for rewind purposes (current max: 23mb): 9mb | Always use MAX
 *    rin_frame("□：Don't Use max ×：Cancel ○：Save ");
 *    rin_frame("□：Use max <-：Sub ->：Add　×：Cancel ○：Save ");
 *
 * 0 : Limit mode : number of states
 * 1 : Preferred number of rewind states : 10 states | Always use MAX
 *    rin_frame("□：Don't Use max ×：Cancel ○：Save ");
 *    rin_frame("□：Use max <-：Sub ->：Add　×：Cancel ○：Save ");
 */

static void rin_menu_rewind_get_config_toogle_max(SETTING *local) {
	if(local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT){
		local->rewind_always_use_max_memory = (u8)!local->rewind_always_use_max_memory;
	}else{
		local->rewind_always_use_max_states = (u8)!local->rewind_always_use_max_states;
	}
}

#define REWIND_MEMORY_STEP ((int)(1 * 1024 *1024))
#define REWIND_STATES_STEP 10
#define REWIND_MEMORY_STEP_SLOW ((int)(0.1 * 1024 *1024))
#define REWIND_STATES_STEP_SLOW 1

static void rin_menu_rewind_get_config_decrease_value(SETTING *local, short longStep) {
	change_selected_value(local, -1, longStep);
}
static void rin_menu_rewind_get_config_increase_value(SETTING *local, short longStep) {
	change_selected_value(local, 1, longStep);
}

static void change_selected_value(SETTING *local, int direction, short longStep) {
	if(local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT){
		if(!local->rewind_always_use_max_memory){
			change_value(&local->rewind_user_max_memory_ammount,REWIND_MIN_USER_MEMORY,
			             REWIND_MAX_USER_MEMORY,longStep?REWIND_MEMORY_STEP:REWIND_MEMORY_STEP_SLOW,direction);
		}
	}else{
		if(!local->rewind_always_use_max_states){
			change_value(&local->rewind_user_max_states_ammount,REWIND_MIN_USER_STATES,
			             REWIND_MAX_USER_STATES,longStep?REWIND_STATES_STEP:REWIND_STATES_STEP_SLOW,direction);
		}
	}
}

static void change_value(u32 *baseValue, int lowerBound, int upperBound, int step, int direction){
	int bv = (*baseValue);
	bv += (step*direction);
	if(bv > upperBound){
		bv = lowerBound;
	} else if(bv < lowerBound){
		bv = upperBound;
	}
	(*baseValue) = (u32)bv;
}

static void rin_menu_rewind_get_config_save_value(SETTING* localSettings) {
	memcpy(&setting,localSettings, sizeof(SETTING));
}

static int rin_menu_rewind_get_config_increase_row(const int MAX_ROW, int sel) {
	sel--;
	if(sel<0) sel=MAX_ROW;
	return sel;
}

static int rin_menu_rewind_get_config_decrease_row(const int MAX_ROW, int sel) {
	sel++;
	if(sel>MAX_ROW) sel=0;
	return sel;
}

static unsigned long rewind_get_text_color(const long sel,const int element) {
//	return setting.color[sel==element?2:3];
	return setting.color[3];
}

void rin_menu_rewind_get_config_show_current(long sel,int* crs_count,SETTING* local) {

	unsigned long x=4,y=5;
	rin_frame_rewind(local, sel);
	if(local->rewind_enabled) {
		pgPrintf(x, y++, rewind_get_text_color(sel, 0), "Limit mode : %s",
		         local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT ? "by memory amount"
		                                                                     : "by number of states");
		y++;
		if (local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT) {
			print_rewind_memory_limit_line(&x, &y, local, sel);
		} else {
			print_rewind_states_limit_line(&x, &y, local, sel);
		}
		print_rewind_states_help(&x, &y,local,sel);
	}else{
		pgPrintf(x, y++, rewind_get_text_color(sel, 0), "Rewind disabled (press triangle to enable).");
	}
	if ((*crs_count)++>=30) (*crs_count)=0;
	if ((*crs_count) < 15  && local->rewind_enabled) {
		x = 3;
		if (sel == 0) {
			y = 5;
		} else if (sel == 1) {
			y = 7;
		}
		pgPutChar(x * 8, y * 8, setting.color[3], 0, 127, 1, 0, 1);
	}
	pgScreenFlipV();
}

void rin_frame_rewind(SETTING *local, long sel) {
	if(local->rewind_enabled){
		if(sel == 0){
			rin_frame_rewind_change_mode();
		}else{
			if(local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT){
				if(local->rewind_always_use_max_memory){
					rin_frame_rewind_use_max();
				}else{
					rin_frame_rewind_no_max();
				}
			}else{
				if(local->rewind_always_use_max_states){
					rin_frame_rewind_use_max();
				}else{
					rin_frame_rewind_no_max();
				}
			}
		}
	}else{
		rin_frame_rewind_disabled();
	}
}

static void print_rewind_states_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel) {
	char tmpString[MAX_MENU_ENTRY_LENGTH] = {0};
	if(local->rewind_always_use_max_states){
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"Always use max");
	}else{
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"%d states",local->rewind_user_max_states_ammount);
	}
	pgPrintf(*x,*y,rewind_get_text_color(sel,1),
	         "Preferred number of rewind states: %s",tmpString);
}

static void print_rewind_states_help(unsigned long *x, unsigned long *y, const SETTING *local, const long sel) {
#define HELP_COLOR RGB(26, 163, 255)
	unsigned help_y = 15;
	unsigned help_x = 4;
	char tmpString[MAX_MENU_ENTRY_LENGTH] = {0};
	if(local->rewind_always_use_max_states){
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"Always use max");
	}else{
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"%d states",local->rewind_user_max_states_ammount);
	}

	pgPrintf(help_x,help_y,HELP_COLOR, "---------------------------------------------------");
	help_y += 2;
	pgPrintf(help_x,help_y,HELP_COLOR, "On newer PSP models more memory for rewind can be ");
	help_y += 1;
	pgPrintf(help_x,help_y,HELP_COLOR, "enabled by using CFW with extra memory for homebrew");
	help_y += 1;
	pgPrintf(help_x,help_y,HELP_COLOR, "support. Enter recovery menu and enable:");
	help_y += 1;
	pgPrintf(help_x,help_y,HELP_COLOR, "\"Advanced/Force High Memory Layout\".");
	help_y += 2;
	pgPrintf(help_x,help_y,HELP_COLOR, "---------------------------------------------------");
	help_y += 1;
	pgPrintf(help_x,help_y,HELP_COLOR, "");
}

static void print_rewind_memory_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel) {
	char tmpString[MAX_MENU_ENTRY_LENGTH] = {0};
	char maxStr[MAX_MENU_ENTRY_LENGTH] = {0};

	if(local->rewind_always_use_max_memory){
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"Always use max");
	}else{
		float num = byte2mb_asFloat(local->rewind_user_max_memory_ammount);
		ftoa(num,tmpString,1);
		strcat(tmpString," MB");
	}
	ftoa(byte2mb_asFloat(max_rewind_memory),maxStr,1);

	pgPrintf(*x,*y,rewind_get_text_color(sel,1),
	         "Preferred memory amount designated for rewind");
	(*y)++;
	pgPrintf(*x,*y,rewind_get_text_color(sel,1),
	         "(current max: %s MB): %s",maxStr,tmpString);
}

static char* rin_frame_get_title(){
	return "Tweak Rewind Settings";
}

static void rin_frame_rewind_use_max() {
	rin_frame(rin_frame_get_title(),"□：Don't use max ×：Cancel ○：Save ");
}

static void rin_frame_rewind_no_max(){
	rin_frame(rin_frame_get_title(),"□：Use max ←：Sub →：Add　△:(press) Fine tune ×：Cancel ○：Save ");
}

static void rin_frame_rewind_disabled(){
	rin_frame(rin_frame_get_title(),"△: Enable Rewind ×：Cancel ○：Save ");
}

static void rin_frame_rewind_change_mode(){
	rin_frame(rin_frame_get_title(),"←→：Change limit mode △: Disable Rewind ×：Cancel ○：Save ");
}

char *rin_menu_rewind_get_main_menu_string() {
	static char buf[MAX_MENU_ENTRY_LENGTH] = {0};
#if 0
	if(setting.rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT) {
		if(setting.rewind_always_use_max_memory){
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"Memory in use %s MB (%d states for current rom)",
					get_current_rewind_memory_string(setting),num_rwnd_states);
		} else{
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"Memory: user limit, using %s MB",get_current_rewind_memory_string(setting));
		}
	}else{
		if(setting.rewind_always_use_max_states){
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"States: no limit, using %u states",num_rwnd_states);
		}else{
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"States: user limit, using %u states",num_rwnd_states);
		}
	}
#endif
	if(setting.rewind_enabled){
		snprintf(buf,MAX_MENU_ENTRY_LENGTH,"RAM used: %sMB (holds %d states)",
		         get_current_rewind_memory_string(setting),num_rwnd_states);
	}else{
		snprintf(buf,MAX_MENU_ENTRY_LENGTH,"Disabled");
	}

	return buf;
}
void rin_menu_rewind_get_config(void) {
	const int MAX_ROW = 1;
	SETTING localSettings;
	memcpy(&localSettings,&setting, sizeof(SETTING));
	long sel=0;
	int crs_count=0;
	short longStep = 1;
	for(;;){
		readpad();
		if(now_pad & (CTRL_UP|CTRL_DOWN|CTRL_LEFT|CTRL_RIGHT))
			crs_count=0;
		if(now_pad & CTRL_TRIANGLE) {
			longStep = 0;
		}else{
			longStep = 1;
		}
		if(new_pad & CTRL_TRIANGLE){
			if(sel == 0){
				localSettings.rewind_enabled = (u8)!localSettings.rewind_enabled;
			}
		}
		if(new_pad & CTRL_CIRCLE){
			rin_menu_rewind_get_config_save_value(&localSettings);
			free_rewind_states();
			allocate_rewind_states();
			break;
		}else if(new_pad & CTRL_CROSS){
			break;
		}else if(new_pad & CTRL_SQUARE && localSettings.rewind_enabled){
			if(sel == 1 ) {
				rin_menu_rewind_get_config_toogle_max(&localSettings);
			}
		}else if(new_pad & CTRL_DOWN && localSettings.rewind_enabled){
			sel = rin_menu_rewind_get_config_decrease_row(MAX_ROW, sel);
		}else if(new_pad & CTRL_UP && localSettings.rewind_enabled){
			sel = rin_menu_rewind_get_config_increase_row(MAX_ROW, sel);
		}else if(new_pad & CTRL_RIGHT && localSettings.rewind_enabled){
			if(sel == 0){
				localSettings.rewind_limit_mode = (u8)!localSettings.rewind_limit_mode;
			}else if (sel == 1){
				rin_menu_rewind_get_config_increase_value(&localSettings, longStep);
			}
		}else if(new_pad & CTRL_LEFT && localSettings.rewind_enabled){
			if(sel == 0){
				localSettings.rewind_limit_mode = (u8)!localSettings.rewind_limit_mode;
			}else if (sel == 1){
				rin_menu_rewind_get_config_decrease_value(&localSettings, longStep);
			}
		}
		rin_menu_rewind_get_config_show_current(sel,&crs_count,&localSettings);
	}
}
