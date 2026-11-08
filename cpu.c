#include "cpu.h"

/*
 * Processor Status Register (P): NV1BDIZC
 * Bit 7 - N: Negative
 * Bit 6 - V: Overflow
 * Bit 5 - 1: Special status bit
 * Bit 4 - B: Break (special behavior)
 * Bit 3 - D: Decimal (ignored by NES arithmetic)
 * Bit 2 - I: Interrupt Disable
 * Bit 1 - Z: Zero
 * Bit 0 - C: Carry
 */

 // Helper function implementation
static void updateZN(cpuRegisters* cpu, uint8_t value){
    if (value == 0){
        cpu->P |= (1 << 1); // Bitwise OR - if our value is 0 then we update the zero flag setting it to 1
    }else{
        cpu->P &= ~(1 << 1); // Bitwise AND - if our value is not 0 then we set the zero flag bit to 1 and then flip that to 0
    }
    if(value & (1<<7)){ // Checks if there is a bit that is set at 7
        cpu->P |= (1<<7); // Sets the bit at 7 to 1
    }else{
        cpu->P &= ~(1<<7); // Flips the bit at 7 and then sets it to 0
    }
}

/* Instruction calls but written in C */

void LDA(cpuRegisters* cpu, uint8_t value){
    cpu->A = value;
    updateZN(cpu, value);
}

void STA(cpuRegisters* cpu, uint16_t address, Memory* memory){
    writeMemory(memory, address, cpu->A);
}

void LDX(cpuRegisters* cpu, uint8_t value){
    cpu->X = value;
    updateZN(cpu, value);
}
void STX(cpuRegisters* cpu, uint16_t address, Memory* memory){
    writeMemory(memory, address, cpu->X);
}

void LDY(cpuRegisters* cpu, uint8_t value){
    cpu->Y = value;
    updateZN(cpu, value);
}

void STY(cpuRegisters* cpu, uint16_t address, Memory* memory){
   writeMemory(memory, address, cpu->Y);
}

void TAX(cpuRegisters* cpu){
    cpu->X = cpu->A;
    updateZN(cpu, cpu->X);
}

void TXA(cpuRegisters* cpu){
    cpu->A = cpu->X;
    updateZN(cpu, cpu->A);
}

void TAY(cpuRegisters* cpu){
    cpu->Y = cpu->A;
    updateZN(cpu, cpu->Y);
}

void TYA(cpuRegisters* cpu){
    cpu->A = cpu->Y;
    updateZN(cpu, cpu->A);
}