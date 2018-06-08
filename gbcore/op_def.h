#ifndef OP_DEF_
#define OP_DEF

// プリフィックスなしZ80オペコード

/*
#define REG_A c_regs.AF.b.h
#define REG_F c_regs.AF.b.l
#define REG_B c_regs.BC.b.h
#define REG_C c_regs.BC.b.l
#define REG_D c_regs.DE.b.h
#define REG_E c_regs.DE.b.l
#define REG_H c_regs.HL.b.h
#define REG_L c_regs.HL.b.l
#define REG_AF c_regs.AF.w
#define REG_BC c_regs.BC.w
#define REG_DE c_regs.DE.w
#define REG_HL c_regs.HL.w
#define REG_SP c_regs.SP
#define REG_PC c_regs.PC
*/

//c_regsを構造体じゃなくしたので変更
#define REG_A (((byte *)&c_regs_AF)[1])
#define REG_F (((byte *)&c_regs_AF)[0])
#define REG_B (((byte *)&c_regs_BC)[1])
#define REG_C (((byte *)&c_regs_BC)[0])
#define REG_D (((byte *)&c_regs_DE)[1])
#define REG_E (((byte *)&c_regs_DE)[0])
#define REG_H (((byte *)&c_regs_HL)[1])
#define REG_L (((byte *)&c_regs_HL)[0])
#define REG_AF c_regs_AF
#define REG_BC c_regs_BC
#define REG_DE c_regs_DE
#define REG_HL c_regs_HL
#define REG_SP c_regs_SP
#define REG_PC c_regs_PC

#define ADD(arg) \
	union pare_reg tmp; \
	tmp.w=REG_A+arg; \
	REG_F=tmp.b.h|GenZF(tmp.b.l)|((REG_A^arg^tmp.b.l)&H_FLAG); \
	REG_A=tmp.b.l
#define ADC(arg) \
	union pare_reg tmp; \
	tmp.w=REG_A+arg+(REG_F&C_FLAG); \
	REG_F=tmp.b.h|GenZF(tmp.b.l)|((REG_A^arg^tmp.b.l)&H_FLAG); \
	REG_A=tmp.b.l
#define SUB(arg) \
	union pare_reg tmp; \
	tmp.w=REG_A-arg; \
	REG_F=N_FLAG|-tmp.b.h|GenZF(tmp.b.l)|((REG_A^arg^tmp.b.l)&H_FLAG); \
	REG_A=tmp.b.l
#define SBC(arg) \
	union pare_reg tmp; \
	tmp.w=REG_A-arg-(REG_F&C_FLAG); \
	REG_F=N_FLAG|-tmp.b.h|GenZF(tmp.b.l)|((REG_A^arg^tmp.b.l)&H_FLAG); \
	REG_A=tmp.b.l
#define CP(arg) \
	union pare_reg tmp; \
	tmp.w=REG_A-arg; \
	REG_F=N_FLAG|-tmp.b.h|GenZF(tmp.b.l)|((REG_A^arg^tmp.b.l)&H_FLAG)
#define AND(arg) REG_A&=arg;REG_F=H_FLAG|GenZF(REG_A)
#define OR(arg)  REG_A|=arg;REG_F=GenZF(REG_A)
#define XOR(arg) REG_A^=arg;REG_F=GenZF(REG_A)
#define INC(arg) arg++;REG_F=(REG_F&C_FLAG)|GenZF(arg)|((arg&0x0F)?0:H_FLAG)
#define DEC(arg) arg--;REG_F=N_FLAG|(REG_F&C_FLAG)|GenZF(arg)|(((arg&0x0F)==0x0F)?H_FLAG:0)
#define ADDW(arg) \
	union pare_reg tmp; \
	tmp.w=REG_HL+arg; \
	REG_F=(REG_F&Z_FLAG)|(((REG_HL^arg^tmp.w)&0x1000)?H_FLAG:0)|((((unsigned long)REG_HL+(unsigned long)arg)&0x10000)?C_FLAG:0); \
	REG_HL=tmp.w

#endif
