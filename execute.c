/**
 * @file execute.c
 * @brief Execute instruction functions.
 * @details This file contains functions to execute various CPU instructions.
 * @date 2024-06-22
 * @author Temitope Onafalujo
 */

#include "Emulator.h"


 // Global variable that holds info about the operands and instruction type
InstructionInfo global_inst_operands;

unsigned short last_executed_address = INVALID; // Initialize to an invalid value
int skip_update_last_executed_address = FALSE; // Flag to skip updating the last executed address

/**
 * @brief Execute instructions based on the instruction type.
 */
void E0() {
    if (!skip_update_last_executed_address) {
        last_executed_address = IMAR - 2; // Track the address of the current instruction being executed
    }
    skip_update_last_executed_address = FALSE; // Reset the flag

    // Log the instruction value to be displayed under execute
    sprintf(diagnostics[diag_index].execute, "E0:%04X", global_inst_operands.instruct_val);

    switch (global_inst_operands.instruction_type) {
    case BL_EXEC:
    case BEQ_BZ_EXEC:
    case BNE_BNZ_EXEC:
    case BC_BHS_EXEC:
    case BNC_BLO_EXEC:
    case BN_EXEC:
    case BGE_EXEC:
    case BLT_EXEC:
    case BRA_EXEC:
        execute_Branching_inst();
        break;
    case ADD_EXEC:
        execute_ADD();
        //printf("Executed ADD\n");
        break;
    case ADDC_EXEC:
        execute_ADDC();
        //printf("Executed ADDC\n");
        break;
    case SUB_EXEC:
        execute_SUB();
        //printf("Executed SUB\n");
        break;
    case SUBC_EXEC:
        execute_SUBC();
        //printf("Executed SUBC\n");
        break;
    case DADD_EXEC:
        execute_DADD();
        //printf("Executed DADD\n");
        break;
    case CMP_EXEC:
        execute_CMP();
        //printf("Executed CMP\n");
        break;
    case XOR_EXEC:
        execute_XOR();
        //printf("Executed XOR\n");
        break;
    case AND_EXEC:
        execute_AND();
        //printf("Executed AND\n");
        break;
    case OR_EXEC:
        execute_OR();
        //printf("Executed OR\n");
        break;
    case BIT_EXEC:
        execute_BIT();
        //printf("Executed BIT\n");
        break;
    case BIC_EXEC:
        execute_BIC();
        //printf("Executed BIC\n");
        break;
    case BIS_EXEC:
        execute_BIS();
        //printf("Executed BIS\n");
        break;
    case MOV_EXEC:
        if (global_inst_operands.src_con == 0 && global_inst_operands.dst == 0) {
            // If this is MOV R0, R0 (representing NOP), skip updating last_executed_address
            skip_update_last_executed_address = TRUE;
        }
        execute_MOV();
        //printf("Executed MOV\n");
        break;
    case SWAP_EXEC:
        execute_SWAP();
        //printf("Executed SWAP\n");
        break;
    case SRA_EXEC:
        execute_SRA();
        //printf("Executed SRA\n");
        break;
    case RRC_EXEC:
        execute_RRC();
        //printf("Executed RRC\n");
        break;
    case SWPB_EXEC:
        execute_SWPB();
        //printf("Executed SWPB\n");
        break;
    case SXT_EXEC:
        execute_SXT();
        //printf("Executed SXT\n");
        break;
    case SETCC_EXEC:
        execute_SETCC();
        //printf("Executed SETCC\n");
        break;
    case CLRCC_EXEC:
        execute_CLRCC();
        //printf("Executed CLRCC\n");
        break;
    case MOVL_EXEC:
        execute_MOVL();
        //printf("Executed MOVL\n");
        break;
    case MOVLZ_EXEC:
        execute_MOVLZ();
        //printf("Executed MOVLZ\n");
        break;
    case MOVLS_EXEC:
        execute_MOVLS();
        //printf("Executed MOVLS\n");
        break;
    case MOVH_EXEC:
        execute_MOVH();
        //printf("Executed MOVH\n");
        break;
    case LD_EXEC:
        ld_effective_addr();
        break;
    case ST_EXEC:
        st_effective_addr();
        break;
    case LDR_EXEC:
        ldr_effective_addr();
        break;
    case STR_EXEC:
        str_effective_addr();
        break;
    default:
        //printf("\nUnknown instruction type.\n\n");
        break;
    }
}

void E1() {

    sprintf(diagnostics[diag_index].execute, "E1:%04X", global_inst_operands.instruct_val);

    switch (global_inst_operands.instruction_type)
    {
    case LD_EXEC:
        execute_LD();
        break;
    case ST_EXEC:
        execute_ST();
        break;
    case LDR_EXEC:
        execute_LDR();
        break;
    case STR_EXEC:
        execute_STR();
        break;
    default:
        break;
    }
}
