#pragma once
#ifndef EMULATOR_H
#define EMULATOR_H

/**
 * @file Emulator.h
 * @brief Header file for the emulator project.
 * @details This file contains all the necessary includes, constants, and function declarations required for the emulator.
 * @date 2024-05-28
 * @author Temitope Onafalujo
 */

#define DEBUG //To be fully implemented in Assignment 3 for all debugging print statements in the D0() and E0()

#include "Bitwise_manipulation.h"
#include "PSW.h"
#include <stdio.h>
#include<stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h> /* Signal handling software */

 // Memory sizes
#define WDMEMSIZE (1 << 15) // 32768 words
#define BTMEMSIZE (1 << 16) // 65536 bytes
#define MEM_SIZE 65536
#define diagnostic_index 500000
#define diag_buf_len 20

// Buffer and offset sizes
#define BUFFER_LEN 256
#define NUM_VALUES 2 // for regfile
#define NUM_REG_OR_CONS 8 // for regfile
#define OFFSET 8 // Offset in the record string where the data bytes start
#define ADDRESS_AND_CHECKSUM_BYTES 3 // Bytes for the address and checksum combined
#define len_start_pos 2 // Starting position of length in the record (3rd element)
#define addr_start_pos 4 // Starting position of address in the record (5th element)
#define PC_INCREMENT 2

// Boolean values
#define FALSE 0
#define TRUE 1

// Special constants
#define INVALID 0xFFFF
#define NOP 0x4C00 // No-operation instruction (Mov R0, R0)

// ICTRL and DCTRL codes
#define READ_WORD 0x0 // Read word operation
#define READ_BYTE 0x1 // Read byte operation
#define WRITE_WORD 0x2 // Write word operation
#define WRITE_BYTE 0x3 // Write byte operation

// BYTE_MASK constant
#define BYTE_MASK 0xFF

/* External variables */
extern unsigned int cpu_clock;
extern unsigned short IR;
extern unsigned short IMAR; //put in fetch_decode.c
extern unsigned short ICTRL; //put in fetch_decode.c
extern unsigned short EA; //put in mem_access_inst.c
extern unsigned short DMAR; //put in mem_access_inst.c
extern unsigned short DCTRL;// put in mem_access_inst.c
extern unsigned short regfile[NUM_VALUES][NUM_REG_OR_CONS];
#define BP regfile[0][4]
#define LR regfile[0][5]
#define SP regfile[0][6]
#define PC regfile[0][7]

// Enums for setting and clearing bits
enum clr_or_set_bit { clr_bit, set_bit };
enum INST_OR_DATA_MEM { instruction_mem, data_mem };
enum READ_OR_WRITE { read, write };
enum WORD_OR_BYTE { word, byte };

// Union for memory
union mem {
    unsigned short wdmem[WDMEMSIZE];
    unsigned char btmem[BTMEMSIZE];
};

// Union for word and byte access
union w_b {
    unsigned short word;
    unsigned char byte[2];
};

/* External variables from other files */
extern char s_record[BUFFER_LEN];
extern FILE* s_recfile_descriptor;
extern int program_running;
extern unsigned short breakpoint_address;
extern union mem imemory;
extern union mem dmemory;



/* Function declarations for loader.c */
void loadFile();
void func_for_s0_record(char* record);
void func_for_s1_record(char* record);
void func_for_s2_record(char* record);
void func_for_s9_record(char* record);
void displayMemoryRegion(unsigned char* memory);

/* Function declarations for decoder.c */
void f0();
void f1();
void xMC_BUS(unsigned short MAR, unsigned short* MBR, unsigned short CTRL, int isInstruction);
void D0();
void display_instruction(unsigned short instruction, const char* name);

/* Function declarations for execute.c */
void E0();
void E1();

/* Function declarations for display_change.c */
void displayPswBits();
void change_memory_value();
void displayRegisterFile();
void change_register_value();

/* Function declarations for psw_code.c */
extern void update_psw(unsigned short src, unsigned short dst, unsigned short res, unsigned short wb);
extern void update_psw2(unsigned short result, unsigned short wb);

/* CPU control function */
void CPU();

/* Function declarations for register instructions execution (ADD - SXT) */
void execute_ADD();
void execute_ADDC();
void execute_SUB();
void execute_SUBC();
void execute_DADD();
unsigned short bcd_add(unsigned short nibble_A, unsigned short nibble_B);
void execute_CMP();
void execute_XOR();
void execute_AND();
void execute_OR();
void execute_BIT();
void execute_BIC();
void execute_BIS();
void execute_MOV();
void execute_SWAP();
void execute_SRA();
void execute_RRC();
void execute_SWPB();
void execute_SXT();

/* Function declaration for SETCC and CLRCC in setcc_clrcc_execute.c */
void execute_SETCC();
void execute_CLRCC();

/* Function declarations for additional instructions execution */
void execute_MOVL();
void execute_MOVLZ();
void execute_MOVLS();
void execute_MOVH();

/* Function declarations for memory accessing instructions */
void execute_LD();
void execute_ST();
void execute_STR();
void execute_LDR();
void ld_effective_addr();
void st_effective_addr();
void ldr_effective_addr();
void str_effective_addr();

/* Function declarations for Branching Instruction */
void execute_Branching_inst();
void execute_BL();
void execute_other_branches();

/* variable that aid branching instruction */
extern bool d_bubble;
extern bool e_bubble;

/* Other external variables */
extern unsigned short last_executed_address;
extern int skip_update_last_executed_address;
extern int breakpoint_set;

/* Enum for instruction execution */
enum instruct_table {
    BL_EXEC = 0x00, BEQ_BZ_EXEC = 0x01, BNE_BNZ_EXEC = 0x02, BC_BHS_EXEC = 0x03, BNC_BLO_EXEC = 0x04,
    BN_EXEC = 0x05, BGE_EXEC = 0x06, BLT_EXEC = 0x07, BRA_EXEC = 0x08,
    ADD_EXEC = 0x09, ADDC_EXEC = 0x0A, SUB_EXEC = 0x0B, SUBC_EXEC = 0x0C, DADD_EXEC = 0x0D,
    CMP_EXEC = 0x0E, XOR_EXEC = 0x0F, AND_EXEC = 0x10, OR_EXEC = 0x11, BIT_EXEC = 0x12,
    BIC_EXEC = 0x13, BIS_EXEC = 0x14, MOV_EXEC = 0x15, SWAP_EXEC = 0x16, SRA_EXEC = 0x17,
    RRC_EXEC = 0x18, SWPB_EXEC = 0x19, SXT_EXEC = 0x1A,
    SETPRI_EXEC = 0x1B, SVC_EXEC = 0x1C, SETCC_EXEC = 0x1D, CLRCC_EXEC = 0x1E,
    CEX_EXEC = 0x1F, LD_EXEC = 0x20, ST_EXEC = 0x21, MOVL_EXEC = 0x22,
    MOVLZ_EXEC = 0x23, MOVLS_EXEC = 0x24, MOVH_EXEC = 0x25, LDR_EXEC = 0x26, STR_EXEC = 0x27
};

typedef struct {

    unsigned short c;
    unsigned short z;
    unsigned short n;
    unsigned short slp;
    unsigned short v;

} SETCLRCC;

/* Struct and function for instruction information */
typedef struct {
    unsigned char r_c;
    unsigned char w_b;
    unsigned char src_con;
    unsigned char dst;
    unsigned char data;
    unsigned char prpo;
    unsigned char dec;
    unsigned char inc;
    unsigned short relative_offset;
    unsigned short branch_offset;
    unsigned short instruct_val;
    SETCLRCC setclr_bits;
    enum instruct_table instruction_type;
} InstructionInfo;

extern InstructionInfo global_inst_operands;
InstructionInfo extract_inst_operands(unsigned short instruction);


/* Structs and unions for executing the DADD instruction */
struct bcd_nibbles {
    unsigned short nib0 : 4;
    unsigned short nib1 : 4;
    unsigned short nib2 : 4;
    unsigned short nib3 : 4;
};

union bcd_word_nibble {
    unsigned short word;
    struct bcd_nibbles nibble;
};


/* struct for diagnostic display */
typedef struct {
    unsigned int clock;
    unsigned short pc;
    unsigned short instruction;
    char fetch[diag_buf_len];
    char decode[diag_buf_len];
    char execute[diag_buf_len];
} DiagnosticInfo;
// global variables for diagnostic display, defined in main.c
extern DiagnosticInfo diagnostics[diagnostic_index]; // Adjust size as needed
extern int diag_index;

/* Globals for control c software */
extern volatile sig_atomic_t ctrl_c_fnd;
void sigint_hdlr();
void init_signal();
void run_xm();



#endif // EMULATOR_H
