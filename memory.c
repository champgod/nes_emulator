#include "memory.h"

uint8_t readMemory(Memory* memory, uint16_t address){
    return memory->memory[address]; //returns the data at the address
}
void writeMemory(Memory* memory, uint16_t address, uint8_t data){
    memory->memory[address] = data;
}