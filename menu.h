#ifndef MENU_H
#define MENU_H

#include "pg.h"

enum{
	STATE_SLOT_MAX=9,
	DEF_COLOR0=0x9063,
	DEF_COLOR1=RGB(85,85,95),
	DEF_COLOR2=RGB(105,105,115),
	DEF_COLOR3=0xffff,
};

typedef struct
{
	unsigned long buttons;
	int n;
} S_BUTTON;

#define VERRIN "1.32 RM mod v2"
#define VERCNF "RINv1.31_rmv2"

typedef struct
{
	char vercnf[16];
	u8 compress;
	u8 thumb;
	u8 quickslot;
	u8 screensize;
	u8 bScreenSizes[16]; //余分に確保
	u8 gb_type;
	u8 gb_palette;
	u8 bGB_Pals[32]; //余分に確保
	u8 frameskip;
	u8 vsync;
	u8 sound;
	u8 sound_buffer;
	u8 cpu_clock;
	S_BUTTON skeys[32]; //余分に確保
	u8 analog2dpad;
	unsigned long color[4];
	u8 bgbright;
	char lastpath[256];
	u32 rewind_user_max_memory_ammount;
	u8 rewind_always_use_max_memory;
	u32 rewind_user_max_states_ammount;
	u32 rewind_always_use_max_states;
	u8 rewind_limit_mode;
	u8 rewind_enabled;
} SETTING;

extern SETTING setting, tmpsetting;
extern int bTurbo, bBitmap;
extern unsigned short bgBitmap[];
extern unsigned short thumb_w[160*144];
extern const char *cpu_clocks[];

extern byte *state_tmp;
extern unsigned short thumb_tmp[160*144];
extern ScePspDateTime state_tmp_time;

void init_config();
void check_config();
void set_gb_type();
int load_menu_bg();
void rin_frame(const char *msg0, const char *msg1);
void rin_menu(void);
int rin_MessageBox(const char *msg, int type);
int get_nShortcutKey(u32 buttons);
void bgbright_change();
void showMsgAboutLoadedRom();

#endif
