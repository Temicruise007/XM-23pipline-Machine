/**
 * @file cpu.c
 * @brief CPU simulation functions.
 * @details This file contains functions to simulate CPU behavior.
 * @date 2024-06-23
 * @autho Temitope Onafalujo
 */

#include "Emulator.h"

unsigned int cpu_clock;

/**
 * @brief Simulate the CPU clock and instruction execution.
 */
void CPU() {
    static int header_printed = FALSE; // Static variable to keep track of header printing
    static int mem_exec_stage = FALSE; // used to know when E1 is to be executed

    if (!header_printed) {
#ifdef DEBUG
        printf("%-10s %-10s %-15s %-10s %-10s %-10s\n", "Clock", "PC", "Instruction", "Fetch", "Decode", "Execute");
#endif
        header_printed = TRUE; // Set the variable to 1 after printing the header
    }

    // printf("Start PC: %04x Clk: %d\n", PC, cpu_clock);
    if (cpu_clock % 2 == 0) { // even clock tick

        if (!d_bubble) {
            f0(); //IMAR <- PC, PC <- PC + 2

            if (mem_exec_stage == TRUE) {
                E1();
                mem_exec_stage = FALSE; //resetting the stage to allow E0() run first.
            }

            D0(); //decode IMBR of the previous odd clock-tick
            diag_index++;
            cpu_clock++;
        }
        else {
            IR = NOP;
            d_bubble = false;
        }
    }
    else { // odd clock tick
        if (!e_bubble) {
            f1();
            E0();
            cpu_clock++;
            // Check if the instruction is a memory access instruction and set the stage
            if (global_inst_operands.instruction_type == LD_EXEC ||
                global_inst_operands.instruction_type == ST_EXEC ||
                global_inst_operands.instruction_type == LDR_EXEC ||
                global_inst_operands.instruction_type == STR_EXEC) {
                mem_exec_stage = TRUE; // Set the stage to execute E1 on the next even tick
            }

            // Print diagnostic info after odd clock tick (complete cycle)
#ifdef DEBUG
            int index = diag_index - 1;
            for (index; index < (diag_index + 1); index++) {

                printf("%-10u %-10X %-15X %-10s %-10s %-10s\n",
                    diagnostics[index].clock,
                    diagnostics[index].pc,
                    diagnostics[index].instruction,
                    diagnostics[index].fetch,
                    diagnostics[index].decode,
                    diagnostics[index].execute);

            }
#endif
            diag_index++;


            if (last_executed_address == breakpoint_address) {
                program_running = FALSE; // Stop the program
            }
        }
        else {
            e_bubble = false;
        }
    }
    // printf("End PC: %04x Clk: %d\n\n", PC, cpu_clock);
    // previously increment cpu_clock here
}
