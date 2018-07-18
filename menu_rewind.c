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
static void rin_menu_rewind_get_config_increase_value(SETTING* local);
static void rin_menu_rewind_get_config_decrease_value(SETTING* local);
static void rin_menu_rewind_get_config_show_current(long sel,SETTING* local);
static void rin_menu_rewind_get_config_toogle_max(SETTING *local);
static void change_value(u32 * baseValue,int lowerBound,int upperBound,int step,int direction);
static void rin_frame_rewind_use_max();
static void rin_frame_rewind_no_max();
static void change_selected_value(SETTING *local, int direction);
static void print_rewind_memory_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel);
static void print_rewind_states_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel);
static void rin_frame_rewind(SETTING *local);

/*
 *
 * 0 : Limit mode : memory amount
 * 1 : Prefered memory amount designed for rewind purposes (current max: 23mb): 9mb | Always use MAX
 *    rin_frame("□：Don't Use max ×：Cancel ○：Save ");
 *    rin_frame("□：Use max <-：Sub ->：Add　×：Cancel ○：Save ");
 *
 * 0 : Limit mode : number of states
 * 1 : Prefered number of rewind states : 10 states | Always use MAX
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

#define REWIND_MEMORY_STEP ((int)(0.5 * 1024 *1024))
#define REWIND_STATES_STEP 10

static void rin_menu_rewind_get_config_decrease_value(SETTING* local) {
	change_selected_value(local, -1);
}
static void rin_menu_rewind_get_config_increase_value(SETTING* local) {
	change_selected_value(local, 1);
}

static void change_selected_value(SETTING *local, int direction) {
	if(local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT){
		if(!local->rewind_always_use_max_memory){
			change_value(&local->rewind_user_max_memory_ammount,REWIND_MIN_USER_MEMORY,
			             REWIND_MAX_USER_MEMORY,REWIND_MEMORY_STEP,direction);
		}
	}else{
		if(!local->rewind_always_use_max_states){
			change_value(&local->rewind_user_max_states_ammount,REWIND_MIN_USER_STATES,
			             REWIND_MAX_USER_STATES,REWIND_STATES_STEP,direction);
		}
	}
}

static void change_value(u32* baseValue,int lowerBound,int upperBound,int step,int direction){
	(*baseValue) += (REWIND_MEMORY_STEP*direction);
	if((*baseValue) > REWIND_MAX_USER_MEMORY){
		(*baseValue) = REWIND_MIN_USER_MEMORY;
	} else if((*baseValue) < REWIND_MIN_USER_MEMORY){
		(*baseValue) = REWIND_MAX_USER_MEMORY;
	}
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
	return setting.color[sel==element?2:3];
}

void rin_menu_rewind_get_config_show_current(long sel,SETTING* local) {

	unsigned long x=4,y=5;
	rin_frame_rewind(local);
	pgPrintf(x,y++,rewind_get_text_color(sel,0),"Limit mode : %s",
	         local->rewind_limit_mode==REWIND_MODE_LIMIT_MEMORY_AMOUNT?"memory amount":"number of states");
	if(local->rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT){
		print_rewind_memory_limit_line(&x, &y, local, sel);
	}else{
		print_rewind_states_limit_line(&x, &y, local, sel);
	}
	pgScreenFlipV();
}
void rin_frame_rewind(SETTING *local) {
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

static void print_rewind_states_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel) {
	char tmpString[MAX_MENU_ENTRY_LENGTH] = {0};
	if(local->rewind_always_use_max_states){
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"Always use MAX");
	}else{
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"%d states",local->rewind_user_max_states_ammount);
	}
	pgPrintf(*x,*y,rewind_get_text_color(sel,1),
	         "Prefered number of rewind states : %s",tmpString);
}

static void print_rewind_memory_limit_line(unsigned long *x, unsigned long *y, const SETTING *local, const long sel) {
	char tmpString[MAX_MENU_ENTRY_LENGTH] = {0};
	char maxStr[MAX_MENU_ENTRY_LENGTH] = {0};

	if(local->rewind_always_use_max_memory){
		snprintf(tmpString,MAX_MENU_ENTRY_LENGTH,"Always use MAX");
	}else{
		float num = byte2mb_asFloat(local->rewind_user_max_memory_ammount);
		ftoa(num,tmpString,1);
		strcat(tmpString,"mb");
	}
	ftoa(byte2mb_asFloat(max_rewind_memory),maxStr,1);

	pgPrintf(*x,*y,rewind_get_text_color(sel,1),
	         "Prefered memory amount designed for rewind purposes (current max:%smb): %s",maxStr,tmpString);
}

static char* rin_frame_get_title(){
	return "Tweak Rewind Settings";
}

static void rin_frame_rewind_use_max() {
	rin_frame(rin_frame_get_title(),"□：Don't Use max ×：Cancel ○：Save ");
}

static void rin_frame_rewind_no_max(){
	rin_frame(rin_frame_get_title(),"□：Use max <-：Sub ->：Add　×：Cancel ○：Save ");
}

char *rin_menu_rewind_get_main_menu_string() {
	static char buf[MAX_MENU_ENTRY_LENGTH] = {0};
	if(setting.rewind_limit_mode == REWIND_MODE_LIMIT_MEMORY_AMOUNT) {
		if(setting.rewind_always_use_max_memory){
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"%uMB(no limit)",byte2mb(max_rewind_memory));
		} else{
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"%uMB(user limit)",byte2mb(setting.rewind_user_max_memory_ammount));
		}
	}else{
		if(setting.rewind_always_use_max_states){
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"%u states(no limit)",setting.rewind_user_max_states_ammount);
		}else{
			snprintf(buf,MAX_MENU_ENTRY_LENGTH,"%u states(user limit)",num_rwnd_states);
		}
	}
	return buf;
}
void rin_menu_rewind_get_config(void) {
	const int MAX_ROW = 1;
	SETTING localSettings;
	memcpy(&localSettings,&setting, sizeof(SETTING));
	long sel=0;
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE){
			rin_menu_rewind_get_config_save_value(&localSettings);
			break;
		}else if(new_pad & CTRL_CROSS){
			break;
		}else if(new_pad & CTRL_SQUARE){
			rin_menu_rewind_get_config_toogle_max(&localSettings);
		}else if(new_pad & CTRL_DOWN){
			sel = rin_menu_rewind_get_config_decrease_row(MAX_ROW, sel);
		}else if(new_pad & CTRL_UP){
			sel = rin_menu_rewind_get_config_increase_row(MAX_ROW, sel);
		}else if(new_pad & CTRL_RIGHT){
			if(sel == 0){
				localSettings.rewind_limit_mode = (u8)!localSettings.rewind_limit_mode;
			}else if (sel == 1){
				rin_menu_rewind_get_config_increase_value(&localSettings);
			}
		}else if(new_pad & CTRL_LEFT){
			if(sel == 0){
				localSettings.rewind_limit_mode = (u8
				)!localSettings.rewind_limit_mode;
			}else if (sel == 1){
				rin_menu_rewind_get_config_decrease_value(&localSettings);
			}
		}
		rin_menu_rewind_get_config_show_current(sel,&localSettings);
	}
}
