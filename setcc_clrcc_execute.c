#include "Emulator.h"

void execute_SETCC() {
	psw.c = (global_inst_operands.setclr_bits.c) ? set_bit : psw.c;
	psw.z = (global_inst_operands.setclr_bits.z) ? set_bit : psw.z;
	psw.n = (global_inst_operands.setclr_bits.n) ? set_bit : psw.n;
	psw.v = (global_inst_operands.setclr_bits.v) ? set_bit : psw.v;


	if (psw.current == 7) {
		psw.slp = clr_bit;
	}
	else {
		psw.slp = (global_inst_operands.setclr_bits.slp) ? set_bit : psw.slp;
	}
}



void execute_CLRCC() {
	psw.c = (global_inst_operands.setclr_bits.c) ? clr_bit : psw.c;
	psw.z = (global_inst_operands.setclr_bits.z) ? clr_bit : psw.z;
	psw.n = (global_inst_operands.setclr_bits.n) ? clr_bit : psw.n;
	psw.slp = (global_inst_operands.setclr_bits.slp) ? clr_bit : psw.slp;
	psw.v = (global_inst_operands.setclr_bits.v) ? clr_bit : psw.v;
}