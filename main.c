#include <stdio.h>
#include "cpu.h"
#include "memory.h"

int main(void) {
    cpuRegisters cpu = {0};

    LDA(&cpu, 128);
    LDA(&cpu, 0);

    printf("A: %u\n", (unsigned)cpu.A);
    printf("P: %u\n", (unsigned)cpu.P);

    Memory mem = {0};

    writeMemory(&mem, 255, 20);

    printf("Memory at 255: %u\n", readMemory(&mem, 255));
    return 0;
}