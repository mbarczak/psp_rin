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

//-----------------------------------------------
// ROMイメージ管理部 (含SRAM)

#include "gb.h"

struct rom_info info;
byte *dat;
byte *sram;
byte *first_page;
bool b_loaded;
byte rom_image[MAX_ROM_SIZE];
byte sram_space[16*0x2000+4];
int sgb_mode, org_gbtype;

void rom_init()
{
	b_loaded=false;

	dat=NULL;
	sram=NULL;
}

bool rom_has_battery()
{
	static const int has_bat[]=
		{0,0,0,1,0, 0,1,0,0,1,
		 0,0,1,1,0, 1,1,0,0,1,
		 0,0,0,0,0, 0,0,1,0,1,
		 1,0, 0,0,0,0,0,0,0,0}; // 0x20以下
	return has_bat[(info.cart_type>0x20)?3:info.cart_type]==1;
}

int rom_get_sram_size()
{
	static const int tbl_ram[]={1,1,1,4,16,8};//0と1は保険
	return 0x2000*tbl_ram[info.ram_size];
}

bool rom_load_rom(byte *buf,int size,byte *ram,int ram_size)
{
	byte momocol_title[16]={0x4D,0x4F,0x4D,0x4F,0x43,0x4F,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//	if (b_loaded){
//		free(dat);
//		free(sram);
//	}
	
	memcpy(info.cart_name,buf+0x134,16);
	info.cart_name[16]='\0';
	info.cart_name[17]='\0';
	info.cart_type=buf[0x147];
	info.rom_size=buf[0x148];
	info.ram_size=buf[0x149];

	if (memcmp(info.cart_name,momocol_title,16)==0){
		info.cart_type=0x100;//mmm01
	}
	
	byte tmp=buf[0x143];
	
	info.gb_type=(tmp&0x80)?3:(buf[0x146]==0x03)?2:1;
	sgb_mode = (buf[0x146]==0x03)?1:0;
	if(buf[0x143]&0x80)
		org_gbtype = 3;
	else if(buf[0x146]==0x03)
		org_gbtype = 2;
	else
		org_gbtype = 1;
	
	if (info.rom_size>8)
		return false;

//	dat=(byte*)malloc(size);
//	memcpy(dat,buf,size);
	dat = buf;
	first_page = dat;

//	sram=(byte*)malloc(rom_get_sram_size());
//	if (ram_size)
//		memcpy(sram,ram,ram_size&0xffffff00);
//	else
//		memset(sram,0x00,rom_get_sram_size());
	sram = ram;
	
	b_loaded=true;
	
	return true;
}

//インライン化
/*
struct rom_info *rom_get_info()
{
	return &info;
}

byte *get_rom()
{
	return first_page;
}

byte *get_sram()
{
	return sram;
}
*/
bool rom_get_loaded()
{
	return b_loaded;
}

void rom_set_first(int page)
{
	first_page=dat+0x4000*page;
}
