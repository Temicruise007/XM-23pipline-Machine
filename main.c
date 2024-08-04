/**
 * @file main.c
 * @brief Main file for the emulator project.
 * @details This file contains the main function and user control functions for the emulator.
 * @date 2024-06-20
 * @author Temitope Onafalujo
 */

#include "Emulator.h"

union mem imemory = { 0 };
union mem dmemory = { 0 };

// Define the 2D array regfile
unsigned short regfile[NUM_VALUES][NUM_REG_OR_CONS] = {
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0xFFFF}
};

int program_running = TRUE; // Global flag to indicate if the program is running
unsigned short breakpoint_address = INVALID; // Initialized to an invalid address
int breakpoint_set = FALSE; // Flag to indicate if a breakpoint has been set

bool d_bubble = false;
bool e_bubble = false;

DiagnosticInfo diagnostics[diagnostic_index]; // Adjust size as needed
int diag_index = 0;

void user_control();
void set_breakpoint();
void display_memory_submenu();

/**
 * @brief Main function to run the emulator.
 * @return int Return status code.
 */
int main() {
    int temp_ch;
    int choice;

    // Initialize signal handling
    init_signal();

    do {
        printf("1. Load a new file\n");
        printf("2. Start Emulation\n");
        printf("3. Change memory value\n");
        printf("4. Quit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n\n");
            while ((temp_ch = getchar()) != '\n' && temp_ch != EOF); // Clear the invalid input
            continue; // Skip the rest of the loop and prompt the user again
        }

        switch (choice) {
        case 1:
            loadFile();
            breakpoint_set = FALSE; // Reset the breakpoint flag when a new file is loaded
            break;
        case 2:
            program_running = TRUE; // Reset the program_running flag
            user_control();
            break;
        case 3:
            change_memory_value();
            break;
        case 4:
            printf("Exiting the program.\n");
            program_running = 0;
            break;
        default:
            printf("Invalid choice. Please try again.\n\n");
        }
    } while (choice != 4);

    return 0;
}

/**
 * @brief User control function for the emulator.
 */
void user_control() {
    int ch;
    char user_choice;
    int menu_displayed = FALSE;
    int single_step = FALSE;
    int control_c_detected;

    while (program_running) {
        if (!menu_displayed) {
            printf("\nWhat would you like to choose from the menu: \n");
            printf("Press and enter I -> to Toggle Single Step Execution (currently %s)\n", single_step ? "Enabled" : "Disabled");
            printf("Press and enter R -> to display Registers\n");
            printf("Press and enter V -> to Change Register Value\n");
            printf("Press and enter G -> to Go (execute continuously)\n");
            printf("Press and enter B -> to Set Breakpoint\n");
            printf("Press and enter P -> to Display PSW bits\n");
            printf("Press and enter M -> to Display Memory\n");
            printf("Press and enter Q -> to Quit\n");
            printf("Enter option here ==> ");
            menu_displayed = TRUE; // Set the flag to indicate that the menu has been displayed
        }
        else {
            printf("\nEnter option (or ? to display the menu) ==> ");
        }

        (void)scanf(" %c", &user_choice);
        while ((ch = getchar()) != '\n' && ch != EOF);

        if (user_choice == '?') {
            menu_displayed = FALSE; // Reset the flag to display the menu again
            continue;
        }

        switch (user_choice) {
        case 'I':
        case 'i':
            single_step = !single_step; // Toggle the single step flag
            printf("Single step is now %s.\n", single_step ? "Enabled" : "Disabled");
            break;
        case 'R':
        case 'r':
            displayRegisterFile();
            break;
        case 'V':
        case 'v':
            change_register_value();
            break;
        case 'G':
        case 'g':
            control_c_detected = FALSE;
            if (single_step) {
                // Single-step mode: Execute two clock cycles (even and odd)
                run_xm(); // First clock cycle (even)
                if (program_running) {
                    run_xm(); // Second clock cycle (odd)
                }
            }
            else {
                // Continuous mode: Execute until the program is no longer running or a breakpoint is reached or control-C is detected
                while (program_running && (!breakpoint_set || last_executed_address != breakpoint_address)) {
                    if (ctrl_c_fnd) { // Check if control-C was detected
                        ctrl_c_fnd = FALSE;
                        control_c_detected = TRUE;
                        break; // Exit the while loop
                    }
                    run_xm();
                }
                if (!control_c_detected) {
                    printf("End of instruction execution cycle or breakpoint reached.\n\n");
                }
                else {
                    printf("Execution interrupted by Control-C\n\n");
                }
            }
            program_running = TRUE; // Reset the flag to keep the menu active
            break;
        case 'B':
        case 'b':
            set_breakpoint();
            break;
        case 'P':
        case 'p':
            displayPswBits();
            break;
        case 'M':
        case 'm':
            display_memory_submenu();
            break;
        case 'Q':
        case 'q':
            program_running = FALSE;
            break; // Exit the user control function to end the program
        default:
            printf("Invalid option. Try again.\n\n");
        }
    }
}

/**
 * @brief Submenu to display memory regions.
 */
void display_memory_submenu() {
    int ch;
    char mem_choice;

    printf("\nDisplay which memory? (I for Instruction, D for Data): ");
    (void)scanf(" %c", &mem_choice);
    while ((ch = getchar()) != '\n' && ch != EOF); //consume invalid input

    switch (mem_choice) {
    case 'I':
    case 'i':
        displayMemoryRegion(imemory.btmem);
        break;
    case 'D':
    case 'd':
        displayMemoryRegion(dmemory.btmem);
        break;
    default:
        printf("Invalid option.\n\n");
        break;
    }
}

/**
 * @brief Set a breakpoint at a specified address.
 */
void set_breakpoint() {
    unsigned short address;
    int ch;

    printf("Enter the breakpoint address (in hexadecimal): ");
    if (scanf("%4hx", &address) != 1) {
        printf("Invalid address. Please enter a valid hexadecimal number.\n\n");
        while ((ch = getchar()) != '\n' && ch != EOF); // Consume invalid input
        return;
    }

    breakpoint_address = address;
    breakpoint_set = TRUE; // Set the flag to indicate that a breakpoint has been set
    printf("Breakpoint set at address: 0x%04X\n\n", breakpoint_address);

    while ((ch = getchar()) != '\n' && ch != EOF); // Consume any leftover invalid input
}
