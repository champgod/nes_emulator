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

    // CLI TEST (0x58)
    {
        cpuRegisters cpu = {0};
        Memory memory = {0};

        cpu.PC = 0x8000;
        cpu.P = 0x24;

        memory.memory[0x8000] = 0x58;

        cpuStep(&cpu, &memory);

        assert(cpu.PC == 0x8001);
        assert(cpu.P == 0x20);

        printf("CLI passed!\n");
    }

    // BRK TEST (0x00)
    {
        cpuRegisters cpu = {0};
        Memory memory = {0};

        cpu.PC = 0x8000;
        cpu.S = 0xFD;
        cpu.P = 0x20;

        memory.memory[0x8000] = 0x00;

        // IRQ/BRK vector points to $9000
        memory.memory[0xFFFE] = 0x00;
        memory.memory[0xFFFF] = 0x90;

        cpuStep(&cpu, &memory);

        assert(cpu.PC == 0x9000);
        assert(cpu.S == 0xFA);

        // Return address should be $8002
        assert(memory.memory[0x01FD] == 0x80);
        assert(memory.memory[0x01FC] == 0x02);

        // Status pushed with B and bit 5 set
        assert(memory.memory[0x01FB] == 0x30);

        // Interrupt-disable flag should be set
        assert((cpu.P & 0x04) != 0);

        printf("BRK passed!\n");
    }

    if (!readROM("nestest.nes", &memory)){
        printf("this didn't work man");
        return 1;
    }

    /*
    for (int i = 0; i < 5000; i++) {
        printf("PC:%04X OP:%02X A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
                cpu.PC, readMemory(&memory, cpu.PC),
                cpu.A, cpu.X, cpu.Y, cpu.P, cpu.S);
        cpuStep(&cpu, &memory);
    }
    */
    return 0;
}
