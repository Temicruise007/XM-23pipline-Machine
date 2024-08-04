/*
 * Bitwise_manipulation.h
 * This file contains macros for bit manipulation and color definitions for output.
 * Author: Temitope Onafalujo
 * Date: 2024-06-28
 */

#pragma once
#ifndef BITWISE_MANIPULATION_H
#define BITWISE_MANIPULATION_H

 // Color definitions for terminal output
#define RESET_COLOR   "\033[0m"
#define BLACK         "\033[0;30m"
#define RED           "\033[0;31m"
#define GREEN         "\033[0;32m"
#define YELLOW        "\033[0;33m"
#define LIGHT_YELLOW "\033[1;33m"
#define BLUE          "\033[0;34m"
#define PURPLE        "\033[0;35m"
#define CYAN          "\033[0;36m"
#define WHITE         "\033[0;37m"

// Bright or Light colors
#define BRIGHT_BLACK   "\033[1;30m"
#define BRIGHT_RED     "\033[1;31m"
#define BRIGHT_GREEN   "\033[1;32m"
#define BRIGHT_YELLOW  "\033[1;33m"
#define BRIGHT_BLUE    "\033[1;34m"
#define BRIGHT_PURPLE  "\033[1;35m"
#define BRIGHT_CYAN    "\033[1;36m"
#define BRIGHT_WHITE   "\033[1;37m"
#define ORANGE         "\033[1;38;5;208m"

// Macros for bit-masking
#define DST(x) ((x) & 0x07)
#define SRC_CON(x) (((x) >> 3) & 0x07)
#define WB(x) (((x) >> 6) & 0x01)
#define RC(x) (((x) >> 7) & 0x01)
#define BL_OFFSET(x)	((x) & 0x1FFF)
#define OTHER_BRANCH_CHECK(x) (((x) >> 10) & 0x7)
#define OTHER_BRANCHES_OFFSET(x)	((x) & 0x03FF)
#define RELATIVE_OFFSET(x) (((x) >> 7) & 0x7F)
#define FIRST_3_BITS(x) (((x) >> 13) & 0x07)
#define ADD_TO_BIS_CHECK(x) (((x) >> 8) & 0x0F)
#define MOVL_TO_MOVH_CHECK(x) (((x) >> 11) & 0x03)
#define MOV_TO_CLRCC(x) (((x) >> 7) & 0x1F) // Extracts bits 11 to 7 to verify instructions from MOV to SWAP
#define FIRST_4_BITS(x) (((x) >> 12) & 0x0F)
#define EXTRACT_3_BITS(inst, bit) (((inst) >> (bit)) & 0x07)
#define EXTRACT_2_BITS(inst, bit) (((inst) >> (bit)) & 0x03)
#define EXTRACT_BIT(inst, bit) (((inst) >> (bit)) & 0x01)
#define DATA(x) (((x) >> 3) & BYTE_MASK) // Extracts bits 3 to 10 of the MOVL-MOVH instructions

// Macros for one's and two's complement computation
#define ONES_COMPLEMENT(x) (~(x))
#define TWOS_COMPLEMENT(x) (ONES_COMPLEMENT(x) + 1)

// Macro to sign-extend a given value `x` based on the MSB at `bit` position
#define SIGN_EXTEND(x, bit) (((x) ^ (1 << bit)) - (1 << bit))

#endif // BITWISE_MANIPULATION_H
