/* The purpose of this header file is to give us a general sense of how we're going to emulate the CPU address space of the NES. 
The address space is 64 KiB, which is 65, 536 bytes. So we're going to create an array that stores all of these values */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {
    uint8_t memory [65536];

} Memory;

uint8_t readMemory(Memory* memory, uint16_t address);
void writeMemory(Memory* memory, uint16_t address, uint8_t data);

#endif