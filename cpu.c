#include "cpu.h"


void LDA(cpuRegisters* cpu, uint8_t value){
    cpu->A = value;
}

void STA(cpuRegisters* cpu, uint16_t address){
    address = cpu->A; // no memory system yet
}

void LDX(cpuRegisters* cpu, uint8_t value){
    cpu->X = value;
}
void STX(cpuRegisters* cpu, uint16_t address){
    address = cpu->X;
}

void LDY(cpuRegisters* cpu, uint8_t value){
    cpu->Y = value;
}

void STY(cpuRegisters* cpu, uint16_t address){
    address = cpu->Y;
}