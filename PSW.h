/*
 * PSW.h
 * This file defines the Program Status Word (PSW) structure and related macros.
 * Author: Temitope Onafalujo
 * Date: 2024-06-28
 */

#pragma once
#ifndef PSW_H
#define PSW_H

struct psw_bits {
    unsigned short c : 1;
    unsigned short z : 1;
    unsigned short n : 1;
    unsigned short slp : 1;
    unsigned short v : 1;
    unsigned short current : 3;    // Current priority
    unsigned short faulting : 1;   // 0 - No fault; 1 - Active fault
    unsigned short reserved : 4;
    unsigned short previous : 3;   // Previous priority
};

#define B15(x) (((x) >> 15) & 0x01) // Extracts bit 15
#define B7(x)  (((x) >> 7) & 0x01)  // Extracts bit 7

extern struct psw_bits psw; // Declaring a variable type PSW

#endif // PSW_H
