/**
 * @file fetch_decode.c
 * @brief Fetch and decode functions for the emulator.
 * @details This file contains the fetch and decode functions necessary for instruction execution in the emulator.
 * @date 2024-06-20
 * @author Temitope Onafalujo
 */

#include "Emulator.h"

unsigned short IR;   // Decode Instruction Register to be used in DO()
unsigned short IMBR; // Instruction Memory Buffer Register
unsigned short IMAR; // Instruction Memory Address Register
unsigned short ICTRL;

// Enum for instruction types
enum instruction_type {
    BL, BEQ_TO_BRA, BEQ = 0x00, BNE = 0x01, BC = 0x02, BNC = 0x03, BN = 0x04,
    BGE = 0x05, BLT = 0x06, BRA = 0x07, ADD_TO_SXT = 0x02, MOVL_TO_MOVH = 0x03,
    ADD = 0x00, ADDC = 0x01, SUB = 0x02, SUBC = 0x03, DADD = 0x04, CMP = 0x05,
    XOR = 0x06, AND = 0x07, OR = 0x08, BIT = 0x09, BIC = 0x0A, BIS = 0x0B,
    MOV = 0x18, SWAP = 0x19, SRA = 0x0, RRC = 0x01, SWPB = 0x03, SXT = 0x04,
    SETCC = 0x01, CLRCC = 0x02, MOVL = 0, MOVLZ = 1, MOVLS = 2, MOVH = 3,
    LD = 6, ST = 7, LDR = 2, STR = 3
};

/**
 * @brief Extracts operands from the instruction.
 * @param instruction The instruction to decode.
 * @return The extracted instruction information.
 */
InstructionInfo extract_inst_operands(unsigned short instruction) {
    InstructionInfo temp_info;
    temp_info.r_c = RC(instruction);
    temp_info.w_b = WB(instruction);
    temp_info.src_con = SRC_CON(instruction);
    temp_info.dst = DST(instruction);
    temp_info.data = DATA(instruction);
    temp_info.prpo = EXTRACT_BIT(instruction, 9);
    temp_info.dec = EXTRACT_BIT(instruction, 8);
    temp_info.inc = EXTRACT_BIT(instruction, 7);
    temp_info.relative_offset = RELATIVE_OFFSET(instruction);
    temp_info.instruct_val = instruction;
    temp_info.setclr_bits.c = EXTRACT_BIT(instruction, 0);
    temp_info.setclr_bits.z = EXTRACT_BIT(instruction, 1);
    temp_info.setclr_bits.n = EXTRACT_BIT(instruction, 2);
    temp_info.setclr_bits.slp = EXTRACT_BIT(instruction, 3);
    temp_info.setclr_bits.v = EXTRACT_BIT(instruction, 4);
    return temp_info;
}

/**
 * @brief Simulates the generic Memory Controller bus.
 * @param MAR Memory Address Register.
 * @param MBR Memory Buffer Register.
 * @param CTRL --> Instruction or Data Control signal.
 * @param isInstruction Flag indicating the type of memory we want to access.
 */
void xMC_BUS(unsigned short MAR, unsigned short* MBR, unsigned short CTRL, int isInstruction) {
    unsigned char wb_bit = EXTRACT_BIT(CTRL, 0); // Extracting the word or byte bit signal (LSbit of the two bits)
    unsigned char rw_bit = EXTRACT_BIT(CTRL, 1); // Extracting the read or write addressing bit signal (MSbit of the two bits)

    union mem* memory = isInstruction ? &dmemory : &imemory;

    if (rw_bit) { // Write operation
        if (wb_bit) { // Byte write operation
            memory->btmem[MAR] = *MBR & BYTE_MASK;
        }
        else { // Word write operation
            memory->wdmem[MAR >> 1] = *MBR;
        }
    }
    else { // Read operation
        if (wb_bit) { // Byte read operation
            *MBR = memory->btmem[MAR];
        }
        else { // Word read operation
            *MBR = memory->wdmem[MAR >> 1];
        }
    }
}

/**
 * @brief F0 stage: Write PC to IMAR, signal ICTRL to read a word, increment PC by 2.
 */
void f0() {
    IMAR = PC;
    //printf("IMAR = %04X --- ", PC);
    ICTRL = READ_WORD; // Signal to read instruction memory as word
    PC += 2;

    // Store diagnostic info for F0
    diagnostics[diag_index].clock = cpu_clock;
    diagnostics[diag_index].pc = IMAR;
    sprintf(diagnostics[diag_index].fetch, "F0:%04X", IMAR);
}

/**
 * @brief F1 stage: Fetch instruction into IMBR. will eventually be placed in an Instruction Register for D0 to use
 */
void f1() {
    xMC_BUS(IMAR, &IMBR, ICTRL, instruction_mem); // 0 indicates instruction memory
    IR = IMBR; // Instruction register gets contents of IMBR

    // Display fetched instruction for debugging
    //printf("IMBR <--- 0x%04X\n", IR);

    // Store diagnostic info for F1
    diagnostics[diag_index].clock = cpu_clock;
    sprintf(diagnostics[diag_index].fetch, "F1:%04X", IR);
    diagnostics[diag_index - 1].instruction = IR; // Store the instruction value
}

/**
 * @brief D0 stage: Decode the instruction.
 */
void D0() {
    if (cpu_clock == 0) {
        IR = NOP; //instruction no operation is basically mov R0, R0
    }

    unsigned short instruction = IR;
    unsigned short address = IMAR;
    unsigned short first_3_bits = FIRST_3_BITS(instruction);
    unsigned short ld_st_check = EXTRACT_3_BITS(instruction, 10);
    unsigned short ldr_str_check = EXTRACT_2_BITS(instruction, 14);
    unsigned short other_branch_check = OTHER_BRANCH_CHECK(instruction);

    global_inst_operands = extract_inst_operands(instruction);// Extract operands directly into the global operand struct


    if (ldr_str_check == LDR) {
        global_inst_operands.instruction_type = LDR_EXEC;
    }
    else if (ldr_str_check == STR) {
        global_inst_operands.instruction_type = STR_EXEC;
    }
    else {
        switch (first_3_bits) {
        case BL:
            global_inst_operands.branch_offset = BL_OFFSET(instruction);
            global_inst_operands.instruction_type = BL_EXEC;
            break;
        case BEQ_TO_BRA:
            global_inst_operands.branch_offset = OTHER_BRANCHES_OFFSET(instruction);
            switch (other_branch_check)
            {
            case BEQ:
                global_inst_operands.instruction_type = BEQ_BZ_EXEC;
                break;
            case BNE:
                global_inst_operands.instruction_type = BNE_BNZ_EXEC;
                break;
            case BC:
                global_inst_operands.instruction_type = BC_BHS_EXEC;
                break;
            case BNC:
                global_inst_operands.instruction_type = BNC_BLO_EXEC;
                break;
            case BN:
                global_inst_operands.instruction_type = BN_EXEC;
                break;
            case BGE:
                global_inst_operands.instruction_type = BGE_EXEC;
                break;
            case BLT:
                global_inst_operands.instruction_type = BLT_EXEC;
                break;
            case BRA:
                global_inst_operands.instruction_type = BRA_EXEC;
                break;

            }
            break;
        case ADD_TO_SXT:
        {
            unsigned short first_4_bits = FIRST_4_BITS(instruction);
            unsigned short add_to_bis_check = ADD_TO_BIS_CHECK(instruction);
            unsigned short mov_to_clrcc_check = MOV_TO_CLRCC(instruction);

            if (first_4_bits == 0x04 && add_to_bis_check <= 0x0B) {
                switch (add_to_bis_check) {
                case ADD:
                    //display_instruction(instruction, "ADD");
                    global_inst_operands.instruction_type = ADD_EXEC;
                    if (global_inst_operands.dst == 7) {
                        IR = NOP;
                        global_inst_operands = extract_inst_operands(IR); // Update global_inst_operands to reflect NOP
                        global_inst_operands.instruction_type = MOV_EXEC; // Set to NOP_EXEC or similar if you have it defined
                    }
                    break;
                case ADDC:
                    //display_instruction(instruction, "ADDC");
                    global_inst_operands.instruction_type = ADDC_EXEC;
                    break;
                case SUB:
                    //display_instruction(instruction, "SUB");
                    global_inst_operands.instruction_type = SUB_EXEC;
                    break;
                case SUBC:
                    //display_instruction(instruction, "SUBC");
                    global_inst_operands.instruction_type = SUBC_EXEC;
                    break;
                case DADD:
                    //display_instruction(instruction, "DADD");
                    global_inst_operands.instruction_type = DADD_EXEC;
                    break;
                case CMP:
                    //display_instruction(instruction, "CMP");
                    global_inst_operands.instruction_type = CMP_EXEC;
                    break;
                case XOR:
                    //display_instruction(instruction, "XOR");
                    global_inst_operands.instruction_type = XOR_EXEC;
                    break;
                case AND:
                    //display_instruction(instruction, "AND");
                    global_inst_operands.instruction_type = AND_EXEC;
                    break;
                case OR:
                    //display_instruction(instruction, "OR");
                    global_inst_operands.instruction_type = OR_EXEC;
                    break;
                case BIT:
                    //display_instruction(instruction, "BIT");
                    global_inst_operands.instruction_type = BIT_EXEC;
                    break;
                case BIC:
                    //display_instruction(instruction, "BIC");
                    global_inst_operands.instruction_type = BIC_EXEC;
                    break;
                case BIS:
                    //display_instruction(instruction, "BIS");
                    global_inst_operands.instruction_type = BIS_EXEC;
                    break;
                }
            }
            else if (first_4_bits == 0x04 && (mov_to_clrcc_check == 0x18 || mov_to_clrcc_check == 0x19)) {
                switch (mov_to_clrcc_check) {
                case MOV:
                    //display_instruction(instruction, "MOV");
                    global_inst_operands.instruction_type = MOV_EXEC;
                    break;
                case SWAP:
                    if (EXTRACT_BIT(instruction, 6) == clr_bit) {
                        //display_instruction(instruction, "SWAP");
                        global_inst_operands.instruction_type = SWAP_EXEC;
                    }
                    break;
                }
            }
            else if (first_4_bits == 0x04 && mov_to_clrcc_check == 0x1A) {
                unsigned short bit_5_to_3 = EXTRACT_3_BITS(instruction, 3);
                switch (bit_5_to_3) {
                case SRA:
                    //display_instruction(instruction, "SRA");
                    global_inst_operands.instruction_type = SRA_EXEC;
                    break;
                case RRC:
                    //display_instruction(instruction, "RRC");
                    global_inst_operands.instruction_type = RRC_EXEC;
                    break;
                case SWPB:
                    if (EXTRACT_BIT(instruction, 6) == clr_bit) {
                        //display_instruction(instruction, "SWPB");
                        global_inst_operands.instruction_type = SWPB_EXEC;
                    }
                    break;
                case SXT:
                    if (EXTRACT_BIT(instruction, 6) == clr_bit) {
                        //display_instruction(instruction, "SXT");
                        global_inst_operands.instruction_type = SXT_EXEC;
                    }
                    break;
                }
            }
            else if (first_4_bits == 0x04 && mov_to_clrcc_check == 0x1B) { //SETCC to CLRCC instruction
                unsigned short setpri_to_clrcc_check = EXTRACT_2_BITS(instruction, 5);
                switch (setpri_to_clrcc_check)
                {
                case SETCC:
                    //display_instruction(instruction, "SETCC");
                    global_inst_operands.instruction_type = SETCC_EXEC;
                    break;

                case CLRCC:
                    //display_instruction(instruction, "CLRCC");
                    global_inst_operands.instruction_type = CLRCC_EXEC;
                    break;

                }
            }
            else if (first_4_bits == 0x05) { // LD to ST instruction
                switch (ld_st_check)
                {
                case LD:
                    global_inst_operands.instruction_type = LD_EXEC;
                    break;
                case ST:
                    global_inst_operands.instruction_type = ST_EXEC;
                }
            }
            else {
                printf("%04X: %04X\n\n", address, instruction);
            }
        }
        break;
        case MOVL_TO_MOVH:
        {
            unsigned short movl_to_movh_check = MOVL_TO_MOVH_CHECK(instruction);
            switch (movl_to_movh_check) {
            case MOVL:
                //display_instruction(instruction, "MOVL");
                global_inst_operands.instruction_type = MOVL_EXEC;
                break;
            case MOVLZ:
                //display_instruction(instruction, "MOVLZ");
                global_inst_operands.instruction_type = MOVLZ_EXEC;
                break;
            case MOVLS:
                //display_instruction(instruction, "MOVLS");
                global_inst_operands.instruction_type = MOVLS_EXEC;
                break;
            case MOVH:
                //display_instruction(instruction, "MOVH");
                global_inst_operands.instruction_type = MOVH_EXEC;
                break;
            }
        }
        break;
        default:
            printf("%04X: %04X\n\n", address, instruction);
            break;
        }
    }

    // Store diagnostic info for D0
    sprintf(diagnostics[diag_index].decode, "D0:%04X", instruction);
}

/**
 * @brief Displays the instruction details for debugging purposes.
 * @param instruction The instruction to display.
 * @param name The name of the instruction.
 */
void display_instruction(unsigned short instruction, const char* name) {
    InstructionInfo info = extract_inst_operands(instruction); //This line is useful incase we need to print out operands of the instruction
    printf("Decoded Instruction --> %s\n", name);
}
