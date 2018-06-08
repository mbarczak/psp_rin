#include "main.h"
#include "image.h"
#include "colbl.c"

SETTING setting, tmpsetting;
int bTurbo=0, bBitmap;
unsigned short bgBitmap[480*272];
word thumb_w[160*144];

byte *state_tmp = NULL;
word thumb_tmp[160*144];
ScePspDateTime state_tmp_time;

void init_config()
{
	int i;

	strcpy(setting.vercnf, VERCNF);
	
	setting.screensize = SCR_X2_FIT;
	setting.gb_type = 0;
	setting.gb_palette = PAL_DARK_GREEN;
	setting.frameskip = 2;
	setting.vsync = 1;
	setting.sound = 1;
	setting.sound_buffer = 0;
	
	setting.color[0] = DEF_COLOR0;
	setting.color[1] = DEF_COLOR1;
	setting.color[2] = DEF_COLOR2;
	setting.color[3] = DEF_COLOR3;
	setting.bgbright=100;
	
	for(i=0; i<32; i++){
		setting.skeys[i].buttons = 0;
		setting.skeys[i].n = -1;
	}
	for(i=0; i<=6; i++)
		setting.skeys[i].n = i;
	setting.skeys[0].buttons = CTRL_CIRCLE;
	setting.skeys[1].buttons = CTRL_CROSS;
	setting.skeys[2].buttons = CTRL_TRIANGLE;
	setting.skeys[3].buttons = CTRL_SQUARE;
	setting.skeys[4].buttons = CTRL_SELECT;
	setting.skeys[5].buttons = CTRL_START;
	setting.skeys[6].buttons = CTRL_LTRIGGER;
	setting.skeys[7].buttons = CTRL_RTRIGGER|CTRL_SELECT;
	setting.skeys[7].n = 11;
	setting.skeys[8].buttons = CTRL_RTRIGGER|CTRL_START;
	setting.skeys[8].n = 12;
	
	//davex: default key for rewinding
	setting.skeys[9].buttons = CTRL_RTRIGGER;
	setting.skeys[9].n = 8;
	
	
	setting.analog2dpad=1;
	setting.thumb = 1;
	setting.cpu_clock = 0;
	strcpy(setting.lastpath,RinPath);
	for(i=0; i<16; i++)
		setting.bScreenSizes[i] = 1;
	for(i=0; i<32; i++)
		setting.bGB_Pals[i] = 1;
	setting.compress = 1;
	setting.quickslot = 0;
}

void check_config()
{
	int i;

	if(strcmp(setting.vercnf, VERCNF)){
		init_config();
		return;
	}

	if(setting.screensize>=SCR_END)
		setting.screensize = SCR_X2_FIT;
	if(setting.gb_type>4)
		setting.gb_type = 0;
	if(setting.gb_palette<=PAL_MONOCHROME || setting.gb_palette>=PAL_SGB)
		setting.gb_palette = PAL_DARK_GREEN;
	if(setting.frameskip > 9)
		setting.frameskip=0;
	if(setting.sound_buffer>2)
		setting.sound_buffer = 0;
	if(setting.bgbright>100)
		setting.bgbright=100;
	if(setting.cpu_clock>2)
		setting.cpu_clock = 0;
	if(strncmp(setting.lastpath, "ms0:/", 5))
		strcpy(setting.lastpath,RinPath);
	for(i=1; i<PAL_SGB; i++)
		if(setting.bGB_Pals[i]) break;
	if(i>=PAL_SGB)
		setting.bGB_Pals[1] = 1;
	for(i=0; i<SCR_END; i++)
		if(setting.bScreenSizes[i]) break;
	if(i>=SCR_END)
		setting.bScreenSizes[0] = 1;
}

void set_gb_type()
{
	if (rom_get_loaded()){
		switch(setting.gb_type){
		case 0:
			if(org_gbtype==1){
				rom_get_info()->gb_type = 2;
				lcd_set_mpal(setting.gb_palette);
			}else if(org_gbtype == 2){
				rom_get_info()->gb_type = 2;
				lcd_set_mpal(PAL_SGB);
			}else if(org_gbtype == 3){
				rom_get_info()->gb_type = 3;
				lcd_set_mpal(PAL_SGB);
			}
			break;
		case 1:
			rom_get_info()->gb_type = 1;
			lcd_set_mpal(PAL_MONOCHROME);
			break;
		case 2:
			rom_get_info()->gb_type = 2;
			if(sgb_mode)
				lcd_set_mpal(PAL_SGB);
			else
				lcd_set_mpal(setting.gb_palette);
			break;
		case 3:
			rom_get_info()->gb_type = 3;
			lcd_set_mpal(setting.gb_palette);
			break;
		case 4:
			rom_get_info()->gb_type = 4;
			lcd_set_mpal(setting.gb_palette);
			break;
		}
		
		if(rom_get_info()->gb_type>=3 && org_gbtype==3)
			now_gb_mode = 3;
		else if(rom_get_info()->gb_type==2 && sgb_mode)
			now_gb_mode = 2;
		else
			now_gb_mode = 1;
	}
}

int get_nShortcutKey(u32 buttons)
{
	int i;
	for(i=6; i<32; i++){
		if (setting.skeys[i].buttons==0)
			return -1;
		if ((buttons & setting.skeys[i].buttons)==setting.skeys[i].buttons)
			return setting.skeys[i].n;
	}
	return -1;
}

// by kwn
int load_menu_bg()
{
	char path[MAX_PATH], *p;
	strcpy(path, RinPath);

	p = strrchr(path, '/');
	strcpy(p+1, "RINMENU.PNG");
	if(read_png(path,bgBitmap,sizeof(bgBitmap)))
		return 1;

	p = strrchr(path, '.');
	strcpy(p, ".BMP");
	return read_bitmap(path,bgBitmap,sizeof(bgBitmap));
}

// 半透明処理
unsigned short rgbTransp(unsigned short fgRGB, unsigned short bgRGB, int alpha) {
    unsigned short fgR, fgG, fgB;
    unsigned short bgR, bgG, bgB;
	unsigned short R, G, B;
 	unsigned short rgb;

    fgB = (fgRGB >> 10) & 0x1F;
    fgG = (fgRGB >> 5) & 0x1F;
    fgR = fgRGB & 0x1F;

    bgB = (bgRGB >> 10) & 0x1F;
    bgG = (bgRGB >> 5) & 0x1F;
    bgR = bgRGB & 0x1F;

	R = coltbl[fgR][bgR][alpha/10];
	G = coltbl[fgG][bgG][alpha/10];
	B = coltbl[fgB][bgB][alpha/10];

	rgb = (((B & 0x1F)<<10)+((G & 0x1F)<<5)+((R & 0x1F)<<0)+0x8000);
    return rgb;
}

void bgbright_change()
{
	unsigned short *vptr=bgBitmap, rgb;
	int i;
	for(i=0; i<272*480; i++){
		rgb = *vptr;
		*vptr++ = rgbTransp(rgb, 0x0000, setting.bgbright);
	}
}

void rin_frame(const char *msg0, const char *msg1)
{
	char tmp[128];

	// 背景
	if(bBitmap)
		pgBitBlt(0,0,480,272,1,bgBitmap);
	else
		pgFillvram(setting.color[0]);

	// バージョンナンバー
	sprintf(tmp, "■ ＲＩＮ Ver%s ■", VERRIN);
	mh_print(465-strlen(tmp)*5, 0, tmp, setting.color[1]);

	// メッセージなど
	if(msg0) mh_print(17, 14, msg0, setting.color[2]);
	pgDrawFrame(17,25,463,248,setting.color[1]);
	pgDrawFrame(18,26,462,247,setting.color[1]);

	// 操作説明
	if(msg1) mh_print(17, 252, msg1, setting.color[2]);

	// バッテリーライフ
	if(scePowerIsBatteryExist()){
		sprintf(tmp,"Battery[%d%%]",scePowerGetBatteryLifePercent());
		if(!scePowerIsPowerOnline()){
			int time=scePowerGetBatteryLifeTime();
			if(time >= 0)
				sprintf(&tmp[strlen(tmp)-1],"(%d:%02d)]",time/60,time%60);
		}
		mh_print(465-strlen(tmp)*5, 252, tmp, setting.color[2]);
	}
}

int rin_MessageBox(const char *msg, int type){
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE){
			return 1;
		}else if(new_pad & CTRL_CROSS && type){
			return 0;
		}
		
		if(type)
			rin_frame(0,"○：OK  ×：Cancel");
		else
			rin_frame(0,"○：OK");
		mh_print(28,32,msg,setting.color[3]);
		pgScreenFlipV();
	}
}

void rin_colorconfig(void)
{
	enum
	{
		COLOR0_R,
		COLOR0_G,
		COLOR0_B,
		COLOR1_R,
		COLOR1_G,
		COLOR1_B,
		COLOR2_R,
		COLOR2_G,
		COLOR2_B,
		COLOR3_R,
		COLOR3_G,
		COLOR3_B,
		BG_BRIGHT,
		EXIT,
		INIT,
	};
	char tmp[4], msg[256];
	int color[4][3];
	int sel=0, x, y, i, crs_count=0, bLoop=1;

	memset(color, 0, sizeof(int)*4*3);
	for(i=0; i<4; i++){
		color[i][2] = setting.color[i]>>10 & 0x1F;
		color[i][1] = setting.color[i]>>5 & 0x1F;
		color[i][0] = setting.color[i] & 0x1F;
	}

	for(;;){
		readpad();
		if(now_pad & (CTRL_UP|CTRL_DOWN|CTRL_LEFT|CTRL_RIGHT))
			crs_count=0;
		if(new_pad & CTRL_CIRCLE){
			switch(sel){
			case EXIT:
				bLoop=0;
				break;
			case INIT:
				color[0][2] = DEF_COLOR0>>10 & 0x1F;
				color[0][1] = DEF_COLOR0>>5 & 0x1F;
				color[0][0] = DEF_COLOR0 & 0x1F;
				color[1][2] = DEF_COLOR1>>10 & 0x1F;
				color[1][1] = DEF_COLOR1>>5 & 0x1F;
				color[1][0] = DEF_COLOR1 & 0x1F;
				color[2][2] = DEF_COLOR2>>10 & 0x1F;
				color[2][1] = DEF_COLOR2>>5 & 0x1F;
				color[2][0] = DEF_COLOR2 & 0x1F;
				color[3][2] = DEF_COLOR3>>10 & 0x1F;
				color[3][1] = DEF_COLOR3>>5 & 0x1F;
				color[3][0] = DEF_COLOR3 & 0x1F;
				setting.bgbright = 100;
				if(bBitmap){
					load_menu_bg();
					bgbright_change();
				}
				break;
			case BG_BRIGHT:
				//輝度変更
				setting.bgbright += 10;
				if(setting.bgbright > 100) setting.bgbright=0;
				if(bBitmap){
					load_menu_bg();
					bgbright_change();
				}
				break;
			default:
				if(color[sel/3][sel%3]<31)
					color[sel/3][sel%3]++;
				break;
			}
		}else if(new_pad & CTRL_SQUARE){
			if(sel == BG_BRIGHT) {
				//輝度変更
				if(setting.bgbright > 10)
					setting.bgbright-=10;
				else
					setting.bgbright=100;
				if(bBitmap){
					load_menu_bg();
					bgbright_change();
				}
			}else if(sel>=COLOR0_R && sel<=COLOR3_B){
				if(color[sel/3][sel%3]>0)
					color[sel/3][sel%3]--;
			}
		}else if(new_pad & CTRL_CROSS){
			break;
		}else if(new_pad & CTRL_UP){
			if(sel!=0)	sel--;
			else		sel=INIT;
		}else if(new_pad & CTRL_DOWN){
			if(sel!=INIT)	sel++;
			else			sel=0;
		}else if(new_pad & CTRL_RIGHT){
			if(sel<COLOR1_R) 		sel=COLOR1_R;
			else if(sel<COLOR2_R)	sel=COLOR2_R;
			else if(sel<COLOR3_R)	sel=COLOR3_R;
			else if(sel<BG_BRIGHT)	sel=BG_BRIGHT;
			else if(sel<EXIT)		sel=EXIT;
		}else if(new_pad & CTRL_LEFT){
			if(sel>BG_BRIGHT)		sel=BG_BRIGHT;
			else if(sel>COLOR3_B)	sel=COLOR3_R;
			else if(sel>COLOR2_B)	sel=COLOR2_R;
			else if(sel>COLOR1_B)	sel=COLOR1_R;
			else					sel=COLOR0_R;
		}
		if(!bLoop) break;
		
		for(i=0; i<4; i++)
			setting.color[i]=color[i][2]<<10|color[i][1]<<5|color[i][0]|0x8000;
		
		if (crs_count++>=30) crs_count=0;
		
		x = 2;
		y = 5;
		
		if(sel>=COLOR0_R && sel<=BG_BRIGHT)
			strcpy(msg, "○：Add　□：Sub ×：Return");
		else
			strcpy(msg, "○：OK ×：Return");

		rin_frame(0, msg);

		pgPrint(x,y++,setting.color[3],"  COLOR0 R:");
		pgPrint(x,y++,setting.color[3],"  COLOR0 G:");
		pgPrint(x,y++,setting.color[3],"  COLOR0 B:");
		y++;
		pgPrint(x,y++,setting.color[3],"  COLOR1 R:");
		pgPrint(x,y++,setting.color[3],"  COLOR1 G:");
		pgPrint(x,y++,setting.color[3],"  COLOR1 B:");
		y++;
		pgPrint(x,y++,setting.color[3],"  COLOR2 R:");
		pgPrint(x,y++,setting.color[3],"  COLOR2 G:");
		pgPrint(x,y++,setting.color[3],"  COLOR2 B:");
		y++;
		pgPrint(x,y++,setting.color[3],"  COLOR3 R:");
		pgPrint(x,y++,setting.color[3],"  COLOR3 G:");
		pgPrint(x,y++,setting.color[3],"  COLOR3 B:");
		y++;
		if(setting.bgbright / 100 == 1)
			pgPrint(x,y++,setting.color[3],"  BG BRIGHT:100%");
		else
			pgPrint(x,y++,setting.color[3],"  BG BRIGHT:  0%");
		if(setting.bgbright % 100 != 0)			// 10%～90%
			pgPutChar((x+13)*8,(y-1)*8,setting.color[3],0,'0'+setting.bgbright/10,1,0,1);
		y++;
		pgPrint(x,y++,setting.color[3],"  Return to Main Menu");
		pgPrint(x,y++,setting.color[3],"  Initialize");

		x=14; y=5;
		for(i=0; i<12; i++){
			if(i!=0 && i%3==0) y++;
			sprintf(tmp, "%d", color[i/3][i%3]);
			pgPrint(x,y++,setting.color[3],tmp);
		}
		
		if (crs_count < 15){
			x = 2;
			y = sel + 5;
			if(sel>=COLOR1_R) y++;
			if(sel>=COLOR2_R) y++;
			if(sel>=COLOR3_R) y++;
			if(sel>=BG_BRIGHT) y++;
			if(sel>=EXIT) y++;
			pgPutChar((x+1)*8,y*8,setting.color[3],0,127,1,0,1);
		}
		
		pgScreenFlipV();
	}
}

int cmp_skey(S_BUTTON *a, S_BUTTON *b)
{
	int i, na=0, nb=0;
	
	for(i=0; i<32; i++){
		if ((a->buttons >> i) & 1) na++;
		if ((b->buttons >> i) & 1) nb++;
	}
    return nb-na;
}

void sort_skeys(S_BUTTON *a, int left, int right) {
	S_BUTTON tmp, pivot;
	int i, p;
	
	if (left < right) {
		pivot = a[left];
		p = left;
		for (i=left+1; i<=right; i++) {
			if (cmp_skey(&a[i],&pivot)<0){
				p=p+1;
				tmp=a[p];
				a[p]=a[i];
				a[i]=tmp;
			}
		}
		a[left] = a[p];
		a[p] = pivot;
		sort_skeys(a, left, p-1);
		sort_skeys(a, p+1, right);
	}
}

// by kwn
void rin_keyconfig(void)
{
	enum
	{
		CONFIG_A,
		CONFIG_B,
		CONFIG_RAPIDA,
		CONFIG_RAPIDB,
		CONFIG_SELECT,
		CONFIG_START,
		CONFIG_MENU,
		CONFIG_WAIT,
		CONFIG_REWIND,//davex: CONFIG_VSYNC replaced
		CONFIG_SOUND,
		CONFIG_SCREENSIZE,
		CONFIG_QUICKSAVE,
		CONFIG_QUICKLOAD,
		CONFIG_STATE_SLOT,
		CONFIG_GB_COLOR,
		CONFIG_CPU_CLOCK,
		CONFIG_ANALOG2DPAD,
		CONFIG_EXIT,
	};
	char msg[256];
	int key_config[32];
	int sel=0, x, y, i, bPad=0, crs_count=0;
	
	for(i=0; i<32; i++)
		key_config[i] = 0;
	for(i=0; i<32; i++){
		if(setting.skeys[i].n >= 0)
			key_config[setting.skeys[i].n] = setting.skeys[i].buttons;
	}
	
	for(;;){
		readpad();
		if(now_pad & (CTRL_UP|CTRL_DOWN|CTRL_LEFT|CTRL_RIGHT))
			crs_count=0;
		if(now_pad==CTRL_LEFT || now_pad==CTRL_RIGHT){
			if(sel!=CONFIG_EXIT && sel!=CONFIG_MENU && sel!=CONFIG_ANALOG2DPAD)
				key_config[sel] = 0;
		}else if(now_pad==CTRL_UP){
			if(bPad==0){
				if(sel!=0)	sel--;
				else		sel=CONFIG_EXIT;
				bPad++;
			}else if(bPad >= 25){
				if(sel!=0)	sel--;
				else		sel=CONFIG_EXIT;
				bPad=20;
			}else
				bPad++;
		}else if(now_pad==CTRL_DOWN){
			if(bPad==0){
				if(sel!=CONFIG_EXIT)sel++;
				else				sel=0;
				bPad++;
			}else if(bPad >= 25){
				if(sel!=CONFIG_EXIT)sel++;
				else				sel=0;
				bPad=20;
			}else
				bPad++;
		}else if(new_pad != 0){
			if(sel==CONFIG_EXIT && new_pad&CTRL_CIRCLE)
				break;
			else if(sel==CONFIG_ANALOG2DPAD && new_pad&CTRL_CIRCLE)
				setting.analog2dpad = !setting.analog2dpad;
			else
				key_config[sel] = now_pad;
		}else{
			bPad=0;
		}
		
		if (crs_count++>=30) crs_count=0;
		
		if(sel>=CONFIG_ANALOG2DPAD)
			strcpy(msg,"○：OK");
		else
			strcpy(msg,"←→：Clear");
		
		rin_frame(0, msg);
		
		x=2; y=5;
		pgPrint(x,y++,setting.color[3],"  A BUTTON       :");
		pgPrint(x,y++,setting.color[3],"  B BUTTON       :");
		pgPrint(x,y++,setting.color[3],"  A BUTTON(RAPID):");
		pgPrint(x,y++,setting.color[3],"  B BUTTON(RAPID):");
		pgPrint(x,y++,setting.color[3],"  SELECT BUTTON  :");
		pgPrint(x,y++,setting.color[3],"  START BUTTON   :");
		pgPrint(x,y++,setting.color[3],"  MENU BUTTON    :");
		pgPrint(x,y++,setting.color[3],"  TURBO ON/OFF   :");
		pgPrint(x,y++,setting.color[3],"  REWIND         :");
		//pgPrint(x,y++,setting.color[3],"  VSYNC ON/OFF   :");//davex: VSYNC removed as key
		pgPrint(x,y++,setting.color[3],"  SOUND ON/OFF   :");
		pgPrint(x,y++,setting.color[3],"  SCREEN SIZE    :");
		pgPrint(x,y++,setting.color[3],"  QUICK SAVE     :");
		pgPrint(x,y++,setting.color[3],"  QUICK LOAD     :");
		pgPrint(x,y++,setting.color[3],"  QUICK SLOT     :");
		pgPrint(x,y++,setting.color[3],"  GB PALETTE     :");
		pgPrint(x,y++,setting.color[3],"  CPU CLOCK      :");
		y++;
		if(setting.analog2dpad)
			pgPrint(x,y++,setting.color[3],"  AnalogPad to D-Pad: ON");
		else
			pgPrint(x,y++,setting.color[3],"  AnalogPad to D-Pad: OFF");
		y++;
		pgPrint(x,y++,setting.color[3],"  Return to Main Menu");
		
		for (i=0; i<CONFIG_ANALOG2DPAD; i++){
			y = i + 5;
			int j = 0;
			msg[0]=0;
			if(key_config[i] == 0){
				strcpy(msg,"UNDEFINED");
			}else{
				if (key_config[i] & CTRL_LTRIGGER){
					msg[j++]='L'; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_RTRIGGER){
					msg[j++]='R'; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_CIRCLE){
					msg[j++]=1; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_CROSS){
					msg[j++]=2; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_SQUARE){
					msg[j++]=3; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_TRIANGLE){
					msg[j++]=4; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_START){
					strcat(msg,"START+"); j+=6;
				}
				if (key_config[i] & CTRL_SELECT){
					strcat(msg,"SELECT+"); j+=7;
				}
				if (key_config[i] & CTRL_UP){
					msg[j++]=5; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_RIGHT){
					msg[j++]=6; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_DOWN){
					msg[j++]=7; msg[j++]='+'; msg[j]=0;
				}
				if (key_config[i] & CTRL_LEFT){
					msg[j++]=8; msg[j++]='+'; msg[j]=0;
				}
				msg[strlen(msg)-1]=0;
			}
			pgPrint(21,y,setting.color[3],msg);
		}
		
		
		if (crs_count < 15){
			x = 2;
			y = sel + 5;
			if(sel >= CONFIG_ANALOG2DPAD) y++;
			if(sel >= CONFIG_EXIT)        y++;
			pgPutChar((x+1)*8,y*8,setting.color[3],0,127,1,0,1);
		}
		
		pgScreenFlipV();
	}
	
	for(i=0; i<32; i++){
		if (i!=6 && key_config[i] == key_config[6])
			key_config[i] = 0;
		if(key_config[i]){
			setting.skeys[i].buttons = key_config[i];
			setting.skeys[i].n = i;
		}else{
			setting.skeys[i].buttons = 0;
			setting.skeys[i].n = -1;
		}
	}
	sort_skeys(&setting.skeys[6],0,25);
}

const char *gbtype_names[] = {
	"AUTO",
	"GB",
	"SGB",
	"GBC",
	"GBA",
};
int rin_gbtype(int n)
{
	int x,y,i,sel=n;
	
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE)
			return sel;
		else if(new_pad & CTRL_CROSS)
			return -1;
		else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>4) sel=0;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<0) sel=4;
		}else if(new_pad & CTRL_RIGHT){
			sel+=2;
			if(sel>4) sel=4;
		}else if(new_pad & CTRL_LEFT){
			sel-=2;
			if(sel<0) sel=0;
		}
		
		rin_frame("Select GB Type", "○：OK  ×：Cancel");
		
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],"GB TYPE:");
		for(i=0; i<=4; i++)
			pgPrint(x,y++,setting.color[i==sel?2:3],gbtype_names[i]);
		
		pgScreenFlipV();
	}
}

int rin_screensize(int n)
{
	int x,y,i,sel=n;
	
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE)
			return sel;
		else if(new_pad & CTRL_CROSS)
			return -1;
		else if(new_pad & CTRL_SELECT){
			setting.bScreenSizes[sel] = !setting.bScreenSizes[sel];
			for(i=0; i<SCR_END; i++)
				if(setting.bScreenSizes[i]) break;
			if(i>=SCR_END)
				setting.bScreenSizes[sel] = 1;
		}else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>=SCR_END) sel=0;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<0) sel=SCR_END-1;
		}else if(new_pad & CTRL_RIGHT){
			sel+=SCR_END/2;
			if(sel>=SCR_END) sel=SCR_END-1;
		}else if(new_pad & CTRL_LEFT){
			sel-=SCR_END/2;
			if(sel<0) sel=0;
		}
		
		if(setting.bScreenSizes[sel])
			rin_frame("Select Screen Size", "○：OK  ×：Cancel   SELECT：Disable");
		else
			rin_frame("Select Screen Size", "○：OK  ×：Cancel   SELECT：Enable");
		
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],"SCREEN SIZE:");
		for(i=0; i<SCR_END; i++){
			if(setting.bScreenSizes[i])
				pgPrint(x-2,y,setting.color[1],"+");
			pgPrint(x,y++,setting.color[i==sel?2:3],scr_names[i]);
		}
		
		pgScreenFlipV();
	}
}

int rin_gbcolor(int n)
{
	int x,y,i,sel=n;
	
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE)
			return sel;
		else if(new_pad & CTRL_CROSS)
			return -1;
		else if(new_pad & CTRL_SELECT){
			setting.bGB_Pals[sel] = !setting.bGB_Pals[sel];
			for(i=1; i<PAL_SGB; i++)
				if(setting.bGB_Pals[i]) break;
			if(i>=PAL_SGB)
				setting.bGB_Pals[sel] = 1;
		}else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>=PAL_SGB) sel=1;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<1) sel=PAL_SGB-1;
		}else if(new_pad & CTRL_RIGHT){
			sel+=(PAL_SGB-1)/2;
			if(sel>=PAL_SGB) sel=PAL_SGB-1;
		}else if(new_pad & CTRL_LEFT){
			sel-=(PAL_SGB-1)/2;
			if(sel<1) sel=1;
		}
		
		if(setting.bGB_Pals[sel])
			rin_frame("Select GB Palette", "○：OK  ×：Cancel   SELECT:Disable");
		else
			rin_frame("Select GB Palette", "○：OK  ×：Cancel   SELECT:Enable");
		
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],"GB PALETTE:");
		for(i=1; i<PAL_SGB; i++){
			if(setting.bGB_Pals[i])
				pgPrint(x-2,y,setting.color[1],"+");
			pgPrint(x,y++,setting.color[i==sel?2:3],pal_names[i]);
		}
		
		pgScreenFlipV();
	}
}

int rin_frameskip(int sel)
{
	char tmp[8];
	int x,y,i;
	
	strcpy(tmp,"0");
	
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE)
			return sel;
		else if(new_pad & CTRL_CROSS)
			return -1;
		else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>9) sel=0;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<0) sel=9;
		}else if(new_pad & CTRL_RIGHT){
			sel+=5;
			if(sel>9) sel=9;
		}else if(new_pad & CTRL_LEFT){
			sel-=5;
			if(sel<0) sel=0;
		}
		
		rin_frame("Select Max Frame Skip", "○：OK  ×：Cancel");
		
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],"MAX FRAME SKIP:");
		for(i=0; i<=9; i++){
			tmp[0] = i + '0';
			pgPrint(x,y++,setting.color[i==sel?2:3],tmp);
		}
		
		pgScreenFlipV();
	}
}

const char *cpu_clocks[] = {
	"222MHz (default)",
	"266MHz",
	"333MHz",
};
int rin_cpuclock(int sel)
{
	int x,y,i;
	
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE)
			return sel;
		else if(new_pad & CTRL_CROSS)
			return -1;
		else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>2) sel=0;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<0) sel=2;
		}
		
		rin_frame("Select CPU Clock (DANGER! DANGER! DANGER! DANGER! DANGER!)", "○：OK  ×：Cancel");
		
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],"CPU CLOCK:");
		for(i=0; i<3; i++)
			pgPrint(x,y++,setting.color[i==sel?2:3],cpu_clocks[i]);
		
		pgScreenFlipV();
	}
}

const char *sound_buffers[] = {
	"FAST",
	"NORMAL",
	"SLOW",
};
int rin_sound_buffers(int sel)
{
	int x,y,i;
	
	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE)
			return sel;
		else if(new_pad & CTRL_CROSS)
			return -1;
		else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>2) sel=0;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<0) sel=2;
		}
		
		rin_frame("Select Sound Buffer", "○：OK  ×：Cancel");
		
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],"SOUND BUFFER:");
		for(i=0; i<3; i++)
			pgPrint(x,y++,setting.color[i==sel?2:3],sound_buffers[i]);
		
		pgScreenFlipV();
	}
}

void rin_findState(int nState[], int nThumb[])
{
	char tmp[MAX_PATH], *p;
	int i, j;
	
	strcpy(tmp,SavePath);
	p = strrchr(tmp,'/') + 1;
	*p = 0;
	
	nfiles = 0;
	strcpy(path_files, tmp);
	int fd = sceIoDopen(tmp);
	while(nfiles<MAX_ENTRY){
		memset(&files[nfiles], 0x00, sizeof(SceIoDirent));
		if(sceIoDread(fd, &files[nfiles])<=0) break;
		nfiles++;
	}
	sceIoDclose(fd);
	
	for(i=0; i<=STATE_SLOT_MAX; i++){
		get_state_path(i,tmp);
		nState[i]=-1;
		for(j=0; j<nfiles; j++){
			if(!stricmp(p,files[j].name)){
				nState[i] = j;
				break;
			}
		}

		if (nState[i]<0){
			strcat(p, ".gz");
			for(j=0; j<nfiles; j++){
				if(!stricmp(p,files[j].name)){
					nState[i] = j;
					break;
				}
			}
		}
		
		get_thumb_path(i,tmp);
		nThumb[i]=-1;
		for(j=0; j<nfiles; j++){
			if(!stricmp(p,files[j].name)){
				nThumb[i] = j;
				break;
			}
		}
		if(nThumb[i]>=0)
			continue;

		*strrchr(tmp, '.') = 0;
		for(j=0; j<nfiles; j++){
			if(!stricmp(p,files[j].name)){
				nThumb[i] = j;
				break;
			}
		}
	}
}

enum
{
	RIN_STATE_SAVE,
	RIN_STATE_LOAD,
	RIN_QUICK_SLOT,
};
int rin_stateslot(int type)
{
	const int MAX_ITEM = STATE_SLOT_MAX+1;
	char msg[256], *p;
	static int ex_sel=0;
	int nState[STATE_SLOT_MAX+1], nThumb[STATE_SLOT_MAX+1];
	int x, y, i, ret, sel=ex_sel, sel_bak=-1;

	rin_findState(nState, nThumb);

	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE){
			if (type != RIN_STATE_LOAD)
				break;
			else{
				if(sel > STATE_SLOT_MAX){
					if(state_tmp)
						break;
				}else if (nState[sel]>=0)
					break;
			}
		}else if(new_pad & CTRL_CROSS){
			return -1;
		}else if((new_pad & CTRL_SELECT)){
			if (sel > STATE_SLOT_MAX){
				free(state_tmp);
				state_tmp = NULL;
			}
			else if (nState[sel]>=0){
				if (delete_state(sel)>=0)
					nState[sel] = nThumb[sel] = -1;
			}
		}else if(new_pad & CTRL_DOWN){
			sel++;
			if(sel>MAX_ITEM) sel=0;
		}else if(new_pad & CTRL_UP){
			sel--;
			if(sel<0) sel=MAX_ITEM;
		}else if(new_pad & CTRL_RIGHT){
			sel+=(MAX_ITEM+1)/2;
			if(sel>MAX_ITEM) sel=MAX_ITEM;
		}else if(new_pad & CTRL_LEFT){
			sel-=(MAX_ITEM+1)/2;
			if(sel<0) sel=0;
		}
		
		if (sel!=sel_bak){
			sel_bak = sel;
			if (sel > STATE_SLOT_MAX){
				if (state_tmp)
					memcpy(thumb_w,thumb_tmp,sizeof(thumb_w));
			}
			else if (nState[sel]>=0 && nThumb[sel]>=0){
				p = strrchr(files[nThumb[sel]].name, '.');
				if(!stricmp(p, ".png"))
					ret = load_thumb(sel,thumb_w,sizeof(thumb_w));
				else
					ret = load_thumb_old(sel,thumb_w,sizeof(thumb_w));
				if(!ret)
					nThumb[sel] = -1;
			}
		}
		
		switch(type)
		{
		case RIN_STATE_LOAD:
			p = "Select State Load Slot";
			break;
		case RIN_STATE_SAVE:
			p = "Select State Save Slot";
			break;
		case RIN_QUICK_SLOT:
			p = "Select Quick Slot";
			break;
		default:
			p = NULL;
		}
		rin_frame(p,"○：OK  ×：Cancel   SELECT：Remove");
		
		if ((sel>STATE_SLOT_MAX && state_tmp) ||
			(sel<=STATE_SLOT_MAX && nState[sel]>=0 && nThumb[sel]>=0)){
			pgBitBlt(272,50,160,144,1,thumb_w);
			pgDrawFrame(270,48,433,195,setting.color[1]);
			pgDrawFrame(271,49,432,194,setting.color[1]);
		}
		
		switch(type)
		{
		case RIN_STATE_LOAD:
			p = "STATE LOAD:";
			break;
		case RIN_STATE_SAVE:
			p = "STATE SAVE:";
			break;
		case RIN_QUICK_SLOT:
			p = "QUICK SLOT:";
			break;
		}
		x=4, y=5;
		pgPrint(x++,y++,setting.color[3],p);

		for(i=0; i<=STATE_SLOT_MAX; i++){
			if(nState[i] < 0){
				sprintf(msg,"%d - None", i);
			}else{
				sprintf(msg, "%d - %04d/%02d/%02d %02d:%02d:%02d", i,
					files[nState[i]].mtime.year,
					files[nState[i]].mtime.mon,
					files[nState[i]].mtime.mday,
					files[nState[i]].mtime.hour,
					files[nState[i]].mtime.min,
					files[nState[i]].mtime.sec);
			}
			pgPrint(x,y++,setting.color[i==sel?2:3],msg);
		}
		y++;
		if (state_tmp){
			sprintf(msg, "TMP:%04d/%02d/%02d %02d:%02d:%02d",
				state_tmp_time.year,
				state_tmp_time.mon,
				state_tmp_time.mday,
				state_tmp_time.hour,
				state_tmp_time.min,
				state_tmp_time.sec);
		}else
			strcpy(msg,"TMP:None");
		pgPrint(x,y++,setting.color[i==sel?2:3],msg);
		
		pgScreenFlipV();
	}

	ex_sel = sel;
	return sel;
}

void select_cheat(void)
{
	static int sel=0;
	int top=0, rows=21, x, y, h, i;
	
	cheat_decreate_cheat_map();

	for(;;){
		readpad();
		if(new_pad & CTRL_CIRCLE){
			cheats[sel].enable = !cheats[sel].enable;
		}else if(new_pad & CTRL_SQUARE){
			cheats[0].enable = !cheats[0].enable;
			for (i=0; i<nCheats; i++)
				cheats[i].enable = cheats[0].enable;
		}else if(new_pad & CTRL_CROSS){
			break;
		}else if(new_pad & CTRL_UP){
			sel--;
		}else if(new_pad & CTRL_DOWN){
			sel++;
		}else if(new_pad & CTRL_LEFT){
			sel-=rows/2;
		}else if(new_pad & CTRL_RIGHT){
			sel+=rows/2;
		}
		
		if(top > nCheats-rows)	top=nCheats-rows;
		if(top < 0)				top=0;
		if(sel >= nCheats)		sel=nCheats-1;
		if(sel < 0)				sel=0;
		if(sel >= top+rows)		top=sel-rows+1;
		if(sel < top)			top=sel;
		
		rin_frame("","○：OK  ×：Return  □：All");
		
		// スクロールバー
		if(nCheats > rows){
			h = 219;
			pgDrawFrame(445,25,446,248,setting.color[1]);
			pgFillBox(448, h*top/nCheats + 27,
				460, h*(top+rows)/nCheats + 27,setting.color[1]);
		}
		
		x=30; y=32;
		for (i=0; i<rows; i++){
			if (top+i >= nCheats) break;
			mh_print(x, y, cheats[top+i].name, setting.color[top+i==sel?2:3]);
			if (cheats[top+i].enable)
				mh_print(x-6, y, "*", setting.color[3]);
			y+=10;
		}
		
		pgScreenFlipV();
	}

	cheat_create_cheat_map();
	return;
}

void rin_menu(void)
{
	enum
	{
		STATE_SAVE,
		STATE_LOAD,
		QUICK_SLOT,
		SAVE_THUMB,
		COMPRESS,
		SCREEN_SIZE,
		GB_TYPE,
		GB_PALETTE,
		TURBO,
		VSYNC,
		SOUND,
		SOUND_BUFFER,
		MAX_FRAME_SKIP,
		CPU_CLOCK,
		COLOR_CONFIG,
		KEY_CONFIG,
		LOAD_CHEAT,
		SELECT_CHEAT,
		LOAD_ROM,
		RESET,
		CONTINUE,
	};
	char tmp[MAX_PATH], msg[256]={0};
	static int sel=0;
	int x, y, ret, crs_count=0, bLoop=1;
	int romsize, ramsize;
	
	old_pad = 0;
	readpad();
	old_pad = paddata.buttons;
	
	for(;;){
		readpad();
		if(now_pad & (CTRL_UP|CTRL_DOWN|CTRL_LEFT|CTRL_RIGHT))
			crs_count=0;
		if(new_pad)
			msg[0]=0;
		if(new_pad & CTRL_CIRCLE){
			switch(sel)
			{
			case STATE_SAVE:
				ret = rin_stateslot(RIN_STATE_SAVE);
				if(ret>=0){
					strcpy(msg, "State Save Failed");
					if(ret>STATE_SLOT_MAX){
						free(state_tmp);
						state_tmp = save_state_tmp();
						if(state_tmp)
							strcpy(msg, "State Saved Successfully");
					}else{
						if(save_state(ret))
							strcpy(msg, "State Saved Successfully");
					}
				}
				crs_count=0;
				break;
			case STATE_LOAD:
				ret = rin_stateslot(RIN_STATE_LOAD);
				if(ret>=0){
					strcpy(msg, "State Load Failed");
					if(ret>STATE_SLOT_MAX){
						if(load_state_tmp(state_tmp))
//							strcpy(msg, "State Loaded Successfully");
							bLoop = 0;
					}else{
						if(load_state(ret))
//							strcpy(msg, "State Loaded Successfully");
							bLoop = 0;
					}
				}
				crs_count=0;
				break;
			case SAVE_THUMB:
				setting.thumb = !setting.thumb;
				break;
			case COMPRESS:
				setting.compress = !setting.compress;
				break;
			case QUICK_SLOT:
				if (++setting.quickslot>STATE_SLOT_MAX+1) setting.quickslot=0;
				break;
			case SCREEN_SIZE:
				ret = rin_screensize(setting.screensize);
				if(ret>=0)
					setting.screensize = ret;
				crs_count=0;
				break;
			case MAX_FRAME_SKIP:
				ret = rin_frameskip(setting.frameskip);
				if(ret>=0)
					setting.frameskip = ret;
				crs_count=0;
				break;
			case TURBO:
				bTurbo = !bTurbo;
				break;
			case SOUND:
				setting.sound = !setting.sound;
				break;
			case VSYNC:
				setting.vsync = !setting.vsync;
				break;
			case GB_TYPE:
				ret = rin_gbtype(setting.gb_type);
				if(ret>=0){
					setting.gb_type = ret;
					gb_reset();
					bTurbo = 0;
				}
				crs_count=0;
				break;
			case GB_PALETTE:
				ret = rin_gbcolor(setting.gb_palette);
				if(ret>=0){
					setting.gb_palette = ret;
					if(rom_get_info()->gb_type!=1 && now_gb_mode==1)
						lcd_set_mpal(setting.gb_palette);
				}
				crs_count=0;
				break;
			case SOUND_BUFFER:
				ret = rin_sound_buffers(setting.sound_buffer);
				if (ret>=0)
					setting.sound_buffer = ret;
				break;
			case CPU_CLOCK:
				ret = rin_cpuclock(setting.cpu_clock);
				if (ret>=0)
					setting.cpu_clock = ret;
				break;
			case COLOR_CONFIG:
				rin_colorconfig();
				crs_count=0;
				break;
			case KEY_CONFIG:
				rin_keyconfig();
				crs_count=0;
				break;
			case LOAD_CHEAT: 
				if(getFilePath(CheatPath,EXT_TCH)){
					strcpy(msg, "Cheat Load Failed");
					FILE *fp = fopen(CheatPath,"r");
					if (fp){
						if (cheat_load(fp))
							strcpy(msg, "Cheat Loaded Successfully");
						fclose(fp);
					}
				}
				crs_count=0;
				break;
			case SELECT_CHEAT:
				if (nCheats>0){
					select_cheat();
					crs_count=0;
				}
				break;
			case LOAD_ROM:
				for (;;){
					if (!getFilePath(RomPath,EXT_GB|EXT_GZ|EXT_ZIP)){
						if (bLoop)
							break;
						else
							continue;
					}
					strcpy(tmp, RomPath);
					*(strrchr(tmp,'/')+1) = 0;
					strcpy(setting.lastpath, tmp);

					bTurbo = 0;
					bLoop = 0;
					
					if (rom_get_loaded() && rom_has_battery())
						save_sram(get_sram(), rom_get_info()->ram_size);

					gb_init();

					// 指定したファイルをロードする。 by ruka
					romsize = load_rom(RomPath);
					if (!romsize){
						strcpy(filer_msg,"ROM Load Failed");
						continue;
					}
					ramsize = load_sram(sram_space, sizeof(sram_space));
					if (!gb_load_rom(rom_image, romsize, sram_space, ramsize)){
						strcpy(filer_msg,"ROM Load Failed");
						continue;
					}

					if(org_gbtype==1)
						renderer_set_msg("ROM TYPE:GB");
					else if(org_gbtype==2)
						renderer_set_msg("ROM TYPE:SGB");
					else if(org_gbtype==3)
						renderer_set_msg("ROM TYPE:GBC");

					free(state_tmp);
					state_tmp = NULL;

					break;
				}
				crs_count=0;
				break;
			case RESET:
				gb_reset();
				bTurbo = 0;
				bLoop = 0;
				break;
			case CONTINUE:
				bLoop = 0;
				break;
			}
		}else if(new_pad & CTRL_SQUARE){
			if (sel==QUICK_SLOT){
				if (setting.quickslot>0)
					setting.quickslot--;
				else
					setting.quickslot=STATE_SLOT_MAX+1;
			}
		}else if(new_pad & CTRL_CROSS){
			bLoop = 0;
		}else if(new_pad & CTRL_UP){
			if(sel!=0) sel--;
			else       sel=CONTINUE;
		}else if(new_pad & CTRL_DOWN){
			if(sel!=CONTINUE)	sel++;
			else				sel=0;
		}else if(new_pad & CTRL_LEFT){
			if(sel>LOAD_CHEAT)
				sel=LOAD_CHEAT;
			else if(sel>KEY_CONFIG)
				sel=SCREEN_SIZE;
			else if(sel>0)
				sel=0;
		}else if(new_pad & CTRL_RIGHT){
			if(sel<SCREEN_SIZE)
				sel=SCREEN_SIZE;
			else if(sel<LOAD_CHEAT)
				sel=LOAD_CHEAT;
			else if(sel<LOAD_ROM)
				sel=LOAD_ROM;
		}else if(get_nShortcutKey(new_pad)==6){
			bLoop = 0;
			break;
		}
		
		if(!bLoop) break;
		if (crs_count++>=30) crs_count=0;
		
		rin_frame(msg, "○：OK  ×：Continue  MenuBTN：Continue");
		
		x = 4;
		y = 5;
		
		pgPrintf(x,y++,setting.color[3],"STATE SAVE");
		pgPrintf(x,y++,setting.color[3],"STATE LOAD");
		if (setting.quickslot > STATE_SLOT_MAX)
			pgPrintf(x,y++,setting.color[3],"QUICK SLOT    : TEMP");
		else
			pgPrintf(x,y++,setting.color[3],"QUICK SLOT    : %d",setting.quickslot);
		pgPrintf(x,y++,setting.color[3],"SAVE THUMBNAIL: %s",setting.thumb?"ON":"OFF");
		pgPrintf(x,y++,setting.color[3],"COMPRESS FILE : %s",setting.compress?"ON":"OFF");
		y++;
		pgPrintf(x,y++,setting.color[3],"SCREEN SIZE   : %s",scr_names[setting.screensize]);
		pgPrintf(x,y++,setting.color[3],"GB TYPE       : %s",gbtype_names[setting.gb_type]);
		pgPrintf(x,y++,setting.color[3],"GB PALETTE    : %s",pal_names[setting.gb_palette]);
		pgPrintf(x,y++,setting.color[3],"TURBO         : %s",bTurbo?"ON":"OFF");
		pgPrintf(x,y++,setting.color[3],"VSYNC         : %s",setting.vsync?"ON":"OFF");
		pgPrintf(x,y++,setting.color[3],"SOUND         : %s",setting.sound?"ON":"OFF");
		pgPrintf(x,y++,setting.color[3],"SOUND BUFFER  : %s",sound_buffers[setting.sound_buffer]);
		pgPrintf(x,y++,setting.color[3],"MAX FRAME SKIP: %d",setting.frameskip);
		pgPrintf(x,y++,setting.color[3],"CPU CLOCK     : %s",cpu_clocks[setting.cpu_clock]);
		pgPrintf(x,y++,setting.color[3],"MENU COLOR CONFIG");
		pgPrintf(x,y++,setting.color[3],"KEY CONFIG");
		y++;
		pgPrintf(x,y++,setting.color[3],"Load Cheat File");
		pgPrintf(x,y++,setting.color[nCheats>0?3:2],"Select Cheatcode");
		y++;
		pgPrintf(x,y++,setting.color[3],"Back to ROM list");
		pgPrintf(x,y++,setting.color[3],"Reset");
		pgPrintf(x,y++,setting.color[3],"Continue");
		
		if(crs_count < 15){
			y = sel + 5;
			if(sel >= SCREEN_SIZE)	y++;
			if(sel >= LOAD_CHEAT)	y++;
			if(sel >= LOAD_ROM)		y++;
			tmp[0]=127; tmp[1]=0;
			pgPrintf(x-1,y,setting.color[3],tmp);
		}
		
		pgScreenFlipV();
	}
	
	pgFillvram(0);
	pgScreenFlipV();
	pgFillvram(0);
	pgScreenFlipV();
	gb_fill_vframe(0);
	pgWaitVn(10);
	memset(&paddata, 0x00, sizeof(paddata));
	wavoutClear();
	
	if(render_msg_mode!=6)
		render_msg_mode = 0;
	border_uploaded = 2;
}
