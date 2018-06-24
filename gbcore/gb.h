/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef GB_H
#define GB_H

#include <stdio.h>
#include <string.h>
#include "gb_types.h"
//#include "syscall.h"
#include <pspiofilemgr.h>
#include "../renderer.h"
#include <zlib.h>

#define INT_VBLANK 1
#define INT_LCDC 2
#define INT_TIMER 4
#define INT_SERIAL 8
#define INT_PAD 16

struct ext_hook{
	byte (*send)(byte);
	bool (*led)(void);
};

typedef struct {
	bool enable;
	byte code;
	word adr;
	byte dat;
	byte dat_old;
	char name[255];
} cheat_dat;

struct gb_regs {
	byte P1,SB,SC,DIV,TIMA,TMA,TAC,IF,LCDC,STAT,SCY,SCX,LY,LYC,DMA,BGP,OBP1,OBP2,WY,WX,IE;
};

struct gbc_regs {
	byte KEY1,VBK,HDMA1,HDMA2,HDMA3,HDMA4,HDMA5,RP,BCPS,BCPD,OCPS,OCPD,SVBK;
};

union pare_reg {
	word w;
	struct{byte l,h;}b;
};

struct cpu_regs {
	union pare_reg AF;
	union pare_reg BC;
	union pare_reg DE;
	union pare_reg HL;
	word SP;
	word PC;
	byte I;
};


struct apu_stat{
	bool sq1_playing;
	int sq1_sw_time;
	int sq1_sw_dir;
	int sq1_sw_shift;

	int sq1_len;
	int sq1_init_len;
	int sq1_type;

	int sq1_vol;
	int sq1_init_vol;
	int sq1_env_dir;
	int sq1_env_speed;

	int sq1_freq;
	int sq1_init_freq;

	int sq1_hold;


	bool sq2_playing;

	int sq2_len;
	int sq2_init_len;
	int sq2_type;

	int sq2_vol;
	int sq2_init_vol;
	int sq2_env_dir;
	int sq2_env_speed;

	int sq2_freq;
	int sq2_init_freq;

	int sq2_hold;


	bool wav_playing;
	int wav_vol;
	int wav_freq;
	int wav_init_freq;
	int wav_init_len;
	int wav_len;
	int wav_hold;


	bool noi_playing;
	int noi_len;
	int noi_init_len;

	int noi_vol;
	int noi_init_vol;
	int noi_env_dir;
	int noi_env_speed;

	int noi_freq;
	int noi_init_freq;
	int noi_hold;
	int noi_step;

	int master_enable;
	int ch_enable[4][2];
	int master_vol[2];
	int ch_on[4];
	int wav_enable;
};

struct apu_que {
	word adr;
	byte dat;
	int clock;
};

struct rom_info {
	char cart_name[18];
	int cart_type;
	byte rom_size;
	byte ram_size;

	bool check_sum;
	int gb_type;
};

// gb.c
size_t gb_save_state(byte *out);
void gb_restore_state(gzFile fd, const byte *buf);

void gb_init(void);
void gb_reset(void);
void gb_hook_extport(struct ext_hook *ext);
void gb_unhook_extport();
void gb_set_skip(int frame);
bool gb_load_rom(byte *buf,int size,byte *ram,int ram_size);
void gb_refresh_pal();
void gb_run();
void gb_fill_vframe(word color);

extern word vframe_mem[];
extern word *vframe;
extern struct gb_regs g_regs;
extern struct gbc_regs cg_regs;
extern struct ext_hook hook_proc;
extern bool hook_ext;
extern bool use_gba;
extern int now_gb_mode;
extern int skip, now_frame;

// lcd.c
#ifdef USE_GPU
#define SIZE_LINE    256		//in pixel
#define GUARD_LINE   48			//in pixel
#else
#define SIZE_LINE    176		//in pixel
#define GUARD_LINE   8			//in pixel
#endif

enum{
	PAL_MONOCHROME,
	PAL_BRIGHT_SEPIA,
	PAL_RED,
	PAL_DARK_SEPIA,
	PAL_PASTEL_COLOR,
	PAL_ORANGE,
	PAL_YELLOW,
	PAL_BRIGHT_BLUE,
	PAL_DARK_BLUE,
	PAL_GRAY,
	PAL_BRIGHT_GREEN,
	PAL_DARK_GREEN,
	PAL_REVERSE,
	PAL_SGB,
};
extern const char *pal_names[];

void lcd_init(void);
void lcd_set_mpal(int n);
int lcd_get_mpal();
void lcd_render(void *buf,int scanline);
void lcd_reset();
void lcd_clear_win_count();
word *lcd_get_pal(int num);
void lcd_set_enable(int layer,bool enable);
bool lcd_get_enable(int layer);
int lcd_get_sprite_count();
void lcd_bg_render(void *buf,int scanline);
void lcd_win_render(void *buf,int scanline);
void lcd_sprite_render(void *buf,int scanline);
void lcd_bg_render_color(void *buf,int scanline);
void lcd_win_render_color(void *buf,int scanline);
void lcd_sprite_render_color(void *buf,int scanline);

// mbc.c
extern void (*mbc_write)(word adr,byte dat);

void mbc_init();

//byte *mbc_get_rom();
//byte *mbc_get_sram();
//bool mbc_is_ext_ram();
//void set_ext_is(bool ext);

int mbc_get_state();
void mbc_set_state(int dat);
void mbc_set_page(int arg_rom,int arg_sram);

byte mbc_read(word adr);
void mbc_set_write();
//void mbc_write(word adr,byte dat);
byte mbc_ext_read(word adr);
void mbc_ext_write(word adr,byte dat);
void mbc_reset();

void mbc_nop(word adr,byte dat);
void mbc_mbc1_write(word adr,byte dat);
void mbc_mbc2_write(word adr,byte dat);
void mbc_mbc3_write(word adr,byte dat);
void mbc_mbc5_write(word adr,byte dat);
void mbc_mbc7_write(word adr,byte dat);
void mbc_huc1_write(word adr,byte dat);
void mbc_huc3_write(word adr,byte dat);
void mbc_tama5_write(word adr,byte dat);
void mbc_mmm01_write(word adr,byte dat);

// cpu.c
extern byte vram[];
extern struct cpu_regs c_regs;
extern byte ram[];
extern byte oam[];

extern byte *vram_bank;
extern byte *ram_bank;
extern bool halt,speed,speed_change,dma_executing;
extern int dma_src;
extern int dma_dest;
extern int dma_rest;
extern int gdma_rest;
extern bool b_dma_first;
extern byte *dma_src_bank;
extern byte *dma_dest_bank;

void cpu_init(void);
void cpu_reset(void);
void cpu_save_state(int *dat);
void cpu_save_state_ex(int *dat);
void cpu_restore_state(int *dat);
void cpu_restore_state_ex(int *dat);
inline byte cpu_read_direct(word adr);
inline byte cpu_read(word adr);
inline word readw(word adr);
inline void writew(word adr,word dat);
inline byte op_read();
inline word op_readw();
inline void cpu_write(word adr,byte dat);
inline void cpu_write_direct(word adr,byte dat);
byte cpu_io_read(word adr);
void cpu_io_write(word adr,byte dat);
void cpu_irq(int irq_type);
void cpu_irq_check();
void cpu_irq_process();
void cpu_exec(int clocks);

void cpu_set_trace(bool trace);
byte *cpu_get_vram();
byte *cpu_get_ram();
byte *cpu_get_oam();
byte *cpu_get_stack();
byte *cpu_get_ram_bank();
void cpu_set_ram_bank(int bank);
struct cpu_regs *cpu_get_regs();
int cpu_get_clock();
bool cpu_get_speed();
bool *cpu_get_halt();

struct cpu_regs *cpu_get_c_regs();
struct cpu_regs *cpu_set_c_regs();

// rom.c
#define MAX_ROM_SIZE 8*1024*1024

extern byte rom_image[MAX_ROM_SIZE];
extern byte sram_space[16*0x2000+4];
extern int sgb_mode, org_gbtype;

void rom_init();
bool rom_has_battery();
int rom_get_sram_size();
bool rom_load_rom(byte *buf,int size,byte *ram,int ram_size);
//struct rom_info *rom_get_info();
//byte *get_rom();
//byte *get_sram();
bool rom_get_loaded();
void rom_set_first(int page);

// apu.c
void apu_init();
void apu_reset();
byte apu_read(word adr);
void apu_write(word adr,byte dat,int clock);
void apu_update();
struct apu_stat *apu_get_stat_cpu();
struct apu_stat *apu_get_stat_gen();
byte *apu_get_mem();

void snd_init();
void snd_reset();
void snd_set_enable(int ch,bool enable);
bool snd_get_enable(int ch);

void snd_process(word adr,byte dat);
void snd_update();

inline short snd_sq1_produce(int freq);
inline short snd_sq2_produce(int freq);
inline short snd_wav_produce(int freq);
inline short snd_noi_produce(int freq);
void snd_render(short *buf,int sample);

// cheat.c
extern byte cheat_map[0x10000];
extern cheat_dat cheats[];
extern int nCheats;

void cheat_decreate_cheat_map();
void cheat_create_cheat_map();
int cheat_load(FILE *file);
byte cheat_read(word adr);
void cheat_init();
byte cheat_write(word adr, byte dat);

// sgb.c
extern int bit_received;
extern int bits_received;
extern int packets_received;
extern int sgb_state; 
extern int sgb_index;
extern int sgb_multiplayer;
extern int sgb_fourplayers;
extern int sgb_nextcontrol;
extern int sgb_readingcontrol;
extern int sgb_mask;
extern unsigned short sgb_palette[8*16];
extern unsigned short sgb_palette_memory[512*4];
extern byte sgb_buffer[16*7];
extern byte sgb_ATF[18*20];
extern byte sgb_ATF_list[45 * 20 * 18];
extern byte sgb_border[2048];
extern byte sgb_borderchar[32*256];
extern unsigned short sgb_border_buffer[256*224];
extern int border_uploaded;

void sgb_transfer(byte bdata);
void sgb_init();
void sgb_reset_state();
void sgb_reset();


extern struct rom_info info;
extern byte *sram;
extern byte *first_page;
extern byte *rom_page;
extern byte *sram_page;
extern bool ext_is_ram;

static inline struct rom_info *rom_get_info()
{
	return &info;
}

static inline byte *get_rom()
{
	return first_page;
}

static inline byte *get_sram()
{
	return sram;
}


static inline byte *mbc_get_rom()
{
	return rom_page;
}

static inline byte *mbc_get_sram()
{
	return sram_page;
}

static inline bool mbc_is_ext_ram()
{
	return ext_is_ram;
}

static inline void mbc_set_ext_is(bool ext)
{
	ext_is_ram=ext;
}

extern word c_regs_PC;

#endif
