/* The purpose of this header file is to give us some kind of idea as to what our emulator is going to do. Very briefly I want to 
explain that this emulator's purpose is to emulate the hardware base that the NES traditionally runs on. We obviously do not have 
the NTSC Ricoh 2A03 CPU that the NES traditionally uses. Here we are defining what the CPU registers are and how we should operate on them. */
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

typedef struct {
    uint8_t A; // the accumulator and basically helps with arithmetic/logic operations across the cpu
    uint8_t X; // helps with calculating memory addresses but also doubles as a counter
    uint8_t Y; // same thing as X
    uint16_t PC; // holds address of following instruction
    uint8_t S; // where cpu offset is from 0x0100 (our stack is 256 bytes)
    uint8_t P; // flagging cpu state
} cpuRegisters;

/* ACCESS */

void LDA(cpuRegisters* cpu, uint8_t value); // Loads memory value into A
void STA(cpuRegisters* cpu, uint16_t address, Memory* memory); // Stores value of A in the memory address

void LDX(cpuRegisters* cpu, uint8_t value); // Loads memory value into X
void STX(cpuRegisters* cpu, uint16_t address, Memory* memory); // Stores value of X in the memory address

void LDY(cpuRegisters* cpu, uint8_t value); // Loads memory value into Y
void STY(cpuRegisters* cpu, uint16_t address, Memory* memory); // Stores value of Y into the memory address

void LAX(cpuRegisters* cpu, uint8_t value); // Loads memory value into A and X

/* TRANSFER */
void TAX(cpuRegisters* cpu); // Transfer A to X
void TXA(cpuRegisters* cpu); // Transfer X to A
void TAY(cpuRegisters* cpu); // Transfer A to Y
void TYA(cpuRegisters* cpu); // Transfer Y to A

/* ARITHMETIC*/
void ADC(cpuRegisters* cpu, uint8_t value); // Adds with carry  - A = A + memory value + C
void SBC(cpuRegisters* cpu, uint8_t value); // Subtracts with carry A = A - memory value - ~C
void INC(cpuRegisters* cpu, Memory* memory, uint16_t address); // Increments the memory, memory = memory + 1
void DEC(cpuRegisters* cpu, Memory* memory, uint16_t address); // Decrements the memory, memory = memory - 1
void INX(cpuRegisters* cpu); // Increments X, X = X + 1
void DEX(cpuRegisters* cpu); // Decrements X, X = X - 1 
void INY(cpuRegisters* cpu); // Increments Y, Y = Y + 1
void DEY(cpuRegisters* cpu); // Decrements Y, Y = Y - 1

/* SHIFT */
void ASL(cpuRegisters* cpu, Memory* memory, uint16_t address, bool accumulator);
void LSR(cpuRegisters* cpu, Memory* memory, uint16_t address, bool accumulator);
void ROL(cpuRegisters* cpu, Memory* memory, uint16_t address, bool accumulator);
void ROR(cpuRegisters* cpu, Memory* memory, uint16_t address, bool accumulator);

/* BITWISE */
void AND(cpuRegisters* cpu, uint8_t memory); // A = A & memory
void ORA(cpuRegisters* cpu, uint8_t memory); // A = A | memory
void EOR(cpuRegisters* cpu, uint8_t memory); // A = A ^ memory
void BIT(cpuRegisters* cpu, uint8_t memory); // A & memory bit test

/* COMPARE */
void CMP(cpuRegisters* cpu, uint8_t memory); // Compares memory with A and updates status flags but no registers
void CPX(cpuRegisters* cpu, uint8_t memory); // Compares memory with X and updates status flags but no registers
void CPY(cpuRegisters* cpu, uint8_t memory); // Compares memory with Y and updates status flags but no registers

/* BRANCH */
void BCC(cpuRegisters* cpu, int8_t offset);
void BCS(cpuRegisters* cpu, int8_t offset);
void BEQ(cpuRegisters* cpu, int8_t offset);
void BNE(cpuRegisters* cpu, int8_t offset);
void BPL(cpuRegisters* cpu, int8_t offset);
void BMI(cpuRegisters* cpu, int8_t offset);
void BVC(cpuRegisters* cpu, int8_t offset);
void BVS(cpuRegisters* cpu, int8_t offset);

/* JUMP*/
void JMP(cpuRegisters* cpu, uint16_t address); // Sets PC to memory
void JSR(cpuRegisters* cpu, Memory* memory, uint16_t address); // Jump to subroutine
void RTS(cpuRegisters* cpu, Memory* memory); // Return from subroutine
void BRK(cpuRegisters* cpu, Memory* memory); // Break
void RTI(cpuRegisters* cpu, Memory* memory); // Return from interrupt

/*STACK*/
void PHA(cpuRegisters* cpu, Memory* memory); 
void PLA(cpuRegisters* cpu, Memory* memory);
void PHP(cpuRegisters* cpu, Memory* memory);
void PLP(cpuRegisters* cpu, Memory* memory);
void TXS(cpuRegisters* cpu); // Transfers X to S
void TSX(cpuRegisters* cpu); // Transfers S to X

/*FLAGs*/
void CLC(cpuRegisters* cpu); // Clears C flag
void CLI(cpuRegisters* cpu); // Clears I flag
void CLD(cpuRegisters* cpu); // Clears D flag
void CLV(cpuRegisters* cpu); // Clears V flag
void SEC(cpuRegisters* cpu); // Sets C flag
void SEI(cpuRegisters* cpu); // Sets I flag
void SED(cpuRegisters* cpu); // Sets D flag

/*OTHER*/
void NOP(void);


void cpuStep(cpuRegisters* cpu, Memory* memory); 

#endif