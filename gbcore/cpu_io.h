//long配列をコピー。配列境界は4バイトアラインされている必要あり
inline void _memcpy4x(void *d, void *s, unsigned long c)
{
	//for (; c>0; --c) *(((unsigned long *)d)++)=*(((unsigned long *)s)++);
	unsigned long *dd=d,*ss=s;
	for (; c>0; --c) *dd++ = *ss++;
}

inline void _memcpy40(void *d, void *s, unsigned long c)
{
	unsigned long *dd=d,*ss=s;
	int i;
	for(i=0; i<5; i++){
		dd[0] = ss[0];
		dd[1] = ss[1];
		dd[2] = ss[2];
		dd[3] = ss[3];
		dd[4] = ss[4];
		dd[5] = ss[5];
		dd[6] = ss[6];
		dd[7] = ss[7];
		dd+=8;
		ss+=8;
	}
}

static byte cpu_io_read_00( word adr )
{
		if(now_gb_mode==2){
			sgb_readingcontrol |= 4;
			sgb_reset_state();
			/*
			if(sgb_multiplayer){
				switch(sgb_nextcontrol)
				{
				case 0x0F:
					//Check_KBInput(0);
				break;
				case 0x0E:
					//Check_KBInput(1);
				break;
				case 0x0D:
					//Check_KBInput(2);
				break;
				case 0x0C:
					//Check_KBInput(3);
				break;
				}
			}
			*/
		}
		
		byte tmp=pad_state;
		if (g_regs.P1==0x03)
			return 0xff;
		switch((g_regs.P1>>4)&0x3){
		case 0:
			return 0xC0|((tmp&0x81?0:1)|(tmp&0x42?0:2)|(tmp&0x24?0:4)|(tmp&0x18?0:8));
		case 1:
			return 0xD0|((tmp&0x01?0:1)|(tmp&0x02?0:2)|(tmp&0x04?0:4)|(tmp&0x08?0:8));
		case 2:
			return 0xE0|((tmp&0x80?0:1)|(tmp&0x40?0:2)|(tmp&0x20?0:4)|(tmp&0x10?0:8));
		case 3:
			if(now_gb_mode==2 && sgb_multiplayer) 
				return 0xF0 | sgb_nextcontrol;
			else
				return 0xFF;
		}
		return 0x00;
}
static byte cpu_io_read_01( word adr ){ return g_regs.SB; }
static byte cpu_io_read_02( word adr ){ return (g_regs.SC&0x83)|0x7C; }
static byte cpu_io_read_03( word adr ){ return 0; }
static byte cpu_io_read_04( word adr ){ return g_regs.DIV ; }
static byte cpu_io_read_05( word adr ){ return g_regs.TIMA; }
static byte cpu_io_read_06( word adr ){ return g_regs.TMA ; }
static byte cpu_io_read_07( word adr ){ return g_regs.TAC ; }
static byte cpu_io_read_08( word adr ){ return 0; }
static byte cpu_io_read_09( word adr ){ return 0; }
static byte cpu_io_read_0A( word adr ){ return 0; }
static byte cpu_io_read_0B( word adr ){ return 0; }
static byte cpu_io_read_0C( word adr ){ return 0; }
static byte cpu_io_read_0D( word adr ){ return 0; }
static byte cpu_io_read_0E( word adr ){ return 0; }
static byte cpu_io_read_0F( word adr ){ return g_regs.IF ; }

static byte cpu_io_read_10_3F( word adr ){ return apu_read(adr) ; }
/*
static byte cpu_io_read_10( word adr ){  ; }
static byte cpu_io_read_11( word adr ){  ; }
static byte cpu_io_read_12( word adr ){  ; }
static byte cpu_io_read_13( word adr ){  ; }
static byte cpu_io_read_14( word adr ){  ; }
static byte cpu_io_read_15( word adr ){  ; }
static byte cpu_io_read_16( word adr ){  ; }
static byte cpu_io_read_17( word adr ){  ; }
static byte cpu_io_read_18( word adr ){  ; }
static byte cpu_io_read_19( word adr ){  ; }
static byte cpu_io_read_1A( word adr ){  ; }
static byte cpu_io_read_1B( word adr ){  ; }
static byte cpu_io_read_1C( word adr ){  ; }
static byte cpu_io_read_1D( word adr ){  ; }
static byte cpu_io_read_1E( word adr ){  ; }
static byte cpu_io_read_1F( word adr ){  ; }
static byte cpu_io_read_20( word adr ){  ; }
static byte cpu_io_read_21( word adr ){  ; }
static byte cpu_io_read_22( word adr ){  ; }
static byte cpu_io_read_23( word adr ){  ; }
static byte cpu_io_read_24( word adr ){  ; }
static byte cpu_io_read_25( word adr ){  ; }
static byte cpu_io_read_26( word adr ){  ; }
static byte cpu_io_read_27( word adr ){  ; }
static byte cpu_io_read_28( word adr ){  ; }
static byte cpu_io_read_29( word adr ){  ; }
static byte cpu_io_read_2A( word adr ){  ; }
static byte cpu_io_read_2B( word adr ){  ; }
static byte cpu_io_read_2C( word adr ){  ; }
static byte cpu_io_read_2D( word adr ){  ; }
static byte cpu_io_read_2E( word adr ){  ; }
static byte cpu_io_read_2F( word adr ){  ; }
static byte cpu_io_read_30( word adr ){  ; }
static byte cpu_io_read_31( word adr ){  ; }
static byte cpu_io_read_32( word adr ){  ; }
static byte cpu_io_read_33( word adr ){  ; }
static byte cpu_io_read_34( word adr ){  ; }
static byte cpu_io_read_35( word adr ){  ; }
static byte cpu_io_read_36( word adr ){  ; }
static byte cpu_io_read_37( word adr ){  ; }
static byte cpu_io_read_38( word adr ){  ; }
static byte cpu_io_read_39( word adr ){  ; }
static byte cpu_io_read_3A( word adr ){  ; }
static byte cpu_io_read_3B( word adr ){  ; }
static byte cpu_io_read_3C( word adr ){  ; }
static byte cpu_io_read_3D( word adr ){  ; }
static byte cpu_io_read_3E( word adr ){  ; }
static byte cpu_io_read_3F( word adr ){  ; }
*/

static byte cpu_io_read_40( word adr ){ return g_regs.LCDC ; }
static byte cpu_io_read_41( word adr ){ return g_regs.STAT|0x80;; }
static byte cpu_io_read_42( word adr ){ return g_regs.SCY; }
static byte cpu_io_read_43( word adr ){ return g_regs.SCX; }
static byte cpu_io_read_44( word adr ){ return g_regs.LY; }
static byte cpu_io_read_45( word adr ){ return g_regs.LYC; }
static byte cpu_io_read_46( word adr ){ return 0; }
static byte cpu_io_read_47( word adr ){ return g_regs.BGP; }
static byte cpu_io_read_48( word adr ){ return g_regs.OBP1; }
static byte cpu_io_read_49( word adr ){ return g_regs.OBP2; }
static byte cpu_io_read_4A( word adr ){ return g_regs.WY; }
static byte cpu_io_read_4B( word adr ){ return g_regs.WX; }
static byte cpu_io_read_4C( word adr ){ return 0 ; }
static byte cpu_io_read_4D( word adr ){ return (speed?0x80:(cg_regs.KEY1&1)?1:0x7E); }
static byte cpu_io_read_4E( word adr ){ return 0 ; }
static byte cpu_io_read_4F( word adr ){ return cg_regs.VBK; }
static byte cpu_io_read_50( word adr ){ return 0 ; }
static byte cpu_io_read_51( word adr ){ return dma_src>>8; }
static byte cpu_io_read_52( word adr ){ return dma_src&0xff; }
static byte cpu_io_read_53( word adr ){ return dma_dest>>8; }
static byte cpu_io_read_54( word adr ){ return dma_dest&0xff; }
static byte cpu_io_read_55( word adr ){ return (dma_executing?((dma_rest-1)&0x7f):0xFF); }
static byte cpu_io_read_56( word adr )
{ 
	// 赤外線
	int bit, size;
	unsigned long cur_time;
	if ((cg_regs.RP&0xC0)==0xC0){
		cur_time = sceKernelLibcClock();
		while(cur_time-rp_time >= 25000){
			size = sceIoRead(rp_fd, &rp_data, 1);
			if(size==0){
				rp_time = cur_time-25000;
				break;
			}else{
				rp_time += 25000;
			}
		}
		bit = rp_data >> ( (cur_time-rp_time)/3125 ) & 1;
	}else{
		bit = 0;
	}
	return (cg_regs.RP & 0xFD) | ((bit^1)<<1);
}

static byte cpu_io_read_57_67( word adr ){ return 0 ; }
/*
static byte cpu_io_read_57( word adr ){ ; }
static byte cpu_io_read_58( word adr ){ ; }
static byte cpu_io_read_59( word adr ){ ; }
static byte cpu_io_read_5A( word adr ){ ; }
static byte cpu_io_read_5B( word adr ){ ; }
static byte cpu_io_read_5C( word adr ){ ; }
static byte cpu_io_read_5D( word adr ){ ; }
static byte cpu_io_read_5E( word adr ){ ; }
static byte cpu_io_read_5F( word adr ){ ; }
static byte cpu_io_read_60( word adr ){ ; }
static byte cpu_io_read_61( word adr ){ ; }
static byte cpu_io_read_62( word adr ){ ; }
static byte cpu_io_read_63( word adr ){ ; }
static byte cpu_io_read_64( word adr ){ ; }
static byte cpu_io_read_65( word adr ){ ; }
static byte cpu_io_read_66( word adr ){ ; }
static byte cpu_io_read_67( word adr ){ ; }
*/

static byte cpu_io_read_68( word adr ){ return cg_regs.BCPS ; }
static byte cpu_io_read_69( word adr )
{ 
	byte ret ;

		if (cg_regs.BCPS&1)
			ret=lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]>>8;
		else
			ret=lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]&0xff;
/*		if (cg_regs.BCPS&1)
			ret=renderer_unmap_color(lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3])>>8;
		else
			ret=renderer_unmap_color(lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3])&0xff;
*/		//ポインタはインクリメントされない(おじゃる丸)
		return ret;
}
static byte cpu_io_read_6A( word adr ){ return cg_regs.OCPS; }
static byte cpu_io_read_6B( word adr )
{ 
	byte ret ;

		if (cg_regs.OCPS&1)
			ret=lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]>>8;
		else
			ret=lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]&0xff;
/*		if (cg_regs.OCPS&1)
			ret=renderer_unmap_color(lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3])>>8;
		else
			ret=renderer_unmap_color(lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3])&0xff;
*/		return ret;
}
static byte cpu_io_read_6C( word adr ){ return _ff6c&1 ; }
static byte cpu_io_read_6D( word adr ){ return 0 ; }
static byte cpu_io_read_6E( word adr ){ return 0 ; }
static byte cpu_io_read_6F( word adr ){ return 0 ; }
static byte cpu_io_read_70( word adr ){ return cg_regs.SVBK ; }
static byte cpu_io_read_71( word adr ){ return ext_mem[0]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_72( word adr ){ return _ff72 ; }
static byte cpu_io_read_73( word adr ){ return _ff73 ; }
static byte cpu_io_read_74( word adr ){ return _ff74 ; }
static byte cpu_io_read_75( word adr ){ return _ff75&0x70 ; }
static byte cpu_io_read_76( word adr ){ return 0; } // ?
static byte cpu_io_read_77( word adr ){ return 0; } // ?
static byte cpu_io_read_78( word adr ){ return ext_mem[ 7]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_79( word adr ){ return ext_mem[ 8]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_7A( word adr ){ return ext_mem[ 9]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_7B( word adr ){ return ext_mem[10]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_7C( word adr ){ return ext_mem[11]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_7D( word adr ){ return ext_mem[12]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_7E( word adr ){ return ext_mem[13]; }//ext_mem[adr-0xff71]; }
static byte cpu_io_read_7F( word adr ){ return ext_mem[14]; }//ext_mem[adr-0xff71]; }

static byte cpu_io_read_80_FE( word adr ){ return stack[adr-0xFF80] ; }
/*
static byte cpu_io_read_80( word adr ){ ; }
static byte cpu_io_read_81( word adr ){ ; }
static byte cpu_io_read_82( word adr ){ ; }
static byte cpu_io_read_83( word adr ){ ; }
static byte cpu_io_read_84( word adr ){ ; }
static byte cpu_io_read_85( word adr ){ ; }
static byte cpu_io_read_86( word adr ){ ; }
static byte cpu_io_read_87( word adr ){ ; }
static byte cpu_io_read_88( word adr ){ ; }
static byte cpu_io_read_89( word adr ){ ; }
static byte cpu_io_read_8A( word adr ){ ; }
static byte cpu_io_read_8B( word adr ){ ; }
static byte cpu_io_read_8C( word adr ){ ; }
static byte cpu_io_read_8D( word adr ){ ; }
static byte cpu_io_read_8E( word adr ){ ; }
static byte cpu_io_read_8F( word adr ){ ; }
static byte cpu_io_read_90( word adr ){ ; }
static byte cpu_io_read_91( word adr ){ ; }
static byte cpu_io_read_92( word adr ){ ; }
static byte cpu_io_read_93( word adr ){ ; }
static byte cpu_io_read_94( word adr ){ ; }
static byte cpu_io_read_95( word adr ){ ; }
static byte cpu_io_read_96( word adr ){ ; }
static byte cpu_io_read_97( word adr ){ ; }
static byte cpu_io_read_98( word adr ){ ; }
static byte cpu_io_read_99( word adr ){ ; }
static byte cpu_io_read_9A( word adr ){ ; }
static byte cpu_io_read_9B( word adr ){ ; }
static byte cpu_io_read_9C( word adr ){ ; }
static byte cpu_io_read_9D( word adr ){ ; }
static byte cpu_io_read_9E( word adr ){ ; }
static byte cpu_io_read_9F( word adr ){ ; }
static byte cpu_io_read_A0( word adr ){ ; }
static byte cpu_io_read_A1( word adr ){ ; }
static byte cpu_io_read_A2( word adr ){ ; }
static byte cpu_io_read_A3( word adr ){ ; }
static byte cpu_io_read_A4( word adr ){ ; }
static byte cpu_io_read_A5( word adr ){ ; }
static byte cpu_io_read_A6( word adr ){ ; }
static byte cpu_io_read_A7( word adr ){ ; }
static byte cpu_io_read_A8( word adr ){ ; }
static byte cpu_io_read_A9( word adr ){ ; }
static byte cpu_io_read_AA( word adr ){ ; }
static byte cpu_io_read_AB( word adr ){ ; }
static byte cpu_io_read_AC( word adr ){ ; }
static byte cpu_io_read_AD( word adr ){ ; }
static byte cpu_io_read_AE( word adr ){ ; }
static byte cpu_io_read_AF( word adr ){ ; }
static byte cpu_io_read_B0( word adr ){ ; }
static byte cpu_io_read_B1( word adr ){ ; }
static byte cpu_io_read_B2( word adr ){ ; }
static byte cpu_io_read_B3( word adr ){ ; }
static byte cpu_io_read_B4( word adr ){ ; }
static byte cpu_io_read_B5( word adr ){ ; }
static byte cpu_io_read_B6( word adr ){ ; }
static byte cpu_io_read_B7( word adr ){ ; }
static byte cpu_io_read_B8( word adr ){ ; }
static byte cpu_io_read_B9( word adr ){ ; }
static byte cpu_io_read_BA( word adr ){ ; }
static byte cpu_io_read_BB( word adr ){ ; }
static byte cpu_io_read_BC( word adr ){ ; }
static byte cpu_io_read_BD( word adr ){ ; }
static byte cpu_io_read_BE( word adr ){ ; }
static byte cpu_io_read_BF( word adr ){ ; }
static byte cpu_io_read_C0( word adr ){ ; }
static byte cpu_io_read_C1( word adr ){ ; }
static byte cpu_io_read_C2( word adr ){ ; }
static byte cpu_io_read_C3( word adr ){ ; }
static byte cpu_io_read_C4( word adr ){ ; }
static byte cpu_io_read_C5( word adr ){ ; }
static byte cpu_io_read_C6( word adr ){ ; }
static byte cpu_io_read_C7( word adr ){ ; }
static byte cpu_io_read_C8( word adr ){ ; }
static byte cpu_io_read_C9( word adr ){ ; }
static byte cpu_io_read_CA( word adr ){ ; }
static byte cpu_io_read_CB( word adr ){ ; }
static byte cpu_io_read_CC( word adr ){ ; }
static byte cpu_io_read_CD( word adr ){ ; }
static byte cpu_io_read_CE( word adr ){ ; }
static byte cpu_io_read_CF( word adr ){ ; }
static byte cpu_io_read_D0( word adr ){ ; }
static byte cpu_io_read_D1( word adr ){ ; }
static byte cpu_io_read_D2( word adr ){ ; }
static byte cpu_io_read_D3( word adr ){ ; }
static byte cpu_io_read_D4( word adr ){ ; }
static byte cpu_io_read_D5( word adr ){ ; }
static byte cpu_io_read_D6( word adr ){ ; }
static byte cpu_io_read_D7( word adr ){ ; }
static byte cpu_io_read_D8( word adr ){ ; }
static byte cpu_io_read_D9( word adr ){ ; }
static byte cpu_io_read_DA( word adr ){ ; }
static byte cpu_io_read_DB( word adr ){ ; }
static byte cpu_io_read_DC( word adr ){ ; }
static byte cpu_io_read_DD( word adr ){ ; }
static byte cpu_io_read_DE( word adr ){ ; }
static byte cpu_io_read_DF( word adr ){ ; }
static byte cpu_io_read_E0( word adr ){ ; }
static byte cpu_io_read_E1( word adr ){ ; }
static byte cpu_io_read_E2( word adr ){ ; }
static byte cpu_io_read_E3( word adr ){ ; }
static byte cpu_io_read_E4( word adr ){ ; }
static byte cpu_io_read_E5( word adr ){ ; }
static byte cpu_io_read_E6( word adr ){ ; }
static byte cpu_io_read_E7( word adr ){ ; }
static byte cpu_io_read_E8( word adr ){ ; }
static byte cpu_io_read_E9( word adr ){ ; }
static byte cpu_io_read_EA( word adr ){ ; }
static byte cpu_io_read_EB( word adr ){ ; }
static byte cpu_io_read_EC( word adr ){ ; }
static byte cpu_io_read_ED( word adr ){ ; }
static byte cpu_io_read_EE( word adr ){ ; }
static byte cpu_io_read_EF( word adr ){ ; }
static byte cpu_io_read_F0( word adr ){ ; }
static byte cpu_io_read_F1( word adr ){ ; }
static byte cpu_io_read_F2( word adr ){ ; }
static byte cpu_io_read_F3( word adr ){ ; }
static byte cpu_io_read_F4( word adr ){ ; }
static byte cpu_io_read_F5( word adr ){ ; }
static byte cpu_io_read_F6( word adr ){ ; }
static byte cpu_io_read_F7( word adr ){ ; }
static byte cpu_io_read_F8( word adr ){ ; }
static byte cpu_io_read_F9( word adr ){ ; }
static byte cpu_io_read_FA( word adr ){ ; }
static byte cpu_io_read_FB( word adr ){ ; }
static byte cpu_io_read_FC( word adr ){ ; }
static byte cpu_io_read_FD( word adr ){ ; }
static byte cpu_io_read_FE( word adr ){ ; }
*/
static byte cpu_io_read_FF( word adr ){ return g_regs.IE ; }


byte (*cpu_io_read_JmpTbl[256])(word adr) = 
{
cpu_io_read_00,cpu_io_read_01,cpu_io_read_02,cpu_io_read_03,
cpu_io_read_04,cpu_io_read_05,cpu_io_read_06,cpu_io_read_07,
cpu_io_read_08,cpu_io_read_09,cpu_io_read_0A,cpu_io_read_0B,
cpu_io_read_0C,cpu_io_read_0D,cpu_io_read_0E,cpu_io_read_0F,
/*
cpu_io_read_10,cpu_io_read_11,cpu_io_read_12,cpu_io_read_13,
cpu_io_read_14,cpu_io_read_15,cpu_io_read_16,cpu_io_read_17,
cpu_io_read_18,cpu_io_read_19,cpu_io_read_1A,cpu_io_read_1B,
cpu_io_read_1C,cpu_io_read_1D,cpu_io_read_1E,cpu_io_read_1F,
cpu_io_read_20,cpu_io_read_21,cpu_io_read_22,cpu_io_read_23,
cpu_io_read_24,cpu_io_read_25,cpu_io_read_26,cpu_io_read_27,
cpu_io_read_28,cpu_io_read_29,cpu_io_read_2A,cpu_io_read_2B,
cpu_io_read_2C,cpu_io_read_2D,cpu_io_read_2E,cpu_io_read_2F,
cpu_io_read_30,cpu_io_read_31,cpu_io_read_32,cpu_io_read_33,
cpu_io_read_34,cpu_io_read_35,cpu_io_read_36,cpu_io_read_37,
cpu_io_read_38,cpu_io_read_39,cpu_io_read_3A,cpu_io_read_3B,
cpu_io_read_3C,cpu_io_read_3D,cpu_io_read_3E,cpu_io_read_3F,
*/
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,
cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,cpu_io_read_10_3F,

cpu_io_read_40,cpu_io_read_41,cpu_io_read_42,cpu_io_read_43,
cpu_io_read_44,cpu_io_read_45,cpu_io_read_46,cpu_io_read_47,
cpu_io_read_48,cpu_io_read_49,cpu_io_read_4A,cpu_io_read_4B,
cpu_io_read_4C,cpu_io_read_4D,cpu_io_read_4E,cpu_io_read_4F,
cpu_io_read_50,cpu_io_read_51,cpu_io_read_52,cpu_io_read_53,
cpu_io_read_54,cpu_io_read_55,cpu_io_read_56,

/*
cpu_io_read_57,
cpu_io_read_58,cpu_io_read_59,cpu_io_read_5A,cpu_io_read_5B,
cpu_io_read_5C,cpu_io_read_5D,cpu_io_read_5E,cpu_io_read_5F,
cpu_io_read_60,cpu_io_read_61,cpu_io_read_62,cpu_io_read_63,
cpu_io_read_64,cpu_io_read_65,cpu_io_read_66,cpu_io_read_67,
*/

cpu_io_read_57_67,
cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,
cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,
cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,
cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,cpu_io_read_57_67,

cpu_io_read_68,cpu_io_read_69,cpu_io_read_6A,cpu_io_read_6B,
cpu_io_read_6C,cpu_io_read_6D,cpu_io_read_6E,cpu_io_read_6F,
cpu_io_read_70,cpu_io_read_71,cpu_io_read_72,cpu_io_read_73,
cpu_io_read_74,cpu_io_read_75,cpu_io_read_76,cpu_io_read_77,
cpu_io_read_78,cpu_io_read_79,cpu_io_read_7A,cpu_io_read_7B,
cpu_io_read_7C,cpu_io_read_7D,cpu_io_read_7E,cpu_io_read_7F,
/*
cpu_io_read_80,cpu_io_read_81,cpu_io_read_82,cpu_io_read_83,
cpu_io_read_84,cpu_io_read_85,cpu_io_read_86,cpu_io_read_87,
cpu_io_read_88,cpu_io_read_89,cpu_io_read_8A,cpu_io_read_8B,
cpu_io_read_8C,cpu_io_read_8D,cpu_io_read_8E,cpu_io_read_8F,
cpu_io_read_90,cpu_io_read_91,cpu_io_read_92,cpu_io_read_93,
cpu_io_read_94,cpu_io_read_95,cpu_io_read_96,cpu_io_read_97,
cpu_io_read_98,cpu_io_read_99,cpu_io_read_9A,cpu_io_read_9B,
cpu_io_read_9C,cpu_io_read_9D,cpu_io_read_9E,cpu_io_read_9F,
cpu_io_read_A0,cpu_io_read_A1,cpu_io_read_A2,cpu_io_read_A3,
cpu_io_read_A4,cpu_io_read_A5,cpu_io_read_A6,cpu_io_read_A7,
cpu_io_read_A8,cpu_io_read_A9,cpu_io_read_AA,cpu_io_read_AB,
cpu_io_read_AC,cpu_io_read_AD,cpu_io_read_AE,cpu_io_read_AF,
cpu_io_read_B0,cpu_io_read_B1,cpu_io_read_B2,cpu_io_read_B3,
cpu_io_read_B4,cpu_io_read_B5,cpu_io_read_B6,cpu_io_read_B7,
cpu_io_read_B8,cpu_io_read_B9,cpu_io_read_BA,cpu_io_read_BB,
cpu_io_read_BC,cpu_io_read_BD,cpu_io_read_BE,cpu_io_read_BF,
cpu_io_read_C0,cpu_io_read_C1,cpu_io_read_C2,cpu_io_read_C3,
cpu_io_read_C4,cpu_io_read_C5,cpu_io_read_C6,cpu_io_read_C7,
cpu_io_read_C8,cpu_io_read_C9,cpu_io_read_CA,cpu_io_read_CB,
cpu_io_read_CC,cpu_io_read_CD,cpu_io_read_CE,cpu_io_read_CF,
cpu_io_read_D0,cpu_io_read_D1,cpu_io_read_D2,cpu_io_read_D3,
cpu_io_read_D4,cpu_io_read_D5,cpu_io_read_D6,cpu_io_read_D7,
cpu_io_read_D8,cpu_io_read_D9,cpu_io_read_DA,cpu_io_read_DB,
cpu_io_read_DC,cpu_io_read_DD,cpu_io_read_DE,cpu_io_read_DF,
cpu_io_read_E0,cpu_io_read_E1,cpu_io_read_E2,cpu_io_read_E3,
cpu_io_read_E4,cpu_io_read_E5,cpu_io_read_E6,cpu_io_read_E7,
cpu_io_read_E8,cpu_io_read_E9,cpu_io_read_EA,cpu_io_read_EB,
cpu_io_read_EC,cpu_io_read_ED,cpu_io_read_EE,cpu_io_read_EF,
cpu_io_read_F0,cpu_io_read_F1,cpu_io_read_F2,cpu_io_read_F3,
cpu_io_read_F4,cpu_io_read_F5,cpu_io_read_F6,cpu_io_read_F7,
cpu_io_read_F8,cpu_io_read_F9,cpu_io_read_FA,cpu_io_read_FB,
cpu_io_read_FC,cpu_io_read_FD,cpu_io_read_FE,
*/
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,
cpu_io_read_80_FE,cpu_io_read_80_FE,cpu_io_read_80_FE,

cpu_io_read_FF
} ;
static void cpu_io_write_00( word adr,byte dat )
{
	dat &= 0x30;
	if(now_gb_mode==2) sgb_transfer(dat);
//	g_regs.P1=dat;
	g_regs.P1 = ((g_regs.P1 & 0xCF)|dat);
}
static void cpu_io_write_01( word adr,byte dat ){ g_regs.SB=dat; }
static void cpu_io_write_02( word adr,byte dat )
{ 
		if (rom_get_info()->gb_type<=2){
			g_regs.SC=dat&0x81;
			if ((dat&0x80)&&(dat&1)) // 送信開始
				seri_occer=total_clock+512;
		}
		else{ // GBCでの拡張
			g_regs.SC=dat&0x83;
			if ((dat&0x80)&&(dat&1)){ // 送信開始
				if (dat&2)
					seri_occer=total_clock+512*8/32; // 転送速度通常の32倍
				else
					seri_occer=total_clock+512*8;
			}
		}
}
static void cpu_io_write_03( word adr,byte dat ){ ; }
static void cpu_io_write_04( word adr,byte dat ){ g_regs.DIV=0; }
static void cpu_io_write_05( word adr,byte dat ){ g_regs.TIMA=dat; }
static void cpu_io_write_06( word adr,byte dat ){ g_regs.TMA=dat; }
static void cpu_io_write_07( word adr,byte dat )
{ 
		if ((dat&0x04)&&!(g_regs.TAC&0x04))
			sys_clock=0;
		g_regs.TAC=dat;
}
static void cpu_io_write_08( word adr,byte dat ){ ; }
static void cpu_io_write_09( word adr,byte dat ){ ; }
static void cpu_io_write_0A( word adr,byte dat ){ ; }
static void cpu_io_write_0B( word adr,byte dat ){ ; }
static void cpu_io_write_0C( word adr,byte dat ){ ; }
static void cpu_io_write_0D( word adr,byte dat ){ ; }
static void cpu_io_write_0E( word adr,byte dat ){ ; }
static void cpu_io_write_0F( word adr,byte dat ){ g_regs.IF=dat; cpu_irq_check(); }

static void cpu_io_write_10_3F( word adr,byte dat ){ apu_write(adr,dat,total_clock); }
/*
static void cpu_io_write_10( word adr,byte dat ){ ; }
static void cpu_io_write_11( word adr,byte dat ){ ; }
static void cpu_io_write_12( word adr,byte dat ){ ; }
static void cpu_io_write_13( word adr,byte dat ){ ; }
static void cpu_io_write_14( word adr,byte dat ){ ; }
static void cpu_io_write_15( word adr,byte dat ){ ; }
static void cpu_io_write_16( word adr,byte dat ){ ; }
static void cpu_io_write_17( word adr,byte dat ){ ; }
static void cpu_io_write_18( word adr,byte dat ){ ; }
static void cpu_io_write_19( word adr,byte dat ){ ; }
static void cpu_io_write_1A( word adr,byte dat ){ ; }
static void cpu_io_write_1B( word adr,byte dat ){ ; }
static void cpu_io_write_1C( word adr,byte dat ){ ; }
static void cpu_io_write_1D( word adr,byte dat ){ ; }
static void cpu_io_write_1E( word adr,byte dat ){ ; }
static void cpu_io_write_1F( word adr,byte dat ){ ; }
static void cpu_io_write_20( word adr,byte dat ){ ; }
static void cpu_io_write_21( word adr,byte dat ){ ; }
static void cpu_io_write_22( word adr,byte dat ){ ; }
static void cpu_io_write_23( word adr,byte dat ){ ; }
static void cpu_io_write_24( word adr,byte dat ){ ; }
static void cpu_io_write_25( word adr,byte dat ){ ; }
static void cpu_io_write_26( word adr,byte dat ){ ; }
static void cpu_io_write_27( word adr,byte dat ){ ; }
static void cpu_io_write_28( word adr,byte dat ){ ; }
static void cpu_io_write_29( word adr,byte dat ){ ; }
static void cpu_io_write_2A( word adr,byte dat ){ ; }
static void cpu_io_write_2B( word adr,byte dat ){ ; }
static void cpu_io_write_2C( word adr,byte dat ){ ; }
static void cpu_io_write_2D( word adr,byte dat ){ ; }
static void cpu_io_write_2E( word adr,byte dat ){ ; }
static void cpu_io_write_2F( word adr,byte dat ){ ; }
static void cpu_io_write_30( word adr,byte dat ){ ; }
static void cpu_io_write_31( word adr,byte dat ){ ; }
static void cpu_io_write_32( word adr,byte dat ){ ; }
static void cpu_io_write_33( word adr,byte dat ){ ; }
static void cpu_io_write_34( word adr,byte dat ){ ; }
static void cpu_io_write_35( word adr,byte dat ){ ; }
static void cpu_io_write_36( word adr,byte dat ){ ; }
static void cpu_io_write_37( word adr,byte dat ){ ; }
static void cpu_io_write_38( word adr,byte dat ){ ; }
static void cpu_io_write_39( word adr,byte dat ){ ; }
static void cpu_io_write_3A( word adr,byte dat ){ ; }
static void cpu_io_write_3B( word adr,byte dat ){ ; }
static void cpu_io_write_3C( word adr,byte dat ){ ; }
static void cpu_io_write_3D( word adr,byte dat ){ ; }
static void cpu_io_write_3E( word adr,byte dat ){ ; }
static void cpu_io_write_3F( word adr,byte dat ){ ; }
*/
static void cpu_io_write_40( word adr,byte dat )
{ 
		if ((dat&0x80)&&(!(g_regs.LCDC&0x80))){
			g_regs.LY=0;
			lcd_clear_win_count();
		}
		g_regs.LCDC=dat;
}
static void cpu_io_write_41( word adr,byte dat )
{ 
//		if (rom_get_info()->gb_type==1) // オリジナルGBにおいてこのような現象が起こるらしい
		if (rom_get_info()->gb_type<=2)
			if (!(g_regs.STAT&0x02))
				g_regs.IF|=INT_LCDC;

		g_regs.STAT=(g_regs.STAT&0x7)|(dat&0x78);
		cpu_irq_check();
}
static void cpu_io_write_42( word adr,byte dat ){ g_regs.SCY=dat; }
static void cpu_io_write_43( word adr,byte dat ){ g_regs.SCX=dat; }
static void cpu_io_write_44( word adr,byte dat ){ lcd_clear_win_count(); }
static void cpu_io_write_45( word adr,byte dat ){ g_regs.LYC=dat; }
static void cpu_io_write_46( word adr,byte dat )
{ 
		switch(dat>>5){
		case 0:
		case 1:
			_memcpy4x((unsigned long *)oam,(unsigned long *)(get_rom()+dat*256),0xA0/4); 
			//_memcpy(oam,get_rom()+dat*256,0xA0);
			break;
		case 2:
		case 3:
			_memcpy4x((unsigned long *)oam,(unsigned long *)(mbc_get_rom()+dat*256),0xA0/4); 
			//_memcpy(oam,mbc_get_rom()+dat*256,0xA0);
			break;
		case 4:
			_memcpy4x((unsigned long *)oam,(unsigned long *)(vram_bank+(dat&0x1F)*256),0xA0/4); 
			//_memcpy(oam,vram_bank+(dat&0x1F)*256,0xA0);
			break;
		case 5:
			_memcpy4x((unsigned long *)oam,(unsigned long *)(mbc_get_sram()+(dat&0x1F)*256),0xA0/4); 
			//_memcpy(oam,mbc_get_sram()+(dat&0x1F)*256,0xA0);
			break;
		case 6:
			if (dat&0x10)
				_memcpy4x((unsigned long *)oam,(unsigned long *)(ram_bank+(dat&0x0F)*256),0xA0/4); 
			//_memcpy(oam,ram_bank+(dat&0x0F)*256,0xA0);
			else
				_memcpy4x((unsigned long *)oam,(unsigned long *)(ram+(dat&0x0F)*256),0xA0/4); 
			//_memcpy(oam,ram+(dat&0x0F)*256,0xA0);
			break;
		case 7:
			if (dat<0xF2){
				if (dat&0x10)
					_memcpy4x((unsigned long *)oam,(unsigned long *)(ram_bank+(dat&0x0F)*256),0xA0/4); 
				//_memcpy(oam,ram_bank+(dat&0x0F)*256,0xA0);
				else
					_memcpy4x((unsigned long *)oam,(unsigned long *)(ram+(dat&0x0F)*256),0xA0/4); 
				//_memcpy(oam,ram+(dat&0x0F)*256,0xA0);
			}
			break;
		}
}
static void cpu_io_write_47( word adr,byte dat ){ g_regs.BGP=dat; }
static void cpu_io_write_48( word adr,byte dat ){ g_regs.OBP1=dat; }
static void cpu_io_write_49( word adr,byte dat ){ g_regs.OBP2=dat; }
static void cpu_io_write_4A( word adr,byte dat ){ g_regs.WY=dat; }
static void cpu_io_write_4B( word adr,byte dat ){ g_regs.WX=dat; }
static void cpu_io_write_4C( word adr,byte dat ){ ; }
static void cpu_io_write_4D( word adr,byte dat ){ cg_regs.KEY1=dat&1; speed_change=dat&1; }
static void cpu_io_write_4E( word adr,byte dat ){ ; }
static void cpu_io_write_4F( word adr,byte dat )
{ 
		if (dma_executing)
			return;
		vram_bank=vram+0x2000*(dat&0x01);
		cg_regs.VBK=dat;//&0x01;
}
static void cpu_io_write_50( word adr,byte dat ){ ; }
static void cpu_io_write_51( word adr,byte dat ){ dma_src&=0x00F0; dma_src|=(dat<<8); }
static void cpu_io_write_52( word adr,byte dat ){ dma_src&=0xFF00; dma_src|=(dat&0xF0); }
static void cpu_io_write_53( word adr,byte dat ){ dma_dest&=0x00F0; dma_dest|=((dat&0xFF)<<8); }
static void cpu_io_write_54( word adr,byte dat ){ dma_dest&=0xFF00; dma_dest|=(dat&0xF0); }
static void cpu_io_write_55( word adr,byte dat )
{ 
		word tmp_adr=0x8000+(dma_dest&0x1ff0);
//			fprintf(file,"%03d : %04X -> %04X  %d byte %s\n",g_regs.LY,dma_src,dma_dest,((dat&0x7f)+1)*16,(dat&0x80)?"delay":"immidiately");
		if ((dma_src>=0x8000&&dma_src<0xA000)||(dma_src>=0xE000)||(!(tmp_adr>=0x8000&&tmp_adr<0xA000))){
			cg_regs.HDMA5=0;
			return;
		}
		if (dat&0x80){ //HBlank毎
			if (dma_executing){
				dma_executing=false;
				dma_rest=0;
				cg_regs.HDMA5=0xFF;
				return;
			}
			dma_executing=true;
			b_dma_first=true;
			dma_rest=(dat&0x7F)+1;
			cg_regs.HDMA5=0;
/*				dma_dest_bank=vram_bank;
			if (dma_src<0x4000)
				dma_src_bank=get_rom();
			else if (dma_src<0x8000)
				dma_src_bank=mbc_get_rom()-0x4000;
			else if (dma_src>=0xA000&&dma_src<0xC000)
				dma_src_bank=mbc_get_sram()-0xA000;
			else if (dma_src>=0xC000&&dma_src<0xD000)
				dma_src_bank=ram-0xC000;
			else if (dma_src>=0xD000&&dma_src<0xE000)
				dma_src_bank=ram_bank-0xD000;
			else dma_src_bank=NULL;
*/			}
		else{ //通常DMA
			if (dma_executing){
				dma_executing=false;
				dma_rest=0;
				cg_regs.HDMA5=0xFF;
//					fprintf(file,"dma stopped\n");
				return;
			}
			// どうやら､HBlank以外ならいつでもOKみたいだ
//				if (!(((g_regs.STAT&3)==1)||(!(g_regs.LCDC&0x80)))){
//					cg_regs.HDMA5=0;
//					return;
//				}

			dma_executing=false;
			dma_rest=0;
			cg_regs.HDMA5=0xFF;

			switch(dma_src>>13){
			case 0:
			case 1:
				_memcpy4x((unsigned long *)(vram_bank+(dma_dest&0x1ff0)),(unsigned long *)(get_rom()+(dma_src)),4*(dat&0x7F)+4);
				//_memcpy(vram_bank+(dma_dest&0x1ff0),get_rom()+(dma_src),16*(dat&0x7F)+16);
				break;
			case 2:
			case 3:
				_memcpy4x((unsigned long *)(vram_bank+(dma_dest&0x1ff0)),(unsigned long *)(mbc_get_rom()+(dma_src)),4*(dat&0x7F)+4);
				//_memcpy(vram_bank+(dma_dest&0x1ff0),mbc_get_rom()+(dma_src),16*(dat&0x7F)+16);
				break;
			case 4:
				break;
			case 5:
				_memcpy4x((unsigned long *)(vram_bank+(dma_dest&0x1ff0)),(unsigned long *)(mbc_get_sram()+(dma_src&0x1FFF)),4*(dat&0x7F)+4);
				//_memcpy(vram_bank+(dma_dest&0x1ff0),mbc_get_sram()+(dma_src&0x1FFF),16*(dat&0x7F)+16);
				break;
			case 6:
				if (dma_src&0x1000)
					_memcpy4x((unsigned long *)(vram_bank+(dma_dest&0x1ff0)),(unsigned long *)(ram_bank+(dma_src&0x0FFF)),4*(dat&0x7F)+4);
					//_memcpy(vram_bank+(dma_dest&0x1ff0),ram_bank+(dma_src&0x0FFF),16*(dat&0x7F)+16);
				else
					_memcpy4x((unsigned long *)(vram_bank+(dma_dest&0x1ff0)),(unsigned long *)(ram+(dma_src&0x0FFF)),4*(dat&0x7F)+4);
					//_memcpy(vram_bank+(dma_dest&0x1ff0),ram+(dma_src&0x0FFF),16*(dat&0x7F)+16);
				break;
			case 7:
				break;
			}
			dma_src+=((dat&0x7F)+1)*16;
			dma_dest+=((dat&0x7F)+1)*16;

			gdma_rest=456*2+((dat&0x7f)+1)*32*(speed?2:1); // CPU パワーを占領
		}
}
static void cpu_io_write_56( word adr,byte dat )
{
	// 赤外線
	cg_regs.RP=dat;
	if ((cg_regs.RP&0xC0)==0xC0){
		if (rp_fd<0){
			rp_fd = sceIoOpen("irda0:",SCE_O_RDWR, 777);
			rp_data = sceIoRead(rp_fd, &rp_data, 1);
			rp_time = sceKernelLibcClock();
		}
	}else if (rp_fd>=0){
		sceIoClose(rp_fd);
		rp_fd = -1;
		rp_time = 0;
	}
}

static void cpu_io_write_57_67( word adr,byte dat ){ ; }

/*
static void cpu_io_write_57( word adr,byte dat ){ ; }
static void cpu_io_write_58( word adr,byte dat ){ ; }
static void cpu_io_write_59( word adr,byte dat ){ ; }
static void cpu_io_write_5A( word adr,byte dat ){ ; }
static void cpu_io_write_5B( word adr,byte dat ){ ; }
static void cpu_io_write_5C( word adr,byte dat ){ ; }
static void cpu_io_write_5D( word adr,byte dat ){ ; }
static void cpu_io_write_5E( word adr,byte dat ){ ; }
static void cpu_io_write_5F( word adr,byte dat ){ ; }
static void cpu_io_write_60( word adr,byte dat ){ ; }
static void cpu_io_write_61( word adr,byte dat ){ ; }
static void cpu_io_write_62( word adr,byte dat ){ ; }
static void cpu_io_write_63( word adr,byte dat ){ ; }
static void cpu_io_write_64( word adr,byte dat ){ ; }
static void cpu_io_write_65( word adr,byte dat ){ ; }
static void cpu_io_write_66( word adr,byte dat ){ ; }
static void cpu_io_write_67( word adr,byte dat ){ ; }
*/

static void cpu_io_write_68( word adr,byte dat ){ cg_regs.BCPS=dat; }
static void cpu_io_write_69( word adr,byte dat )
{ 
		if (cg_regs.BCPS&1){
			lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]=
			(lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]&0xff)|(dat<<8);
		}
		else{
			lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]=
			(lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]&0xff00)|dat;
		}
/*			if (cg_regs.BCPS&1){
			lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]=
				renderer_map_color(((renderer_unmap_color(lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3])&0xff)|(dat<<8)));
		}
		else{
			lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3]=
				renderer_map_color(((renderer_unmap_color(lcd_get_pal((cg_regs.BCPS>>3)&7)[(cg_regs.BCPS>>1)&3])&0xff00)|(dat)));
		}*/
		cg_regs.BCPD=dat;
		if (cg_regs.BCPS&0x80)
			cg_regs.BCPS=0x80|((cg_regs.BCPS+1)&0x3f);
//			fprintf(file,"%d :BCPS = %02X\n",g_regs.LY,dat);
}
static void cpu_io_write_6A( word adr,byte dat ){ cg_regs.OCPS=dat; }
static void cpu_io_write_6B( word adr,byte dat )
{ 
		if (cg_regs.OCPS&1){
			lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]=
			(lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]&0xff)|(dat<<8);
		}
		else{
			lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]=
			(lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]&0xff00)|dat;
		}
/*			if (cg_regs.OCPS&1){
			lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]=
				renderer_map_color(((renderer_unmap_color(lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3])&0xff)|(dat<<8)));
		}
		else{
			lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3]=
				renderer_map_color(((renderer_unmap_color(lcd_get_pal(((cg_regs.OCPS>>3)&7)+8)[(cg_regs.OCPS>>1)&3])&0xff00)|(dat)));
		}*/
		cg_regs.OCPD=dat;
		if (cg_regs.OCPS&0x80)
			cg_regs.OCPS=0x80|((cg_regs.OCPS+1)&0x3f);
}
static void cpu_io_write_6C( word adr,byte dat ){ _ff6c=dat&1 ; }
static void cpu_io_write_6D( word adr,byte dat ){ ; }
static void cpu_io_write_6E( word adr,byte dat ){ ; }
static void cpu_io_write_6F( word adr,byte dat ){ ; }
static void cpu_io_write_70( word adr,byte dat )
{ 
//			if (dma_executing)
//				return;

		dat=(!(dat&7))?1:(dat&7);
		cg_regs.SVBK=dat;
		ram_bank=ram+0x1000*dat;
}
static void cpu_io_write_71( word adr,byte dat ){ ext_mem[0]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_72( word adr,byte dat ){ _ff72=dat; }
static void cpu_io_write_73( word adr,byte dat ){ _ff73=dat; }
static void cpu_io_write_74( word adr,byte dat ){ _ff74=dat; }
static void cpu_io_write_75( word adr,byte dat ){ _ff75=dat&0x70; }
static void cpu_io_write_76( word adr,byte dat ){ ext_mem[ 5]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_77( word adr,byte dat ){ ext_mem[ 6]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_78( word adr,byte dat ){ ext_mem[ 7]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_79( word adr,byte dat ){ ext_mem[ 8]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_7A( word adr,byte dat ){ ext_mem[ 9]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_7B( word adr,byte dat ){ ext_mem[10]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_7C( word adr,byte dat ){ ext_mem[11]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_7D( word adr,byte dat ){ ext_mem[12]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_7E( word adr,byte dat ){ ext_mem[13]=dat; }//ext_mem[adr-0xff71]=dat; }
static void cpu_io_write_7F( word adr,byte dat ){ ext_mem[14]=dat; }//ext_mem[adr-0xff71]=dat; }

static void cpu_io_write_80_FE( word adr,byte dat ){ stack[adr-0xFF80]=dat; }

/*
static void cpu_io_write_80( word adr,byte dat ){ ; }
static void cpu_io_write_81( word adr,byte dat ){ ; }
static void cpu_io_write_82( word adr,byte dat ){ ; }
static void cpu_io_write_83( word adr,byte dat ){ ; }
static void cpu_io_write_84( word adr,byte dat ){ ; }
static void cpu_io_write_85( word adr,byte dat ){ ; }
static void cpu_io_write_86( word adr,byte dat ){ ; }
static void cpu_io_write_87( word adr,byte dat ){ ; }
static void cpu_io_write_88( word adr,byte dat ){ ; }
static void cpu_io_write_89( word adr,byte dat ){ ; }
static void cpu_io_write_8A( word adr,byte dat ){ ; }
static void cpu_io_write_8B( word adr,byte dat ){ ; }
static void cpu_io_write_8C( word adr,byte dat ){ ; }
static void cpu_io_write_8D( word adr,byte dat ){ ; }
static void cpu_io_write_8E( word adr,byte dat ){ ; }
static void cpu_io_write_8F( word adr,byte dat ){ ; }
static void cpu_io_write_90( word adr,byte dat ){ ; }
static void cpu_io_write_91( word adr,byte dat ){ ; }
static void cpu_io_write_92( word adr,byte dat ){ ; }
static void cpu_io_write_93( word adr,byte dat ){ ; }
static void cpu_io_write_94( word adr,byte dat ){ ; }
static void cpu_io_write_95( word adr,byte dat ){ ; }
static void cpu_io_write_96( word adr,byte dat ){ ; }
static void cpu_io_write_97( word adr,byte dat ){ ; }
static void cpu_io_write_98( word adr,byte dat ){ ; }
static void cpu_io_write_99( word adr,byte dat ){ ; }
static void cpu_io_write_9A( word adr,byte dat ){ ; }
static void cpu_io_write_9B( word adr,byte dat ){ ; }
static void cpu_io_write_9C( word adr,byte dat ){ ; }
static void cpu_io_write_9D( word adr,byte dat ){ ; }
static void cpu_io_write_9E( word adr,byte dat ){ ; }
static void cpu_io_write_9F( word adr,byte dat ){ ; }
static void cpu_io_write_A0( word adr,byte dat ){ ; }
static void cpu_io_write_A1( word adr,byte dat ){ ; }
static void cpu_io_write_A2( word adr,byte dat ){ ; }
static void cpu_io_write_A3( word adr,byte dat ){ ; }
static void cpu_io_write_A4( word adr,byte dat ){ ; }
static void cpu_io_write_A5( word adr,byte dat ){ ; }
static void cpu_io_write_A6( word adr,byte dat ){ ; }
static void cpu_io_write_A7( word adr,byte dat ){ ; }
static void cpu_io_write_A8( word adr,byte dat ){ ; }
static void cpu_io_write_A9( word adr,byte dat ){ ; }
static void cpu_io_write_AA( word adr,byte dat ){ ; }
static void cpu_io_write_AB( word adr,byte dat ){ ; }
static void cpu_io_write_AC( word adr,byte dat ){ ; }
static void cpu_io_write_AD( word adr,byte dat ){ ; }
static void cpu_io_write_AE( word adr,byte dat ){ ; }
static void cpu_io_write_AF( word adr,byte dat ){ ; }
static void cpu_io_write_B0( word adr,byte dat ){ ; }
static void cpu_io_write_B1( word adr,byte dat ){ ; }
static void cpu_io_write_B2( word adr,byte dat ){ ; }
static void cpu_io_write_B3( word adr,byte dat ){ ; }
static void cpu_io_write_B4( word adr,byte dat ){ ; }
static void cpu_io_write_B5( word adr,byte dat ){ ; }
static void cpu_io_write_B6( word adr,byte dat ){ ; }
static void cpu_io_write_B7( word adr,byte dat ){ ; }
static void cpu_io_write_B8( word adr,byte dat ){ ; }
static void cpu_io_write_B9( word adr,byte dat ){ ; }
static void cpu_io_write_BA( word adr,byte dat ){ ; }
static void cpu_io_write_BB( word adr,byte dat ){ ; }
static void cpu_io_write_BC( word adr,byte dat ){ ; }
static void cpu_io_write_BD( word adr,byte dat ){ ; }
static void cpu_io_write_BE( word adr,byte dat ){ ; }
static void cpu_io_write_BF( word adr,byte dat ){ ; }
static void cpu_io_write_C0( word adr,byte dat ){ ; }
static void cpu_io_write_C1( word adr,byte dat ){ ; }
static void cpu_io_write_C2( word adr,byte dat ){ ; }
static void cpu_io_write_C3( word adr,byte dat ){ ; }
static void cpu_io_write_C4( word adr,byte dat ){ ; }
static void cpu_io_write_C5( word adr,byte dat ){ ; }
static void cpu_io_write_C6( word adr,byte dat ){ ; }
static void cpu_io_write_C7( word adr,byte dat ){ ; }
static void cpu_io_write_C8( word adr,byte dat ){ ; }
static void cpu_io_write_C9( word adr,byte dat ){ ; }
static void cpu_io_write_CA( word adr,byte dat ){ ; }
static void cpu_io_write_CB( word adr,byte dat ){ ; }
static void cpu_io_write_CC( word adr,byte dat ){ ; }
static void cpu_io_write_CD( word adr,byte dat ){ ; }
static void cpu_io_write_CE( word adr,byte dat ){ ; }
static void cpu_io_write_CF( word adr,byte dat ){ ; }
static void cpu_io_write_D0( word adr,byte dat ){ ; }
static void cpu_io_write_D1( word adr,byte dat ){ ; }
static void cpu_io_write_D2( word adr,byte dat ){ ; }
static void cpu_io_write_D3( word adr,byte dat ){ ; }
static void cpu_io_write_D4( word adr,byte dat ){ ; }
static void cpu_io_write_D5( word adr,byte dat ){ ; }
static void cpu_io_write_D6( word adr,byte dat ){ ; }
static void cpu_io_write_D7( word adr,byte dat ){ ; }
static void cpu_io_write_D8( word adr,byte dat ){ ; }
static void cpu_io_write_D9( word adr,byte dat ){ ; }
static void cpu_io_write_DA( word adr,byte dat ){ ; }
static void cpu_io_write_DB( word adr,byte dat ){ ; }
static void cpu_io_write_DC( word adr,byte dat ){ ; }
static void cpu_io_write_DD( word adr,byte dat ){ ; }
static void cpu_io_write_DE( word adr,byte dat ){ ; }
static void cpu_io_write_DF( word adr,byte dat ){ ; }
static void cpu_io_write_E0( word adr,byte dat ){ ; }
static void cpu_io_write_E1( word adr,byte dat ){ ; }
static void cpu_io_write_E2( word adr,byte dat ){ ; }
static void cpu_io_write_E3( word adr,byte dat ){ ; }
static void cpu_io_write_E4( word adr,byte dat ){ ; }
static void cpu_io_write_E5( word adr,byte dat ){ ; }
static void cpu_io_write_E6( word adr,byte dat ){ ; }
static void cpu_io_write_E7( word adr,byte dat ){ ; }
static void cpu_io_write_E8( word adr,byte dat ){ ; }
static void cpu_io_write_E9( word adr,byte dat ){ ; }
static void cpu_io_write_EA( word adr,byte dat ){ ; }
static void cpu_io_write_EB( word adr,byte dat ){ ; }
static void cpu_io_write_EC( word adr,byte dat ){ ; }
static void cpu_io_write_ED( word adr,byte dat ){ ; }
static void cpu_io_write_EE( word adr,byte dat ){ ; }
static void cpu_io_write_EF( word adr,byte dat ){ ; }
static void cpu_io_write_F0( word adr,byte dat ){ ; }
static void cpu_io_write_F1( word adr,byte dat ){ ; }
static void cpu_io_write_F2( word adr,byte dat ){ ; }
static void cpu_io_write_F3( word adr,byte dat ){ ; }
static void cpu_io_write_F4( word adr,byte dat ){ ; }
static void cpu_io_write_F5( word adr,byte dat ){ ; }
static void cpu_io_write_F6( word adr,byte dat ){ ; }
static void cpu_io_write_F7( word adr,byte dat ){ ; }
static void cpu_io_write_F8( word adr,byte dat ){ ; }
static void cpu_io_write_F9( word adr,byte dat ){ ; }
static void cpu_io_write_FA( word adr,byte dat ){ ; }
static void cpu_io_write_FB( word adr,byte dat ){ ; }
static void cpu_io_write_FC( word adr,byte dat ){ ; }
static void cpu_io_write_FD( word adr,byte dat ){ ; }
static void cpu_io_write_FE( word adr,byte dat ){ ; }
*/

static void cpu_io_write_FF( word adr,byte dat )
{ 
		g_regs.IE=dat;
//			g_regs.IF=0;
//			fprintf(file,"IE = %02X\n",dat);
		cpu_irq_check();
}

void (*cpu_io_write_JmpTbl[256])(word adr,byte dat) = 
{
cpu_io_write_00,cpu_io_write_01,cpu_io_write_02,cpu_io_write_03,
cpu_io_write_04,cpu_io_write_05,cpu_io_write_06,cpu_io_write_07,
cpu_io_write_08,cpu_io_write_09,cpu_io_write_0A,cpu_io_write_0B,
cpu_io_write_0C,cpu_io_write_0D,cpu_io_write_0E,cpu_io_write_0F,
/*
cpu_io_write_10,cpu_io_write_11,cpu_io_write_12,cpu_io_write_13,
cpu_io_write_14,cpu_io_write_15,cpu_io_write_16,cpu_io_write_17,
cpu_io_write_18,cpu_io_write_19,cpu_io_write_1A,cpu_io_write_1B,
cpu_io_write_1C,cpu_io_write_1D,cpu_io_write_1E,cpu_io_write_1F,
cpu_io_write_20,cpu_io_write_21,cpu_io_write_22,cpu_io_write_23,
cpu_io_write_24,cpu_io_write_25,cpu_io_write_26,cpu_io_write_27,
cpu_io_write_28,cpu_io_write_29,cpu_io_write_2A,cpu_io_write_2B,
cpu_io_write_2C,cpu_io_write_2D,cpu_io_write_2E,cpu_io_write_2F,
cpu_io_write_30,cpu_io_write_31,cpu_io_write_32,cpu_io_write_33,
cpu_io_write_34,cpu_io_write_35,cpu_io_write_36,cpu_io_write_37,
cpu_io_write_38,cpu_io_write_39,cpu_io_write_3A,cpu_io_write_3B,
cpu_io_write_3C,cpu_io_write_3D,cpu_io_write_3E,cpu_io_write_3F,
*/

cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,
cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,cpu_io_write_10_3F,

cpu_io_write_40,cpu_io_write_41,cpu_io_write_42,cpu_io_write_43,
cpu_io_write_44,cpu_io_write_45,cpu_io_write_46,cpu_io_write_47,
cpu_io_write_48,cpu_io_write_49,cpu_io_write_4A,cpu_io_write_4B,
cpu_io_write_4C,cpu_io_write_4D,cpu_io_write_4E,cpu_io_write_4F,
cpu_io_write_50,cpu_io_write_51,cpu_io_write_52,cpu_io_write_53,
cpu_io_write_54,cpu_io_write_55,cpu_io_write_56,

/*
cpu_io_write_57,
cpu_io_write_58,cpu_io_write_59,cpu_io_write_5A,cpu_io_write_5B,
cpu_io_write_5C,cpu_io_write_5D,cpu_io_write_5E,cpu_io_write_5F,
cpu_io_write_60,cpu_io_write_61,cpu_io_write_62,cpu_io_write_63,
cpu_io_write_64,cpu_io_write_65,cpu_io_write_66,cpu_io_write_67,
*/
cpu_io_write_57_67,
cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,
cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,
cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,
cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,cpu_io_write_57_67,

cpu_io_write_68,cpu_io_write_69,cpu_io_write_6A,cpu_io_write_6B,
cpu_io_write_6C,cpu_io_write_6D,cpu_io_write_6E,cpu_io_write_6F,
cpu_io_write_70,cpu_io_write_71,cpu_io_write_72,cpu_io_write_73,
cpu_io_write_74,cpu_io_write_75,cpu_io_write_76,cpu_io_write_77,
cpu_io_write_78,cpu_io_write_79,cpu_io_write_7A,cpu_io_write_7B,
cpu_io_write_7C,cpu_io_write_7D,cpu_io_write_7E,cpu_io_write_7F,

/*
cpu_io_write_80,cpu_io_write_81,cpu_io_write_82,cpu_io_write_83,
cpu_io_write_84,cpu_io_write_85,cpu_io_write_86,cpu_io_write_87,
cpu_io_write_88,cpu_io_write_89,cpu_io_write_8A,cpu_io_write_8B,
cpu_io_write_8C,cpu_io_write_8D,cpu_io_write_8E,cpu_io_write_8F,
cpu_io_write_90,cpu_io_write_91,cpu_io_write_92,cpu_io_write_93,
cpu_io_write_94,cpu_io_write_95,cpu_io_write_96,cpu_io_write_97,
cpu_io_write_98,cpu_io_write_99,cpu_io_write_9A,cpu_io_write_9B,
cpu_io_write_9C,cpu_io_write_9D,cpu_io_write_9E,cpu_io_write_9F,
cpu_io_write_A0,cpu_io_write_A1,cpu_io_write_A2,cpu_io_write_A3,
cpu_io_write_A4,cpu_io_write_A5,cpu_io_write_A6,cpu_io_write_A7,
cpu_io_write_A8,cpu_io_write_A9,cpu_io_write_AA,cpu_io_write_AB,
cpu_io_write_AC,cpu_io_write_AD,cpu_io_write_AE,cpu_io_write_AF,
cpu_io_write_B0,cpu_io_write_B1,cpu_io_write_B2,cpu_io_write_B3,
cpu_io_write_B4,cpu_io_write_B5,cpu_io_write_B6,cpu_io_write_B7,
cpu_io_write_B8,cpu_io_write_B9,cpu_io_write_BA,cpu_io_write_BB,
cpu_io_write_BC,cpu_io_write_BD,cpu_io_write_BE,cpu_io_write_BF,
cpu_io_write_C0,cpu_io_write_C1,cpu_io_write_C2,cpu_io_write_C3,
cpu_io_write_C4,cpu_io_write_C5,cpu_io_write_C6,cpu_io_write_C7,
cpu_io_write_C8,cpu_io_write_C9,cpu_io_write_CA,cpu_io_write_CB,
cpu_io_write_CC,cpu_io_write_CD,cpu_io_write_CE,cpu_io_write_CF,
cpu_io_write_D0,cpu_io_write_D1,cpu_io_write_D2,cpu_io_write_D3,
cpu_io_write_D4,cpu_io_write_D5,cpu_io_write_D6,cpu_io_write_D7,
cpu_io_write_D8,cpu_io_write_D9,cpu_io_write_DA,cpu_io_write_DB,
cpu_io_write_DC,cpu_io_write_DD,cpu_io_write_DE,cpu_io_write_DF,
cpu_io_write_E0,cpu_io_write_E1,cpu_io_write_E2,cpu_io_write_E3,
cpu_io_write_E4,cpu_io_write_E5,cpu_io_write_E6,cpu_io_write_E7,
cpu_io_write_E8,cpu_io_write_E9,cpu_io_write_EA,cpu_io_write_EB,
cpu_io_write_EC,cpu_io_write_ED,cpu_io_write_EE,cpu_io_write_EF,
cpu_io_write_F0,cpu_io_write_F1,cpu_io_write_F2,cpu_io_write_F3,
cpu_io_write_F4,cpu_io_write_F5,cpu_io_write_F6,cpu_io_write_F7,
cpu_io_write_F8,cpu_io_write_F9,cpu_io_write_FA,cpu_io_write_FB,
cpu_io_write_FC,cpu_io_write_FD,cpu_io_write_FE,
*/
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,
cpu_io_write_80_FE,cpu_io_write_80_FE,cpu_io_write_80_FE,

cpu_io_write_FF
} ;


