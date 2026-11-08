#include <stdio.h>
#include "cpu.h"
#include "memory.h"

int main(void) {
    cpuRegisters cpu = {0};
    Memory mem = {0};

    LDA(&cpu, 20);
    STA(&cpu, 255, &mem);

    SBC(&cpu, 240);

    printf("A: %u\n", (unsigned)cpu.A);
    return 0;
}