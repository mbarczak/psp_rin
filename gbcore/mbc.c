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

//---------------------------------------
// MBC エミュレーション部 (MBC1/2/3/5/7,HuC-1,MMM01,Rumble,RTC,Motion-Sensor,etc...)

#include "gb.h"

byte *rom_page;
byte *sram_page;

bool mbc1_16_8;
byte mbc1_dat;

byte mbc3_latch; // 1 bits
byte mbc3_sec; // 6
byte mbc3_min; // 6
byte mbc3_hour; // 5
byte mbc3_dayl; // 8
byte mbc3_dayh; // 1

byte mbc3_timer; // 4
bool ext_is_ram; // 1
// total 32bits

int mbc5_dat;

bool mbc7_write_enable;
bool mbc7_idle;
byte mbc7_cs;
byte mbc7_sk;
byte mbc7_op_code;
byte mbc7_adr;
word mbc7_dat;
byte mbc7_ret;
byte mbc7_state;
word mbc7_buf;
byte mbc7_count;

bool huc1_16_8;
byte huc1_dat;

void (*mbc_write)(word adr,byte dat) = NULL;

void mbc_init(void)
{
	mbc_reset();
}

void mbc_reset(void)
{
	rom_set_first(0);
	rom_page=get_rom();
	sram_page=get_sram();

	mbc1_16_8=true;
	mbc1_dat=0;
	ext_is_ram=true;

	mbc7_adr=0;
	mbc7_dat=0;
	mbc7_write_enable=false;
	mbc7_idle=false;
	mbc7_cs=0;
	mbc7_sk=0;
	mbc7_state=0;
	mbc7_buf=0;
	mbc7_count=0;
	mbc7_ret=0;

	huc1_16_8=true;
	huc1_dat=0;

	if (!rom_get_loaded())
		return;

	if (rom_get_info()->cart_type==0xFD)
		ext_is_ram=false;
	
	mbc_set_write();
}

byte mbc_read(word adr)
{
	return 0;
}

/*
void mbc_write_old(word adr,byte dat)
{
	switch(rom_get_info()->cart_type){
	case 1:
	case 2:
	case 3:
		mbc_mbc1_write(adr,dat);
		break;
	case 5:
	case 6:
		mbc_mbc2_write(adr,dat);
		break;
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		mbc_mbc3_write(adr,dat);
		break;
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		mbc_mbc5_write(adr,dat);
		break;
	case 0x22:
		mbc_mbc7_write(adr,dat);
		break;
	case 0xFD:
		mbc_tama5_write(adr,dat);
		break;
	case 0xFE:
		mbc_huc3_write(adr,dat);
		break;
	case 0xFF:
		mbc_huc1_write(adr,dat);
		break;
	case 0x100:
		mbc_mmm01_write(adr,dat);
		break;
	}
}
*/

void mbc_set_write()
{
	//mbc_write = mbc_write_old;

	switch(rom_get_info()->cart_type){
	case 1:
	case 2:
	case 3:
		mbc_write = mbc_mbc1_write;
		break;
	case 5:
	case 6:
		mbc_write = mbc_mbc2_write;
		break;
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		mbc_write = mbc_mbc3_write;
		break;
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		mbc_write = mbc_mbc5_write;
		break;
	case 0x22:
		mbc_write = mbc_mbc7_write;
		break;
	case 0xFD:
		mbc_write = mbc_tama5_write;
		break;
	case 0xFE:
		mbc_write = mbc_huc3_write;
		break;
	case 0xFF:
		mbc_write = mbc_huc1_write;
		break;
	case 0x100:
		mbc_write = mbc_mmm01_write;
		break;
	default:
		mbc_write = mbc_nop;
	}
}

byte mbc_ext_read(word adr)
{
	switch(rom_get_info()->cart_type){
	case 1:
	case 2:
	case 3:
	case 5:
	case 6:
		return 0;
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
//		extern FILE *file;
//		fprintf(file,"external read [%04X]\n",adr);
		if (mbc3_latch){
			switch(mbc3_timer){
			case 8: return mbc3_sec;
			case 9: return mbc3_min;
			case 10: return mbc3_hour;
			case 11: return mbc3_dayl;
			case 12: return mbc3_dayh;
			}
		}
		return renderer_get_time(mbc3_timer);
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		return 0;
	case 0x22: // コロコロカービィ
		switch(adr&0xa0f0)
		{
		case 0xA000:
			return 0;
		case 0xA010:
			return 0;
		case 0xA020:
			return renderer_get_sensor(true)&0xff;
		case 0xA030:
			return (renderer_get_sensor(true)>>8)&0xf;
		case 0xA040:
			return renderer_get_sensor(false)&0xff;
		case 0xA050:
			return (renderer_get_sensor(false)>>8)&0xf;
		case 0xA060:
			return 0;
		case 0xA070:
			return 0;
		case 0xA080:
			return mbc7_ret;
		}
		return 0xff;
	case 0xFD:
//		extern FILE *file;
//		fprintf(file,"%04X : TAMA5 ext_read %04X \n",ref_gb->get_cpu()->get_regs()->PC,adr);
		return 1;
	case 0xFE:
//		extern FILE *file;
//		fprintf(file,"%04X : HuC-3 ext_read %04X \n",ref_gb->get_cpu()->get_regs()->PC,adr);
		return 1;
	case 0xFF:
		return 0;
	}
	return 0;
}

void mbc_ext_write(word adr,byte dat)
{
	//char op_name[][20]={"プリフィックス","書込み","読込み","消去"};
	//char pre_op_name[][20]={"書込み消去禁止","全アドレス書込み","全アドレス消去","書込み消去許可"};
	int i;

	switch(rom_get_info()->cart_type){
	case 1:
	case 2:
	case 3:
	case 5:
	case 6:
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0xFF:
		break;
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		renderer_set_time(mbc3_timer,dat);
		break;
	case 0xFE: //HuC-3
//		extern FILE *file;
//		fprintf(file,"%04X : HuC-3 ext_write %04X <= %02X\n",ref_gb->get_cpu()->get_regs()->PC,adr,dat);
		break;
	case 0xFD: //TAMA5
//		extern FILE *file;
//		fprintf(file,"%04X : TAMA5 ext_write %04X <= %02X\n",ref_gb->get_cpu()->get_regs()->PC,adr,dat);
		break;
	case 0x22: // コロコロカービィ
		if (adr==0xA080){
			int bef_cs=mbc7_cs,bef_sk=mbc7_sk;

			mbc7_cs=dat>>7;
			mbc7_sk=(dat>>6)&1;

			if (!bef_cs&&mbc7_cs){
				if (mbc7_state==5){
					if (mbc7_write_enable){
						*(get_sram()+mbc7_adr*2)=mbc7_buf>>8;
						*(get_sram()+mbc7_adr*2+1)=mbc7_buf&0xff;
//						fprintf(file,"書き込み完了\n");
					}
					mbc7_state=0;
					mbc7_ret=1;
//					fprintf(file,"書き込み受理 ステート:なし\n");
				}
				else{
					mbc7_idle=true; // アイドル状態突入
					mbc7_state=0;
//					fprintf(file,"アイドル状態突入 ステート:アイドル状態\n");
				}
			}

			if (!bef_sk&&mbc7_sk){ // クロック立ち上がり
				if (mbc7_idle){ // アイドル状態であれば
					if (dat&0x02){
						mbc7_idle=false; // アイドル状態解除
						mbc7_count=0;
						mbc7_state=1;
//						fprintf(file,"アイドル状態解除 ステート:コマンド認識\n");
					}
				}
				else{
					switch(mbc7_state){
					case 1: // コマンド受付
						mbc7_buf<<=1;
						mbc7_buf|=(dat&0x02)?1:0;
						mbc7_count++;
						if (mbc7_count==2){ // 受付終了
							mbc7_state=2;
							mbc7_count=0;
							mbc7_op_code=mbc7_buf&3;
//							fprintf(file,"コマンド:%s ステート:アドレス受信\n",op_name[mbc7_op_code]);
						}
						break;
					case 2: // アドレス受信
						mbc7_buf<<=1;
						mbc7_buf|=(dat&0x02)?1:0;
						mbc7_count++;
						if (mbc7_count==8){ // 受付終了
							mbc7_state=3;
							mbc7_count=0;
							mbc7_adr=mbc7_buf&0xff;
							if (mbc7_op_code==0){
								if ((mbc7_adr>>6)==0){
//									fprintf(file,"書き込み消去禁止 ステート:なし\n");
									mbc7_write_enable=false;
									mbc7_state=0;
								}
								else if ((mbc7_adr>>6)==3){
//									fprintf(file,"書き込み消去許可 ステート:なし\n");
									mbc7_write_enable=true;
									mbc7_state=0;
								}
//								fprintf(file,"プリフィックスオペコード:%s ステート:データ受信\n",pre_op_name[mbc7_adr>>6]);
							}
							else{
//								fprintf(file,"アドレス:%02X ステート:データ受信\n",mbc7_adr);
							}
						}
						break;
					case 3: // データ
						mbc7_buf<<=1;
						mbc7_buf|=(dat&0x02)?1:0;
						mbc7_count++;

						switch(mbc7_op_code){
						case 0:
							if (mbc7_count==16){
								if ((mbc7_adr>>6)==0){
//									fprintf(file,"書き込み消去禁止 ステート:なし\n");
									mbc7_write_enable=false;
									mbc7_state=0;
								}
								else if ((mbc7_adr>>6)==1){
									if (mbc7_write_enable){
										for (i=0;i<256;i++){
											*(get_sram()+i*2)=mbc7_buf>>8;
											*(get_sram()+i*2)=mbc7_buf&0xff;
										}
									}
//									fprintf(file,"全アドレス書き込み %04X ステート:なし\n",mbc7_buf);
									mbc7_state=5;
								}
								else if ((mbc7_adr>>6)==2){
									if (mbc7_write_enable){
										for (i=0;i<256;i++)
											*(word*)(get_sram()+i*2)=0xffff;
									}
//									fprintf(file,"全アドレス消去 ステート:なし\n");
									mbc7_state=5;
								}
								else if ((mbc7_adr>>6)==3){
//									fprintf(file,"書き込み消去許可 ステート:なし\n");
									mbc7_write_enable=true;
									mbc7_state=0;
								}
								mbc7_count=0;
							}
							break;
						case 1:
							if (mbc7_count==16){
//								fprintf(file,"書き込み [%02X]<-%04X ステート:書き込み待ちフレーム\n",mbc7_adr,mbc7_buf);
								mbc7_count=0;
								mbc7_state=5;
								mbc7_ret=0;
							}
							break;
						case 2:
							if (mbc7_count==1){
//								fprintf(file,"ダミー受信完了 ステート:読み出し可\n");
								mbc7_state=4;
								mbc7_count=0;
								mbc7_buf=(get_sram()[mbc7_adr*2]<<8)|(get_sram()[mbc7_adr*2+1]);
//								fprintf(file,"受信データ %04X\n",mbc7_buf);
							}
							break;
						case 3:
							if (mbc7_count==16){
//								fprintf(file,"消去 [%02X] ステート:書き込み待ちフレーム\n",mbc7_adr,mbc7_buf);
								mbc7_count=0;
								mbc7_state=5;
								mbc7_ret=0;
								mbc7_buf=0xffff;
							}
							break;
						}
						break;
					}
				}
			}

			if (bef_sk&&!mbc7_sk){ // クロック立ち下り
				if (mbc7_state==4){ // 読み出し中
					mbc7_ret=(mbc7_buf&0x8000)?1:0;
					mbc7_buf<<=1;
					mbc7_count++;
//					fprintf(file,"読み出し中 %d ビット目\n",mbc7_count);
					if (mbc7_count==16){
						mbc7_count=0;
						mbc7_state=0;
//						fprintf(file,"読み出し完了 ステート:なし\n");
					}
				}
			}
		}

		break;
	}
}

int mbc_get_state()
{
	switch(rom_get_info()->cart_type){
	case 1:
	case 2:
	case 3:
		return mbc1_dat|(mbc1_16_8?0x100:0);
	case 5:
	case 6:
		return 0;
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		return (mbc3_timer&0xf)|((mbc3_latch&1)<<4)|((mbc3_sec&0x3f)<<5)|((mbc3_min&0x3f)<<11)|
			((mbc3_hour&0x1f)<<17)|(mbc3_dayl<<22)|((mbc3_dayh&1)<<30);
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		return mbc5_dat;
	case 0xFF:
		return huc1_dat|(huc1_16_8?0x100:0);
	default:
		return 0;
	}
}

void mbc_set_state(int dat)
{
	switch(rom_get_info()->cart_type){
	case 1:
	case 2:
	case 3:
		mbc1_dat=dat&0xFF;
		mbc1_16_8=((dat>>8)&1?true:false);
		break;
	case 5:
	case 6:
		break;
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		mbc3_timer=dat&0x0F;
		dat>>=4;
		mbc3_latch=dat&1;
		dat>>=1;
		mbc3_sec=dat&0x3f;
		dat>>=6;
		mbc3_min=dat&0x3f;
		dat>>=6;
		mbc3_hour=dat&0x1f;
		dat>>=5;
		mbc3_dayl=dat&0xff;
		dat>>=8;
		mbc3_dayh=dat&1;
		break;
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		mbc5_dat=dat&0xFFFF;
		break;
	case 0xFF:
		huc1_dat=dat&0xFF;
		huc1_16_8=((dat>>8)&1?true:false);
		break;
	}
}

void mbc_set_page(int rom,int sram)
{
	rom_page=get_rom()+rom*0x4000;
	sram_page=get_sram()+sram*0x2000;
}

static int rom_size_tbl[]={2,4,8,16,32,64,128,256,512};
static int ram_size_tbl[]={0,1,1,4,16,8};

void mbc_nop(word adr,byte dat)
{
	return;
}

void mbc_mbc1_write(word adr,byte dat)
{
	if (mbc1_16_8){//16/8モード
		switch(adr>>13){
		case 0:
			break;
		case 1:
			mbc1_dat=(mbc1_dat&0x60)+(dat&0x1F);
			rom_page=get_rom()+0x4000*((mbc1_dat==0?1:mbc1_dat)&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 2:
			mbc1_dat=((dat<<5)&0x60)+(mbc1_dat&0x1F);
			rom_page=get_rom()+0x4000*((mbc1_dat==0?1:mbc1_dat)&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 3:
			if (dat&1)
				mbc1_16_8=false;
			else
				mbc1_16_8=true;
//			mbc1_dat=0;
			break;
		}
	}
	else{//4/32モード
		switch(adr>>13){
		case 0:
			break;
		case 1:
			rom_page=get_rom()+0x4000*((dat==0?1:dat)&0x1F&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 2:
			sram_page=get_sram()+0x2000*(dat&3);
			break;
		case 3:
			if (dat&1)
				mbc1_16_8=false;
			else
				mbc1_16_8=true;
//			mbc1_dat=0;
			break;
		}
	}
}

void mbc_mbc2_write(word adr,byte dat)
{
	if ((adr>=0x2000)&&(adr<=0x3FFF))
		rom_page=get_rom()+0x4000*(((dat&0x0F)==0?1:dat&0x0F)-1);
}

void mbc_mbc3_write(word adr,byte dat)
{
	switch(adr>>13){
	case 0:
		if (dat==0x0a)
			ext_is_ram=true;
		else{
			ext_is_ram=false;
			mbc3_timer=0;
		}
		break;
	case 1:
		rom_page=get_rom()+0x4000*((dat==0?1:dat)&0x7F&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
		break;
	case 2:
		if (dat<8){
			sram_page=get_sram()+0x2000*(dat&7&(ram_size_tbl[rom_get_info()->ram_size]-1));
			ext_is_ram=true;
		}
		else{
			ext_is_ram=false;
			mbc3_timer=dat&0x0F;
		}
		break;
	case 3://RTCラッチ
		if (dat==0){ // Latchはずす
			mbc3_latch=0;
		}
		else if (dat==1){ // データをLatchする
			if (!mbc3_latch){
				mbc3_sec=renderer_get_time(8);
				mbc3_min=renderer_get_time(9);
				mbc3_hour=renderer_get_time(10);
				mbc3_dayl=renderer_get_time(11);
				mbc3_dayh=renderer_get_time(12);
			}
			mbc3_latch=1;
		}

		break;
	}
}

void mbc_mbc5_write(word adr,byte dat)
{
	switch(adr>>12){
	case 0:
	case 1:
		break;
	case 2:
		mbc5_dat&=0x0100;
		mbc5_dat|=dat;
		rom_page=get_rom()+0x4000*(mbc5_dat&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
		break;
	case 3:
		mbc5_dat&=0x00FF;
		mbc5_dat|=(dat&1)<<8;
		rom_page=get_rom()+0x4000*(mbc5_dat&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
		break;
	case 4:
	case 5:
		if (rom_get_info()->cart_type==0x1C||rom_get_info()->cart_type==0x1D||rom_get_info()->cart_type==0x1E){//Rumble カートリッジ
			sram_page=get_sram()+0x2000*(dat&0x07&(ram_size_tbl[rom_get_info()->ram_size]-1));
			if (dat&0x8)
				renderer_set_bibrate(true);
			else
				renderer_set_bibrate(false);
		}
		else
			sram_page=get_sram()+0x2000*(dat&0x0f&(ram_size_tbl[rom_get_info()->ram_size]-1));
		break;
	}
}

void mbc_mbc7_write(word adr,byte dat)
{
	switch(adr>>13){
	case 0:
		break;
	case 1:
		rom_page=get_rom()+0x4000*((dat==0?1:dat)&0x7F&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
//		rom_page=get_rom()+0x4000*(dat&0x3f)-0x4000;
		break;
	case 2:
		if (dat<8){
			sram_page=get_sram()+0x2000*(dat&3);
			ext_is_ram=false;
		}
		else
			ext_is_ram=false;
		break;
	case 3: // 0x40 が モーションセンサーにマップだが､他のものがマップされることは無い。
		break;
	}
}

void mbc_huc1_write(word adr,byte dat)
{
	if (huc1_16_8){//16/8モード
		switch(adr>>13){
		case 0:
			break;
		case 1:
			huc1_dat=(huc1_dat&0x60)+(dat&0x3F);
			rom_page=get_rom()+0x4000*((huc1_dat==0?1:huc1_dat)&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 2:
			huc1_dat=((dat<<5)&0x60)+(huc1_dat&0x3F);
			rom_page=get_rom()+0x4000*((huc1_dat==0?1:huc1_dat)&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 3:
			if (dat&1)
				huc1_16_8=false;
			else
				huc1_16_8=true;
			huc1_dat=0;
			break;
		}
	}
	else{//4/32モード
		switch(adr>>13){
		case 0:
			break;
		case 1:
			rom_page=get_rom()+0x4000*((dat==0?1:dat)&0x3F&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 2:
			sram_page=get_sram()+0x2000*(dat&3);
			break;
		case 3:
			if (dat&1)
				huc1_16_8=false;
			else
				huc1_16_8=true;
			huc1_dat=0;
			break;
		}
	}
}

void mbc_huc3_write(word adr,byte dat)
{
//	extern FILE *file;
//	fprintf(file,"%04X : HuC-3 write %04X <= %02X\n",ref_gb->get_cpu()->get_regs()->PC,adr,dat);
	switch(adr>>13){
	case 0:
		if (dat==0xA)
			ext_is_ram=true;
		else if (dat==0x0B){
			ext_is_ram=false;
		}
		else if (dat==0x0C){
			ext_is_ram=false;
		}
		else if (dat==0x0D){
			ext_is_ram=false;
		}
		else {
			ext_is_ram=false;
		}
		break;
	case 1:
		rom_page=get_rom()+0x4000*((dat==0?1:dat)&0x7F&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
		break;
	case 2:
		if (dat<8){
			sram_page=get_sram()+0x2000*(dat&3);
			ext_is_ram=true;
		}
		else{
//			ext_is_ram=false;
//			mbc3_timer=dat&0x0F;
		}
		break;
	case 3://RTCラッチ
/*		if (dat==0){ // Latchはずす
			mbc3_latch=0;
		}
		else if (dat==1){ // データをLatchする
			if (!mbc3_latch){
				mbc3_sec=renderer_get_time(8);
				mbc3_min=renderer_get_time(9);
				mbc3_hour=renderer_get_time(10);
				mbc3_dayl=renderer_get_time(11);
				mbc3_dayh=renderer_get_time(12);
			}
			mbc3_latch=1;
		}
*/
		break;
	}
}

void mbc_tama5_write(word adr,byte dat)
{
//	extern FILE *file;
//	fprintf(file,"TAMA5 write %04X <= %02X\n",adr,dat);
}

void mbc_mmm01_write(word adr,byte dat)
{
	if (mbc1_16_8){//16/8モード
		switch(adr>>13){
		case 0:
			break;
		case 1:
			mbc1_dat=(mbc1_dat&0x60)+(dat&0x1F);
			rom_page=get_rom()+0x4000*((mbc1_dat==0?1:mbc1_dat)&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 2:
			mbc1_dat=((dat<<5)&0x60)+(mbc1_dat&0x1F);
			rom_page=get_rom()+0x4000*((mbc1_dat==0?1:mbc1_dat)&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 3:
			if (dat&1)
				mbc1_16_8=false;
			else
				mbc1_16_8=true;
			mbc1_dat=0;
			break;
		}
	}
	else{//4/32モード
		switch(adr>>13){
		case 0:
			break;
		case 1:
			rom_page=get_rom()+0x4000*(((dat&3)*0x10+(dat==0?1:dat))&0x0f&(rom_size_tbl[rom_get_info()->rom_size]-1))-0x4000;
			break;
		case 2:
			rom_set_first((dat&3)*0x10);
			rom_page=get_rom()+0x4000*((dat&3)*0x10);
			mbc1_dat=dat&3;
//			sram_page=get_sram()+0x2000*(dat&3);
			break;
		case 3:
			if (dat&1)
				mbc1_16_8=false;
			else
				mbc1_16_8=true;
//			mbc1_dat=0;
			break;
		}
	}
}

//このへんインライン化
/*
byte *mbc_get_rom()
{
	return rom_page;
}

byte *mbc_get_sram()
{
	return sram_page;
}
bool mbc_is_ext_ram()
{
	return ext_is_ram;
}

void mbc_set_ext_is(bool ext)
{
	ext_is_ram=ext;
}
*/
