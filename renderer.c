#include "main.h"
#include <time.h>

int pad_state;
int cur_time;
int now_sensor_x,now_sensor_y;

char render_msg[128];
int render_msg_mode;
long render_msg_time;

const char *scr_names[] = {
	"x1",
	"x1.5 (filtered)",
	"x2 (uncropped)",
	"x2 (fit)",
	"x2 (scanline)",
	"x2 (uncropped without top)",
	"x2 (uncropped without bottom)",
	"FULL",
#ifdef USE_GPU
	"x1.5 BL",
	"x2 (fit) BL",
	"FULL BL",
#endif
    "x1 MIRROR",//davex: mirror modes
	"x1.5 (filtered) MIRROR",
	"x2 (uncropped) MIRROR",
	"x2 (fit) MIRROR",
	"x2 (scanline) MIRROR",
	"x2 (uncropped without top) MIRROR",
	"x2 (uncropped without bottom) MIRROR",
	"FULL MIRROR",

};

#define GBWIDTH  160
#define GBHEIGHT 144

unsigned short vframe_mirror[SIZE_LINE*(144+112)];
int g_mirror_on = 0;

//davex: vframe mirroring (only works if USE_GPU is not defined)
void vframe_mirroring( unsigned short *pvframe  ){    
    int i, j;
    unsigned short *pvframe_mirror;
    pvframe_mirror = vframe_mirror;
    for( j = 0; j<256; j++){
    	for( i = 0; i< 176; i++){
    		pvframe_mirror[i] = pvframe[175 - i];
    	}
    	pvframe_mirror += 176;
    	pvframe += 176;
    }
}




void render_screen(void *buf)
{
	if(render_msg_mode!=0 && render_msg_mode!=3){
		pgFillBox(0,263,479,271,0);
		render_msg_mode--;
	}
	
	
	
	g_mirror_on = 0; //default
	
	switch(setting.screensize){
		case SCR_X1:
			if (border_uploaded && now_gb_mode==2){
				pgBitBltSgb(112,24,(unsigned long *)sgb_border_buffer);
				border_uploaded--;
			}
			pgBitBltN1(160,64,(unsigned long *)buf);
			break;
		case SCR_X15:
			pgBitBltN15(120,28,(unsigned long *)buf);
			break;
		case SCR_X2_UNCROPED:
			pgBitBltN2(80,0,GBHEIGHT-8,(unsigned long *)buf);
			break;
		case SCR_X2_FIT:
			pgBitBltSt2Fix(80,0,GBHEIGHT,2,(unsigned short *)buf);
			break;
		case SCR_X2_SCANLINE:
			pgBitBltStScan(80,0,GBHEIGHT-8,(unsigned long *)buf);
			break;
		case SCR_X2_UTOP:
			pgBitBltSt2wotop(80,0,GBHEIGHT,(unsigned long *)buf);
			break;
		case SCR_X2_UBOTTOM:
			pgBitBltSt2wobot(80,0,GBHEIGHT,(unsigned long *)buf);
			break;
		case SCR_FULL:
			pgBitBltStFull(0,0,GBHEIGHT,3,(unsigned short *)buf);
			break;
#ifdef USE_GPU	
		case SCR_X15_BL:
			pgBitBltGe(120,28,240,216,buf);
			break;
		case SCR_X2_FIT_BL:
			pgBitBltGe(89,0,302,272,buf);
			break;
		case SCR_FULL_BL:
			pgBitBltGe(0,0,480,272,buf);
			break;
#endif
		case SCR_X1_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;	
			
			if (border_uploaded && now_gb_mode==2){
				pgBitBltSgb(112,24,(unsigned long *)sgb_border_buffer);
				border_uploaded--;
			}
			pgBitBltN1(160,64,(unsigned long *)vframe_mirror);
			break;
		case SCR_X15_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltN15(120,28,(unsigned long *)vframe_mirror);
			break;
		case SCR_X2_UNCROPED_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltN2(80,0,GBHEIGHT-8,(unsigned long *)vframe_mirror);
			break;
		case SCR_X2_FIT_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltSt2Fix(80,0,GBHEIGHT,2,(unsigned short *)vframe_mirror);
			break;
		case SCR_X2_SCANLINE_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltStScan(80,0,GBHEIGHT-8,(unsigned long *)vframe_mirror);
			break;
		case SCR_X2_UTOP_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltSt2wotop(80,0,GBHEIGHT,(unsigned long *)vframe_mirror);
			break;
		case SCR_X2_UBOTTOM_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltSt2wobot(80,0,GBHEIGHT,(unsigned long *)vframe_mirror);
			break;
		case SCR_FULL_MIRROR:
			vframe_mirroring( (unsigned short*) buf);
			g_mirror_on = 1;
			
			pgBitBltStFull(0,0,GBHEIGHT,3,(unsigned short *)vframe_mirror);
			break;

		default:
			pgBitBltN1(160,64,(unsigned long *)buf);
	}
	
	if(render_msg_mode>=3){
		pgPrint_drawbg(0,33,0xffff,0,render_msg);
		if (sceKernelLibcClock() > render_msg_time + 1000000)
			render_msg_mode=2;
	}
}

//４バンクに分けて、現在出力中じゃないバンクに書いていく - LCK
void renderer_update_sound()
{
	int sound_buf_len = 768 + 256 * setting.sound_buffer;
	static unsigned int nextbank=1;
	unsigned int playbank=cur_play/sound_buf_len;
	if (nextbank!=playbank) {
		snd_render(&sound_buf[nextbank*sound_buf_len*2],sound_buf_len);
		nextbank=((nextbank+1)&3);
	}
}

#define UPPER_THRESHOLD  0xcf
#define LOWER_THRESHOLD  0x2f
void renderer_update_pad()
{
	static int rapid_state=0;
	static int bPad=0;
	int pad_state_bak;
	char tmp[256];
	
	pad_state=0;
	sceCtrlPeekBufferPositive(&paddata, 1);
	//sceCtrlReadBufferPositive(&paddata, 1);
	if(setting.skeys[2].buttons && (paddata.buttons&setting.skeys[2].buttons)==setting.skeys[2].buttons)
		pad_state|=1 * (rapid_state/2);  // by kwn
	else if(setting.skeys[0].buttons && (paddata.buttons&setting.skeys[0].buttons)==setting.skeys[0].buttons)
		pad_state|=1;
	if(setting.skeys[3].buttons && (paddata.buttons&setting.skeys[3].buttons)==setting.skeys[3].buttons)
		pad_state|=2 * (rapid_state/2);  // by kwn
	else if(setting.skeys[1].buttons && (paddata.buttons&setting.skeys[1].buttons)==setting.skeys[1].buttons)
		pad_state|=2;
	if(setting.skeys[4].buttons && (paddata.buttons&setting.skeys[4].buttons)==setting.skeys[4].buttons)
		pad_state|=4;
	if(setting.skeys[5].buttons && (paddata.buttons&setting.skeys[5].buttons)==setting.skeys[5].buttons)
		pad_state|=8;
	if(paddata.buttons & CTRL_DOWN)  pad_state|=16;
	if(paddata.buttons & CTRL_UP)    pad_state|=32;
	
	
	//davex
	if( g_mirror_on){
		if(paddata.buttons & CTRL_LEFT)  pad_state|=128;
		if(paddata.buttons & CTRL_RIGHT) pad_state|=64;
	}else{
		if(paddata.buttons & CTRL_LEFT)  pad_state|=64;
		if(paddata.buttons & CTRL_RIGHT) pad_state|=128;
	}
	
	
	// kmg
	// Analog pad state
	if(setting.analog2dpad){
		if (paddata.analog[CTRL_ANALOG_Y] > UPPER_THRESHOLD) pad_state|=0x10; // DOWN
		if (paddata.analog[CTRL_ANALOG_Y] < LOWER_THRESHOLD) pad_state|=0x20; // UP
		
		
		if( g_mirror_on){
			if (paddata.analog[CTRL_ANALOG_X] < LOWER_THRESHOLD) pad_state|=0x80; 
			if (paddata.analog[CTRL_ANALOG_X] > UPPER_THRESHOLD) pad_state|=0x40; 
		}else{
			if (paddata.analog[CTRL_ANALOG_X] < LOWER_THRESHOLD) pad_state|=0x40; // LEFT
			if (paddata.analog[CTRL_ANALOG_X] > UPPER_THRESHOLD) pad_state|=0x80; // RIGHT
		}
	}
	
	int n = get_nShortcutKey(paddata.buttons);
	if(n == -1)
		bPad = 1;
	else{
		if(!bPad) n = -1;
		bPad = 0;
		pad_state_bak = pad_state;
		pad_state=0;
		
		switch(n)
		{
		case -1:
			break;
		case 6:
			bMenu = 1;
			break;
		case 7:
			bTurbo = !bTurbo;
			if(bTurbo)
				renderer_set_msg("TURBO:ON");
			else
				renderer_set_msg("TURBO:OFF");
			bPad=0;
			break;
		case 8:
			//davex: key assigned to rewind
			/*
			setting.vsync = !setting.vsync;
			if(setting.vsync)
				renderer_set_msg("VSYNC:ON");
			else
				renderer_set_msg("VSYNC:OFF");
			
			renderer_set_msg("REWIND KEY PRESS");
			*/
			break;
		case 9:
			setting.sound = !setting.sound;
			wavout_enable = setting.sound;
			if(setting.sound)
				renderer_set_msg("SOUND:ON");
			else
				renderer_set_msg("SOUND:OFF");
			break;
		case 10:
			for(;;){
				if (++setting.screensize >= SCR_END) setting.screensize=0;
				if (setting.bScreenSizes[setting.screensize]) break;
			}
			sprintf(tmp,"SCREEN SIZE:%s",scr_names[setting.screensize]);
			renderer_set_msg(tmp);
			pgFillvram(0);
			pgScreenFlip();
			pgFillvram(0);
			pgScreenFlip();
			border_uploaded = 2;
			break;
		case 11:
			set_cpu_clock(0);
			wavout_enable=0; // kmg
			// by Smiths
			if(setting.quickslot > STATE_SLOT_MAX){
				free(state_tmp);
				state_tmp = save_state_tmp();
				if(state_tmp)
					sprintf(tmp,"Save State at TEMP");
				else
					sprintf(tmp,"State Save Failed at TEMP");
			}else{
				if(save_state(setting.quickslot))
					sprintf(tmp,"Save State at %d",setting.quickslot);
				else
					sprintf(tmp,"State Save Failed at %d",setting.quickslot);
			}
			renderer_set_msg(tmp);
			if(setting.sound) wavout_enable=1; // kmg
			set_cpu_clock(setting.cpu_clock);
			break;
		case 12:
			set_cpu_clock(0);
			wavout_enable=0; // kmg
			// by Smiths
			if(setting.quickslot > STATE_SLOT_MAX){
				if(load_state_tmp(state_tmp))
					sprintf(tmp,"Load State at TEMP");
				else
					sprintf(tmp,"State Load Failed at TEMP");
			}else{
				if(load_state(setting.quickslot))
					sprintf(tmp,"Load State at %d",setting.quickslot);
				else
					sprintf(tmp,"State Load Failed at %d",setting.quickslot);
			}
			renderer_set_msg(tmp);
			if(setting.sound) wavout_enable=1; // kmg
			set_cpu_clock(setting.cpu_clock);
			break;
		case 13:
			if (++setting.quickslot > STATE_SLOT_MAX+1) setting.quickslot=0;
			if (setting.quickslot > STATE_SLOT_MAX)
				sprintf(tmp,"Quick Slot:TEMP");
			else
				sprintf(tmp,"Quick Slot:%d",setting.quickslot);
			renderer_set_msg(tmp);
			bPad=0;
			break;
		case 14:
			if (rom_get_info()->gb_type!=1 && now_gb_mode==1){
				for(;;){
					if (++setting.gb_palette >= PAL_SGB) setting.gb_palette=1;
					if (setting.bGB_Pals[setting.gb_palette]) break;
				}
				lcd_set_mpal(setting.gb_palette);
				sprintf(tmp,"GB PALETTE:%s",pal_names[setting.gb_palette]);
				renderer_set_msg(tmp);
			}
			break;
		case 15:
			if(++setting.cpu_clock > 2) setting.cpu_clock=0;
			set_cpu_clock(setting.cpu_clock);
			sprintf(tmp,"CPU CLOCK:%s",cpu_clocks[setting.cpu_clock]);
			renderer_set_msg(tmp);
			break;
		default:
			pad_state = pad_state_bak;
			bPad = 1;
			break;
		}
	}
	rapid_state = (rapid_state + 1) % 4;
	// モーションセンサー
	now_sensor_x=2047-paddata.analog[CTRL_ANALOG_X]+127;
	now_sensor_y=2047-paddata.analog[CTRL_ANALOG_Y]+127;
}

void renderer_refresh()
{
	renderer_update_pad();
	renderer_update_sound();
}

void renderer_init()
{
	cur_time=0;
	render_msg[0]=0;
	render_msg_mode=0;
	
	renderer_reset();
}

void renderer_reset()
{
	pad_state=0;
	now_sensor_x=now_sensor_y=2047;
}

byte renderer_get_time(int type)
{
	unsigned long now=time(NULL)-cur_time;

	switch(type){
	case 8: // 秒
		return (byte)(now%60);
	case 9: // 分
		return (byte)((now/60)%60);
	case 10: // 時
		return (byte)((now/(60*60))%24);
	case 11: // 日(L)
		return (byte)((now/(24*60*60))&0xff);
	case 12: // 日(H)
		return (byte)((now/(256*24*60*60))&1);
	}
	return 0;
}

void renderer_set_time(int type,byte dat)
{
	unsigned long now=time(NULL);
	unsigned long adj=now-cur_time;

	switch(type){
	case 8: // 秒
		adj=(adj/60)*60+(dat%60);
		break;
	case 9: // 分
		adj=(adj/(60*60))*60*60+(dat%60)*60+(adj%60);
		break;
	case 10: // 時
		adj=(adj/(24*60*60))*24*60*60+(dat%24)*60*60+(adj%(60*60));
		break;
	case 11: // 日(L)
		adj=(adj/(256*24*60*60))*256*24*60*60+(dat*24*60*60)+(adj%(24*60*60));
		break;
	case 12: // 日(H)
		adj=(dat&1)*256*24*60*60+(adj%(256*24*60*60));
		break;
	}
	cur_time=now-adj;
}

void renderer_set_bibrate(bool bibrate)
{
}

word renderer_get_sensor(bool x_y)
{
	return (x_y?(now_sensor_x&0x0fff):(now_sensor_y&0x0fff));
}

int renderer_get_timer_state()
{
	return cur_time;
}

void renderer_set_timer_state(int timer)
{
	cur_time=timer;
}

void renderer_set_msg(const char msg[])
{
	render_msg_mode = 6;
	render_msg_time = sceKernelLibcClock();
	strcpy(render_msg, msg);
}

