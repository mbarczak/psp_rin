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

//--------------------------------------------------
// APU(PSG?)エミュレーション部 (レジスタ/波形生成)

#define UPDATE_INTERVAL 172 // 1/256秒あたりのサンプル数
#define CLOKS_PER_INTERVAL 16384 // 1/256秒あたりのクロック数 (4MHz時)

#include "gb.h"

static dword sq1_cur_pos=0;
static dword sq2_cur_pos=0;
static dword wav_cur_pos=0;
static dword noi_cur_pos=0;

struct apu_stat snd_stat;
struct apu_stat snd_stat_cpy,snd_stat_tmp;
struct apu_que snd_write_que[0x10000];
int snd_que_count;
int snd_bef_clock;
bool snd_b_lowpass;
byte snd_mem[0x100];
bool snd_b_enable[4];


void apu_init()
{
	snd_init();
	apu_reset();
}

void apu_reset()
{
	snd_reset();
}

byte apu_read(word adr)
{
	if (adr==0xff26)
		return (!snd_stat.master_enable)?0x00:
				(0x80|(((snd_stat.sq1_playing&&snd_stat.wav_vol)?1:0)|
					((snd_stat.sq2_playing&&snd_stat.wav_vol)?2:0)|
					((snd_stat.wav_enable&&snd_stat.wav_playing&&snd_stat.wav_vol)?4:0)|
					((snd_stat.noi_playing&&snd_stat.noi_vol)?8:0)));
	else
		return snd_mem[adr-0xff10];
}

void apu_write(word adr,byte dat,int clock)
{
//	static int bef_clock=clock;
	int bef_clock=clock;
	static int clocks=0;

	//snd_mem[adr-0xFF10]=dat;

	snd_write_que[snd_que_count].adr=adr;
	snd_write_que[snd_que_count].dat=dat;
	snd_write_que[snd_que_count++].clock=clock;

	if (snd_que_count>=0x10000)
		snd_que_count=0xffff;

	snd_process(adr,dat);

	if (bef_clock>clock)
		bef_clock=clock;

	clocks+=clock-bef_clock;

	while (clocks>CLOKS_PER_INTERVAL*(cpu_get_speed()?2:1)){
		if( snd_stat.master_enable )
			snd_update();
		clocks-=CLOKS_PER_INTERVAL*(cpu_get_speed()?2:1);
	}

	bef_clock=clock;
}

void apu_update()
{
}

struct apu_stat *apu_get_stat_cpu()
{
	return &snd_stat;
}

struct apu_stat *apu_get_stat_gen()
{
	return &snd_stat_cpy;
}

byte *apu_get_mem()
{
	return snd_mem;
}

//---------------------------------------------------------------------

void snd_init()
{
	snd_b_enable[0]=snd_b_enable[1]=snd_b_enable[2]=snd_b_enable[3]=true;
	snd_b_lowpass=false;
}

void snd_reset()
{
	snd_que_count=0;
	snd_bef_clock=0;
	memset(&snd_stat,0,sizeof(snd_stat));
	snd_stat.sq1_playing=false;
	snd_stat.sq2_playing=false;
	snd_stat.wav_playing=false;
	snd_stat.noi_playing=false;
	snd_stat.ch_enable[0][0]=snd_stat.ch_enable[0][1]=snd_stat.ch_enable[1][0]=snd_stat.ch_enable[1][1]=
		snd_stat.ch_enable[2][0]=snd_stat.ch_enable[2][1]=snd_stat.ch_enable[3][0]=snd_stat.ch_enable[3][1]=1;
	snd_stat.ch_on[0]=snd_stat.ch_on[1]=snd_stat.ch_on[2]=snd_stat.ch_on[3]=1;
	snd_stat.master_enable=1;
	snd_stat.master_vol[0]=snd_stat.master_vol[1]=7;

	memcpy(&snd_stat_cpy,&snd_stat,sizeof(snd_stat));

	byte gb_init_wav[]={0x06,0xFE,0x0E,0x7F,0x00,0xFF,0x58,0xDF,0x00,0xEC,0x00,0xBF,0x0C,0xED,0x03,0xF7};
	byte gbc_init_wav[]={0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF};

	if (rom_get_info()->gb_type<=2) // 初期型GB & SGB
		memcpy(snd_mem+20,gb_init_wav,16);
	else if (rom_get_info()->gb_type>=3) // GBC
		memcpy(snd_mem+20,gbc_init_wav,16);
}

void snd_set_enable(int ch,bool enable)
{
	snd_b_enable[ch]=enable;
}

bool snd_get_enable(int ch)
{
	return snd_b_enable[ch];
}

//extern FILE *file;

void snd_process(word adr,byte dat)
{
	const char tb[]={0,4,2,1};

	/*
	const char mul_t[]={2,1,1,1,1,1,1,1};
	const char div_t[]={1,1,2,3,4,5,6,7};
	const short div_t2[]={2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,1,1};
	*/
	const int noi_freq_tbl[] = 
	{
		0x00080000,0x00040000,0x00020000,0x00015555,0x00010000,0x0000CCCC,0x0000AAAA,0x00009249,
		0x00080000,0x00040000,0x00020000,0x00015555,0x00010000,0x0000CCCC,0x0000AAAA,0x00009249,
		0x00040000,0x00020000,0x00010000,0x0000AAAA,0x00008000,0x00006666,0x00005555,0x00004924,
		0x00040000,0x00020000,0x00010000,0x0000AAAA,0x00008000,0x00006666,0x00005555,0x00004924,
		0x00020000,0x00010000,0x00008000,0x00005555,0x00004000,0x00003333,0x00002AAA,0x00002492,
		0x00020000,0x00010000,0x00008000,0x00005555,0x00004000,0x00003333,0x00002AAA,0x00002492,
		0x00010000,0x00008000,0x00004000,0x00002AAA,0x00002000,0x00001999,0x00001555,0x00001249,
		0x00010000,0x00008000,0x00004000,0x00002AAA,0x00002000,0x00001999,0x00001555,0x00001249,
		0x00008000,0x00004000,0x00002000,0x00001555,0x00001000,0x00000CCC,0x00000AAA,0x00000924,
		0x00008000,0x00004000,0x00002000,0x00001555,0x00001000,0x00000CCC,0x00000AAA,0x00000924,
		0x00004000,0x00002000,0x00001000,0x00000AAA,0x00000800,0x00000666,0x00000555,0x00000492,
		0x00004000,0x00002000,0x00001000,0x00000AAA,0x00000800,0x00000666,0x00000555,0x00000492,
		0x00002000,0x00001000,0x00000800,0x00000555,0x00000400,0x00000333,0x000002AA,0x00000249,
		0x00002000,0x00001000,0x00000800,0x00000555,0x00000400,0x00000333,0x000002AA,0x00000249,
		0x00001000,0x00000800,0x00000400,0x000002AA,0x00000200,0x00000199,0x00000155,0x00000124,
		0x00001000,0x00000800,0x00000400,0x000002AA,0x00000200,0x00000199,0x00000155,0x00000124,
		0x00000800,0x00000400,0x00000200,0x00000155,0x00000100,0x000000CC,0x000000AA,0x00000092,
		0x00000800,0x00000400,0x00000200,0x00000155,0x00000100,0x000000CC,0x000000AA,0x00000092,
		0x00000400,0x00000200,0x00000100,0x000000AA,0x00000080,0x00000066,0x00000055,0x00000049,
		0x00000400,0x00000200,0x00000100,0x000000AA,0x00000080,0x00000066,0x00000055,0x00000049,
		0x00000200,0x00000100,0x00000080,0x00000055,0x00000040,0x00000033,0x0000002A,0x00000024,
		0x00000200,0x00000100,0x00000080,0x00000055,0x00000040,0x00000033,0x0000002A,0x00000024,
		0x00000100,0x00000080,0x00000040,0x0000002A,0x00000020,0x00000019,0x00000015,0x00000012,
		0x00000100,0x00000080,0x00000040,0x0000002A,0x00000020,0x00000019,0x00000015,0x00000012,
		0x00000080,0x00000040,0x00000020,0x00000015,0x00000010,0x0000000C,0x0000000A,0x00000009,
		0x00000080,0x00000040,0x00000020,0x00000015,0x00000010,0x0000000C,0x0000000A,0x00000009,
		0x00000040,0x00000020,0x00000010,0x0000000A,0x00000008,0x00000006,0x00000005,0x00000004,
		0x00000040,0x00000020,0x00000010,0x0000000A,0x00000008,0x00000006,0x00000005,0x00000004,
		0x00100000,0x00080000,0x00040000,0x0002AAAA,0x00020000,0x00019999,0x00015555,0x00012492,
		0x00100000,0x00080000,0x00040000,0x0002AAAA,0x00020000,0x00019999,0x00015555,0x00012492,
		0x00100000,0x00080000,0x00040000,0x0002AAAA,0x00020000,0x00019999,0x00015555,0x00012492,
		0x00100000,0x00080000,0x00040000,0x0002AAAA,0x00020000,0x00019999,0x00015555,0x00012492
	};


	snd_mem[adr-0xFF10]=dat;

	switch(adr){
	case 0xFF10:
		snd_stat.sq1_sw_time=(dat>>4)&7;
		snd_stat.sq1_sw_dir=(dat>>3)&1;
		snd_stat.sq1_sw_shift=dat&7;
		break;
	case 0xFF11:
		snd_stat.sq1_type=dat>>6;
		snd_stat.sq1_init_len=64-(dat&0x3F);
		snd_stat.sq1_len=snd_stat.sq1_init_len;
		break;
	case 0xFF12:
		snd_stat.sq1_init_vol=dat>>4;
		snd_stat.sq1_vol=snd_stat.sq1_init_vol;
		snd_stat.sq1_env_dir=(dat>>3)&1;
		snd_stat.sq1_env_speed=dat&7;
		break;
	case 0xFF13:
		snd_stat.sq1_init_freq&=0x700;
		snd_stat.sq1_init_freq|=dat;
		snd_stat.sq1_freq=snd_stat.sq1_init_freq;
		break;
	case 0xFF14:
		snd_stat.sq1_init_freq&=0xFF;
		snd_stat.sq1_init_freq|=((dat&7)<<8);
		snd_stat.sq1_freq=snd_stat.sq1_init_freq;
		snd_stat.sq1_hold=(dat>>6)&1;
		if (dat&0x80){
			snd_stat.sq1_playing=true;
			snd_stat.sq1_vol=snd_stat.sq1_init_vol;
			snd_stat.sq1_len=snd_stat.sq1_init_len;
			if ((!snd_stat.sq1_playing)||(!snd_stat.sq1_vol)) sq1_cur_pos=0;
		}
		break;
	case 0xFF16:
		snd_stat.sq2_type=dat>>6;
		snd_stat.sq2_init_len=64-(dat&0x3F);
		snd_stat.sq2_len=snd_stat.sq2_init_len;
		break;
	case 0xFF17:
		snd_stat.sq2_init_vol=dat>>4;
		snd_stat.sq2_vol=snd_stat.sq2_init_vol;
		snd_stat.sq2_env_dir=(dat>>3)&1;
		snd_stat.sq2_env_speed=dat&7;
		break;
	case 0xFF18:
		snd_stat.sq2_init_freq&=0x700;
		snd_stat.sq2_init_freq|=dat;
		snd_stat.sq2_freq=snd_stat.sq2_init_freq;
		break;
	case 0xFF19:
		snd_stat.sq2_init_freq&=0xFF;
		snd_stat.sq2_init_freq|=((dat&7)<<8);
		snd_stat.sq2_freq=snd_stat.sq2_init_freq;
		snd_stat.sq2_hold=(dat>>6)&1;
		if (dat&0x80){
			if ((!snd_stat.sq2_playing)||(!snd_stat.sq2_vol)) sq2_cur_pos=0;
			snd_stat.sq2_playing=true;
			snd_stat.sq2_vol=snd_stat.sq2_init_vol;
			snd_stat.sq2_len=snd_stat.sq2_init_len;
		}
		break;
	case 0xFF1A:
		snd_stat.wav_enable=dat>>7;
		break;
	case 0xFF1B:
		snd_stat.wav_init_len=(256-dat);
		snd_stat.wav_len=snd_stat.wav_init_len;
		if ((snd_stat.wav_len&&dat)||!snd_stat.wav_hold)
			snd_stat.wav_playing=true;
		else
			snd_stat.wav_playing=false;
		break;
	case 0xFF1C:
		snd_stat.wav_vol=tb[(dat>>5)&3];
//		byte tmp;
//		tmp=snd_stat.wav_vol*128/4;
//		voice_kind_count++;
		break;
	case 0xFF1D:
		snd_stat.wav_freq&=0x700;
		snd_stat.wav_freq|=dat;
		break;
	case 0xFF1E:
		snd_stat.wav_freq&=0xFF;
		snd_stat.wav_freq|=((dat&7)<<8);
		snd_stat.wav_hold=(dat>>6)&1;
		if (dat&0x80){
			snd_stat.wav_len=snd_stat.wav_init_len;
			snd_stat.wav_playing=true;
			if (!snd_stat.wav_playing) wav_cur_pos=0;
		}
		break;
	case 0xFF20://noi len
		snd_stat.noi_init_len=64-(dat&0x3F);
		snd_stat.noi_len=snd_stat.noi_init_len;
		if (snd_stat.noi_len==0)
			snd_stat.noi_playing=false;
		break;
	case 0xFF21://noi env
		snd_stat.noi_init_vol=dat>>4;
		snd_stat.noi_vol=snd_stat.noi_init_vol;
		snd_stat.noi_env_dir=(dat>>3)&1;
		snd_stat.noi_env_speed=dat&7;
		if (snd_stat.noi_vol==0)
			snd_stat.noi_playing=false;
		break;
	case 0xFF22://noi freq
		snd_stat.noi_init_freq= noi_freq_tbl[ dat ] ; //4194304*mul_t[dat&7]/div_t[dat&7]/div_t2[(dat>>4)&15]/8;
		snd_stat.noi_freq=snd_stat.noi_init_freq;
		snd_stat.noi_step=(dat&8)?7:15;

		if ((dat>>6)==3)
			snd_stat.noi_playing=false;
		break;
	case 0xFF23://noi kick
		snd_stat.noi_hold=(dat>>6)&1;
		if (dat&0x80){
			snd_stat.noi_playing=true;
			snd_stat.noi_len=snd_stat.noi_init_len;
			snd_stat.noi_vol=snd_stat.noi_init_vol;
			if ((!snd_stat.noi_playing)||(!snd_stat.noi_vol)) noi_cur_pos=0;
		}
		break;
	case 0xFF24:
		snd_stat.master_vol[0]=(dat&7);
		snd_stat.master_vol[1]=((dat>>4)&7);
//		voice_kind_count++;
		break;
	case 0xFF25:
		snd_stat.ch_enable[0][0]=dat & 1;
		snd_stat.ch_enable[0][1]=dat & 16;
		snd_stat.ch_enable[1][0]=dat & 2;
		snd_stat.ch_enable[1][1]=dat & 32;
		snd_stat.ch_enable[2][0]=dat & 4;
		snd_stat.ch_enable[2][1]=dat & 64;
		snd_stat.ch_enable[3][0]=dat & 8;
		snd_stat.ch_enable[3][1]=dat & 128;

//		voice_kind_count++;
		break;
	case 0xFF26:
		snd_stat.master_enable=dat&0x80;

		snd_stat.ch_on[0]=dat&1;
		snd_stat.ch_on[1]=dat&2;
		snd_stat.ch_on[2]=dat&4;
		snd_stat.ch_on[3]=dat&8;
		break;
	}

//	voice_buffer[voice_count].time=clock;
//	voice_buffer[voice_count].adr=ptr;
//	voice_buffer[voice_count++].dat=dat;
//	if (voice_count>0xfff)
//		voice_count=0xfff;

//	if ((adr>=0xff1a)&&(adr<=0xff26))
//		fprintf(file,"[%04X] %02X\n",adr,dat);
}

static const int sq_wav_dat[4][8]={
	{-10000, 10000,-10000,-10000,-10000,-10000,-10000,-10000},
	{ 10000, 10000,-10000,-10000,-10000,-10000,-10000,-10000},
	{ 10000, 10000, 10000, 10000,-10000,-10000,-10000,-10000},
	{-10000,-10000, 10000, 10000, 10000, 10000, 10000, 10000}
};

inline short snd_sq1_produce(int freq)
{
	static dword cur_sample=0;
//	dword cur_freq;
	short ret;

	if (freq>65000)
		return 15000;

	if (freq){
		ret=sq_wav_dat[snd_stat.sq1_type][cur_sample];
/*		cur_freq=((freq*8)>0x10000)?0xffff:freq*8;
		sq1_cur_pos+=(cur_freq<<16)/44100;
		if (sq1_cur_pos&0xffff0000){
			cur_sample=(cur_sample+(sq1_cur_pos>>16))&7;
			sq1_cur_pos&=0xffff;
		}*/
		sq1_cur_pos+=(freq<<16)/44100;		//扱いを若干変更したので、関連で変更が必要になってるかも。詳細未確認 - LCK
		if (sq1_cur_pos&0xffffe000){
			cur_sample=(cur_sample+(sq1_cur_pos>>13))&7;
			sq1_cur_pos&=0x1fff;
		}
	}
	else
		ret=0;

	return ret;
}

inline short snd_sq2_produce(int freq)
{
	static dword cur_sample=0;
//	dword cur_freq;
	short ret;

	if (freq>65000)
		return 15000;

	if (freq){
		ret=sq_wav_dat[snd_stat.sq2_type][cur_sample];
/*		cur_freq=((freq*8)>0x10000)?0xffff:freq*8;
		sq2_cur_pos+=(cur_freq<<16)/44100;
		if (sq2_cur_pos&0xffff0000){
			cur_sample=(cur_sample+(sq2_cur_pos>>16))&7;
			sq2_cur_pos&=0xffff;
		}*/
		sq2_cur_pos+=(freq<<16)/44100;
		if (sq2_cur_pos&0xffffe000){
			cur_sample=(cur_sample+(sq2_cur_pos>>13))&7;
			sq2_cur_pos&=0x1fff;
		}
	}
	else
		ret=0;

	return ret;
}

inline short snd_wav_produce(int freq)
{
	static dword cur_pos2=0;
	static byte bef_sample=0,cur_sample=0;
	dword cur_freq;
	short ret;

	if (freq>65000)
		return (snd_mem[0x20]>>4)*4000-30000;

	if (freq){
		ret=cur_sample*2500-15000;

		cur_freq=(freq>0x10000)?0xffff:freq;
		wav_cur_pos+=(cur_freq<<16)/44100;
		if (wav_cur_pos&0xffff0000){
			bef_sample=cur_sample;
			cur_pos2=(cur_pos2+(wav_cur_pos>>16))&31;
			if (cur_pos2&1)
				cur_sample=snd_mem[0x20+cur_pos2/2]&0xf;
			else
				cur_sample=snd_mem[0x20+cur_pos2/2]>>4;
			wav_cur_pos&=0xffff;
		}
	}
	else
		ret=0;

	return ret;
}

#include "_mrand_table.h"

inline short snd_noi_produce(int freq)
{
 	static int cur_sample=10000;
 	short ret;
 	int sc;
 	if (freq){
 		ret=cur_sample;
 		noi_cur_pos+=freq;
 		sc=0;
		static int mrand_step = 0 ;
		if( snd_stat.noi_step == 7 ){
 			while(noi_cur_pos>44100){
 				if(sc==0)
 					cur_sample=(_mrand7[ mrand_step & MASK_MRAND ])?12000:-10000;
				else
 					cur_sample+=(_mrand7[ mrand_step & MASK_MRAND ])?12000:-10000;
				noi_cur_pos-=44100;
 				sc++;
				mrand_step++ ;
 			}
		}else{
 			while(noi_cur_pos>44100){
 				if(sc==0)
 					cur_sample=(_mrand15[ mrand_step & MASK_MRAND ])?12000:-10000;
				else
 					cur_sample+=(_mrand15[ mrand_step & MASK_MRAND ])?12000:-10000;
				noi_cur_pos-=44100;
 				sc++;
				mrand_step++ ;
 			}
		}
		if(sc > 0)
 			cur_sample /= sc;
	}else
		ret=0;
 	return ret;
}

void snd_update()
{
	//このルーチンはcpu側とサウンド生成側で共用しているので、このcounterを共用するのは多分よくないと思われ。必要時に修正のこと  - LCK
	static int counter=0;

//	if( snd_stat.master_enable ){
		if (snd_stat.sq1_playing){//&&snd_stat.master_enable){
			if (snd_stat.sq1_env_speed&&(counter%(4*snd_stat.sq1_env_speed)==0)){
				snd_stat.sq1_vol+=(snd_stat.sq1_env_dir?1:-1);
				if (snd_stat.sq1_vol<0) snd_stat.sq1_vol=0;
				if (snd_stat.sq1_vol>15) snd_stat.sq1_vol=15;
			}
			if (snd_stat.sq1_sw_time&&snd_stat.sq1_sw_shift&&(counter%(2*snd_stat.sq1_sw_time)==0)){
				if (snd_stat.sq1_sw_dir)
					snd_stat.sq1_freq=snd_stat.sq1_freq-(snd_stat.sq1_freq>>snd_stat.sq1_sw_shift);
				else
					snd_stat.sq1_freq=snd_stat.sq1_freq+(snd_stat.sq1_freq>>snd_stat.sq1_sw_shift);
			}
			if (snd_stat.sq1_hold&&snd_stat.sq1_len){
				snd_stat.sq1_len--;
				if (snd_stat.sq1_len<=0){
					snd_stat.sq1_playing=false;
				}
			}
		}

		if (snd_stat.sq2_playing){//&&snd_stat.master_enable){
			if (snd_stat.sq2_env_speed&&(counter%(4*snd_stat.sq2_env_speed)==0)){
				snd_stat.sq2_vol+=(snd_stat.sq2_env_dir?1:-1);
				if (snd_stat.sq2_vol<0) snd_stat.sq2_vol=0;
				if (snd_stat.sq2_vol>15) snd_stat.sq2_vol=15;
			}
			if (snd_stat.sq2_hold&&snd_stat.sq2_len){
				snd_stat.sq2_len--;
				if (snd_stat.sq2_len<=0){
					snd_stat.sq2_playing=false;
				}
			}
		}

		if (snd_stat.wav_playing){//&&snd_stat.master_enable){
			if (snd_stat.wav_hold&&snd_stat.wav_len&&(counter%128==0)){
				snd_stat.wav_len--;
				if (snd_stat.wav_len<=0){
					snd_stat.wav_playing=false;
				}
			}
		}

		if (snd_stat.noi_playing){//&&snd_stat.master_enable){
			if (snd_stat.noi_env_speed&&(counter%(4*snd_stat.noi_env_speed)==0)){
				snd_stat.noi_vol+=(snd_stat.noi_env_dir?1:-1);
				if (snd_stat.noi_vol<0) snd_stat.noi_vol=0;
				if (snd_stat.noi_vol>15) snd_stat.noi_vol=15;
			}
			if (snd_stat.noi_hold&&snd_stat.noi_len){
				snd_stat.noi_len--;
				if (snd_stat.noi_len<=0)
					snd_stat.noi_playing=false;
			}
		}
//	}

	counter++;
}

void snd_render(short *buf,int sample)
{
	memcpy(&snd_stat_tmp,&snd_stat,sizeof(snd_stat));
	memcpy(&snd_stat,&snd_stat_cpy,sizeof(snd_stat_cpy));

	//static int tmp_sample=0;
	//int bef_sample_l[5]={0,0,0,0,0}, bef_sample_r[5]={0,0,0,0,0};
	int tmp_l,tmp_r,tmp;
	int now_clock=cpu_get_clock();
	int cur=0;
	unsigned int update_count=0;
	const unsigned int update_count_inc = CLOKS_PER_INTERVAL*(cpu_get_speed()+1);
	int sq1_freq = 131072/(2048-(snd_stat.sq1_freq&0x7FF));
	int sq2_freq = 131072/(2048-(snd_stat.sq2_freq&0x7FF));
	int wav_freq = (65536/(2048-(snd_stat.wav_freq&0x7FF))) << 5;
	unsigned int now_time = snd_bef_clock;
	const unsigned int time_inc = (now_clock - snd_bef_clock) / sample;
	int i;

	for (i=0;i<sample;i++){
		//now_time = snd_bef_clock + (now_clock - snd_bef_clock) * i / sample;

		if ( snd_que_count>cur && (now_time>snd_write_que[cur].clock) ){
			snd_process(snd_write_que[cur].adr,snd_write_que[cur].dat);
			cur++;
			
			sq1_freq = 131072/(2048-(snd_stat.sq1_freq&0x7FF));
			sq2_freq = 131072/(2048-(snd_stat.sq2_freq&0x7FF));
			wav_freq = (65536/(2048-(snd_stat.wav_freq&0x7FF))) << 5;
		}

		tmp_l=tmp_r=0;
		if (snd_stat.master_enable){
			if (snd_stat.sq1_playing/*&&(snd_stat.sq1_freq!=0x7ff)*/){
				tmp = snd_sq1_produce(sq1_freq) * snd_stat.sq1_vol;
				if (snd_stat.ch_enable[0][0])
					tmp_l+=tmp;
				if (snd_stat.ch_enable[0][1])
					tmp_r+=tmp;
			}
			if (snd_stat.sq2_playing/*&&(snd_stat.sq2_freq!=0x7ff)*/){
				tmp = snd_sq2_produce(sq2_freq) * snd_stat.sq2_vol;
				if (snd_stat.ch_enable[1][0])
					tmp_l+=tmp;
				if (snd_stat.ch_enable[1][1])
					tmp_r+=tmp;
			}
			if (snd_stat.wav_playing/*&&(snd_stat.wav_freq!=0x7ff)*/){
				tmp = snd_wav_produce(wav_freq) * snd_stat.wav_vol << 1;
				if (snd_stat.wav_enable){
					if (snd_stat.ch_enable[2][0])
						tmp_l+=tmp;
					if (snd_stat.ch_enable[2][1])
						tmp_r+=tmp;
				}
			}
			if (snd_stat.noi_playing){
				tmp = snd_noi_produce(snd_stat.noi_freq) * snd_stat.noi_vol;
				if (snd_stat.ch_enable[3][0])
					tmp_l+=tmp;
				if (snd_stat.ch_enable[3][1])
					tmp_r+=tmp;
			}
			buf[0] = tmp_l * snd_stat.master_vol[0] >> 8;
			buf[1] = tmp_r * snd_stat.master_vol[1] >> 8;
			//tmp_l = tmp_l * snd_stat.master_vol[0] / 160;
			//tmp_r = tmp_r * snd_stat.master_vol[1] / 160;
			
			/*
			if (snd_b_lowpass){
				// 出力をフィルタリング
				bef_sample_l[4]=bef_sample_l[3];
				bef_sample_l[3]=bef_sample_l[2];
				bef_sample_l[2]=bef_sample_l[1];
				bef_sample_l[1]=bef_sample_l[0];
				bef_sample_l[0]=buf[0];
				bef_sample_r[4]=bef_sample_r[3];
				bef_sample_r[3]=bef_sample_r[2];
				bef_sample_r[2]=bef_sample_r[1];
				bef_sample_r[1]=bef_sample_r[0];
				bef_sample_r[0]=buf[1];
				buf[0]=(bef_sample_l[4]+bef_sample_l[3]*2+bef_sample_l[2]*8+bef_sample_l[1]*2+bef_sample_l[0])/14;
				buf[1]=(bef_sample_r[4]+bef_sample_r[3]*2+bef_sample_r[2]*8+bef_sample_r[1]*2+bef_sample_r[0])/14;
			}
			*((unsigned long*)buf) = (tmp_r & 0x0000FFFF) | ((tmp_l & 0x0000FFFF) << 16) ;
			*/
			buf += 2 ;

			while(update_count*update_count_inc<now_time-snd_bef_clock){
				snd_update();
				update_count++;
			}
		}else{
			*((unsigned long*)buf) = 0;
			buf+=2;
		}

		now_time += time_inc;
	}

	while (snd_que_count>cur){ // 取りこぼし
		snd_process(snd_write_que[cur].adr,snd_write_que[cur].dat);
		cur++;
	}
	snd_que_count=0;
	snd_bef_clock=now_clock;

	memcpy(&snd_stat_cpy,&snd_stat,sizeof(snd_stat));
	memcpy(&snd_stat,&snd_stat_tmp,sizeof(snd_stat));
}

void snd_set_lowpass(bool lowpass){ snd_b_lowpass=lowpass; };
bool snd_get_lowpass(){ return snd_b_lowpass; };
