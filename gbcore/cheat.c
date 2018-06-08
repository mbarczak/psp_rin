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

#include "gb.h"
#include <ctype.h>

#define MAX_CHEATS 128

byte cheat_map[0x10000];
int cheat_map_n[0x10000];
cheat_dat cheats[MAX_CHEATS];
int nCheats;
int cheat_enable = 0;

void cheat_clear()
{
	nCheats = 0;
	cheat_create_cheat_map();
}

void cheat_init()
{
	cheat_clear();
}

void cheat_decreate_cheat_map()
{
	int i;

	for (i=0; i<nCheats; i++){
		cheat_dat *tmp=&cheats[i];

		if (!tmp->enable)
			continue;

		switch(tmp->code){
		case 0x01:
			cpu_write_direct(tmp->adr, tmp->dat_old);
			break;
		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x97:
			if ((tmp->adr>=0xD000)&&(tmp->adr<0xE000)){
				int ram_adr = (tmp->code-0x90)*0x1000 + tmp->adr - 0xD000;
				ram[ram_adr] = tmp->dat_old;
			}
			else{
				cpu_write_direct(tmp->adr, tmp->dat_old);
			}
			break;
		}
	}
}

void cheat_create_cheat_map()
{
	int i;

	memset(cheat_map,0,sizeof(cheat_map));

	for (i=0; i<nCheats; i++){
		cheat_dat *tmp=&cheats[i];

		if (!tmp->enable)
			continue;

		cheat_map[tmp->adr] = 1;
		cheat_map_n[tmp->adr] = i;

		switch(tmp->code){
		case 0x00:
			cpu_write_direct(tmp->adr, tmp->dat);
			tmp->enable = false;
			cheat_map[tmp->adr] = 0;
			break;
		case 0x01:
			tmp->dat_old = cpu_read(tmp->adr);
			cpu_write_direct(tmp->adr, tmp->dat);
			break;
		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x97:
			if ((tmp->adr>=0xD000)&&(tmp->adr<0xE000)){
				int ram_adr = (tmp->code-0x90)*0x1000 + tmp->adr - 0xD000;
				tmp->dat_old = ram[ram_adr];
				ram[ram_adr] = tmp->dat;
			}
			else{
				tmp->dat_old = cpu_read(tmp->adr);
				cpu_write_direct(tmp->adr, tmp->dat);
			}
			break;
		}
	}
}

byte cheat_write(word adr, byte dat)
{
	cheat_dat *tmp=&cheats[cheat_map_n[adr]];

	switch(tmp->code){
	case 0x01:
		tmp->dat_old = dat;
		return tmp->dat;
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
		if ((adr>=0xD000)&&(adr<0xE000)){
			if(((cpu_get_ram_bank()-cpu_get_ram())/0x1000)==(tmp->code-0x90)){
				tmp->dat_old = dat;
				return tmp->dat;
			}
		}
		else{
			tmp->dat_old = dat;
			return tmp->dat;
		}
	}

	return dat;
}

int hex2n(char c)
{
	c = toupper(c);
	return (isalpha(c)?(c-'A'+10):(c-'0'));
}

int cheat_load(FILE *file)
{
	cheat_dat tmp_dat;
	char buf[256];
	int i;
	bool first=true;

	cheat_decreate_cheat_map();
	cheat_clear();

	while(!feof(file)){
		if (fgets(buf,256,file) && buf[0]!='\n' && buf[0]!='\r'){
			if (first){
				for (i=0;i<256;i++){
					if (buf[i]=='\n' || buf[i]=='\r'){
						buf[i]='\0';
						break;
					}
				}
				strcpy(tmp_dat.name,buf);
				first=false;
			}
			else{
				for (i=0;i<256;i++){
					if (buf[i]=='\n' || buf[i]=='\r'){
						buf[i]='\0';
						break;
					}
				}
				if (i!=8){
					cheat_clear();
					return 0;
				}
				tmp_dat.code = hex2n(buf[0])<< 4 | hex2n(buf[1]);
				tmp_dat.dat  = hex2n(buf[2])<< 4 | hex2n(buf[3]);
				tmp_dat.adr  = hex2n(buf[6])<<12 | hex2n(buf[7])<<8 | hex2n(buf[4])<<4 | hex2n(buf[5]);
				tmp_dat.enable = true;
				cheats[nCheats++] = tmp_dat;

				first = true;
			}
		}
		if (nCheats >= MAX_CHEATS)
			break;
	}

	cheat_create_cheat_map();

	return 1;
}
