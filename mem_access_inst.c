/*
 * File Name: mem_access_inst.c
 * Description: This file contains the implementation of memory access instructions
 *              for the emulator, including load and store operations for various
 *              addressing modes.
 * Author: Temitope Onafalujo
 */

#include "Emulator.h"

 // Global Variables
static union w_b src, dst;
unsigned short EA, DMAR, DCTRL;
unsigned short DMBR;
unsigned offset_table[2][2][2] = { 0, 0, 2, 1, -2, -1, 0, 0 };
int offset;


/*
 * Function: ld_effective_addr
 * Purpose: Calculate the effective address for load instructions and update the
 *          necessary registers and control signals.
 */
void ld_effective_addr() {
    offset = offset_table[global_inst_operands.dec][global_inst_operands.inc][global_inst_operands.w_b];
    src.word = regfile[0][global_inst_operands.src_con];
    switch (global_inst_operands.prpo) {
    case 1:
        src.word = src.word + offset;
        regfile[0][global_inst_operands.src_con] = src.word; // update regfile
        EA = src.word;
        DMAR = EA;
        break;
    case 0:
        EA = src.word;
        DMAR = EA;
        break;
    default:
        break;
    }
    if (global_inst_operands.w_b) {
        DCTRL = READ_BYTE;
    }
    else {
        DCTRL = READ_WORD;
    }
}


/*
 * Function: execute_LD
 * Purpose: Execute the load instruction by transferring data from memory to a register.
 */
void execute_LD() {
    xMC_BUS(DMAR, &DMBR, DCTRL, data_mem);
    // at this point DMBR will have been loaded

    switch (global_inst_operands.prpo) {
    case 1:
        regfile[0][global_inst_operands.dst] = DMBR;
        break;
    case 0:
        regfile[0][global_inst_operands.dst] = DMBR;
        regfile[0][global_inst_operands.src_con] += offset;
    default:
        break;
    }
}


/*
 * Function: st_effective_addr
 * Purpose: Calculate the effective address for store instructions and update the
 *          necessary registers and control signals.
 */
void st_effective_addr() {
    offset = offset_table[global_inst_operands.dec][global_inst_operands.inc][global_inst_operands.w_b];
    dst.word = regfile[0][global_inst_operands.dst];
    switch (global_inst_operands.prpo) {
    case 1:
        dst.word = dst.word + offset;
        regfile[0][global_inst_operands.dst] = dst.word; // update regfile
        EA = dst.word;
        DMAR = EA;
        break;
    case 0:
        EA = dst.word;
        DMAR = EA;
        break;
    default:
        break;
    }
    if (global_inst_operands.w_b) {
        DCTRL = WRITE_BYTE;
    }
    else {
        DCTRL = WRITE_WORD;
    }
}


/*
 * Function: execute_ST
 * Purpose: Execute the store instruction by transferring data from a register to memory.
 */
void execute_ST() {
    DMBR = regfile[0][global_inst_operands.src_con];
    // at this point EA will have been updated with the content from DMBR
    switch (global_inst_operands.prpo) {
    case 1:
        xMC_BUS(DMAR, &DMBR, DCTRL, data_mem);
        break;
    case 0:
        xMC_BUS(DMAR, &DMBR, DCTRL, data_mem);
        regfile[0][global_inst_operands.dst] += offset;
        break;
    default:
        break;
    }
}


/*
 * Function: ldr_effective_addr
 * Purpose: Calculate the effective address for LDR instructions using relative addressing mode.
 */
void ldr_effective_addr() {
    unsigned short ldr_str_offset;
    ldr_str_offset = SIGN_EXTEND(global_inst_operands.relative_offset, 6);
    EA = regfile[0][global_inst_operands.src_con] + ldr_str_offset;
    DMAR = EA;
    if (global_inst_operands.w_b) {
        DCTRL = READ_BYTE;
    }
    else {
        DCTRL = READ_WORD;
    }
}


/*
 * Function: execute_LDR
 * Purpose: Execute the LDR instruction by loading data from memory to a register.
 */
void execute_LDR() {
    xMC_BUS(DMAR, &DMBR, DCTRL, data_mem);
    regfile[0][global_inst_operands.dst] = DMBR;
}


/*
 * Function: str_effective_addr
 * Purpose: Calculate the effective address for STR instructions using relative addressing mode.
 */
void str_effective_addr() {
    unsigned short ldr_str_offset;
    ldr_str_offset = SIGN_EXTEND(global_inst_operands.relative_offset, 6);
    EA = regfile[0][global_inst_operands.dst] + ldr_str_offset;
    DMAR = EA;
    if (global_inst_operands.w_b) {
        DCTRL = WRITE_BYTE;
    }
    else {
        DCTRL = WRITE_WORD;
    }
}


/*
 * Function: execute_STR
 * Purpose: Execute the STR instruction by storing data from a register to memory.
 */
void execute_STR() {
    DMBR = regfile[0][global_inst_operands.src_con];
    xMC_BUS(DMAR, &DMBR, DCTRL, data_mem);
}
