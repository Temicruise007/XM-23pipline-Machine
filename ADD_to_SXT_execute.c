/*
 * ADD_to_SXT_execute.c
 * This file contains the implementation of various execute functions for instructions.
 * Author: Temitope Onafalujo
 * Date: 2024-06-20
 */


#include "Emulator.h"

union w_b src, dst, result;

union bcd_word_nibble srcnum, dstnum;


void execute_ADD() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		result.word = dst.word + src.word;
		update_psw(src.word, dst.word, result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] + src.byte[0];
		update_psw(src.byte[0], dst.byte[0], result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_ADDC() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		result.word = dst.word + src.word + psw.c;
		update_psw(src.word + psw.c, dst.word, result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] + src.byte[0] + psw.c;
		update_psw(src.byte[0] + psw.c, dst.byte[0], result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_SUB() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		result.word = dst.word + TWOS_COMPLEMENT(src.word); //src.word + 1 is basically the two's complement
		update_psw(TWOS_COMPLEMENT(src.word), dst.word, result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] + TWOS_COMPLEMENT(src.byte[0]);
		update_psw(TWOS_COMPLEMENT(src.byte[0]), dst.byte[0], result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_SUBC() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		result.word = dst.word + (~src.word + psw.c);
		update_psw(~src.word + psw.c, dst.word, result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] + (~src.byte[0] + psw.c);
		update_psw(~src.byte[0] + psw.c, dst.byte[0], result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_DADD() {

	srcnum.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con]; //get source register or constant
	dstnum.word = regfile[0][global_inst_operands.dst]; //get destination register

	dstnum.nibble.nib0 = bcd_add(srcnum.nibble.nib0, dstnum.nibble.nib0);
	dstnum.nibble.nib1 = bcd_add(srcnum.nibble.nib1, dstnum.nibble.nib1);

	if (global_inst_operands.w_b == word) {
		dstnum.nibble.nib2 = bcd_add(srcnum.nibble.nib2, dstnum.nibble.nib2);
		dstnum.nibble.nib3 = bcd_add(srcnum.nibble.nib3, dstnum.nibble.nib3);
	}

	result.word = dstnum.word;
	update_psw(srcnum.word, dstnum.word, result.word, global_inst_operands.w_b);
	regfile[0][global_inst_operands.dst] = result.word;
}

unsigned short bcd_add(unsigned short nibble_A, unsigned short nibble_B) {

	unsigned short temp_res = nibble_A + nibble_B + psw.c;
	if (temp_res >= 10) {
		temp_res = temp_res - 10;
		psw.c = 1;
	}
	else {
		psw.c = 0;
	}
	return temp_res;
}

void execute_CMP() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) { // basically subtracting src from dst and updating psw
		result.word = dst.word + TWOS_COMPLEMENT(src.word);
		update_psw(TWOS_COMPLEMENT(src.word), dst.word, result.word, global_inst_operands.w_b);
	}
	else {
		result.byte[0] = dst.byte[0] + TWOS_COMPLEMENT(src.byte[0]);
		update_psw(TWOS_COMPLEMENT(src.byte[0]), dst.byte[0], result.byte[0], global_inst_operands.w_b);
	}
}

void execute_XOR() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		result.word = dst.word ^ src.word;
		update_psw2(result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] ^ src.byte[0];
		update_psw2(result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.w_b];
	}
}

void execute_AND() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		result.word = dst.word & src.word;
		update_psw2(result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] & src.byte[0];
		update_psw2(result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.w_b];
	}
}

void execute_OR() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == clr_bit) {
		result.word = dst.word | src.word;
		update_psw2(result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		result.byte[0] = dst.byte[0] | src.byte[0];
		update_psw2(result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_BIT() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		psw.z = ((dst.word >> src.word) & 1) == clr_bit;
	}
	else {
		psw.z = ((dst.byte[0] >> (src.word & 0xFF)) & 1) == clr_bit;
	}
}

void execute_BIC() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		unsigned short bit = ~(1 << src.word);
		result.word = dst.word & bit;
		update_psw2(result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		unsigned char bit = ~(1 << (src.word & 0xFF));
		result.byte[0] = dst.byte[0] & bit;
		update_psw2(result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_BIS() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		unsigned short bit = (1 << src.word);
		result.word = dst.word | bit;
		update_psw2(result.word, global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.word;
	}
	else {
		unsigned char bit = (1 << (src.word & 0xFF));
		result.byte[0] = dst.byte[0] | bit;
		update_psw2(result.byte[0], global_inst_operands.w_b);
		regfile[0][global_inst_operands.dst] = result.byte[0];
	}
}

void execute_MOV() {

	src.word = regfile[global_inst_operands.r_c][global_inst_operands.src_con];
	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		dst.word = src.word;
		regfile[0][global_inst_operands.dst] = dst.word;
	}
	else {
		dst.byte[0] = src.byte[0];
		regfile[0][global_inst_operands.dst] = dst.byte[0];
	}
}

void execute_SWAP() {

	unsigned short temp = regfile[0][global_inst_operands.dst];
	regfile[0][global_inst_operands.dst] = regfile[0][global_inst_operands.src_con];
	regfile[0][global_inst_operands.src_con] = temp;
}

void execute_SRA() {

	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		dst.word = dst.word >> 1;
		regfile[0][global_inst_operands.dst] = dst.word;
	}
	else {
		dst.byte[0] = dst.byte[0] >> 1;
		regfile[0][global_inst_operands.dst] = dst.byte[0];
	}
}

void execute_RRC() {

	dst.word = regfile[0][global_inst_operands.dst];

	if (global_inst_operands.w_b == word) {
		unsigned short old_carry = psw.c; //saving old carry bit
		psw.c = dst.word & 0x1;//LSbit goes to carry bit
		dst.word >>= 1; // shitf right by 1 bit
		dst.word |= (old_carry << 15); // Moving the old carry into the MSbit
		regfile[0][global_inst_operands.dst] = dst.word;
	}
	else {
		unsigned char new_carry = dst.byte[0] & 0x01;  // Extract the LSBit (which will eventually go into the carry)
		dst.byte[0] >>= 1;  // Right shift by one
		dst.byte[0] |= (psw.c << 7);  // Moving the old carry into the MSBit (of the byte)
		psw.c = new_carry;  // Update the carry flag with the new carry
		regfile[0][global_inst_operands.dst] = dst.byte[0];
	}
}

void execute_SWPB() {
	dst.word = regfile[0][global_inst_operands.dst];

	// Swap the bytes in the word
	unsigned short high_byte = (dst.word & 0xFF00) >> 8;  // Extract the high byte and shift it to the low byte position
	unsigned short low_byte = (dst.word & 0x00FF) << 8;   // Extract the low byte and shift it to the high byte position

	dst.word = low_byte | high_byte;

	regfile[0][global_inst_operands.dst] = dst.word;
}

void execute_SXT() {
	dst.word = regfile[0][global_inst_operands.dst]; // Fetch the byte from the register (though it fetches as a word)

	// Sign-extend the byte to a word
	if (dst.byte[0] & 0x80) {  //if the MSbit (sign bit) of the byte is set
		dst.word = 0xFF00 | dst.byte[0]; //extend the sign (1's) throughout the upper byte
	}
	else {
		dst.word = 0x00FF & dst.byte[0]; // If not, set upper byte to 0's
	}

	regfile[0][global_inst_operands.dst] = dst.word;
}