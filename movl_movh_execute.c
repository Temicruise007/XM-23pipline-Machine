#include"Emulator.h"

static union w_b dst;
static unsigned char data;

void execute_MOVL() {

    dst.word = regfile[0][global_inst_operands.dst];
    data = global_inst_operands.data;

    dst.byte[0] = data; // Moving the data to the low byte of the dst word
    regfile[0][global_inst_operands.dst] = dst.word;
}

void execute_MOVLZ() {
    dst.word = regfile[0][global_inst_operands.dst];
    data = global_inst_operands.data;

    dst.byte[0] = data;  // lower byte gets data
    dst.byte[1] = 0;  // upper byte is to 0's

    regfile[0][global_inst_operands.dst] = dst.word;
}

void execute_MOVLS() {
    dst.word = regfile[0][global_inst_operands.dst];
    data = global_inst_operands.data;

    dst.byte[0] = data;  // lower byte gets data
    dst.byte[1] = 0xFF;  // upper byte is set to 1's

    regfile[0][global_inst_operands.dst] = dst.word;
}

void execute_MOVH() {
    dst.word = regfile[0][global_inst_operands.dst];
    data = global_inst_operands.data;

    dst.byte[1] = data;

    regfile[0][global_inst_operands.dst] = dst.word;
}


