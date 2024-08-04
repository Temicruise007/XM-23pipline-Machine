/*
 * display_change.c
 * This file contains functions for displaying memory, register, and psw bit values as well as
   changing memory and register values.
 * Author: Temitope Onafalujo
 * Date: 2024-06-10
 */


#include"Emulator.h"

void displayPswBits() {
    /* This function prints out the psw bits */
    printf(BRIGHT_YELLOW); // Set color to bright yellow
    printf("\n=========== PSW Bits Display ===========\n");
    printf("PSW bits ===>>> V = %x | C = %x | N = %x | Z = %x\n", psw.v, psw.c, psw.n, psw.z);
    printf("========================================\n\n");
    printf(RESET_COLOR); // Reset color to default
}

void change_memory_value() {
    /*
        This function changes the content of a desired Instruction or Data
        memory location
    */
    unsigned short address, new_value;
    char mem_type;
    int ch;

    printf("\nEnter 'I' to choose instruction memory or 'D' to choose data memory-> ");
    if (scanf(" %c", &mem_type) != 1 || (mem_type != 'I' && mem_type != 'D' && mem_type != 'i' && mem_type != 'd')) {
        printf("Invalid memory type. Please enter 'I' or 'D'.\n");
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    printf("Enter the memory address you want to change (in hexadecimal)-> ");
    if (scanf("%4hx", &address) != 1) {
        printf("Invalid address. Please enter a valid hexadecimal number.\n");
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    printf("Enter the new value (in hexadecimal)-> ");
    if (scanf("%4hx", &new_value) != 1) {
        printf("Invalid value. Please enter a valid hexadecimal number.\n");
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    // Ensure address tha is within a valid range
    if (address >= BTMEMSIZE - 1) {
        printf("Invalid address. Address out of range.\n");
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }

    if (mem_type == 'I' || mem_type == 'i') {
        // Update instruction memory byte by byte
        imemory.btmem[address] = new_value & 0xFF;           // Low byte
        imemory.btmem[address + 1] = (new_value >> 8) & 0xFF; // High byte
        printf("\nInstruction memory at address %04x has been changed to %04x.\n\n", address, new_value);
    }
    else if (mem_type == 'D' || mem_type == 'd') {
        // Update data memory byte by byte
        dmemory.btmem[address] = new_value & 0xFF;           // Low byte
        dmemory.btmem[address + 1] = (new_value >> 8) & 0xFF; // High byte
        printf("\nData memory at address %04x has been changed to %04x.\n\n", address, new_value);
    }

    while ((ch = getchar()) != '\n' && ch != EOF);
}

void displayMemoryRegion(unsigned char* memory) {
    /*
       This function displays the content of a desired Instruction or Data
       memory location
   */

    unsigned short memStart, memEnd;
    unsigned int index;

    printf("Enter start and end address in this format (start end: 1000 2000): ");
    (void)scanf("%4hx %4hx", &memStart, &memEnd);

    unsigned short currentIntervalStart, currentIntervalEnd;

    printf("\n========= Memory Region Display =========\n");

    for (currentIntervalStart = memStart; currentIntervalStart < memEnd; currentIntervalStart += 0x0010) {
        currentIntervalEnd = currentIntervalStart + 0x0010;

        // Print the starting address of the line
        printf("%04x: ", currentIntervalStart);

        // Print the memory bytes
        for (index = currentIntervalStart; index < currentIntervalEnd && index < memEnd; ++index) {
            printf("%02x ", memory[index]);
        }

        // Fill in if the row is incomplete
        for (; index < currentIntervalEnd; ++index) {
            printf("   ");
        }

        printf(" ");

        // Print the ASCII interpretation
        for (index = currentIntervalStart; index < currentIntervalEnd && index < memEnd; ++index) {
            unsigned char byte = memory[index];
            if (isprint(byte)) { //isprint() function is used to check if its a printable character or not
                printf("%c", byte);
            }
            else {
                printf(".");
            }
        }

        printf("\n");
    }

    printf("\n\n");
}

void displayRegisterFile() {
    /* This function prints out the contents of the register file */
    printf(BRIGHT_PURPLE); // Set color to bright purple
    printf("\n\n=============================================================\n");
    printf("============ Displaying Contents of Register File ===========\n");
    printf("=============================================================\n");
    printf(RESET_COLOR); // Reset color

    printf("\n\033[1mRegister\tHex Value\tDecimal Value\tASCII Character\033[0m\n");
    for (int i = 0; i < NUM_REG_OR_CONS; i++) {
        char c;
        if (regfile[0][i] >= 32 && regfile[0][i] <= 126) {
            c = regfile[0][i];
        }
        else {
            c = '.';
        }
        switch (i) {
        case 4:
            printf("R%d (BP)\t\t%04x\t\t%d\t\t%c\n", i, regfile[0][i], regfile[0][i], c);
            break;
        case 5:
            printf("R%d (LR)\t\t%04x\t\t%d\t\t%c\n", i, regfile[0][i], regfile[0][i], c);
            break;
        case 6:
            printf("R%d (SP)\t\t%04x\t\t%d\t\t%c\n", i, regfile[0][i], regfile[0][i], c);
            break;

        case 7:
            printf("R%d (PC)\t\t%04x\t\t%d\t\t%c\n", i, regfile[0][i], regfile[0][i], c);
            break;
        default:
            printf("R%d\t\t%04x\t\t%d\t\t%c\n", i, regfile[0][i], regfile[0][i], c);
            break;
        }
    }

    printf(BRIGHT_PURPLE); // Set color to bright purple
    printf("\n=============================================================\n");
    printf("============ End of Register File Display ==================\n");
    printf("=============================================================\n");
    printf(RESET_COLOR); // Reset color
}

void change_register_value() {
    /*
        This function changes the content of a register file
    */

    while (1) {
        int selected_reg;
        unsigned short new_val;
        char user_choice;

        printf(BRIGHT_RED);
        printf("\n===== Change Register Value =====\n");
        printf("Select an option: \n");
        printf("Press and enter B -> to Go back to main menu\n");
        printf("Press and enter R -> to Choose a Register\n");
        printf("=======================================\n");
        printf("Please enter your option here -> ");
        (void)scanf(" %c", &user_choice);
        printf(RESET_COLOR);
        int ch; // This is to consume invalid input so it doesn't interfere with next input
        while ((ch = getchar()) != '\n' && ch != EOF);

        switch (user_choice) {
        case 'B':
        case 'b':
            return; // Go back to prev menu
        case 'R':
        case 'r':
            printf(ORANGE"Choose which register: \n");
            for (int i = 0; i <= 7; ++i) {
                printf("%d for -> R%d\n", i, i); // Display registers in white
            }
            printf("Enter your option here -> ");
            if (scanf("%d", &selected_reg) != 1 || selected_reg < 0 || selected_reg > 7) {
                printf("Invalid register number. Try again.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue; // Repeat the while loop
            }

            printf("Enter the new value for the register (in hexadecimal):");
            if (scanf("%4hx", &new_val) != 1) {
                printf("Invalid value. Try again.\n");
                while ((ch = getchar()) != '\n' && ch != EOF); // Consume invalid input
                continue; // Repeat the while loop
            }

            regfile[0][selected_reg] = new_val; // Set new value to the register
            printf(BRIGHT_RED"The value of register R%d has been changed to %04x.\n", selected_reg, new_val); // Display register change in white
            printf(RESET_COLOR);
            return; // Return to main menu after successful change
        default:
            printf(BRIGHT_RED"Invalid option. Try again.\n");
            printf(RESET_COLOR);
        }
    }
}