/* The purpose of this header file is to give us a general sense of how we're going to emulate the CPU address space of the NES. 
The address space is 64 KiB, which is 65, 536 bytes. So we're going to create an array that stores all of these values

https://www.nesdev.org/wiki/CPU_memory_map

Right now we're working with testing the CPU, so we're going to work with 0x8000 - 0xFFFF in our memory. 
Should be worth noting the intervals indicate how much memory we allocate to what. We still have yet to implement
the PPU, APU, and I/O Registers.*/

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {
    uint8_t memory [65536];

} Memory;

uint8_t readMemory(Memory* memory, uint16_t address);
void writeMemory(Memory* memory, uint16_t address, uint8_t data);

#endif