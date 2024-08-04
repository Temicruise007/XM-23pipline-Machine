/*
 * psw.c
 * This file contains functions to update the Program Status Word (PSW) based on operation results.
 * Author: Temitope Onafalujo
 * Date: 2024-06-28
 */

#include "Emulator.h"

#define BYTE_MASK 0xFF // Mask for a single byte

 // Carry and overflow lookup tables
unsigned carry[2][2][2] = { 0, 0, 1, 0, 1, 0, 1, 1 };
unsigned overflow[2][2][2] = { 0, 1, 0, 0, 0, 0, 1, 0 };

struct psw_bits psw; // Declaring a variable type PSW

/*
 * Updates the PSW bits (V, N, Z, C) using src, dst, and res values and whether word or byte.
 * Used for ADD, ADDC, SUB, and SUBC instructions.
 */
void update_psw(unsigned short src, unsigned short dst, unsigned short res, unsigned short wb) {
    unsigned short mss, msd, msr; // Most significant src, dst, and res bits

    if (wb == 0) {
        // Word
        mss = B15(src);
        msd = B15(dst);
        msr = B15(res);
    }
    else {
        // Byte
        mss = B7(src);
        msd = B7(dst);
        msr = B7(res);
        res &= BYTE_MASK; // Mask high byte for 'z' check
    }

    // Update carry bit based on lookup table
    psw.c = carry[mss][msd][msr];

    // Update zero bit
    psw.z = (res == 0);

    // Update negative bit
    psw.n = (msr == 1);

    // Update overflow bit based on lookup table
    psw.v = overflow[mss][msd][msr];
}

/*
 * Updates the PSW bits based on the result of the BIC and BIS instructions.
 */
void update_psw2(unsigned short result, unsigned short wb) {
    unsigned short res_bit;

    switch (wb) {
    case 0:
        // Word
        res_bit = B15(result);
        break;
    case 1:
        // Byte
        res_bit = B7(result);
        break;
    default:
        break;
    }

    // Update negative bit
    psw.n = (res_bit == 1);

    // Update zero bit
    psw.z = (res_bit == 0);
}
