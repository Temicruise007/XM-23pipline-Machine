/*
 * File Name: branch_inst.c
 * Description: This file contains the implementation of branch instructions for the emulator,
 *              including branch with link and other conditional branches.
 * Author: Temitope Onafalujo
 */

#include "Emulator.h"

 /*
  * Function: execute_Branching_inst
  * Purpose: Determine and execute the appropriate branch instruction based on the instruction type.
  */
void execute_Branching_inst() {
    if (global_inst_operands.instruction_type == BL_EXEC) {
        execute_BL();
    }
    else {
        execute_other_branches();
    }
}

/*
 * Function: execute_BL
 * Purpose: Execute the Branch with Link (BL) instruction by updating the Link Register (LR) and Program Counter (PC).
 */
void execute_BL() {
    LR = PC - PC_INCREMENT;
    unsigned short offset = global_inst_operands.branch_offset; // mask off instruction to get the offset part of the branch
    if ((offset & 0x1000) == 0x1000) { // check if it is a negative offset
        offset = offset | 0xE000;      // extend the signed bit
    }
    offset = offset << 1; // shift to the left by one to ensure even offset
    PC += offset;
    PC -= PC_INCREMENT;
    d_bubble = true;
    e_bubble = true;
}

/*
 * Function: execute_other_branches
 * Purpose: Execute other conditional branch instructions based on the instruction type and condition flags.
 */
void execute_other_branches() {
    unsigned short offset = global_inst_operands.branch_offset;

    if ((offset & 0x0200) == 0x0200) // check if it is a negative offset
        offset = offset | 0xFC00;    // extend the signed bit
    offset = offset << 1;            // shift to the left by one to ensure even offset

    switch (global_inst_operands.instruction_type) {
    case BEQ_BZ_EXEC:
        if (psw.z) {
            PC += offset;
            PC -= PC_INCREMENT; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BNE_BNZ_EXEC:
        if (!psw.z) {
            PC += offset;
            PC -= PC_INCREMENT; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BC_BHS_EXEC:
        if (psw.c) {
            PC += offset;
            PC -= 2; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BNC_BLO_EXEC:
        if (!psw.c) {
            PC += offset;
            PC -= PC_INCREMENT;
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BN_EXEC:
        if (psw.n) {
            PC += offset;
            PC -= 2; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BGE_EXEC:
        if (psw.n == psw.v) {
            PC += offset;
            PC -= PC_INCREMENT; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BLT_EXEC:
        if (psw.n != psw.v) {
            PC += offset;
            PC -= PC_INCREMENT; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
            d_bubble = true;
            e_bubble = true;
        }
        break;
    case BRA_EXEC:
        PC += offset;
        PC -= PC_INCREMENT; // subtract 2 because PC is already ahead by 4 when an instruction gets executed
        d_bubble = true;
        e_bubble = true;
        break;
    }
}
