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

//--------------------------------------------

// GB orginal op_code

inline void op_code_normal_case0x08(void) { writew(op_readw(),REG_SP);} //LD (mn),SP
inline void op_code_normal_case0x10(void) { if (speed_change) { speed_change=false;speed^=1;REG_PC++;/* 1バイト読み飛ばす */ } else { halt=true;REG_PC--; cpu_irq_check(); }} //STOP(HALT?)

//0x2A LD A,(mn) -> LD A,(HLI) Load A from (HL) and decrement HL
inline void op_code_normal_case0x2A(void) { REG_A=cpu_read(REG_HL);REG_HL++;} // LD A,(HLI) : 00 111 010 :state 13

//0x22 LD (mn),A -> LD (HLI),A Save A at (HL) and decrement HL
inline void op_code_normal_case0x22(void) { cpu_write(REG_HL,REG_A);REG_HL++;} // LD (HLI),A : 00 110 010 :state 13

//0x3A LD A,(mn) -> LD A,(HLD) Load A from (HL) and decrement HL
inline void op_code_normal_case0x3A(void) { REG_A=cpu_read(REG_HL);REG_HL--;} // LD A,(HLD) : 00 111 010 :state 13

//0x32 LD (mn),A -> LD (HLD),A Save A at (HL) and decrement HL
inline void op_code_normal_case0x32(void) { cpu_write(REG_HL,REG_A);REG_HL--;} // LD (HLD),A : 00 110 010 :state 13

inline void op_code_normal_case0xD9(void) { /*Log("Return Interrupts.\n");*/c_regs_I=1;REG_PC=readw(REG_SP);REG_SP+=2;int_disable_next=true;/*;g_regs.IF=0*/;/*res->system_reg.IF&=~Int_hist[(Int_depth>0)?--Int_depth:Int_depth]*//*Int_depth=((Int_depth>0)?--Int_depth:Int_depth);*//*res->system_reg.IF=0;*//*Log("RETI %d\n",Int_depth);*/ cpu_irq_check(); }//RETI state 16
inline void op_code_normal_case0xE0(void) { cpu_write(0xFF00+op_read(),REG_A);}//LDH (n),A
inline void op_code_normal_case0xE2(void) { cpu_write(0xFF00+REG_C,REG_A);}//LDH (C),A
inline void op_code_normal_case0xE8(void) { REG_SP+=(signed char)op_read();}//ADD SP,n
inline void op_code_normal_case0xEA(void) { cpu_write(op_readw(),REG_A);}//LD (mn),A

inline void op_code_normal_case0xF0(void) { REG_A=cpu_read(0xFF00+op_read());}//LDH A,(n)
inline void op_code_normal_case0xF2(void) { REG_A=cpu_read(0xFF00+REG_C);}//LDH A,(c)
inline void op_code_normal_case0xF8(void) { REG_HL=REG_SP+(signed char)op_read();}//LD HL,SP+n 
inline void op_code_normal_case0xFA(void) { REG_A=cpu_read(op_readw());}//LD A,(mn);

// 8bit load op_code

// regs B 000 C 001 D 010 E 011 H 100 L 101 A 111
//LD r,s  :01 r s :state 4(clocks)

inline void op_code_normal_case0x40(void) { } // LD B,B
inline void op_code_normal_case0x41(void) { REG_B=REG_C;} // LD B,C
inline void op_code_normal_case0x42(void) { REG_B=REG_D;} // LD B,D
inline void op_code_normal_case0x43(void) { REG_B=REG_E;} // LD B,E
inline void op_code_normal_case0x44(void) { REG_B=REG_H;} // LD B,H
inline void op_code_normal_case0x45(void) { REG_B=REG_L;} // LD B,L
inline void op_code_normal_case0x47(void) { REG_B=REG_A;} // LD B,A

inline void op_code_normal_case0x48(void) { REG_C=REG_B;} // LD C,B
inline void op_code_normal_case0x49(void) { } // LD C,C
inline void op_code_normal_case0x4A(void) { REG_C=REG_D;} // LD C,D
inline void op_code_normal_case0x4B(void) { REG_C=REG_E;} // LD C,E
inline void op_code_normal_case0x4C(void) { REG_C=REG_H;} // LD C,H
inline void op_code_normal_case0x4D(void) { REG_C=REG_L;} // LD C,L
inline void op_code_normal_case0x4F(void) { REG_C=REG_A;} // LD C,A

inline void op_code_normal_case0x50(void) { REG_D=REG_B;} // LD D,B
inline void op_code_normal_case0x51(void) { REG_D=REG_C;} // LD D,C
inline void op_code_normal_case0x52(void) { } // LD D,D
inline void op_code_normal_case0x53(void) { REG_D=REG_E;} // LD D,E
inline void op_code_normal_case0x54(void) { REG_D=REG_H;} // LD D,H
inline void op_code_normal_case0x55(void) { REG_D=REG_L;} // LD D,L
inline void op_code_normal_case0x57(void) { REG_D=REG_A;} // LD D,A

inline void op_code_normal_case0x58(void) { REG_E=REG_B;} // LD E,B
inline void op_code_normal_case0x59(void) { REG_E=REG_C;} // LD E,C
inline void op_code_normal_case0x5A(void) { REG_E=REG_D;} // LD E,D
inline void op_code_normal_case0x5B(void) { } // LD E,E
inline void op_code_normal_case0x5C(void) { REG_E=REG_H;} // LD E,H
inline void op_code_normal_case0x5D(void) { REG_E=REG_L;} // LD E,L
inline void op_code_normal_case0x5F(void) { REG_E=REG_A;} // LD E,A

inline void op_code_normal_case0x60(void) { REG_H=REG_B;} // LD H,B
inline void op_code_normal_case0x61(void) { REG_H=REG_C;} // LD H,C
inline void op_code_normal_case0x62(void) { REG_H=REG_D;} // LD H,D
inline void op_code_normal_case0x63(void) { REG_H=REG_E;} // LD H,E
inline void op_code_normal_case0x64(void) { } // LD H,H
inline void op_code_normal_case0x65(void) { REG_H=REG_L;} // LD H,L
inline void op_code_normal_case0x67(void) { REG_H=REG_A;} // LD H,A

inline void op_code_normal_case0x68(void) { REG_L=REG_B;} // LD L,B
inline void op_code_normal_case0x69(void) { REG_L=REG_C;} // LD L,C
inline void op_code_normal_case0x6A(void) { REG_L=REG_D;} // LD L,D
inline void op_code_normal_case0x6B(void) { REG_L=REG_E;} // LD L,E
inline void op_code_normal_case0x6C(void) { REG_L=REG_H;} // LD L,H
inline void op_code_normal_case0x6D(void) { } // LD L,L
inline void op_code_normal_case0x6F(void) { REG_L=REG_A;} // LD L,A

inline void op_code_normal_case0x78(void) { REG_A=REG_B;} // LD A,B
inline void op_code_normal_case0x79(void) { REG_A=REG_C;} // LD A,C
inline void op_code_normal_case0x7A(void) { REG_A=REG_D;} // LD A,D
inline void op_code_normal_case0x7B(void) { REG_A=REG_E;} // LD A,E
inline void op_code_normal_case0x7C(void) { REG_A=REG_H;} // LD A,H
inline void op_code_normal_case0x7D(void) { REG_A=REG_L;} // LD A,L
inline void op_code_normal_case0x7F(void) { } // LD A,A

//LD r,n :00 r 110 n :state 7
inline void op_code_normal_case0x06(void) { REG_B=op_read();} // LD B,n
inline void op_code_normal_case0x0E(void) { REG_C=op_read();} // LD C,n
inline void op_code_normal_case0x16(void) { REG_D=op_read();} // LD D,n
inline void op_code_normal_case0x1E(void) { REG_E=op_read();} // LD E,n
inline void op_code_normal_case0x26(void) { REG_H=op_read();} // LD H,n
inline void op_code_normal_case0x2E(void) { REG_L=op_read();} // LD L,n
inline void op_code_normal_case0x3E(void) { REG_A=op_read();} // LD A,n

//LD r,(HL) :01 r 110 :state 7
inline void op_code_normal_case0x46(void) { REG_B=cpu_read(REG_HL);} // LD B,(HL)
inline void op_code_normal_case0x4E(void) { REG_C=cpu_read(REG_HL);} // LD C,(HL)
inline void op_code_normal_case0x56(void) { REG_D=cpu_read(REG_HL);} // LD D,(HL)
inline void op_code_normal_case0x5E(void) { REG_E=cpu_read(REG_HL);} // LD E,(HL)
inline void op_code_normal_case0x66(void) { REG_H=cpu_read(REG_HL);} // LD H,(HL)
inline void op_code_normal_case0x6E(void) { REG_L=cpu_read(REG_HL);} // LD L,(HL)
inline void op_code_normal_case0x7E(void) { REG_A=cpu_read(REG_HL);} // LD A,(HL)

//LD (HL),r :01 110 r :state 7
inline void op_code_normal_case0x70(void) { cpu_write(REG_HL,REG_B);} // LD (HL),B
inline void op_code_normal_case0x71(void) { cpu_write(REG_HL,REG_C);} // LD (HL),C
inline void op_code_normal_case0x72(void) { cpu_write(REG_HL,REG_D);} // LD (HL),D
inline void op_code_normal_case0x73(void) { cpu_write(REG_HL,REG_E);} // LD (HL),E
inline void op_code_normal_case0x74(void) { cpu_write(REG_HL,REG_H);} // LD (HL),H
inline void op_code_normal_case0x75(void) { cpu_write(REG_HL,REG_L);} // LD (HL),L
inline void op_code_normal_case0x77(void) { cpu_write(REG_HL,REG_A);} // LD (HL),A

inline void op_code_normal_case0x36(void) { cpu_write(REG_HL,op_read());} // LD (HL),n :00 110 110 :state 10
inline void op_code_normal_case0x0A(void) { REG_A=cpu_read(REG_BC);} // LD A,(BC) :00 001 010 :state 7
inline void op_code_normal_case0x1A(void) { REG_A=cpu_read(REG_DE);} // LD A,(DE) :00 011 010 : state 7
inline void op_code_normal_case0x02(void) { cpu_write(REG_BC,REG_A);} // LD (BC),A : 00 000 010 :state 7
inline void op_code_normal_case0x12(void) { cpu_write(REG_DE,REG_A);} // LD (DE),A : 00 010 010 :state 7

//16bit load opcode
//rp Pair Reg 00 BC 01 DE 10 HL 11 SP

//LD rp,mn : 00 rp0 001 n m :state 10
inline void op_code_normal_case0x01(void) { REG_BC=op_readw();} //LD BC,(mn)
inline void op_code_normal_case0x11(void) { REG_DE=op_readw();} //LD DE,(mn)
inline void op_code_normal_case0x21(void) { REG_HL=op_readw();} //LD HL,(mn)
inline void op_code_normal_case0x31(void) { REG_SP=op_readw();} //LD SP,(mn)

inline void op_code_normal_case0xF9(void) { REG_SP=REG_HL;} //LD SP,HL : 11 111 001 :state 6

//stack opcode
//rq Pair Reg 00 BC 01 DE 10 HL 11 AF

//PUSH rq : 11 rq0 101 : state 11(16?)
inline void op_code_normal_case0xC5(void) { REG_SP-=2;writew(REG_SP,REG_BC);} //PUSH BC
inline void op_code_normal_case0xD5(void) { REG_SP-=2;writew(REG_SP,REG_DE);} //PUSH DE
inline void op_code_normal_case0xE5(void) { REG_SP-=2;writew(REG_SP,REG_HL);} //PUSH HL
inline void op_code_normal_case0xF5(void) { cpu_write(REG_SP-2,z802gb[REG_F]|0xe);cpu_write(REG_SP-1,REG_A);REG_SP-=2;} //PUSH AF // 未使用ビットは1になるみたい(メタルギアより)

//POP rq : 11 rq0 001 : state 10 (12?)
inline void op_code_normal_case0xC1(void) { REG_B=cpu_read(REG_SP+1);REG_C=cpu_read(REG_SP);REG_SP+=2;} //POP BC
inline void op_code_normal_case0xD1(void) { REG_D=cpu_read(REG_SP+1);REG_E=cpu_read(REG_SP);REG_SP+=2;} //POP DE
inline void op_code_normal_case0xE1(void) { REG_H=cpu_read(REG_SP+1);REG_L=cpu_read(REG_SP);REG_SP+=2;} //POP HL
inline void op_code_normal_case0xF1(void) { REG_A=cpu_read(REG_SP+1);REG_F=gb2z80[cpu_read(REG_SP)&0xf0];REG_SP+=2;} //POP AF

//8bit arithmetic/logical opcode
//regs B 000 C 001 D 010 E 011 H 100 L 101 A 111

//ADD A,r : 10 000 r : state 4
inline void op_code_normal_case0x80(void) { ADD(REG_B);} //ADD A,B
inline void op_code_normal_case0x81(void) { ADD(REG_C);} //ADD A,C
inline void op_code_normal_case0x82(void) { ADD(REG_D);} //ADD A,D
inline void op_code_normal_case0x83(void) { ADD(REG_E);} //ADD A,E
inline void op_code_normal_case0x84(void) { ADD(REG_H);} //ADD A,H
inline void op_code_normal_case0x85(void) { ADD(REG_L);} //ADD A,L
inline void op_code_normal_case0x87(void) { ADD(REG_A);} //ADD A,A

inline void op_code_normal_case0xC6(void) { byte tmpb;tmpb=op_read();ADD(tmpb);} //ADD A,n : 11 000 110 :state 7
inline void op_code_normal_case0x86(void) { byte tmpb;tmpb=cpu_read(REG_HL);ADD(tmpb);} //ADD A,(HL) : 10 000 110 :state 7

//ADC A,r : 10 001 r : state 4
inline void op_code_normal_case0x88(void) { ADC(REG_B);} //ADC A,B
inline void op_code_normal_case0x89(void) { ADC(REG_C);} //ADC A,C
inline void op_code_normal_case0x8A(void) { ADC(REG_D);} //ADC A,D
inline void op_code_normal_case0x8B(void) { ADC(REG_E);} //ADC A,E
inline void op_code_normal_case0x8C(void) { ADC(REG_H);} //ADC A,H
inline void op_code_normal_case0x8D(void) { ADC(REG_L);} //ADC A,L
inline void op_code_normal_case0x8F(void) { ADC(REG_A);} //ADC A,A

inline void op_code_normal_case0xCE(void) { byte tmpb;tmpb=op_read();ADC(tmpb);} //ADC A,n : 11 001 110 :state 7
inline void op_code_normal_case0x8E(void) { byte tmpb;tmpb=cpu_read(REG_HL);ADC(tmpb);} //ADC A,(HL) : 10 001 110 :state 7

//SUB A,r : 10 010 r : state 4
inline void op_code_normal_case0x90(void) { SUB(REG_B);} //SUB A,B
inline void op_code_normal_case0x91(void) { SUB(REG_C);} //SUB A,C
inline void op_code_normal_case0x92(void) { SUB(REG_D);} //SUB A,D
inline void op_code_normal_case0x93(void) { SUB(REG_E);} //SUB A,E
inline void op_code_normal_case0x94(void) { SUB(REG_H);} //SUB A,H
inline void op_code_normal_case0x95(void) { SUB(REG_L);} //SUB A,L
inline void op_code_normal_case0x97(void) { SUB(REG_A);} //SUB A,A

inline void op_code_normal_case0xD6(void) { byte tmpb;tmpb=op_read();SUB(tmpb);} //SUB A,n : 11 010 110 :state 7
inline void op_code_normal_case0x96(void) { byte tmpb;tmpb=cpu_read(REG_HL);SUB(tmpb);} //SUB A,(HL) : 10 010 110 :state 7

//SBC A,r : 10 011 r : state 4
inline void op_code_normal_case0x98(void) { SBC(REG_B);} //SBC A,B
inline void op_code_normal_case0x99(void) { SBC(REG_C);} //SBC A,C
inline void op_code_normal_case0x9A(void) { SBC(REG_D);} //SBC A,D
inline void op_code_normal_case0x9B(void) { SBC(REG_E);} //SBC A,E
inline void op_code_normal_case0x9C(void) { SBC(REG_H);} //SBC A,H
inline void op_code_normal_case0x9D(void) { SBC(REG_L);} //SBC A,L
inline void op_code_normal_case0x9F(void) { SBC(REG_A);} //SBC A,A

inline void op_code_normal_case0xDE(void) { byte tmpb;tmpb=op_read();SBC(tmpb);} //SBC A,n : 11 011 110 :state 7
inline void op_code_normal_case0x9E(void) { byte tmpb;tmpb=cpu_read(REG_HL);SBC(tmpb);} //SBC A,(HL) : 10 011 110 :state 7

//AND A,r : 10 100 r : state 4
inline void op_code_normal_case0xA0(void) { AND(REG_B);} //AND A,B
inline void op_code_normal_case0xA1(void) { AND(REG_C);} //AND A,C
inline void op_code_normal_case0xA2(void) { AND(REG_D);} //AND A,D
inline void op_code_normal_case0xA3(void) { AND(REG_E);} //AND A,E
inline void op_code_normal_case0xA4(void) { AND(REG_H);} //AND A,H
inline void op_code_normal_case0xA5(void) { AND(REG_L);} //AND A,L
inline void op_code_normal_case0xA7(void) { AND(REG_A);} //AND A,A

inline void op_code_normal_case0xE6(void) { byte tmpb;tmpb=op_read();AND(tmpb);} //AND A,n : 11 100 110 :state 7
inline void op_code_normal_case0xA6(void) { byte tmpb;tmpb=cpu_read(REG_HL);AND(tmpb);} //AND A,(HL) : 10 100 110 :state 7

//XOR A,r : 10 101 r : state 4
inline void op_code_normal_case0xA8(void) { XOR(REG_B);} //XOR A,B
inline void op_code_normal_case0xA9(void) { XOR(REG_C);} //XOR A,C
inline void op_code_normal_case0xAA(void) { XOR(REG_D);} //XOR A,D
inline void op_code_normal_case0xAB(void) { XOR(REG_E);} //XOR A,E
inline void op_code_normal_case0xAC(void) { XOR(REG_H);} //XOR A,H
inline void op_code_normal_case0xAD(void) { XOR(REG_L);} //XOR A,L
inline void op_code_normal_case0xAF(void) { XOR(REG_A);} //XOR A,A

inline void op_code_normal_case0xEE(void) { byte tmpb;tmpb=op_read();XOR(tmpb);} //XOR A,n : 11 101 110 :state 7
inline void op_code_normal_case0xAE(void) { byte tmpb;tmpb=cpu_read(REG_HL);XOR(tmpb);} //XOR A,(HL) : 10 101 110 :state 7

//OR A,r : 10 110 r : state 4
inline void op_code_normal_case0xB0(void) { OR(REG_B);} //OR A,B
inline void op_code_normal_case0xB1(void) { OR(REG_C);} //OR A,C
inline void op_code_normal_case0xB2(void) { OR(REG_D);} //OR A,D
inline void op_code_normal_case0xB3(void) { OR(REG_E);} //OR A,E
inline void op_code_normal_case0xB4(void) { OR(REG_H);} //OR A,H
inline void op_code_normal_case0xB5(void) { OR(REG_L);} //OR A,L
inline void op_code_normal_case0xB7(void) { OR(REG_A);} //OR A,A

inline void op_code_normal_case0xF6(void) { byte tmpb;tmpb=op_read();OR(tmpb);} //OR A,n : 11 110 110 :state 7
inline void op_code_normal_case0xB6(void) { byte tmpb;tmpb=cpu_read(REG_HL);OR(tmpb);} //OR A,(HL) : 10 110 110 :state 7

//CP A,r : 10 111 r : state 4
inline void op_code_normal_case0xB8(void) { CP(REG_B);} //CP A,B
inline void op_code_normal_case0xB9(void) { CP(REG_C);} //CP A,C
inline void op_code_normal_case0xBA(void) { CP(REG_D);} //CP A,D
inline void op_code_normal_case0xBB(void) { CP(REG_E);} //CP A,E
inline void op_code_normal_case0xBC(void) { CP(REG_H);} //CP A,H
inline void op_code_normal_case0xBD(void) { CP(REG_L);} //CP A,L
inline void op_code_normal_case0xBF(void) { CP(REG_A);} //CP A,A

inline void op_code_normal_case0xFE(void) { byte tmpb;tmpb=op_read();CP(tmpb);} //CP A,n : 11 111 110 :state 7
inline void op_code_normal_case0xBE(void) { byte tmpb;tmpb=cpu_read(REG_HL);CP(tmpb);} //CP A,(HL) : 10 111 110 :state 7

//INC r : 00 r 100 : state 4
inline void op_code_normal_case0x04(void) { INC(REG_B);} //INC B
inline void op_code_normal_case0x0C(void) { INC(REG_C);} //INC C
inline void op_code_normal_case0x14(void) { INC(REG_D);} //INC D
inline void op_code_normal_case0x1C(void) { INC(REG_E);} //INC E
inline void op_code_normal_case0x24(void) { INC(REG_H);} //INC H
inline void op_code_normal_case0x2C(void) { INC(REG_L);} //INC L
inline void op_code_normal_case0x3C(void) { INC(REG_A);} //INC A
inline void op_code_normal_case0x34(void) { byte tmpb;tmpb=cpu_read(REG_HL);INC(tmpb);cpu_write(REG_HL,tmpb);} //INC (HL) : 00 110 100 : state 11

//DEC r : 00 r 101 : state 4
inline void op_code_normal_case0x05(void) { DEC(REG_B);} //DEC B
inline void op_code_normal_case0x0D(void) { DEC(REG_C);} //DEC C
inline void op_code_normal_case0x15(void) { DEC(REG_D);} //DEC D
inline void op_code_normal_case0x1D(void) { DEC(REG_E);} //DEC E
inline void op_code_normal_case0x25(void) { DEC(REG_H);} //DEC H
inline void op_code_normal_case0x2D(void) { DEC(REG_L);} //DEC L
inline void op_code_normal_case0x3D(void) { DEC(REG_A);} //DEC A
inline void op_code_normal_case0x35(void) { byte tmpb;tmpb=cpu_read(REG_HL);DEC(tmpb);cpu_write(REG_HL,tmpb);} //DEC (HL) : 00 110 101 : state 11

//16bit arismetic opcode
//rp Pair Reg 00 BC 01 DE 10 HL 11 SP

//ADD HL,BC : 00 rp1 001 :state 11
inline void op_code_normal_case0x09(void) { ADDW(REG_BC);} //ADD HL,BC
inline void op_code_normal_case0x19(void) { ADDW(REG_DE);} //ADD HL,DE
inline void op_code_normal_case0x29(void) { ADDW(REG_HL);} //ADD HL,HL
inline void op_code_normal_case0x39(void) { ADDW(REG_SP);} //ADD HL,SP

//INC BC : 00 rp0 011 :state 11
inline void op_code_normal_case0x03(void) { REG_BC++;;} //INC BC
inline void op_code_normal_case0x13(void) { REG_DE++;} //INC DE
inline void op_code_normal_case0x23(void) { REG_HL++;} //INC HL
inline void op_code_normal_case0x33(void) { REG_SP++;} //INC SP

//DEC BC : 00 rp1 011 :state 11
inline void op_code_normal_case0x0B(void) { REG_BC--;} //DEC BC
inline void op_code_normal_case0x1B(void) { REG_DE--;} //DEC DE
inline void op_code_normal_case0x2B(void) { REG_HL--;} //DEC HL
inline void op_code_normal_case0x3B(void) { REG_SP--;} //DEC SP

//汎用：CPU制御 opcode

/*case 0x27://DAA :state 4
	tmp.b.h=REG_A&0x0F;
	tmp.w=(REG_F&N_FLAG)?
		((REG_F&C_FLAG)?(((REG_F&H_FLAG)?0x9A00:0xA000)+C_FLAG):((REG_F&H_FLAG)?0xFA00:0x0000)):
		((REG_F&C_FLAG)?(((REG_F&H_FLAG)?0x6600:((tmp.b.h<0x0A)?0x6000:0x6600))+C_FLAG):
		((REG_F&H_FLAG)?((REG_A<0xA0)?0x0600:(0x6600+C_FLAG)):((tmp.b.h<0x0A)?((REG_A<0xA0)?0x0000:(0x6000+C_FLAG)):
		((REG_F<0x90)?0x600:(0x6600+C_FLAG)))));
	REG_A+=tmp.b.h;
	REG_F=ZTable[REG_A]|(tmp.b.l|(REG_F&N_FLAG));
	break;
*/
inline void op_code_normal_case0x27(void) {//DAA :state 4
  union pare_reg tmp;
  tmp.b.h=REG_A&0x0F;
  tmp.w=(REG_F&N_FLAG)?
  (
    (REG_F&C_FLAG)?
      (((REG_F&H_FLAG)? 0x9A00:0xA000)+C_FLAG):
      ((REG_F&H_FLAG)? 0xFA00:0x0000)
  )
  :
  (
    (REG_F&C_FLAG)?
      (((REG_F&H_FLAG)? 0x6600:((tmp.b.h<0x0A)? 0x6000:0x6600))+C_FLAG):
      (
        (REG_F&H_FLAG)?
          ((REG_A<0xA0)? 0x0600:(0x6600+C_FLAG)):
          (
            (tmp.b.h<0x0A)? 
              ((REG_A<0xA0)? 0x0000:(0x6000+C_FLAG)): 
	      ((REG_A<0x90)? 0x0600:(0x6600+C_FLAG))
          )
      )
  );
  REG_A+=tmp.b.h;
//  REG_F=ZTable[REG_A]|(tmp.b.l|(REG_F&N_FLAG));
  REG_F=GenZF(REG_A)|(tmp.b.l|(REG_F&N_FLAG));
//  FLAGS(REG_A,tmp.b.l|(REG_F&N_FLAG));
}

inline void op_code_normal_case0x2F(void) { //CPL(1の補数) :state4
	REG_A=~REG_A;
	REG_F|=(N_FLAG|H_FLAG);
}

inline void op_code_normal_case0x3F(void) { //CCF(not carry) :state 4
	REG_F^=0x01;
	REG_F=REG_F&~(N_FLAG|H_FLAG);
//	REG_F|=(REG_F&C_FLAG)?0:H_FLAG;
}

inline void op_code_normal_case0x37(void) { //SCF(set carry) :state 4
	REG_F=(REG_F&~(N_FLAG|H_FLAG))|C_FLAG;
}

inline void op_code_normal_case0x00(void) { } //NOP : state 4
inline void op_code_normal_case0xF3(void) { c_regs_I=0; cpu_irq_check(); } //DI : state 4
inline void op_code_normal_case0xFB(void) { c_regs_I=1;int_disable_next=true; cpu_irq_check(); } //EI : state 4

inline void op_code_normal_case0x76(void) {
#ifndef EXSACT_CORE
	if (g_regs.TAC&0x04){//タイマ割りこみ
		static const int timer_clocks[]={1024,16,64,256};
		word tmp;
		tmp=g_regs.TIMA+(sys_clock+rest_clock)/timer_clocks[g_regs.TAC&0x03];

		if (tmp&0xFF00){//HALT中に割りこみがかかる場合
			total_clock+=(256-g_regs.TIMA)*timer_clocks[g_regs.TAC&0x03]-sys_clock;
			rest_clock-=(256-g_regs.TIMA)*timer_clocks[g_regs.TAC&0x03]-sys_clock;
			g_regs.TIMA=g_regs.TMA;
			halt=true;
			REG_PC--;
			cpu_irq(INT_TIMER);
			sys_clock=(sys_clock+rest_clock)&(timer_clocks[g_regs.TAC&0x03]-1);
		}
		else{
			g_regs.TIMA=tmp&0xFF;
			sys_clock=(sys_clock+rest_clock)&(timer_clocks[g_regs.TAC&0x03]-1);
			halt=true;
			total_clock+=rest_clock;
			rest_clock=0;
			REG_PC--;
		}
	}
	else{
		halt=true;
		total_clock+=rest_clock;
		div_clock+=rest_clock;
		rest_clock=0;
		REG_PC--;
	}
	tmp_clocks=0;
	cpu_irq_check(); 
#else
	halt=true;
	REG_PC--;
	cpu_irq_check(); 
#endif
} //HALT : state 4

//rotate/shift opcode
inline void op_code_normal_case0x07(void) { REG_F=(REG_A>>7);REG_A=(REG_A<<1)|(REG_A>>7);} //RLCA :state 4
inline void op_code_normal_case0x0F(void) { REG_F=(REG_A&1);REG_A=(REG_A>>1)|(REG_A<<7);} //RRCA :state 4
inline void op_code_normal_case0x17(void) { union pare_reg tmp;tmp.b.l=REG_A>>7;REG_A=(REG_A<<1)|(REG_F&C_FLAG);REG_F=tmp.b.l;} //RLA :state 4
inline void op_code_normal_case0x1F(void) { union pare_reg tmp;tmp.b.l=REG_A&1;REG_A=(REG_A>>1)|(REG_F<<7);REG_F=tmp.b.l;} //RRA :state 4

//jump opcode

//cc 条件 000 NZ non zero 001 Z zero 010 NC non carry 011 C carry
inline void op_code_normal_case0xC3(void) { REG_PC=op_readw();}//JP mn : state 10 (16?)

//JP cc,mn : 11 cc 010 : state 16 or 12
inline void op_code_normal_case0xC2(void) { if (REG_F&Z_FLAG) REG_PC+=2; else { REG_PC=op_readw();tmp_clocks=16; };} // JPNZ mn
inline void op_code_normal_case0xCA(void) { if (REG_F&Z_FLAG) { REG_PC=op_readw();tmp_clocks=16; } else REG_PC+=2;;} // JPZ mn
inline void op_code_normal_case0xD2(void) { if (REG_F&C_FLAG) REG_PC+=2; else { REG_PC=op_readw();tmp_clocks=16; };} // JPNC mn
inline void op_code_normal_case0xDA(void) { if (REG_F&C_FLAG) { REG_PC=op_readw();tmp_clocks=16; } else REG_PC+=2;;} // JPC mn

inline void op_code_normal_case0xE9(void) { REG_PC=REG_HL;} //JP HL : state 4 
inline void op_code_normal_case0x18(void) { REG_PC+=(signed char)op_read();}//JR e : state 12

//JR cc,e : 00 1cc 000 : state 12(not jumped ->8)
inline void op_code_normal_case0x20(void) { if (REG_F&Z_FLAG) REG_PC+=1; else {REG_PC+=(signed char)op_read();tmp_clocks=12;} }// JRNZ
inline void op_code_normal_case0x28(void) { if (REG_F&Z_FLAG) {REG_PC+=(signed char)op_read();tmp_clocks=12;} else REG_PC+=1; }// JRZ
inline void op_code_normal_case0x30(void) { if (REG_F&C_FLAG) REG_PC+=1; else {REG_PC+=(signed char)op_read();tmp_clocks=12;} }// JRNC
inline void op_code_normal_case0x38(void) { if (REG_F&C_FLAG) {REG_PC+=(signed char)op_read();tmp_clocks=12;} else REG_PC+=1; }// JRC

//call/ret opcode

inline void op_code_normal_case0xCD(void) { REG_SP-=2;writew(REG_SP,REG_PC+2);REG_PC=op_readw();} //CALL mn :state 24

//CALL cc,mn : 11 0cc 100 : state 24 or 12
inline void op_code_normal_case0xC4(void) { if (REG_F&Z_FLAG) REG_PC+=2; else {REG_SP-=2;writew(REG_SP,REG_PC+2);REG_PC=op_readw();tmp_clocks=24;} } //CALLNZ mn
inline void op_code_normal_case0xCC(void) { if (REG_F&Z_FLAG) {REG_SP-=2;writew(REG_SP,REG_PC+2);REG_PC=op_readw();tmp_clocks=24;} else REG_PC+=2; } //CALLZ mn
inline void op_code_normal_case0xD4(void) { if (REG_F&C_FLAG) REG_PC+=2; else {REG_SP-=2;writew(REG_SP,REG_PC+2);REG_PC=op_readw();tmp_clocks=24;} } //CALLNC mn
inline void op_code_normal_case0xDC(void) { if (REG_F&C_FLAG) {REG_SP-=2;writew(REG_SP,REG_PC+2);REG_PC=op_readw();tmp_clocks=24;} else REG_PC+=2; } //CALLC mn

//RST p : 11 t 111 (p=t<<3) : state 16
inline void op_code_normal_case0xC7(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x00;} //RST 0x00
inline void op_code_normal_case0xCF(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x08;} //RST 0x08
inline void op_code_normal_case0xD7(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x10;} //RST 0x10
inline void op_code_normal_case0xDF(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x18;} //RST 0x18
inline void op_code_normal_case0xE7(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x20;} //RST 0x20
inline void op_code_normal_case0xEF(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x28;} //RST 0x28
inline void op_code_normal_case0xF7(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x30;} //RST 0x30
inline void op_code_normal_case0xFF(void) { REG_SP-=2;writew(REG_SP,REG_PC);REG_PC=0x38;} //RST 0x38

inline void op_code_normal_case0xC9(void) { REG_PC=readw(REG_SP);REG_SP+=2;} //RET state 16

//RET cc : 11 0cc 000 : state 20 or 8
inline void op_code_normal_case0xC0(void) { if (!(REG_F&Z_FLAG)) {REG_PC=readw(REG_SP);REG_SP+=2;tmp_clocks=20;} } //RETNZ
inline void op_code_normal_case0xC8(void) { if (REG_F&Z_FLAG) {REG_PC=readw(REG_SP);REG_SP+=2;tmp_clocks=20;} } //RETZ
inline void op_code_normal_case0xD0(void) { if (!(REG_F&C_FLAG)) {REG_PC=readw(REG_SP);REG_SP+=2;tmp_clocks=20;} } //RETNC
inline void op_code_normal_case0xD8(void) { if (REG_F&C_FLAG) {REG_PC=readw(REG_SP);REG_SP+=2;tmp_clocks=20;} } //RETC

inline void op_code_normal_case0xCB(void) {
	int op_code=op_read();
	tmp_clocks=cycles_cb[op_code];
	op_code_cb_JmpTbl[op_code]();
}

// 0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD
// is no define...

// normal op_code Jump Table
void (*op_code_normal_JmpTbl[256])(void) = {
	op_code_normal_case0x00, op_code_normal_case0x01, op_code_normal_case0x02, op_code_normal_case0x03, op_code_normal_case0x04,
	op_code_normal_case0x05, op_code_normal_case0x06, op_code_normal_case0x07, op_code_normal_case0x08, op_code_normal_case0x09,
	op_code_normal_case0x0A, op_code_normal_case0x0B, op_code_normal_case0x0C, op_code_normal_case0x0D, op_code_normal_case0x0E,
	op_code_normal_case0x0F,
	op_code_normal_case0x10, op_code_normal_case0x11, op_code_normal_case0x12, op_code_normal_case0x13, op_code_normal_case0x14,
	op_code_normal_case0x15, op_code_normal_case0x16, op_code_normal_case0x17, op_code_normal_case0x18, op_code_normal_case0x19,
	op_code_normal_case0x1A, op_code_normal_case0x1B, op_code_normal_case0x1C, op_code_normal_case0x1D, op_code_normal_case0x1E,
	op_code_normal_case0x1F,
	op_code_normal_case0x20, op_code_normal_case0x21, op_code_normal_case0x22, op_code_normal_case0x23, op_code_normal_case0x24,
	op_code_normal_case0x25, op_code_normal_case0x26, op_code_normal_case0x27, op_code_normal_case0x28, op_code_normal_case0x29,
	op_code_normal_case0x2A, op_code_normal_case0x2B, op_code_normal_case0x2C, op_code_normal_case0x2D, op_code_normal_case0x2E,
	op_code_normal_case0x2F,
	op_code_normal_case0x30, op_code_normal_case0x31, op_code_normal_case0x32, op_code_normal_case0x33, op_code_normal_case0x34,
	op_code_normal_case0x35, op_code_normal_case0x36, op_code_normal_case0x37, op_code_normal_case0x38, op_code_normal_case0x39,
	op_code_normal_case0x3A, op_code_normal_case0x3B, op_code_normal_case0x3C, op_code_normal_case0x3D, op_code_normal_case0x3E,
	op_code_normal_case0x3F,
	op_code_normal_case0x40, op_code_normal_case0x41, op_code_normal_case0x42, op_code_normal_case0x43, op_code_normal_case0x44,
	op_code_normal_case0x45, op_code_normal_case0x46, op_code_normal_case0x47, op_code_normal_case0x48, op_code_normal_case0x49,
	op_code_normal_case0x4A, op_code_normal_case0x4B, op_code_normal_case0x4C, op_code_normal_case0x4D, op_code_normal_case0x4E,
	op_code_normal_case0x4F,
	op_code_normal_case0x50, op_code_normal_case0x51, op_code_normal_case0x52, op_code_normal_case0x53, op_code_normal_case0x54,
	op_code_normal_case0x55, op_code_normal_case0x56, op_code_normal_case0x57, op_code_normal_case0x58, op_code_normal_case0x59,
	op_code_normal_case0x5A, op_code_normal_case0x5B, op_code_normal_case0x5C, op_code_normal_case0x5D, op_code_normal_case0x5E,
	op_code_normal_case0x5F,
	op_code_normal_case0x60, op_code_normal_case0x61, op_code_normal_case0x62, op_code_normal_case0x63, op_code_normal_case0x64,
	op_code_normal_case0x65, op_code_normal_case0x66, op_code_normal_case0x67, op_code_normal_case0x68, op_code_normal_case0x69,
	op_code_normal_case0x6A, op_code_normal_case0x6B, op_code_normal_case0x6C, op_code_normal_case0x6D, op_code_normal_case0x6E,
	op_code_normal_case0x6F,
	op_code_normal_case0x70, op_code_normal_case0x71, op_code_normal_case0x72, op_code_normal_case0x73, op_code_normal_case0x74,
	op_code_normal_case0x75, op_code_normal_case0x76, op_code_normal_case0x77, op_code_normal_case0x78, op_code_normal_case0x79,
	op_code_normal_case0x7A, op_code_normal_case0x7B, op_code_normal_case0x7C, op_code_normal_case0x7D, op_code_normal_case0x7E,
	op_code_normal_case0x7F,
	op_code_normal_case0x80, op_code_normal_case0x81, op_code_normal_case0x82, op_code_normal_case0x83, op_code_normal_case0x84,
	op_code_normal_case0x85, op_code_normal_case0x86, op_code_normal_case0x87, op_code_normal_case0x88, op_code_normal_case0x89,
	op_code_normal_case0x8A, op_code_normal_case0x8B, op_code_normal_case0x8C, op_code_normal_case0x8D, op_code_normal_case0x8E,
	op_code_normal_case0x8F,
	op_code_normal_case0x90, op_code_normal_case0x91, op_code_normal_case0x92, op_code_normal_case0x93, op_code_normal_case0x94,
	op_code_normal_case0x95, op_code_normal_case0x96, op_code_normal_case0x97, op_code_normal_case0x98, op_code_normal_case0x99,
	op_code_normal_case0x9A, op_code_normal_case0x9B, op_code_normal_case0x9C, op_code_normal_case0x9D, op_code_normal_case0x9E,
	op_code_normal_case0x9F,
	op_code_normal_case0xA0, op_code_normal_case0xA1, op_code_normal_case0xA2, op_code_normal_case0xA3, op_code_normal_case0xA4,
	op_code_normal_case0xA5, op_code_normal_case0xA6, op_code_normal_case0xA7, op_code_normal_case0xA8, op_code_normal_case0xA9,
	op_code_normal_case0xAA, op_code_normal_case0xAB, op_code_normal_case0xAC, op_code_normal_case0xAD, op_code_normal_case0xAE,
	op_code_normal_case0xAF,
	op_code_normal_case0xB0, op_code_normal_case0xB1, op_code_normal_case0xB2, op_code_normal_case0xB3, op_code_normal_case0xB4,
	op_code_normal_case0xB5, op_code_normal_case0xB6, op_code_normal_case0xB7, op_code_normal_case0xB8, op_code_normal_case0xB9,
	op_code_normal_case0xBA, op_code_normal_case0xBB, op_code_normal_case0xBC, op_code_normal_case0xBD, op_code_normal_case0xBE,
	op_code_normal_case0xBF,
	op_code_normal_case0xC0, op_code_normal_case0xC1, op_code_normal_case0xC2, op_code_normal_case0xC3, op_code_normal_case0xC4,
	op_code_normal_case0xC5, op_code_normal_case0xC6, op_code_normal_case0xC7, op_code_normal_case0xC8, op_code_normal_case0xC9,
	op_code_normal_case0xCA, op_code_normal_case0xCB, op_code_normal_case0xCC, op_code_normal_case0xCD, op_code_normal_case0xCE,
	op_code_normal_case0xCF,
	op_code_normal_case0xD0, op_code_normal_case0xD1, op_code_normal_case0xD2, op_code_normal_case0x00, op_code_normal_case0xD4,
	op_code_normal_case0xD5, op_code_normal_case0xD6, op_code_normal_case0xD7, op_code_normal_case0xD8, op_code_normal_case0xD9,
	op_code_normal_case0xDA, op_code_normal_case0x00, op_code_normal_case0xDC, op_code_normal_case0x00, op_code_normal_case0xDE,
	op_code_normal_case0xDF,
	op_code_normal_case0xE0, op_code_normal_case0xE1, op_code_normal_case0xE2, op_code_normal_case0x00, op_code_normal_case0x00,
	op_code_normal_case0xE5, op_code_normal_case0xE6, op_code_normal_case0xE7, op_code_normal_case0xE8, op_code_normal_case0xE9,
	op_code_normal_case0xEA, op_code_normal_case0x00, op_code_normal_case0x00, op_code_normal_case0x00, op_code_normal_case0xEE,
	op_code_normal_case0xEF,
	op_code_normal_case0xF0, op_code_normal_case0xF1, op_code_normal_case0xF2, op_code_normal_case0xF3, op_code_normal_case0x00,
	op_code_normal_case0xF5, op_code_normal_case0xF6, op_code_normal_case0xF7, op_code_normal_case0xF8, op_code_normal_case0xF9,
	op_code_normal_case0xFA, op_code_normal_case0xFB, op_code_normal_case0x00, op_code_normal_case0x00, op_code_normal_case0xFE,
	op_code_normal_case0xFF
};
