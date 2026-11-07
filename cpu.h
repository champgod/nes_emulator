/* The purpose of this header file is to give us some kind of idea as to what our emulator is going to do. Very briefly I want to 
explain that this emulator's purpose is to emulate the hardware base that the NES traditionally runs on. We obviously do not have 
the NTSC Ricoh 2A03 CPU that the NES traditionally uses. Here we are defining what the CPU registers are and how we should operate on them. */

#include <stdint.h>

typedef struct {
    uint8_t A; // the accumulator and basically helps with arithmetic/logic operations across the cpu
    uint8_t X; // helps with calculating memory addresses but also doubles as a counter
    uint8_t Y; // same thing as X
    uint16_t PC; // holds address of following instruction
    uint8_t S; // where cpu offset is from 0x0100 (our stack is 256 bytes)
    uint8_t P; // flagging cpu state
} cpuRegisters;

void LDA(cpuRegisters* cpu, uint8_t value); // Loads memory value into A
void STA(cpuRegisters* cpu, uint16_t address); // Stores value of A in the memory address

void LDX(cpuRegisters* cpu, uint8_t value); // Loads memory value into X
void STX(cpuRegisters* cpu, uint16_t address); // Stores value of X in the memory address

void LDY(cpuRegisters* cpu, uint8_t value); // Loads memory value into Y
void STY(cpuRegisters* cpu, uint16_t address); // Stores value of Y into the memory address