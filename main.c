#include "main.h"
#include "rewind.h" //davex
//https://ameblo.jp/pspdevblog/theme8-10001513763.html

#define VERS    1
#define REVS    0

PSP_MODULE_INFO("Rin 1.32 RM", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

volatile int bSleep=0;
int bMenu=0;

char RinPath[MAX_PATH];
char RomPath[MAX_PATH];
char RomName[MAX_NAME];
char SavePath[MAX_PATH];
char CheatPath[MAX_PATH];

//davex
int save_period_frames = 0;
const int WAIT_MILIS = (70+70)*1000; 

void set_cpu_clock(int n)
{
	if(n==0)
		scePowerSetClockFrequency(222,222,111);
	else if(n==1)
		scePowerSetClockFrequency(266,266,133);
	else if(n==2)
		scePowerSetClockFrequency(333,333,166);
}

// -----------------------------------------------------------------------------

// ホームボタン終了時にコールバック
int exit_callback(void)
{
	bSleep=1;
	set_cpu_clock(0);
	save_config();
	if (rom_get_loaded() && rom_has_battery())
		save_sram(get_sram(), rom_get_info()->ram_size);
	
	free_rewind_states();//davex
	
	sceKernelExitGame();
	return 0;
}

// 電源スイッチ操作時や不定期にコールバック。
// この関数がまだ実行中でもサスペンド・スタンバイに入る可能性がある。
int power_callback(int unknown, int pwrflags)
{
	//if(pwrflags & (POWER_CB_SUSPEND|POWER_CB_STANDBY)){
	if(pwrflags & PSP_POWER_CB_POWER_SWITCH){
		if (!bSleep){
			bSleep=1;

			// ファイルアクセス中にサスペンド・スタンバイされて
			// 0byteのセーブファイルができてしまうことがあるので、
			// 書き込み中はサスペンド・スタンバイを無効化。
			sceKernelPowerLock(0);
			set_cpu_clock(0);
			save_config();
			if (rom_get_loaded() && rom_has_battery())
				save_sram(get_sram(), rom_get_info()->ram_size);
			sceKernelPowerUnlock(0);
		}
	}
	if(pwrflags & PSP_POWER_CB_BATTERY_LOW){
		//renderer_set_msg("PSP Battery is Low!");
		if (!bSleep){
			bSleep=1;

			sceKernelPowerLock(0);
			set_cpu_clock(0);
			save_config();
			if (rom_get_loaded() && rom_has_battery())
				save_sram(get_sram(), rom_get_info()->ram_size);
			sceKernelPowerUnlock(0);
			
			// 強制サスペンド。
			// バッテリが約10%を切りパワーランプが点滅を始めると、
			// 動作が極端に遅くなりフリーズしたりセーブできなくなったりする。
			// 市販ゲームでは0%まで使えてるようなのが謎。
			scePowerRequestSuspend(); 
		}
	}
	if(pwrflags & PSP_POWER_CB_RESUME_COMPLETE){
		bSleep=0;
	}

	// コールバック関数の再登録
	// （一度呼ばれたら再登録しておかないと次にコールバックされない）
//	int cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
//	scePowerRegisterCallback(0, cbid);
    return 0;
}

// ポーリング用スレッド
int CallbackThread(int args, void *argp)
{
	int cbid;
	
	// コールバック関数の登録
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
	scePowerRegisterCallback(0, cbid);
	
	// ポーリング
	sceKernelSleepThreadCB();

	return 0;
}

int SetupCallbacks(void)
{
	int thid = 0;
	
	// ポーリング用スレッドの生成
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	
	return thid;
}

// -----------------------------------------------------------------------------

void mainloop(void)
{
#ifdef DEBUG	
	unsigned long lastclock=sceKernelLibcClock();
	unsigned long lasttick=lastclock;
#endif

	unsigned long framecount=0;
	const unsigned int sync_time=16666;
	unsigned long cur_time = sceKernelLibcClock();
	unsigned long cur_time_bak = cur_time;
	unsigned long prev_time = cur_time;
	unsigned long next_time = cur_time + sync_time;
	unsigned long waitfc=0;
	int line, turbo_bak=0;
	
	ctrl_data_t paddata;// davex: for rewind checking
//	allocate_rewind_states();
	save_period_frames = 10;

	for(;;) {
		for(line=0; line<154; line++)
			gb_run();
		
		cur_time = sceKernelLibcClock();
#ifdef DEBUG
		
		if (framecount>=60) {
			unsigned long l;

			//フレームレートの指標。60フレームでかかった時間(usec)を１６進で表示。
			//フルフレームで0x000f4240となり、大きいと遅いことになる。解像度が悪いのは勘弁。 - LCK
			framecount=0;
			
			pgcLocate(50,0);
			pgcPuthex8(cur_time-lasttick);
			lasttick=cur_time;
			
			pgcLocate(50,2);
			pgcPuthex8(c_regs_PC);
			l=(cpu_read(c_regs_PC)<<24)+(cpu_read(c_regs_PC+1)<<16)+(cpu_read(c_regs_PC+2)<<8)+(cpu_read(c_regs_PC+3));
			pgcLocate(50,3);
			pgcPuthex8(l);

			pgcLocate(50,5);
			pgcPuthex8(cur_time-lastclock);
			lastclock=cur_time;

			pgcLocate(50,25);
			pgcPuthex8(g_regs.IF);
			pgcLocate(50,26);
			pgcPuthex8(g_regs.IE);
			extern byte c_regs_I;
			pgcLocate(50,27);
			pgcPuthex8(c_regs_I);
			
			// kmg
			pgcLocate(2, 2);
			pgcPuthex8(paddata.analog[CTRL_ANALOG_X]);
			pgcLocate(2, 3);
			pgcPuthex8(paddata.analog[CTRL_ANALOG_Y]);
		}
		pgScreenFlip();
#else

		framecount++;
		
		//davex
		if( framecount ==  save_period_frames){
			save_rewind_state();
			framecount = 0;
		}


		if (bTurbo){
			turbo_bak = 1;
			skip++;
			if (skip > 9){
				skip = 0;
				prev_time = cur_time;
			}
		}else if (cur_time < cur_time_bak){
			prev_time = cur_time;
			skip=0;
		}else if (cur_time > next_time){
			skip++;
			if(skip > setting.frameskip){
				skip=0;
				if(setting.vsync){
					sceDisplayWaitVblank();
					cur_time = sceKernelLibcClock();
				}
				prev_time = cur_time;
			}
		}else{
			if(setting.vsync){
				sceDisplayWaitVblank();
				cur_time = sceKernelLibcClock();
				prev_time = cur_time;
			}else{
				waitfc++;
				while(cur_time < prev_time+10000*(skip+1))
					cur_time = sceKernelLibcClock();
				if (!(waitfc&3)){
					while(cur_time < next_time)
						cur_time = sceKernelLibcClock();
				}
				prev_time = next_time;
			}
			skip=0;
		}
		cur_time_bak = cur_time;
		next_time = prev_time + sync_time * (skip+1);
		if (setting.vsync && !bTurbo){
			if (turbo_bak)
				turbo_bak = 0;
			else if (skip==0) 
				pgScreenFlip();
		}else{
			if (now_frame==0) pgScreenFlip();
		}
#endif
	

		//>>>davex: rewind check
		sceCtrlPeekBufferPositive(&paddata, 1);
		if( get_nShortcutKey(paddata.buttons) == 8 ){ // 8 == REWIND_SHORTCUT KEY
	
			wavout_enable=0; 
			
			while(1){
				
				//begin rewinds
				if( read_rewind_state() > 0 ){
					
					for(line=0; line<154; line++) //emulate a frame
						gb_run();
							
					pgScreenFlip();
				}
				
				sceKernelDelayThread(WAIT_MILIS);
				

				sceCtrlPeekBufferPositive(&paddata, 1);
				if( get_nShortcutKey(paddata.buttons) != 8 )
					break;
				
			}
			
			//continue normal emulation
			if(setting.sound) wavout_enable=1;
			cur_time = sceKernelLibcClock();
			prev_time = cur_time;
			next_time = cur_time + sync_time;
			skip=0;
		}
		//<<<
		
		// メニュー
		if(bMenu){
			wavout_enable=0;
			set_cpu_clock(0);
			rin_menu();
			set_cpu_clock(setting.cpu_clock);
			if(setting.sound) wavout_enable=1;

			cur_time = sceKernelLibcClock();
			prev_time = cur_time;
			next_time = cur_time + sync_time;
			skip=0;
			bMenu = 0;
		}
		
		// スリープ
		if(bSleep){
			wavout_enable=0;
			while(bSleep)
				pgWaitV();
			set_cpu_clock(setting.cpu_clock);
			if(setting.sound) wavout_enable=1;

			cur_time = sceKernelLibcClock();
			prev_time = cur_time;
			next_time = cur_time + sync_time;
			skip=0;
		}
	}
}

int main(int argc, char *argv[])
{
	int romsize, ramsize;
	char *p, tmp[MAX_PATH];
	
	pgInit();

	strcpy(RinPath, argv[0]);
	p = strrchr(RinPath, '/');
	*++p = 0;
	sprintf(CheatPath, "%sCHEAT/", RinPath);

	SetupCallbacks();
	pgScreenFrame(2,0);
	wavoutInit();
    max_rewind_memory = establish_max_rewind_memory();

	load_config();
	bBitmap = load_menu_bg();
	if(bBitmap) bgbright_change();
	if(setting.sound) wavout_enable=1;

	strcpy(tmp,RinPath);
	strcat(tmp,"SAVE");
	sceIoMkdir(tmp,0777);
	strcpy(tmp,RinPath);
	strcat(tmp,"CHEAT");
	sceIoMkdir(tmp,0777);

	gb_init();
	strcpy(RomPath,setting.lastpath);
	for(;;){
		if (!getFilePath(RomPath,EXT_GB|EXT_GZ|EXT_ZIP))
			continue;
		strcpy(tmp, RomPath);
		*(strrchr(tmp,'/')+1) = 0;
		strcpy(setting.lastpath, tmp);

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

		showMsgAboutLoadedRom();

		break;
	}

	pgFillvram(0);
	pgScreenFlipV();
	pgFillvram(0);
	pgScreenFlipV();

	set_cpu_clock(setting.cpu_clock);

	mainloop();

	return 0;
}