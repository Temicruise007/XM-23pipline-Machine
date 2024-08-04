/**
 * @file loader_functions.c
 * @brief Functions to load and process S-record files.
 * @details This file contains functions to load .xme files and process their S-records.
 * @date 2024-05-28
 * @author Temitope Onafalujo
 */

#include "Emulator.h"

char s_record[BUFFER_LEN];
FILE* s_recfile_descriptor;

/**
 * @brief Load a .xme file and process its S-records.
 */
void loadFile() {
    char filename[BUFFER_LEN];

    while (1) {
        printf("Enter the name of the .xme file: ");
        (void)scanf("%255s", filename);

        // Check for .xme extension
        const char* extension = strrchr(filename, '.');
        if (extension == NULL || strcmp(extension, ".xme") != 0) {
            printf("Error loading file, must be a .xme\n\n");
            continue;
        }

        // Check if file exists
        s_recfile_descriptor = fopen(filename, "r");
        if (s_recfile_descriptor == NULL) {
            printf("Error opening file >%s< - possibly missing. Please try again.\n\n", filename);
            continue;
        }

        // Successfully opened the file
        printf("\nFile Exists and has been loaded\n");
        break;
    }

    while (fgets(s_record, BUFFER_LEN, s_recfile_descriptor) > 0) {

        if (s_record[0] != 'S') {
            printf("Error, the following S-record is invalid-> %s\n", s_record);
        }
        else {
            switch (s_record[1]) {
            case '0':
                func_for_s0_record(s_record);
                break;
            case '1':
                func_for_s1_record(s_record);
                break;
            case '2':
                func_for_s2_record(s_record);
                break;
            case '9':
                func_for_s9_record(s_record);
                break;
            default:
                printf("Error, the following S-record is invalid --> %s\n", s_record);
            }
        }
    }

    // Close the file
    fclose(s_recfile_descriptor);
}

/**
 * @brief Process S0 record.
 * @param record The S0 record to process.
 */
void func_for_s0_record(char* record) {
    int len, i;
    char filename[20];
    unsigned char sum_all_bytes = 0, checksum_read;

    sscanf(&record[len_start_pos], "%2x", &len);

    for (i = 0; i < len - ADDRESS_AND_CHECKSUM_BYTES; i++) {
        sscanf(&record[OFFSET + (i * 2)], "%2hhx", &filename[i]);
        sum_all_bytes += filename[i];
    }

    filename[i] = '\0';
    sscanf(&record[OFFSET + (i * 2)], "%2hhx", &checksum_read);
    sum_all_bytes += len + checksum_read;
    sum_all_bytes = (sum_all_bytes)&BYTE_MASK;

    if (sum_all_bytes != BYTE_MASK) {
        printf("\nChecksum Error in S0 record, File might be corrupted\n");
        printf("Sum of all byte: %x instead of 0xFF\n\n", sum_all_bytes);
    }
    else {
        printf("Filename: %s\n\n", filename);
    }
}

/**
 * @brief Process S1 record.
 * @param record The S1 record to process.
 */
void func_for_s1_record(char* record) {
    int len, i;
    unsigned int high_byt, low_byt, address;
    unsigned char data, sum_all_bytes = 0, checksum_read;

    sscanf(&record[len_start_pos], "%2x", &len);
    sscanf(&record[addr_start_pos], "%2x%2x", &high_byt, &low_byt);

    address = high_byt << 8 | low_byt;

    for (i = 0; i < len - ADDRESS_AND_CHECKSUM_BYTES; i++) {
        sscanf(&record[OFFSET + (i * 2)], "%2hhx", &data);
        sum_all_bytes += data;
        imemory.btmem[address + i] = data;
    }

    sscanf(&record[OFFSET + (i * 2)], "%2hhx", &checksum_read);
    sum_all_bytes += len + (address & BYTE_MASK) + ((address >> 8) & BYTE_MASK) + checksum_read;
    sum_all_bytes = (sum_all_bytes)&BYTE_MASK;

    if (sum_all_bytes != BYTE_MASK) {
        printf("Checksum Error in S1 record, the record might be corrupted!!\n");
        printf("Sum of all bytes: %x instead of 0xFF\n\n", sum_all_bytes);
    }
}

/**
 * @brief Process S2 record.
 * @param record The S2 record to process.
 */
void func_for_s2_record(char* record) {
    int len, i;
    unsigned int high_byt, low_byt, address;
    unsigned char data, sum_all_bytes = 0, checksum_read;

    sscanf(&record[len_start_pos], "%2x", &len);
    sscanf(&record[addr_start_pos], "%2x%2x", &high_byt, &low_byt);

    address = high_byt << 8 | low_byt;

    for (i = 0; i < len - ADDRESS_AND_CHECKSUM_BYTES; i++) {
        sscanf(&record[OFFSET + (i * 2)], "%2hhx", &data);
        sum_all_bytes += data;
        dmemory.btmem[address + i] = data;
    }

    sscanf(&record[OFFSET + (i * 2)], "%2hhx", &checksum_read);
    sum_all_bytes += len + (address & BYTE_MASK) + ((address >> 8) & BYTE_MASK) + checksum_read;
    sum_all_bytes = (sum_all_bytes)&BYTE_MASK;

    if (sum_all_bytes != BYTE_MASK) {
        printf("Checksum Error in S2 record, the record might be corrupted!!\n");
        printf("Sum of all bytes: %x instead of 0xFF\n\n", sum_all_bytes);
    }
}

/**
 * @brief Process S9 record.
 * @param record The S9 record to process.
 */
void func_for_s9_record(char* record) {
    int len;
    unsigned int high_byt, low_byt, address;
    unsigned char sum_all_bytes = 0, checksum_read;

    sscanf(&record[len_start_pos], "%2x", &len);
    sscanf(&record[addr_start_pos], "%2x%2x", &high_byt, &low_byt);

    address = high_byt << 8 | low_byt;
    PC = address;

    sscanf(&record[OFFSET], "%2hhx", &checksum_read);
    sum_all_bytes += len + (address & BYTE_MASK) + ((address >> 8) & BYTE_MASK) + checksum_read;
    sum_all_bytes = (sum_all_bytes)&BYTE_MASK;

    if (sum_all_bytes != BYTE_MASK) {
        printf("Checksum Error in S9 record, the record might be corrupted!!\n\n");
        printf("Sum of all bytes: %x\n", sum_all_bytes);
    }
}
