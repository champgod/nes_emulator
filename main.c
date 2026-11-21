#include <stdio.h>
#include <assert.h>
#include "cpu.h"
#include "memory.h"
#include "romloader.h"

int main(void) {
    cpuRegisters cpu = {0};
    Memory memory = {0};

    cpu.PC = 0xC000;
    cpu.S = 0xFD;
    cpu.P = 0x24;

    if (!readROM("nestest.nes", &memory)){
        printf("this didn't work man");
        return 1;
    }

    for (int i = 0; i < 8991; i++) {
        printf("PC:%04X OP:%02X A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
                cpu.PC, readMemory(&memory, cpu.PC),
                cpu.A, cpu.X, cpu.Y, cpu.P, cpu.S);
        cpuStep(&cpu, &memory);
    }
    return 0;
}
