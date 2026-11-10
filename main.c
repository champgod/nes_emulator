
#include <stdio.h>
#include <assert.h>
#include "cpu.h"
#include "memory.h"

int main(void) {
    cpuRegisters cpu = {0};
    Memory memory = {0};

    cpu.PC = 0x8000;

    writeMemory(&memory, 0x8000, 0xA9); // LDA immediate
    writeMemory(&memory, 0x8001, 0x80);
    cpuStep(&cpu, &memory);

    printf("A: 0x%02X\n", cpu.A);
    printf("PC: 0x%04X\n", cpu.PC);
    printf("P: 0x%02X\n", cpu.P);
    return 0;
}
