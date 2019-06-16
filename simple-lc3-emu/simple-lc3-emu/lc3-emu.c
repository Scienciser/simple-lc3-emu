#include <stdio.h>
#include <stdbool.h>

unsigned short getInstruction(FILE* file, bool* end);
void printStatus(bool initial, unsigned short* registers, unsigned short ir, unsigned short pc, char cc);
void operation(bool initial, unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short* pcPtr, unsigned short initPC);
int convMemAdToArrAd(unsigned short initPC, unsigned short memAd);
unsigned short signExtend(unsigned short offset, int length);
void setCondCode(unsigned short value, char* cc);
void lc3_ld(unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short pc, unsigned short initPC);
void lc3_lea(unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short pc, unsigned short initPC);
void lc3_ldi(unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short pc, unsigned short initPC);
void lc3_and(unsigned short ir, unsigned short* registers, char* cc);
void lc3_not(unsigned short ir, unsigned short* registers, char* cc);
void lc3_add(unsigned short ir, unsigned short* registers, char* cc);
void lc3_br(unsigned short ir, unsigned short* pc, char* cc);



// Remember that for each question, only the examined function should printStatus

int main(int argc, char* argv[])
{
	// Open file:
	FILE* file;
	file = fopen(argv[1], "r");


	// Initialisation and getting initPC:
	bool end = 0; // Checks for end of file
	unsigned short registers[8] = { 0,0,0,0,0,0,0,0 };
	unsigned short ir = 0x0000;
	char cc = 'Z';
	unsigned short pc = getInstruction(file, &end);
	unsigned short initPC = pc;
	bool initial = 1;
	printStatus(initial, registers, ir, pc, cc);	// For initial print-out
	initial = 0;


	// Reading instructions into memArr:
	unsigned short memArr[1000]; // Instructions start at memArr[500]

	int arrPos = 500;
	unsigned short irTemp;
	while (end == 0)
	{
		irTemp = getInstruction(file, &end);
		memArr[arrPos] = irTemp;
		arrPos++;

	}
	fclose(file);

	// Execute instructions:
	bool halt = 0;
	unsigned short arrAd = convMemAdToArrAd(initPC, pc);
	ir = memArr[arrAd];
	while (ir != 0xF025) {
		pc++;
		operation(initial, ir, memArr, registers, &cc, &pc, initPC);

		arrAd = convMemAdToArrAd(initPC, pc);
		ir = memArr[arrAd];
	}
}

unsigned short getInstruction(FILE* file, bool* end)
{
	int bytesRead;
	unsigned short bigByte = 0, smallByte = 0;
	bytesRead = fread(&bigByte, 1, 1, file);
	fread(&smallByte, 1, 1, file);
	unsigned short instruction = smallByte | (bigByte << 8);
	if (bytesRead == 0)
		* end = 1;
	return instruction;
}


void printStatus(bool initial, unsigned short* registers, unsigned short ir, unsigned short pc, char cc)
{
	if (initial == 1)
		printf("Initial state\n");
	else {
		printf("after executing instruction\t0x%04x\n", (int)ir);
	}
	printf("R0\t0x%04x\nR1\t0x%04x\nR2\t0x%04x\nR3\t0x%04x\nR4\t0x%04x\nR5\t0x%04x\nR6\t0x%04x\nR7\t0x%04x\n", registers[0], registers[1], registers[2], registers[3], registers[4], registers[5], registers[6], registers[7]);
	printf("PC\t0x%04x\nIR\t0x%04x\nCC\t%c\n==================\n", pc, ir, cc);
}



void operation(bool initial, unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short* pcPtr, unsigned short initPC)
{
	unsigned short opcodeMask = 0xF000;
	unsigned short opcode = ir & opcodeMask;
	unsigned short pc = *pcPtr;

	if (opcode == 0x2000) {		// LD
		lc3_ld(ir, &(*memArr), &(*registers), &(*cc), pc, initPC);
		printStatus(initial, &(*registers), ir, pc, *cc);		// For LD print-out
		return;
	}
	if (opcode == 0xE000) {		// LEA
		lc3_lea(ir, &(*memArr), &(*registers), &(*cc), pc, initPC);
		printStatus(initial, &(*registers), ir, pc, *cc);		// For LEA print-out
		return;
	}
	if (opcode == 0xA000) {		// LDI
		lc3_ldi(ir, &(*memArr), &(*registers), &(*cc), pc, initPC);
		printStatus(initial, &(*registers), ir, pc, *cc);		// For LDI print-out
		return;
	}
	if (opcode == 0x5000) {		// AND
		lc3_and(ir, &(*registers), &(*cc));
		printStatus(initial, &(*registers), ir, pc, *cc);		// For AND print-out
		return;
	}
	if (opcode == 0x9000) {		// NOT
		lc3_not(ir, &(*registers), &(*cc));
		printStatus(initial, &(*registers), ir, pc, *cc);		// For NOT print-out
		return;
	}
	if (opcode == 0x1000) {		// ADD
		lc3_add(ir, &(*registers), &(*cc));
		printStatus(initial, &(*registers), ir, pc, *cc);		// For AND print-out
		return;
	}
	if (opcode == 0x0000) {		// BR
		lc3_br(ir, &(*pcPtr), &(*cc));
		pc = *pcPtr;
		printStatus(initial, &(*registers), ir, *pcPtr, *cc);		// For BR print-out
		return;
	}
	printf("Error - instruction not implemented");
}

int convMemAdToArrAd(unsigned short initPC, unsigned short memAd)
{
	int arrAd = memAd - initPC + 500;
	if (arrAd < 0 || arrAd >= 1000) {
		printf("Error - memory address exceeds +-500 of original PC");
		return 0;
	}
	return arrAd;

}

unsigned short signExtend(unsigned short offset, int length)
{
	if (length == 5)
	{
		if ((offset & 0x0010) == 0x0010) // If value is negative
			offset = offset | 0xFFE0;
		return offset;
	}
	if (length == 9) {
		if ((offset & 0x0100) == 0x0100) // If value is negative
			offset = offset | 0xFE00;
		return offset;
	}
	printf("Invalid length");
	return offset;
}

void setCondCode(unsigned short value, char* cc)
{
	if ((value & 0x8000) == 0x8000)
		* cc = 'N';
	else {
		if (value == 0x0000) {
			*cc = 'Z';
		}
		else {
			*cc = 'P';
		}
	}
}

void lc3_ld(unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short pc, unsigned short initPC)
{
	// Separate fields:
	unsigned short destReg = 0, pcOffset9 = 0;
	destReg = ((0x0E00 & ir) >> 9); // Masks out IR[11:9]
	pcOffset9 = (0x01FF & ir); // Masks out IR[8:0]

	// Get value to be loaded
	unsigned short memAd = pc + signExtend(pcOffset9, 9);
	int arrAd = convMemAdToArrAd(initPC, memAd);
	unsigned short memValue = memArr[arrAd];

	// Perform assignment:
	registers[destReg] = memValue;

	// Update condition code:
	setCondCode(memValue, &(*cc));

}

void lc3_lea(unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short pc, unsigned short initPC)
{
	// Separate fields:
	unsigned short destReg = 0, pcOffset9 = 0;
	destReg = ((0x0E00 & ir) >> 9); // Masks out IR[11:9]
	pcOffset9 = (0x01FF & ir); // Masks out IR[8:0] 

	// Get value to be loaded:
	unsigned short memAd = pc + signExtend(pcOffset9, 9);
	registers[destReg] = memAd;

	// Update condition code:
	setCondCode(memAd, &(*cc));
}

void lc3_ldi(unsigned short ir, unsigned short* memArr, unsigned short* registers, char* cc, unsigned short pc, unsigned short initPC)
{
	// Separate fields:
	unsigned short destReg = 0, pcOffset9 = 0, memAddress = 0;
	destReg = ((0x0E00 & ir) >> 9); // Masks out IR[11:9]
	pcOffset9 = (0x01FF & ir); // Masks out IR[8:0] 

	// Get value to be loaded:
	unsigned short outerMemAd = pc + signExtend(pcOffset9, 9);
	unsigned short outerArrAd = convMemAdToArrAd(initPC, outerMemAd);
	unsigned short memAd = memArr[outerArrAd];
	unsigned short arrAd = convMemAdToArrAd(initPC, memAd);
	unsigned short memValue = memArr[arrAd];

	// Perform assignment:
	registers[destReg] = memValue;

	// Update condition code:
	setCondCode(memValue, &(*cc));
}

void lc3_and(unsigned short ir, unsigned short* registers, char* cc)
{
	// Separate fields and get operands:
	unsigned short destReg = 0, sourceReg1 = 0;
	destReg = ((0x0E00 & ir) >> 9); // Masks out IR[11:9]
	sourceReg1 = ((0x01C0 & ir) >> 6); // Masks out IR[8:6]
	unsigned short operand1 = registers[sourceReg1];

	unsigned short operand2 = 0;

	if ((ir & 0x0020) == 0x0020) {		// Immediate mode
		unsigned short imm5 = 0;
		imm5 = (0x001F & ir);
		operand2 = signExtend(imm5, 5);
	}
	else {								// Register mode
		unsigned short sourceReg2 = 0;
		sourceReg2 = 0x0007 & ir;
		operand2 = registers[sourceReg2];

	}

	// Calculate and assign
	unsigned short result = operand1 & operand2;
	registers[destReg] = result;


	// Update condition code
	setCondCode(result, &(*cc));
}

void lc3_not(unsigned short ir, unsigned short* registers, char* cc)
{
	// Separate fields and get operands:
	unsigned short destReg = 0, sourceReg = 0;
	destReg = ((0x0E00 & ir) >> 9); // Masks out IR[11:9]
	sourceReg = ((0x01C0 & ir) >> 6); // Masks out IR[8:6]
	unsigned short operand = registers[sourceReg];

	// Calculate and assign:
	unsigned short result = ~operand;
	registers[destReg] = result;

	// Update condition code
	setCondCode(result, &(*cc));
}

void lc3_add(unsigned short ir, unsigned short* registers, char* cc)
{
	// Separate fields and get operands:
	unsigned short destReg = 0, sourceReg1 = 0;
	destReg = ((0x0E00 & ir) >> 9); // Masks out IR[11:9]
	sourceReg1 = ((0x01C0 & ir) >> 6); // Masks out IR[8:6]
	unsigned short operand1 = registers[sourceReg1];

	unsigned short operand2 = 0;

	if ((ir & 0x0020) == 0x0020) {		// Immediate mode
		unsigned short imm5 = 0;
		imm5 = (0x001F & ir);
		operand2 = signExtend(imm5, 5);
	}
	else {								// Register mode
		unsigned short sourceReg2 = 0;
		sourceReg2 = 0x0007 & ir;
		operand2 = registers[sourceReg2];

	}

	// Calculate and assign
	unsigned short result = operand1 + operand2;
	registers[destReg] = result;


	// Update condition code
	setCondCode(result, &(*cc));
}

void lc3_br(unsigned short ir, unsigned short* pc, char* cc)
{
	// Separate fields and get operands:
	bool n = 0, z = 0, p = 0;
	if ((0x0800 & ir) == 0x0800)
		n = 1;
	if ((0x0400 & ir) == 0x0400)
		z = 1;
	if ((0x0200 & ir) == 0x0200)
		p = 1;
	unsigned short pcOffset9 = 0;
	pcOffset9 = (0x01FF & ir); // Masks out IR[8:0] 

	// Modify PC:
	char charN = 'N', charZ = 'Z', charP = 'P';
	unsigned short newPC = *pc + signExtend(pcOffset9, 9);
	if ((n == 1) && (*cc == charN)) {
		*pc = newPC;
		return;
	}
	if ((z == 1) && (*cc == charZ)) {
		*pc = newPC;
		return;
	}
	if ((p == 1) && (*cc == charP)) {
		*pc = newPC;
		return;
	}

}