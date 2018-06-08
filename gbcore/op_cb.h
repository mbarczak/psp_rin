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
#include "op_def.h"

//bit test/set/reset opcode

//B 000 C 001 D 010 E 011 H 100 L 101 A 111
//BIT b,r :01 b r :state 8
inline void op_code_cb_case0x40(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_B ] ;} //BIT 0,B
inline void op_code_cb_case0x41(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_C ] ;} //BIT 0,C
inline void op_code_cb_case0x42(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_D ] ;} //BIT 0,D
inline void op_code_cb_case0x43(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_E ] ;} //BIT 0,E
inline void op_code_cb_case0x44(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_H ] ;} //BIT 0,H
inline void op_code_cb_case0x45(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_L ] ;} //BIT 0,L
inline void op_code_cb_case0x47(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ REG_A ] ;} //BIT 0,A

inline void op_code_cb_case0x48(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_B ] ;} //BIT 1,B
inline void op_code_cb_case0x49(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_C ] ;} //BIT 1,C
inline void op_code_cb_case0x4A(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_D ] ;} //BIT 1,D
inline void op_code_cb_case0x4B(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_E ] ;} //BIT 1,E
inline void op_code_cb_case0x4C(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_H ] ;} //BIT 1,H
inline void op_code_cb_case0x4D(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_L ] ;} //BIT 1,L
inline void op_code_cb_case0x4F(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ REG_A ] ;} //BIT 1,A

inline void op_code_cb_case0x50(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_B ] ;} //BIT 2,B
inline void op_code_cb_case0x51(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_C ] ;} //BIT 2,C
inline void op_code_cb_case0x52(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_D ] ;} //BIT 2,D
inline void op_code_cb_case0x53(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_E ] ;} //BIT 2,E
inline void op_code_cb_case0x54(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_H ] ;} //BIT 2,H
inline void op_code_cb_case0x55(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_L ] ;} //BIT 2,L
inline void op_code_cb_case0x57(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ REG_A ] ;} //BIT 2,A

inline void op_code_cb_case0x58(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_B ] ;} //BIT 3,B
inline void op_code_cb_case0x59(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_C ] ;} //BIT 3,C
inline void op_code_cb_case0x5A(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_D ] ;} //BIT 3,D
inline void op_code_cb_case0x5B(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_E ] ;} //BIT 3,E
inline void op_code_cb_case0x5C(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_H ] ;} //BIT 3,H
inline void op_code_cb_case0x5D(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_L ] ;} //BIT 3,L
inline void op_code_cb_case0x5F(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ REG_A ] ;} //BIT 3,A

inline void op_code_cb_case0x60(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_B ] ;} //BIT 4,B
inline void op_code_cb_case0x61(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_C ] ;} //BIT 4,C
inline void op_code_cb_case0x62(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_D ] ;} //BIT 4,D
inline void op_code_cb_case0x63(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_E ] ;} //BIT 4,E
inline void op_code_cb_case0x64(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_H ] ;} //BIT 4,H
inline void op_code_cb_case0x65(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_L ] ;} //BIT 4,L
inline void op_code_cb_case0x67(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ REG_A ] ;} //BIT 4,A

inline void op_code_cb_case0x68(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_B ] ;} //BIT 5,B
inline void op_code_cb_case0x69(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_C ] ;} //BIT 5,C
inline void op_code_cb_case0x6A(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_D ] ;} //BIT 5,D
inline void op_code_cb_case0x6B(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_E ] ;} //BIT 5,E
inline void op_code_cb_case0x6C(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_H ] ;} //BIT 5,H
inline void op_code_cb_case0x6D(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_L ] ;} //BIT 5,L
inline void op_code_cb_case0x6F(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ REG_A ] ;} //BIT 5,A

inline void op_code_cb_case0x70(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_B ] ;} //BIT 6,B
inline void op_code_cb_case0x71(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_C ] ;} //BIT 6,C
inline void op_code_cb_case0x72(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_D ] ;} //BIT 6,D
inline void op_code_cb_case0x73(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_E ] ;} //BIT 6,E
inline void op_code_cb_case0x74(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_H ] ;} //BIT 6,H
inline void op_code_cb_case0x75(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_L ] ;} //BIT 6,L
inline void op_code_cb_case0x77(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ REG_A ] ;} //BIT 6,A

inline void op_code_cb_case0x78(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_B ] ;} //BIT 7,B
inline void op_code_cb_case0x79(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_C ] ;} //BIT 7,C
inline void op_code_cb_case0x7A(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_D ] ;} //BIT 7,D
inline void op_code_cb_case0x7B(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_E ] ;} //BIT 7,E
inline void op_code_cb_case0x7C(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_H ] ;} //BIT 7,H
inline void op_code_cb_case0x7D(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_L ] ;} //BIT 7,L
inline void op_code_cb_case0x7F(void) { REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ REG_A ] ;} //BIT 7,A

//state 12
inline void op_code_cb_case0x46(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable0[ tmp.b.l ];} //BIT 0,(HL)
inline void op_code_cb_case0x4E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable1[ tmp.b.l ];} //BIT 1,(HL)
inline void op_code_cb_case0x56(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable2[ tmp.b.l ];} //BIT 2,(HL)
inline void op_code_cb_case0x5E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable3[ tmp.b.l ];} //BIT 3,(HL)
inline void op_code_cb_case0x66(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable4[ tmp.b.l ];} //BIT 4,(HL)
inline void op_code_cb_case0x6E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable5[ tmp.b.l ];} //BIT 5,(HL)
inline void op_code_cb_case0x76(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable6[ tmp.b.l ];} //BIT 6,(HL)
inline void op_code_cb_case0x7E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=((REG_F&C_FLAG)|H_FLAG)| _BitTable7[ tmp.b.l ];} //BIT 7,(HL)

//bit set opcode
//SET b,r :11 b r : state 8

inline void op_code_cb_case0xC0(void) { REG_B|=0x01;} //SET 0,B
inline void op_code_cb_case0xC1(void) { REG_C|=0x01;} //SET 0,C
inline void op_code_cb_case0xC2(void) { REG_D|=0x01;} //SET 0,D
inline void op_code_cb_case0xC3(void) { REG_E|=0x01;} //SET 0,E
inline void op_code_cb_case0xC4(void) { REG_H|=0x01;} //SET 0,H
inline void op_code_cb_case0xC5(void) { REG_L|=0x01;} //SET 0,L
inline void op_code_cb_case0xC7(void) { REG_A|=0x01;} //SET 0,A

inline void op_code_cb_case0xC8(void) { REG_B|=0x02;} //SET 1,B
inline void op_code_cb_case0xC9(void) { REG_C|=0x02;} //SET 1,C
inline void op_code_cb_case0xCA(void) { REG_D|=0x02;} //SET 1,D
inline void op_code_cb_case0xCB(void) { REG_E|=0x02;} //SET 1,E
inline void op_code_cb_case0xCC(void) { REG_H|=0x02;} //SET 1,H
inline void op_code_cb_case0xCD(void) { REG_L|=0x02;} //SET 1,L
inline void op_code_cb_case0xCF(void) { REG_A|=0x02;} //SET 1,A

inline void op_code_cb_case0xD0(void) { REG_B|=0x04;} //SET 2,B
inline void op_code_cb_case0xD1(void) { REG_C|=0x04;} //SET 2,C
inline void op_code_cb_case0xD2(void) { REG_D|=0x04;} //SET 2,D
inline void op_code_cb_case0xD3(void) { REG_E|=0x04;} //SET 2,E
inline void op_code_cb_case0xD4(void) { REG_H|=0x04;} //SET 2,H
inline void op_code_cb_case0xD5(void) { REG_L|=0x04;} //SET 2,L
inline void op_code_cb_case0xD7(void) { REG_A|=0x04;} //SET 2,A

inline void op_code_cb_case0xD8(void) { REG_B|=0x08;} //SET 3,B
inline void op_code_cb_case0xD9(void) { REG_C|=0x08;} //SET 3,C
inline void op_code_cb_case0xDA(void) { REG_D|=0x08;} //SET 3,D
inline void op_code_cb_case0xDB(void) { REG_E|=0x08;} //SET 3,E
inline void op_code_cb_case0xDC(void) { REG_H|=0x08;} //SET 3,H
inline void op_code_cb_case0xDD(void) { REG_L|=0x08;} //SET 3,L
inline void op_code_cb_case0xDF(void) { REG_A|=0x08;} //SET 3,A

inline void op_code_cb_case0xE0(void) { REG_B|=0x10;} //SET 4,B
inline void op_code_cb_case0xE1(void) { REG_C|=0x10;} //SET 4,C
inline void op_code_cb_case0xE2(void) { REG_D|=0x10;} //SET 4,D
inline void op_code_cb_case0xE3(void) { REG_E|=0x10;} //SET 4,E
inline void op_code_cb_case0xE4(void) { REG_H|=0x10;} //SET 4,H
inline void op_code_cb_case0xE5(void) { REG_L|=0x10;} //SET 4,L
inline void op_code_cb_case0xE7(void) { REG_A|=0x10;} //SET 4,A

inline void op_code_cb_case0xE8(void) { REG_B|=0x20;} //SET 5,B
inline void op_code_cb_case0xE9(void) { REG_C|=0x20;} //SET 5,C
inline void op_code_cb_case0xEA(void) { REG_D|=0x20;} //SET 5,D
inline void op_code_cb_case0xEB(void) { REG_E|=0x20;} //SET 5,E
inline void op_code_cb_case0xEC(void) { REG_H|=0x20;} //SET 5,H
inline void op_code_cb_case0xED(void) { REG_L|=0x20;} //SET 5,L
inline void op_code_cb_case0xEF(void) { REG_A|=0x20;} //SET 5,A

inline void op_code_cb_case0xF0(void) { REG_B|=0x40;} //SET 6,B
inline void op_code_cb_case0xF1(void) { REG_C|=0x40;} //SET 6,C
inline void op_code_cb_case0xF2(void) { REG_D|=0x40;} //SET 6,D
inline void op_code_cb_case0xF3(void) { REG_E|=0x40;} //SET 6,E
inline void op_code_cb_case0xF4(void) { REG_H|=0x40;} //SET 6,H
inline void op_code_cb_case0xF5(void) { REG_L|=0x40;} //SET 6,L
inline void op_code_cb_case0xF7(void) { REG_A|=0x40;} //SET 6,A

inline void op_code_cb_case0xF8(void) { REG_B|=0x80;} //SET 7,B
inline void op_code_cb_case0xF9(void) { REG_C|=0x80;} //SET 7,C
inline void op_code_cb_case0xFA(void) { REG_D|=0x80;} //SET 7,D
inline void op_code_cb_case0xFB(void) { REG_E|=0x80;} //SET 7,E
inline void op_code_cb_case0xFC(void) { REG_H|=0x80;} //SET 7,H
inline void op_code_cb_case0xFD(void) { REG_L|=0x80;} //SET 7,L
inline void op_code_cb_case0xFF(void) { REG_A|=0x80;} //SET 7,A

//state 16
inline void op_code_cb_case0xC6(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x01;cpu_write(REG_HL,tmp.b.l);} //SET 0,(HL)
inline void op_code_cb_case0xCE(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x02;cpu_write(REG_HL,tmp.b.l);} //SET 1,(HL)
inline void op_code_cb_case0xD6(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x04;cpu_write(REG_HL,tmp.b.l);} //SET 2,(HL)
inline void op_code_cb_case0xDE(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x08;cpu_write(REG_HL,tmp.b.l);} //SET 3,(HL)
inline void op_code_cb_case0xE6(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x10;cpu_write(REG_HL,tmp.b.l);} //SET 4,(HL)
inline void op_code_cb_case0xEE(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x20;cpu_write(REG_HL,tmp.b.l);} //SET 5,(HL)
inline void op_code_cb_case0xF6(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x40;cpu_write(REG_HL,tmp.b.l);} //SET 6,(HL)
inline void op_code_cb_case0xFE(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l|=0x80;cpu_write(REG_HL,tmp.b.l);} //SET 7,(HL)

//bit reset opcode
//RES b,r : 10 b r : state 8
inline void op_code_cb_case0x80(void) { REG_B&=0xFE;} //RES 0,B
inline void op_code_cb_case0x81(void) { REG_C&=0xFE;} //RES 0,C
inline void op_code_cb_case0x82(void) { REG_D&=0xFE;} //RES 0,D
inline void op_code_cb_case0x83(void) { REG_E&=0xFE;} //RES 0,E
inline void op_code_cb_case0x84(void) { REG_H&=0xFE;} //RES 0,H
inline void op_code_cb_case0x85(void) { REG_L&=0xFE;} //RES 0,L
inline void op_code_cb_case0x87(void) { REG_A&=0xFE;} //RES 0,A

inline void op_code_cb_case0x88(void) { REG_B&=0xFD;} //RES 1,B
inline void op_code_cb_case0x89(void) { REG_C&=0xFD;} //RES 1,C
inline void op_code_cb_case0x8A(void) { REG_D&=0xFD;} //RES 1,D
inline void op_code_cb_case0x8B(void) { REG_E&=0xFD;} //RES 1,E
inline void op_code_cb_case0x8C(void) { REG_H&=0xFD;} //RES 1,H
inline void op_code_cb_case0x8D(void) { REG_L&=0xFD;} //RES 1,L
inline void op_code_cb_case0x8F(void) { REG_A&=0xFD;} //RES 1,A

inline void op_code_cb_case0x90(void) { REG_B&=0xFB;} //RES 2,B
inline void op_code_cb_case0x91(void) { REG_C&=0xFB;} //RES 2,C
inline void op_code_cb_case0x92(void) { REG_D&=0xFB;} //RES 2,D
inline void op_code_cb_case0x93(void) { REG_E&=0xFB;} //RES 2,E
inline void op_code_cb_case0x94(void) { REG_H&=0xFB;} //RES 2,H
inline void op_code_cb_case0x95(void) { REG_L&=0xFB;} //RES 2,L
inline void op_code_cb_case0x97(void) { REG_A&=0xFB;} //RES 2,A

inline void op_code_cb_case0x98(void) { REG_B&=0xF7;} //RES 3,B
inline void op_code_cb_case0x99(void) { REG_C&=0xF7;} //RES 3,C
inline void op_code_cb_case0x9A(void) { REG_D&=0xF7;} //RES 3,D
inline void op_code_cb_case0x9B(void) { REG_E&=0xF7;} //RES 3,E
inline void op_code_cb_case0x9C(void) { REG_H&=0xF7;} //RES 3,H
inline void op_code_cb_case0x9D(void) { REG_L&=0xF7;} //RES 3,L
inline void op_code_cb_case0x9F(void) { REG_A&=0xF7;} //RES 3,A

inline void op_code_cb_case0xA0(void) { REG_B&=0xEF;} //RES 4,B
inline void op_code_cb_case0xA1(void) { REG_C&=0xEF;} //RES 4,C
inline void op_code_cb_case0xA2(void) { REG_D&=0xEF;} //RES 4,D
inline void op_code_cb_case0xA3(void) { REG_E&=0xEF;} //RES 4,E
inline void op_code_cb_case0xA4(void) { REG_H&=0xEF;} //RES 4,H
inline void op_code_cb_case0xA5(void) { REG_L&=0xEF;} //RES 4,L
inline void op_code_cb_case0xA7(void) { REG_A&=0xEF;} //RES 4,A

inline void op_code_cb_case0xA8(void) { REG_B&=0xDF;} //RES 5,B
inline void op_code_cb_case0xA9(void) { REG_C&=0xDF;} //RES 5,C
inline void op_code_cb_case0xAA(void) { REG_D&=0xDF;} //RES 5,D
inline void op_code_cb_case0xAB(void) { REG_E&=0xDF;} //RES 5,E
inline void op_code_cb_case0xAC(void) { REG_H&=0xDF;} //RES 5,H
inline void op_code_cb_case0xAD(void) { REG_L&=0xDF;} //RES 5,L
inline void op_code_cb_case0xAF(void) { REG_A&=0xDF;} //RES 5,A

inline void op_code_cb_case0xB0(void) { REG_B&=0xBF;} //RES 6,B
inline void op_code_cb_case0xB1(void) { REG_C&=0xBF;} //RES 6,C
inline void op_code_cb_case0xB2(void) { REG_D&=0xBF;} //RES 6,D
inline void op_code_cb_case0xB3(void) { REG_E&=0xBF;} //RES 6,E
inline void op_code_cb_case0xB4(void) { REG_H&=0xBF;} //RES 6,H
inline void op_code_cb_case0xB5(void) { REG_L&=0xBF;} //RES 6,L
inline void op_code_cb_case0xB7(void) { REG_A&=0xBF;} //RES 6,A

inline void op_code_cb_case0xB8(void) { REG_B&=0x7F;} //RES 7,B
inline void op_code_cb_case0xB9(void) { REG_C&=0x7F;} //RES 7,C
inline void op_code_cb_case0xBA(void) { REG_D&=0x7F;} //RES 7,D
inline void op_code_cb_case0xBB(void) { REG_E&=0x7F;} //RES 7,E
inline void op_code_cb_case0xBC(void) { REG_H&=0x7F;} //RES 7,H
inline void op_code_cb_case0xBD(void) { REG_L&=0x7F;} //RES 7,L
inline void op_code_cb_case0xBF(void) { REG_A&=0x7F;} //RES 7,A

//state 16
inline void op_code_cb_case0x86(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xFE;cpu_write(REG_HL,tmp.b.l);} //RES 0,(HL)
inline void op_code_cb_case0x8E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xFD;cpu_write(REG_HL,tmp.b.l);} //RES 1,(HL)
inline void op_code_cb_case0x96(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xFB;cpu_write(REG_HL,tmp.b.l);} //RES 2,(HL)
inline void op_code_cb_case0x9E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xF7;cpu_write(REG_HL,tmp.b.l);} //RES 3,(HL)
inline void op_code_cb_case0xA6(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xEF;cpu_write(REG_HL,tmp.b.l);} //RES 4,(HL)
inline void op_code_cb_case0xAE(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xDF;cpu_write(REG_HL,tmp.b.l);} //RES 5,(HL)
inline void op_code_cb_case0xB6(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0xBF;cpu_write(REG_HL,tmp.b.l);} //RES 6,(HL)
inline void op_code_cb_case0xBE(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l&=0x7F;cpu_write(REG_HL,tmp.b.l);} //RES 7,(HL)

//shift rotate opcode
//RLC s : 00 000 r : state 8
inline void op_code_cb_case0x00(void) { REG_F=(REG_B>>7);REG_B=(REG_B<<1)|(REG_F);REG_F|=GenZF(REG_B);}//RLC B
inline void op_code_cb_case0x01(void) { REG_F=(REG_C>>7);REG_C=(REG_C<<1)|(REG_F);REG_F|=GenZF(REG_C);}//RLC C
inline void op_code_cb_case0x02(void) { REG_F=(REG_D>>7);REG_D=(REG_D<<1)|(REG_F);REG_F|=GenZF(REG_D);}//RLC D
inline void op_code_cb_case0x03(void) { REG_F=(REG_E>>7);REG_E=(REG_E<<1)|(REG_F);REG_F|=GenZF(REG_E);}//RLC E
inline void op_code_cb_case0x04(void) { REG_F=(REG_H>>7);REG_H=(REG_H<<1)|(REG_F);REG_F|=GenZF(REG_H);}//RLC H
inline void op_code_cb_case0x05(void) { REG_F=(REG_L>>7);REG_L=(REG_L<<1)|(REG_F);REG_F|=GenZF(REG_L);}//RLC L
inline void op_code_cb_case0x07(void) { REG_F=(REG_A>>7);REG_A=(REG_A<<1)|(REG_F);REG_F|=GenZF(REG_A);}//RLC A

inline void op_code_cb_case0x06(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=(tmp.b.l>>7);tmp.b.l=(tmp.b.l<<1)|(REG_F);REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//RLC (HL) : state 16

//RRC s : 00 001 r : state 8
inline void op_code_cb_case0x08(void) { REG_F=(REG_B&0x01);REG_B=(REG_B>>1)|(REG_F<<7);REG_F|=GenZF(REG_B);}//RRC B
inline void op_code_cb_case0x09(void) { REG_F=(REG_C&0x01);REG_C=(REG_C>>1)|(REG_F<<7);REG_F|=GenZF(REG_C);}//RRC C
inline void op_code_cb_case0x0A(void) { REG_F=(REG_D&0x01);REG_D=(REG_D>>1)|(REG_F<<7);REG_F|=GenZF(REG_D);}//RRC D
inline void op_code_cb_case0x0B(void) { REG_F=(REG_E&0x01);REG_E=(REG_E>>1)|(REG_F<<7);REG_F|=GenZF(REG_E);}//RRC E
inline void op_code_cb_case0x0C(void) { REG_F=(REG_H&0x01);REG_H=(REG_H>>1)|(REG_F<<7);REG_F|=GenZF(REG_H);}//RRC H
inline void op_code_cb_case0x0D(void) { REG_F=(REG_L&0x01);REG_L=(REG_L>>1)|(REG_F<<7);REG_F|=GenZF(REG_L);}//RRC L
inline void op_code_cb_case0x0F(void) { REG_F=(REG_A&0x01);REG_A=(REG_A>>1)|(REG_F<<7);REG_F|=GenZF(REG_A);}//RRC A

inline void op_code_cb_case0x0E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=(tmp.b.l&0x01);tmp.b.l=(tmp.b.l>>1)|(REG_F<<7);REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//RRC (HL) :state 16

//RL s : 00 010 r : state 8
inline void op_code_cb_case0x10(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_B>>7);REG_B=(REG_B<<1)|tmp.b.l;REG_F|=GenZF(REG_B);}//RL B
inline void op_code_cb_case0x11(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_C>>7);REG_C=(REG_C<<1)|tmp.b.l;REG_F|=GenZF(REG_C);}//RL C
inline void op_code_cb_case0x12(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_D>>7);REG_D=(REG_D<<1)|tmp.b.l;REG_F|=GenZF(REG_D);}//RL D
inline void op_code_cb_case0x13(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_E>>7);REG_E=(REG_E<<1)|tmp.b.l;REG_F|=GenZF(REG_E);}//RL E
inline void op_code_cb_case0x14(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_H>>7);REG_H=(REG_H<<1)|tmp.b.l;REG_F|=GenZF(REG_H);}//RL H
inline void op_code_cb_case0x15(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_L>>7);REG_L=(REG_L<<1)|tmp.b.l;REG_F|=GenZF(REG_L);}//RL L
inline void op_code_cb_case0x17(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_A>>7);REG_A=(REG_A<<1)|tmp.b.l;REG_F|=GenZF(REG_A);}//RL A

inline void op_code_cb_case0x16(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.h=REG_F&0x01;REG_F=(tmp.b.l>>7);tmp.b.l=(tmp.b.l<<1)|tmp.b.h;REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//RL (HL) :state 16

//RR s : 00 011 r : state 8
inline void op_code_cb_case0x18(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_B&0x01);REG_B=(REG_B>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_B);}//RR B
inline void op_code_cb_case0x19(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_C&0x01);REG_C=(REG_C>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_C);}//RR C
inline void op_code_cb_case0x1A(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_D&0x01);REG_D=(REG_D>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_D);}//RR D
inline void op_code_cb_case0x1B(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_E&0x01);REG_E=(REG_E>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_E);}//RR E
inline void op_code_cb_case0x1C(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_H&0x01);REG_H=(REG_H>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_H);}//RR H
inline void op_code_cb_case0x1D(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_L&0x01);REG_L=(REG_L>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_L);}//RR L
inline void op_code_cb_case0x1F(void) { union pare_reg tmp;tmp.b.l=REG_F&0x01;REG_F=(REG_A&0x01);REG_A=(REG_A>>1)|(tmp.b.l<<7);REG_F|=GenZF(REG_A);}//RR A

inline void op_code_cb_case0x1E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.h=REG_F&0x01;REG_F=(tmp.b.l&0x01);tmp.b.l=(tmp.b.l>>1)|(tmp.b.h<<7);REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//RR (HL) :state 16

//SLA s : 00 100 r : state 8
inline void op_code_cb_case0x20(void) { REG_F=REG_B>>7;REG_B<<=1;REG_F|=GenZF(REG_B);}//SLA B
inline void op_code_cb_case0x21(void) { REG_F=REG_C>>7;REG_C<<=1;REG_F|=GenZF(REG_C);}//SLA C
inline void op_code_cb_case0x22(void) { REG_F=REG_D>>7;REG_D<<=1;REG_F|=GenZF(REG_D);}//SLA D
inline void op_code_cb_case0x23(void) { REG_F=REG_E>>7;REG_E<<=1;REG_F|=GenZF(REG_E);}//SLA E
inline void op_code_cb_case0x24(void) { REG_F=REG_H>>7;REG_H<<=1;REG_F|=GenZF(REG_H);}//SLA H
inline void op_code_cb_case0x25(void) { REG_F=REG_L>>7;REG_L<<=1;REG_F|=GenZF(REG_L);}//SLA L
inline void op_code_cb_case0x27(void) { REG_F=REG_A>>7;REG_A<<=1;REG_F|=GenZF(REG_A);}//SLA A

inline void op_code_cb_case0x26(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=tmp.b.l>>7;tmp.b.l<<=1;REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//SLA (HL) :state 16

//SRA s : 00 101 r : state 8
inline void op_code_cb_case0x28(void) { REG_F=REG_B&0x01;REG_B=(REG_B>>1)|(REG_B&0x80);REG_F|=GenZF(REG_B);}//SRA B
inline void op_code_cb_case0x29(void) { REG_F=REG_C&0x01;REG_C=(REG_C>>1)|(REG_C&0x80);REG_F|=GenZF(REG_C);}//SRA C
inline void op_code_cb_case0x2A(void) { REG_F=REG_D&0x01;REG_D=(REG_D>>1)|(REG_D&0x80);REG_F|=GenZF(REG_D);}//SRA D
inline void op_code_cb_case0x2B(void) { REG_F=REG_E&0x01;REG_E=(REG_E>>1)|(REG_E&0x80);REG_F|=GenZF(REG_E);}//SRA E
inline void op_code_cb_case0x2C(void) { REG_F=REG_H&0x01;REG_H=(REG_H>>1)|(REG_H&0x80);REG_F|=GenZF(REG_H);}//SRA H
inline void op_code_cb_case0x2D(void) { REG_F=REG_L&0x01;REG_L=(REG_L>>1)|(REG_L&0x80);REG_F|=GenZF(REG_L);}//SRA L
inline void op_code_cb_case0x2F(void) { REG_F=REG_A&0x01;REG_A=(REG_A>>1)|(REG_A&0x80);REG_F|=GenZF(REG_A);}//SRA A

inline void op_code_cb_case0x2E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=tmp.b.l&0x01;tmp.b.l>>=1;tmp.b.l|=(tmp.b.l<<1)&0x80;REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//SRA (HL) :state 16

//SRL s : 00 111 r : state 8
inline void op_code_cb_case0x38(void) { REG_F=REG_B&0x01;REG_B>>=1;REG_F|=GenZF(REG_B);}//SRL B
inline void op_code_cb_case0x39(void) { REG_F=REG_C&0x01;REG_C>>=1;REG_F|=GenZF(REG_C);}//SRL C
inline void op_code_cb_case0x3A(void) { REG_F=REG_D&0x01;REG_D>>=1;REG_F|=GenZF(REG_D);}//SRL D
inline void op_code_cb_case0x3B(void) { REG_F=REG_E&0x01;REG_E>>=1;REG_F|=GenZF(REG_E);}//SRL E
inline void op_code_cb_case0x3C(void) { REG_F=REG_H&0x01;REG_H>>=1;REG_F|=GenZF(REG_H);}//SRL H
inline void op_code_cb_case0x3D(void) { REG_F=REG_L&0x01;REG_L>>=1;REG_F|=GenZF(REG_L);}//SRL L
inline void op_code_cb_case0x3F(void) { REG_F=REG_A&0x01;REG_A>>=1;REG_F|=GenZF(REG_A);}//SRL A

inline void op_code_cb_case0x3E(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);REG_F=tmp.b.l&0x01;tmp.b.l>>=1;REG_F|=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//SRL (HL) :state 16

//swap opcode
//SWAP n : 00 110 r :state 8
inline void op_code_cb_case0x30(void) { REG_B=(REG_B>>4)|(REG_B<<4);REG_F=GenZF(REG_B);}//SWAP B
inline void op_code_cb_case0x31(void) { REG_C=(REG_C>>4)|(REG_C<<4);REG_F=GenZF(REG_C);}//SWAP C
inline void op_code_cb_case0x32(void) { REG_D=(REG_D>>4)|(REG_D<<4);REG_F=GenZF(REG_D);}//SWAP D
inline void op_code_cb_case0x33(void) { REG_E=(REG_E>>4)|(REG_E<<4);REG_F=GenZF(REG_E);}//SWAP E
inline void op_code_cb_case0x34(void) { REG_H=(REG_H>>4)|(REG_H<<4);REG_F=GenZF(REG_H);}//SWAP H
inline void op_code_cb_case0x35(void) { REG_L=(REG_L>>4)|(REG_L<<4);REG_F=GenZF(REG_L);}//SWAP L
inline void op_code_cb_case0x37(void) { REG_A=(REG_A>>4)|(REG_A<<4);REG_F=GenZF(REG_A);}//SWAP A

inline void op_code_cb_case0x36(void) { union pare_reg tmp;tmp.b.l=cpu_read(REG_HL);tmp.b.l=(tmp.b.l>>4)|(tmp.b.l<<4);REG_F=GenZF(tmp.b.l);cpu_write(REG_HL,tmp.b.l);}//SWAP (HL) : state 16

// cb op_code Jump Table
void (*op_code_cb_JmpTbl[256])(void) = {
	op_code_cb_case0x00, op_code_cb_case0x01, op_code_cb_case0x02, op_code_cb_case0x03, op_code_cb_case0x04,
	op_code_cb_case0x05, op_code_cb_case0x06, op_code_cb_case0x07, op_code_cb_case0x08, op_code_cb_case0x09,
	op_code_cb_case0x0A, op_code_cb_case0x0B, op_code_cb_case0x0C, op_code_cb_case0x0D, op_code_cb_case0x0E,
	op_code_cb_case0x0F,
	op_code_cb_case0x10, op_code_cb_case0x11, op_code_cb_case0x12, op_code_cb_case0x13, op_code_cb_case0x14,
	op_code_cb_case0x15, op_code_cb_case0x16, op_code_cb_case0x17, op_code_cb_case0x18, op_code_cb_case0x19,
	op_code_cb_case0x1A, op_code_cb_case0x1B, op_code_cb_case0x1C, op_code_cb_case0x1D, op_code_cb_case0x1E,
	op_code_cb_case0x1F,
	op_code_cb_case0x20, op_code_cb_case0x21, op_code_cb_case0x22, op_code_cb_case0x23, op_code_cb_case0x24,
	op_code_cb_case0x25, op_code_cb_case0x26, op_code_cb_case0x27, op_code_cb_case0x28, op_code_cb_case0x29,
	op_code_cb_case0x2A, op_code_cb_case0x2B, op_code_cb_case0x2C, op_code_cb_case0x2D, op_code_cb_case0x2E,
	op_code_cb_case0x2F,
	op_code_cb_case0x30, op_code_cb_case0x31, op_code_cb_case0x32, op_code_cb_case0x33, op_code_cb_case0x34,
	op_code_cb_case0x35, op_code_cb_case0x36, op_code_cb_case0x37, op_code_cb_case0x38, op_code_cb_case0x39,
	op_code_cb_case0x3A, op_code_cb_case0x3B, op_code_cb_case0x3C, op_code_cb_case0x3D, op_code_cb_case0x3E,
	op_code_cb_case0x3F,
	op_code_cb_case0x40, op_code_cb_case0x41, op_code_cb_case0x42, op_code_cb_case0x43, op_code_cb_case0x44,
	op_code_cb_case0x45, op_code_cb_case0x46, op_code_cb_case0x47, op_code_cb_case0x48, op_code_cb_case0x49,
	op_code_cb_case0x4A, op_code_cb_case0x4B, op_code_cb_case0x4C, op_code_cb_case0x4D, op_code_cb_case0x4E,
	op_code_cb_case0x4F,
	op_code_cb_case0x50, op_code_cb_case0x51, op_code_cb_case0x52, op_code_cb_case0x53, op_code_cb_case0x54,
	op_code_cb_case0x55, op_code_cb_case0x56, op_code_cb_case0x57, op_code_cb_case0x58, op_code_cb_case0x59,
	op_code_cb_case0x5A, op_code_cb_case0x5B, op_code_cb_case0x5C, op_code_cb_case0x5D, op_code_cb_case0x5E,
	op_code_cb_case0x5F,
	op_code_cb_case0x60, op_code_cb_case0x61, op_code_cb_case0x62, op_code_cb_case0x63, op_code_cb_case0x64,
	op_code_cb_case0x65, op_code_cb_case0x66, op_code_cb_case0x67, op_code_cb_case0x68, op_code_cb_case0x69,
	op_code_cb_case0x6A, op_code_cb_case0x6B, op_code_cb_case0x6C, op_code_cb_case0x6D, op_code_cb_case0x6E,
	op_code_cb_case0x6F,
	op_code_cb_case0x70, op_code_cb_case0x71, op_code_cb_case0x72, op_code_cb_case0x73, op_code_cb_case0x74,
	op_code_cb_case0x75, op_code_cb_case0x76, op_code_cb_case0x77, op_code_cb_case0x78, op_code_cb_case0x79,
	op_code_cb_case0x7A, op_code_cb_case0x7B, op_code_cb_case0x7C, op_code_cb_case0x7D, op_code_cb_case0x7E,
	op_code_cb_case0x7F,
	op_code_cb_case0x80, op_code_cb_case0x81, op_code_cb_case0x82, op_code_cb_case0x83, op_code_cb_case0x84,
	op_code_cb_case0x85, op_code_cb_case0x86, op_code_cb_case0x87, op_code_cb_case0x88, op_code_cb_case0x89,
	op_code_cb_case0x8A, op_code_cb_case0x8B, op_code_cb_case0x8C, op_code_cb_case0x8D, op_code_cb_case0x8E,
	op_code_cb_case0x8F,
	op_code_cb_case0x90, op_code_cb_case0x91, op_code_cb_case0x92, op_code_cb_case0x93, op_code_cb_case0x94,
	op_code_cb_case0x95, op_code_cb_case0x96, op_code_cb_case0x97, op_code_cb_case0x98, op_code_cb_case0x99,
	op_code_cb_case0x9A, op_code_cb_case0x9B, op_code_cb_case0x9C, op_code_cb_case0x9D, op_code_cb_case0x9E,
	op_code_cb_case0x9F,
	op_code_cb_case0xA0, op_code_cb_case0xA1, op_code_cb_case0xA2, op_code_cb_case0xA3, op_code_cb_case0xA4,
	op_code_cb_case0xA5, op_code_cb_case0xA6, op_code_cb_case0xA7, op_code_cb_case0xA8, op_code_cb_case0xA9,
	op_code_cb_case0xAA, op_code_cb_case0xAB, op_code_cb_case0xAC, op_code_cb_case0xAD, op_code_cb_case0xAE,
	op_code_cb_case0xAF,
	op_code_cb_case0xB0, op_code_cb_case0xB1, op_code_cb_case0xB2, op_code_cb_case0xB3, op_code_cb_case0xB4,
	op_code_cb_case0xB5, op_code_cb_case0xB6, op_code_cb_case0xB7, op_code_cb_case0xB8, op_code_cb_case0xB9,
	op_code_cb_case0xBA, op_code_cb_case0xBB, op_code_cb_case0xBC, op_code_cb_case0xBD, op_code_cb_case0xBE,
	op_code_cb_case0xBF,
	op_code_cb_case0xC0, op_code_cb_case0xC1, op_code_cb_case0xC2, op_code_cb_case0xC3, op_code_cb_case0xC4,
	op_code_cb_case0xC5, op_code_cb_case0xC6, op_code_cb_case0xC7, op_code_cb_case0xC8, op_code_cb_case0xC9,
	op_code_cb_case0xCA, op_code_cb_case0xCB, op_code_cb_case0xCC, op_code_cb_case0xCD, op_code_cb_case0xCE,
	op_code_cb_case0xCF,
	op_code_cb_case0xD0, op_code_cb_case0xD1, op_code_cb_case0xD2, op_code_cb_case0xD3, op_code_cb_case0xD4,
	op_code_cb_case0xD5, op_code_cb_case0xD6, op_code_cb_case0xD7, op_code_cb_case0xD8, op_code_cb_case0xD9,
	op_code_cb_case0xDA, op_code_cb_case0xDB, op_code_cb_case0xDC, op_code_cb_case0xDD, op_code_cb_case0xDE,
	op_code_cb_case0xDF,
	op_code_cb_case0xE0, op_code_cb_case0xE1, op_code_cb_case0xE2, op_code_cb_case0xE3, op_code_cb_case0xE4,
	op_code_cb_case0xE5, op_code_cb_case0xE6, op_code_cb_case0xE7, op_code_cb_case0xE8, op_code_cb_case0xE9,
	op_code_cb_case0xEA, op_code_cb_case0xEB, op_code_cb_case0xEC, op_code_cb_case0xED, op_code_cb_case0xEE,
	op_code_cb_case0xEF,
	op_code_cb_case0xF0, op_code_cb_case0xF1, op_code_cb_case0xF2, op_code_cb_case0xF3, op_code_cb_case0xF4,
	op_code_cb_case0xF5, op_code_cb_case0xF6, op_code_cb_case0xF7, op_code_cb_case0xF8, op_code_cb_case0xF9,
	op_code_cb_case0xFA, op_code_cb_case0xFB, op_code_cb_case0xFC, op_code_cb_case0xFD, op_code_cb_case0xFE,
	op_code_cb_case0xFF
};

