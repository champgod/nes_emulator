
#include <stdio.h>
#include <assert.h>
#include "cpu.h"
#include "memory.h"

int main(void) {
    cpuRegisters cpu = {0};
    Memory mem = {0};

    // TEST 1: ADC unsigned wraparound
    cpu.A = 255;
    cpu.P = 0;
    ADC(&cpu, 1);

    assert(cpu.A == 0);
    assert((cpu.P & (1 << 0)) != 0); // Carry
    assert((cpu.P & (1 << 1)) != 0); // Zero
    assert((cpu.P & (1 << 7)) == 0); // Negative

    // TEST 2: ADC signed overflow
    cpu.A = 127;
    cpu.P = 0;
    ADC(&cpu, 1);

    assert(cpu.A == 128);
    assert((cpu.P & (1 << 6)) != 0); // Overflow
    assert((cpu.P & (1 << 7)) != 0); // Negative
    assert((cpu.P & (1 << 0)) == 0); // No carry

    // TEST 3: SBC with no borrow
    cpu.A = 50;
    cpu.P = 1; // Incoming carry = 1
    SBC(&cpu, 20);

    assert(cpu.A == 30);
    assert((cpu.P & (1 << 0)) != 0);

    // TEST 4: SBC with borrow
    cpu.A = 10;
    cpu.P = 1;
    SBC(&cpu, 20);

    assert(cpu.A == 246);
    assert((cpu.P & (1 << 0)) == 0);
    assert((cpu.P & (1 << 7)) != 0);

    // TEST 5: SBC signed overflow
    cpu.A = 128;
    cpu.P = 1;
    SBC(&cpu, 1);

    assert(cpu.A == 127);
    assert((cpu.P & (1 << 6)) != 0);

    // TEST 6: INX wraps to zero
    cpu.X = 255;
    cpu.P = 0;
    INX(&cpu);

    assert(cpu.X == 0);
    assert((cpu.P & (1 << 1)) != 0);

    // TEST 7: DEX wraps to 255
    cpu.X = 0;
    cpu.P = 0;
    DEX(&cpu);

    assert(cpu.X == 255);
    assert((cpu.P & (1 << 7)) != 0);

    // TEST 8: INY and DEY
    cpu.Y = 127;
    cpu.P = 0;
    INY(&cpu);

    assert(cpu.Y == 128);
    assert((cpu.P & (1 << 7)) != 0);

    cpu.Y = 1;
    DEY(&cpu);

    assert(cpu.Y == 0);
    assert((cpu.P & (1 << 1)) != 0);

    // TEST 9: INC memory wraparound
    writeMemory(&mem, 0x0200, 255);
    cpu.P = 0;
    INC(&cpu, &mem, 0x0200);

    assert(readMemory(&mem, 0x0200) == 0);
    assert((cpu.P & (1 << 1)) != 0);

    // TEST 10: DEC memory wraparound
    writeMemory(&mem, 0x0200, 0);
    cpu.P = 0;
    DEC(&cpu, &mem, 0x0200);

    assert(readMemory(&mem, 0x0200) == 255);
    assert((cpu.P & (1 << 7)) != 0);

    printf("All 10 arithmetic tests passed!\n");
    return 0;
}
