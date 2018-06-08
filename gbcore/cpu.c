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

//------------------------------------------------
// CPU ニーモニック以外実装部 (I/O､IRQ 等)

#include "gb.h"
#include "_bit_table.h"

#define Z_FLAG 0x40
#define H_FLAG 0x10
#define N_FLAG 0x02
#define C_FLAG 0x01

/////////////////////////////////////////
//
// メンバ変数
//
/////////////////////////////////////////

//structだとgp相対が使えないみたいなので、普通の変数に変更。関連の修正多めなので注意。 - LCK
struct cpu_regs _c_regs;
word c_regs_AF;
word c_regs_BC;
word c_regs_DE;
word c_regs_HL;
word c_regs_SP;
word c_regs_PC;
byte c_regs_I;

byte ram[0x2000*4];
byte vram[0x2000*2];
byte stack[0x80];
byte oam[0xA0];
byte spare_oam[0x18];
byte ext_mem[16];

byte *vram_bank;
byte *ram_bank;

byte z802gb[256],gb2z80[256];
//	word org_pal[16][4];
int total_clock,rest_clock,sys_clock,seri_occer,div_clock;
bool halt,speed,speed_change,dma_executing;
bool b_trace;
int dma_src;
int dma_dest;
int dma_rest;
int gdma_rest;
bool b_dma_first;

int last_int;
volatile bool int_disable_next;
volatile bool int_invoke_next;

byte *dma_src_bank;
byte *dma_dest_bank;

byte _ff6c,_ff72,_ff73,_ff74,_ff75;

int rp_fd=-1;
unsigned long rp_time;
byte rp_data;

/////////////////////////////////////////
//
// メンバ関数
//
/////////////////////////////////////////
void cpu_init(void)
{
	int i;

	b_trace=false;

	for (i=0;i<256;i++){
		z802gb[i]=((i&0x40)?0x80:0)|((i&0x10)?0x20:0)|((i&0x02)?0x40:0)|((i&0x01)?0x10:0);
		gb2z80[i]=((i&0x80)?0x40:0)|((i&0x40)?0x02:0)|((i&0x20)?0x10:0)|((i&0x10)?0x01:0);
	}
	
	cpu_reset();
}

void cpu_reset(void)
{
	c_regs_AF=(rom_get_info()->gb_type>=3)?0x11b0:0x01b0;
	c_regs_BC=(rom_get_info()->gb_type>=4)?0x0113:0x0013;
	c_regs_DE=0x00D8;
	c_regs_HL=0x014D;
	c_regs_I=0;
	c_regs_SP=0xFFFE;
	c_regs_PC=0x100;

	vram_bank=vram;
	ram_bank=ram+0x1000;

	rest_clock=0;
	total_clock=sys_clock=div_clock=0;
	seri_occer=0x7fffffff;
	halt=false;
	speed=false;
	speed_change=false;
	dma_executing=false;
	b_dma_first=false;
	gdma_rest=0;

	last_int=0;
	int_disable_next=false;
	int_invoke_next=false;

	memset(ram,0,sizeof(ram));
	memset(vram,0,sizeof(vram));
	memset(stack,0,sizeof(stack));
	memset(oam,0,sizeof(oam));
	memset(spare_oam,0,sizeof(spare_oam));
	
	if(rp_fd >= 0){
		sceIoClose(rp_fd);
		rp_fd = -1;
	}
	rp_time = 0;
}

void cpu_save_state(int *dat)
{
	dat[0]=(ram_bank-ram)/0x1000;
	dat[1]=(vram_bank-vram)/0x2000;

	dat[2]=(speed?1:0);
	dat[3]=(dma_executing?1:0);
	dat[4]=dma_src;
	dat[5]=dma_dest;
	dat[6]=dma_rest;
	dat[7]=(speed_change?1:0);
}

void cpu_save_state_ex(int *dat)
{
	dat[0]=div_clock;
	dat[1]=rest_clock;
	dat[2]=sys_clock;
	dat[3]=total_clock;
}

#include "cpu_io.h"

void cpu_restore_state(int *dat)
{
	ram_bank=ram+dat[0]*0x1000;
	vram_bank=vram+dat[1]*0x2000;

	speed=(dat[2]?true:false);
	dma_executing=(dat[3]?true:false);
	dma_src=dat[4];
	dma_dest=dat[5];
	dma_rest=dat[6];
	speed_change=(dat[7]?true:false);
}

void cpu_restore_state_ex(int *dat)
{
	div_clock=dat[0];
	rest_clock=dat[1];
	sys_clock=dat[2];
	total_clock=dat[3];
}

byte cpu_read_direct_ord(word adr)
{
	switch(adr>>13){
	case 0:
	case 1:
		return get_rom()[adr];//ROM領域
	case 2:
	case 3:
		return mbc_get_rom()[adr];//バンク可能ROM
	case 4:
		return vram_bank[adr&0x1FFF];//8KBVRAM
	case 5:
		if (mbc_is_ext_ram())
			return mbc_get_sram()[adr&0x1FFF];//カートリッジRAM
		else
			return mbc_ext_read(adr);
	case 6:
		if (adr&0x1000)
			return ram_bank[adr&0x0fff];
		else
			return ram[adr&0x0fff];
	case 7:
		if (adr<0xFE00){
			if (adr&0x1000)
				return ram_bank[adr&0x0fff];
			else
				return ram[adr&0x0fff];
		}
		else if (adr<0xFEA0)
			return oam[adr-0xFE00];//object attribute memory
		else if (adr<0xFF00)
			return spare_oam[(((adr-0xFFA0)>>5)<<3)|(adr&7)];
		else
			return cpu_io_read_JmpTbl[ adr & 0x00FF ]( adr ) ;
	}
	return 0;
}

//短くしてインラインにぶちこむ - LCK
//inline byte cpu_read_direct(word adr)
inline byte cpu_read(word adr)
{
	if ((adr&0x8000)==0) {
		return ((adr&0x4000)==0)?get_rom()[adr]:mbc_get_rom()[adr];
	} else if ((adr&0xe000)==0xc000) {
		return ((adr&0x1000)==0)?ram[adr&0xfff]:ram_bank[adr&0xfff];
//	} else if ((adr&0xe000)==0xa000) {
//		return (mbc_is_ext_ram())?mbc_get_sram()[adr&0x1FFF]:mbc_ext_read(adr);
	}
	return cpu_read_direct_ord(adr);
}

/*
inline byte cpu_read(word adr)
{
	//return cheat_map[adr]?cheat_read(adr):cpu_read_direct(adr);
	return cpu_read_direct(adr);
}
*/

inline word readw(word adr)
{
	return cpu_read(adr)|(cpu_read(adr+1)<<8);
}

inline void writew(word adr,word dat)
{
	cpu_write(adr,(byte)dat);
	cpu_write(adr+1,dat>>8);
}

inline byte op_read()
{
	return cpu_read(c_regs_PC++);
}

//こっちのほうが早いと思われ - LCK
inline word op_readw()
{
	word r=readw(c_regs_PC);
	c_regs_PC+=2;
	return r;
}

void cpu_write_direct_ord(word adr,byte dat)
{
	switch(adr>>13){
	case 0:
	case 1:
	case 2:
	case 3:
		mbc_write(adr,dat);
		break;
	case 4:
		vram_bank[adr&0x1FFF]=dat;
		break;
	case 5:
		if (mbc_is_ext_ram())
			mbc_get_sram()[adr&0x1FFF]=dat;//カートリッジRAM
		else
			mbc_ext_write(adr,dat);
		break;
	case 6:
		if (adr&0x1000)
			ram_bank[adr&0x0fff]=dat;
		else
			ram[adr&0x0fff]=dat;
		break;
	case 7:
		if (adr<0xFE00){
			if (adr&0x1000)
				ram_bank[adr&0x0fff]=dat;
			else
				ram[adr&0x0fff]=dat;
		}
		else if (adr<0xFEA0)
			oam[adr-0xFE00]=dat;
		else if (adr<0xFF00)
			spare_oam[(((adr-0xFFA0)>>5)<<3)|(adr&7)]=dat;
		else
			cpu_io_write_JmpTbl[ adr & 0x00FF ]( adr, dat ) ;
		break;
	}
}

//短くしてインラインにぶちこむ - LCK
inline void cpu_write_direct(word adr,byte dat)
{
	if ((adr&0xe000)==0xc000) {
		if (adr&0x1000)
			ram_bank[adr&0x0fff]=dat;
		else
			ram[adr&0x0fff]=dat;
	} else if ((adr&0xe000)==0xa000) {
		if (mbc_is_ext_ram())
			mbc_get_sram()[adr&0x1FFF]=dat;//カートリッジRAM
		else
			mbc_ext_write(adr,dat);
	} else {
		cpu_write_direct_ord(adr,dat);
	}
}

inline void cpu_write(word adr,byte dat)
{
	if (cheat_map[adr])
		dat = cheat_write(adr,dat);
	return cpu_write_direct(adr,dat);
}

/*
static int cycles[256] =
{
	4,12,8,8,4,4,8,4,4,12,8,8,4,4,8,4,
	8,12,8,8,4,4,8,4,8,12,8,8,4,4,8,4,
	8,12,8,8,4,4,8,4,8,12,8,8,4,4,8,4,
	8,12,8,8,12,12,12,4,8,12,8,8,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	8,8,8,8,8,8,4,8,4,4,4,4,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
	8,12,16,16,12,16,8,16,8,16,16,0,12,24,8,16,
	8,12,16,16,12,16,8,16,8,16,16,16,12,24,8,16,
	12,12,8,12,12,16,8,16,16,4,16,12,12,12,8,16,
	12,12,8,4,12,16,8,16,12,8,16,4,12,12,8,16
};
*/

static char cycles[256] =
{
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	 4,12, 8, 8, 4, 4, 8, 4,20, 8, 8, 8, 4, 4, 8, 4,//0
	 4,12, 8, 8, 4, 4, 8, 4,12, 8, 8, 8, 4, 4, 8, 4,//1
	 8,12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,//2
	 8,12, 8, 8,12,12,12, 4, 8, 8, 8, 8, 4, 4, 8, 4,//3
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//4
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//5
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//6
	 8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,//7
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//8
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//9
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//A
	 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,//B
	 8,12,12,16,12,16, 8,16, 8,16,12, 0,12,24, 8,16,//C
	 8,12,12, 0,12,16, 8,16, 8,16,12, 0,12, 0, 8,16,//D
	12,12, 8, 0, 0,16, 8,16,16, 4,16, 0, 0, 0, 8,16,//E
	12,12, 8, 4, 0,16, 8,16,12, 8,16, 4, 0, 0, 8,16 //F
};

/*
static int cycles[256] =
{
   4,12, 8, 8, 4, 4, 8, 4,20, 8, 8, 8, 4, 4, 8, 4,
   4,12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,
   8,12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,
   8,12, 8, 8,12,12,12, 4, 8, 8, 8, 8, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,
   8,12,12,12,12,16, 8,32, 8, 8,12, 0,12,12, 8,32,
   8,12,12, 0,12,16, 8,32, 8, 8,12, 0,12, 0, 8,32,
  12,12, 8, 0, 0,16, 8,32,16, 4,16, 0, 0, 0, 8,32,
  12,12, 8, 4, 0,16, 8,32,12, 8,16, 4, 0, 0, 8,32
};
*/

static char cycles_cb[256] =
{
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
   8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
   8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
   8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8
};

/*static int cycles_cb[256] =
{
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
   8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8
};
*/
/*
static const byte ZTable[256] =
{
  Z_FLAG,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};*/


//#define Z_FLAG 0x40
//d==0の時に0x40を返す。それ以外は０を返す。
static inline byte GenZF(byte d)
{
	byte ret;
	asm (
		"		sltiu	%0,%1,1 "				"\n"
		"		sll		%0,%0,6 "				"\n"
			:	"=&r" (ret)		// %0
			:	"r" (d)			// %1
	);
	return ret;
}


byte cpu_seri_send(byte dat)
{
//	if ((!(g_regs.IE&INT_SERIAL))||(g_regs.IF&INT_SERIAL))
//		return 0xFF;

	if ((g_regs.SC&0x81)==0x80){
//		fprintf(file,"seri_recv my:%02X tar:%02X state SC:%02X\n",g_regs.SB,dat,g_regs.SC);
		byte ret=g_regs.SB;
		g_regs.SB=dat;
		g_regs.SC&=1;
		cpu_irq(INT_SERIAL);
		return ret;
	}
	else
		return 0xFF;
}

void cpu_irq(int irq_type)
{
//	fprintf(file,"irq %02X LCDC %02X\n",irq_type,g_regs.LCDC);
	if (!((irq_type==INT_VBLANK||irq_type==INT_LCDC)&&(!(g_regs.LCDC&0x80))))
//		if (last_int!=irq_type)
			g_regs.IF|=(irq_type/*&g_regs.IE*/);
//	g_regs.IF|=irq_type;
	cpu_irq_check();
}

//g_regs.IF, g_regs.IE, c_regs_I, halt, int_disable_next が変更された時に呼ぶこと。これでフラグをつくって、メインループで見る - LCK
void cpu_irq_check()
{
	int_invoke_next=(((g_regs.IF&g_regs.IE)&&(c_regs_I||halt)) || int_disable_next);	//割りこみがかかる時+disable_next
}

//割り込みがかかったら積算clockもいじるように変更
void cpu_irq_process()
{
	if (int_disable_next){
		int_disable_next=false;
		cpu_irq_check();
		return;
	}

	if (!((g_regs.IF&g_regs.IE)&&(c_regs_I||halt))) {
		cpu_irq_check();
		return;
	}

	if (halt)
		c_regs_PC++;

	cpu_write(c_regs_SP-2,c_regs_PC&0xFF);
	cpu_write(c_regs_SP-1,(c_regs_PC>>8));
	c_regs_SP-=2;
	
	if (g_regs.IF&g_regs.IE&INT_VBLANK){//VBlank
		c_regs_PC=0x40;
		g_regs.IF&=0xFE;
		last_int=INT_VBLANK;
	}
	else if (g_regs.IF&g_regs.IE&INT_LCDC){//LCDC
		c_regs_PC=0x48;
		g_regs.IF&=0xFD;
		last_int=INT_LCDC;
	}
	else if (g_regs.IF&g_regs.IE&INT_TIMER){//Timer
		c_regs_PC=0x50;
		g_regs.IF&=0xFB;
		last_int=INT_TIMER;
	}
	else if (g_regs.IF&g_regs.IE&INT_SERIAL){//Serial
		c_regs_PC=0x58;
		g_regs.IF&=0xF7;
		last_int=INT_SERIAL;
	}
	else if (g_regs.IF&g_regs.IE&INT_PAD){//Pad
		c_regs_PC=0x60;
		g_regs.IF&=0xEF;
		last_int=INT_PAD;
	}
	else {}

	halt=false;
	c_regs_I=0;
//		g_regs.IF=0;

	cpu_irq_check();
	
	//割り込みがかかったのでステートを足す
	//13stateは暫定値。GBでどうなってるかは知らない。
	//よくわからんのでsys_clockは足してないが入れてもいいかも - LCK
//	rest_clock-=13;
//	div_clock+=13;
//	total_clock+=13;

	return;
}

int tmp_clocks;
#include "op_cb.h"
#include "op_normal.h"

void cpu_exec(int clocks)
{
	int op_code;
	static const int timer_clocks[]={1024,16,64,256};

	if (speed)
		clocks*=2;

	rest_clock+=clocks;

	if (gdma_rest){
		if (rest_clock<=gdma_rest){
			gdma_rest-=rest_clock;
			sys_clock+=rest_clock;
			div_clock+=rest_clock;
			total_clock+=rest_clock;
			rest_clock=0;
		}
		else{
			rest_clock-=gdma_rest;
			sys_clock+=gdma_rest;
			div_clock+=gdma_rest;
			total_clock+=gdma_rest;
			gdma_rest=0;
		}
	}

	while(rest_clock>0){
		if (int_invoke_next) cpu_irq_process();

		op_code=op_read();
		tmp_clocks=cycles[op_code];

		op_code_normal_JmpTbl[op_code]();

		rest_clock-=tmp_clocks;
		div_clock+=tmp_clocks;
		total_clock+=tmp_clocks;

		if (g_regs.TAC&0x04){//タイマ割りこみ
			sys_clock+=tmp_clocks;
			if (sys_clock>timer_clocks[g_regs.TAC&0x03]){
				sys_clock&=timer_clocks[g_regs.TAC&0x03]-1;
				g_regs.TIMA++;
				if (!g_regs.TIMA){
					cpu_irq(INT_TIMER);
					g_regs.TIMA=g_regs.TMA;
				}
			}
		}

		if (div_clock&0x100){
			g_regs.DIV-=div_clock>>8;
			div_clock&=0xff;
		}

		if (total_clock>seri_occer){
			seri_occer=0x7fffffff;
/*			if (ref_gb->get_target()){
				byte ret=ref_gb->get_target()->get_cpu()->seri_send(g_regs.SB);
				g_regs.SB=ret;
				g_regs.SC&=3;
			}
			else*/{
				if (hook_ext){ // フックします
					byte ret=hook_proc.send(g_regs.SB);
					g_regs.SB=ret;
					g_regs.SC&=3;
				}
				else{
					g_regs.SB=0xff;
					g_regs.SC&=3;
				}
			}
			cpu_irq(INT_SERIAL);
		}
	}
}

void cpu_set_trace(bool trace) { b_trace=trace; }
byte *cpu_get_vram() { return vram; }
byte *cpu_get_ram() { return ram; }
byte *cpu_get_oam() { return oam; }
byte *cpu_get_stack() { return stack; }
byte *cpu_get_ram_bank() { return ram_bank; }
void cpu_set_ram_bank(int bank) { ram_bank=ram+bank*0x1000; }
//struct cpu_regs *cpu_get_regs() { return &c_regs; }
int cpu_get_clock() { return total_clock; }
bool cpu_get_speed() { return speed; }
bool *cpu_get_halt() { return &halt; }

struct cpu_regs *cpu_get_c_regs() 
{ 
	_c_regs.AF.w=c_regs_AF;
	_c_regs.BC.w=c_regs_BC;
	_c_regs.DE.w=c_regs_DE;
	_c_regs.HL.w=c_regs_HL;
	_c_regs.SP  =c_regs_SP;
	_c_regs.PC  =c_regs_PC;
	_c_regs.I   =c_regs_I ;
	return &_c_regs; 
}

struct cpu_regs *cpu_set_c_regs() 
{ 
	c_regs_AF=_c_regs.AF.w;
	c_regs_BC=_c_regs.BC.w;
	c_regs_DE=_c_regs.DE.w;
	c_regs_HL=_c_regs.HL.w;
	c_regs_SP=_c_regs.SP  ;
	c_regs_PC=_c_regs.PC  ;
	c_regs_I =_c_regs.I   ;
	cpu_irq_check(); 
	return &_c_regs; 
}

