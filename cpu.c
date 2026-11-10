#include "cpu.h"
#include <stdio.h>


/*
 * Processor Status Register (P): NV1BDIZC
 * Bit 7 - N: Negative
 * Bit 6 - V: Overflow
 * Bit 5 - 1: Special status bit
 * Bit 4 - B: Break (special behavior)
 * Bit 3 - D: Decimal (ignored by NES arithmetic)
 * Bit 2 - I: Interrupt Disable
 * Bit 1 - Z: Zero
 * Bit 0 - C: Carry
 */

 // Helper function implementation
static void updateZN(cpuRegisters* cpu, uint8_t value){
    if (value == 0){
        cpu->P |= (1 << 1); // Bitwise OR - if our value is 0 then we update the zero flag setting it to 1
    }else{
        cpu->P &= ~(1 << 1); // Bitwise AND - if our value is not 0 then we set the zero flag bit to 1 and then flip that to 0
    }
    if(value & (1<<7)){ // Checks if there is a bit that is set at 7
        cpu->P |= (1<<7); // Sets the bit at 7 to 1
    }else{
        cpu->P &= ~(1<<7); // Flips the bit at 7 and then sets it to 0
    }
}

/* IMMEDIATE ADDRESSING METHOD */
static uint8_t immediateAddress(cpuRegisters* cpu, Memory* memory) {
    return readMemory(memory, cpu->PC++);
}

/* ABSOLUTE ADDRESSING METHOD */
static uint16_t absoluteAddress(cpuRegisters* cpu, Memory* memory) {
    uint8_t low = readMemory(memory, cpu->PC++);
    uint8_t high = readMemory(memory, cpu->PC++);

    return ((uint16_t)high << 8) | low;
}

/* ZERO PAGE ADDRESS METHOD */
static uint8_t zeroPageAddress(cpuRegisters* cpu, Memory* memory) {
    return readMemory(memory, cpu->PC++); // this should ideally read the instruction and give us the operand which is an address

}
/* INDEXED INDIRECT ADDRESS X METHOD */
static uint16_t indirectAddressX(cpuRegisters* cpu, Memory* memory) {
    uint8_t pointer = readMemory(memory, cpu->PC++); // recall that an indirect address will give us yet another address. what then?
    pointer = (uint8_t)(pointer + cpu->X);

    uint8_t low = readMemory(memory, pointer); // we take low as our pointer address (least significant, following endian ordering)
    uint8_t high = readMemory(memory, (uint8_t) (pointer + 1)); // we take high as the following address

    return ((uint16_t) high << 8) | low; // we combine the two and return them
}

/* INDIRECT INDEXED ADDRESS Y METHOD */
static uint16_t indirectAddressY(cpuRegisters* cpu, Memory* memory) {
    uint8_t pointer = readMemory(memory, cpu->PC++); // recall that an indirect address will give us yet another address. what then?

    uint8_t low = readMemory(memory, pointer); // we take low as our pointer address (least significant, following endian ordering)
    uint8_t high = readMemory(memory, (uint8_t) (pointer + 1)); // we take high as the following address

    return (((uint16_t) high << 8) | low) + cpu->Y; // we combine the two and return them
}

/* END OF HELPER FUNCTIONS (what a pain to write but this will be worth it) */
/*==================================================================*/

/* Instruction calls but written in C */

void LDA(cpuRegisters* cpu, uint8_t value){
    cpu->A = value;
    updateZN(cpu, value);
}

void STA(cpuRegisters* cpu, uint16_t address, Memory* memory){
    writeMemory(memory, address, cpu->A);
}

void LDX(cpuRegisters* cpu, uint8_t value){
    cpu->X = value;
    updateZN(cpu, value);
}
void STX(cpuRegisters* cpu, uint16_t address, Memory* memory){
    writeMemory(memory, address, cpu->X);
}

void LDY(cpuRegisters* cpu, uint8_t value){
    cpu->Y = value;
    updateZN(cpu, value);
}

void STY(cpuRegisters* cpu, uint16_t address, Memory* memory){
   writeMemory(memory, address, cpu->Y);
}

void TAX(cpuRegisters* cpu){
    cpu->X = cpu->A;
    updateZN(cpu, cpu->X);
}

void TXA(cpuRegisters* cpu){
    cpu->A = cpu->X;
    updateZN(cpu, cpu->A);
}

void TAY(cpuRegisters* cpu){
    cpu->Y = cpu->A;
    updateZN(cpu, cpu->Y);
}

void TYA(cpuRegisters* cpu){
    cpu->A = cpu->Y;
    updateZN(cpu, cpu->A);
}

void ADC(cpuRegisters* cpu, uint8_t value){
    // Adds with carry: A = A + value + C
    /* There are 4 flags to updated from our P register here, the zero flag (Z), the negative flag (N), the carry flag (C), and the overflow flag (V). 
       We need to note that with 8 bit integers our upper bound is 255 and our lower bound is 0. If we were to add 250 + 10, what 
       would happen? We notice that it exceeds the upper bound 255 and that 260 cannot be stored as an 8 bit value. What we do 
       is have our A wrap back around. So we add 5 onto 250 get 255 and then 0, 1, 2, 3, and land on 4. How would this look in terms of
       our flags? We update our carry flag to 1 since we exceeded 255, our zero flag is set to 1 if our add with carry results in a 0 value.
       Our negative flag is set to whatever the 7th bit position of our result is. Finally our overflow flag is set to 1 if our result is not within
       -128 and 127. If we had 127 + 1 = 128. Notice that we're not exceeding 255, so our C = 0 BUT our V is 1 because it's out of bounds! */

    // In order we'd want to first compute the add value.
    
    uint8_t carry = cpu->P & (1 << 0); // Sets our carry to 0 if bit 0 is 0 otherwise 1
    uint16_t result = cpu->A + value + carry; // We use 16 bit integer here so we can evaluate our bounds and set accordingly

    if (result >= 256){ // Is our result greater than or equal to 256?
        cpu->P |= (1 << 0); // Set the carry flag to 1
    }else{
        cpu->P &= ~(1 << 0); // We create a flipped version of 0000001 to 1111110 and use bitwise AND with our cpu->P to set carry flag to 0
    }
    
    result = result % 256; // Sets the result to the modulus
    
    int signA = (cpu->A & (1 << 7)) != 0; // The signed value of A is true if there's 1 at bit 7 else false
    int signValue = (value & (1 << 7)) != 0; // The signed value of value is true if there's 1 at bit 7 else false
    int signResult = (result & (1 << 7)) != 0; // The signed result is true if there's 1 at bit 7 else false

    if (signA == signValue && signA != signResult) { // Checks if sign A equals sign value and sign A doesnt equal sign result
        cpu->P |= (1 << 6);   // Set V to 1
    } else {
        cpu->P &= ~(1 << 6);  // Clear V to 0
    }

    cpu->A = (uint8_t) result;
    updateZN(cpu, cpu->A);
}

void SBC(cpuRegisters* cpu, uint8_t value){
    // Really lazy implementation but we're basically just flipping everything in value and calling ADC upon the cpu and flipped value
    ADC(cpu, ~value);
}

void INC(cpuRegisters* cpu, Memory* memory, uint16_t address){
    uint8_t value = readMemory(memory, address) + 1;
    writeMemory(memory, address, value);
    updateZN(cpu, value);
} 
void DEC(cpuRegisters* cpu, Memory* memory, uint16_t address){
    uint8_t value = readMemory(memory, address) - 1;
    writeMemory(memory, address, value);
    updateZN(cpu, value);
} 

void INX(cpuRegisters* cpu){
    cpu->X++;
    updateZN(cpu, cpu->X);
}
void DEX(cpuRegisters* cpu){
    cpu->X--;
    updateZN(cpu, cpu->X);
}
void INY(cpuRegisters* cpu){
    cpu->Y++;
    updateZN(cpu, cpu->Y);
}
void DEY(cpuRegisters* cpu){
    cpu->Y--;
    updateZN(cpu, cpu->Y);
}

void cpuStep(cpuRegisters* cpu, Memory* memory){
    uint8_t opcode = readMemory(memory, cpu->PC++); // Recall that PC stores the address of the following instruction 
    switch (opcode){
        case 0xA9:{ // IMMEDIATE ADDRESSING (means we fetch the value and directly load to A)
            uint8_t value = immediateAddress(cpu, memory);
            LDA(cpu, value);
            break;
        }
        case 0xA5:{ // ZERO ADDRESSING (means we fetch the address, find the value at the address and load to A)
            uint8_t address = zeroPageAddress(cpu, memory); // this should ideally read the instruction and give us the operand which is an address
            LDA(cpu, readMemory(memory, address));
            break;
        }
        case 0xAD:{ // ABSOLUTE ADDRESSING (means we fetch the opcode, then low byte and high byte - combine them and get the actual address for which the value is to be loaded)
            uint16_t address = absoluteAddress(cpu, memory);
            LDA(cpu, readMemory(memory, address));
            break; 
        }
        case 0xB5: { // ZERO ADDRESSING (X)
            uint8_t address = zeroPageAddress(cpu, memory); // this should ideally read the instruction and give us the operand which is an address
            LDA(cpu, readMemory(memory, (uint8_t)(address + cpu->X))); // we add X to the memory address and load that to A
            break;
        }
        case 0xBD: { // ABSOLUTE ADDRESSING (X)
            uint16_t address = absoluteAddress(cpu, memory);
            LDA(cpu, readMemory(memory, (uint16_t)(address + cpu->X)));
            break; 
        }
        case 0xB9: { // ABSOLUTE ADDRESSING (Y)
            uint16_t address = absoluteAddress(cpu, memory);
            LDA(cpu, readMemory(memory, (uint16_t)(address + cpu->Y)));
            break; 
        }
        case 0xA1: { // INDIRECT ADDRESSING (X) - we read the memory and that gives us yet another address which contains the value we need for A + X
            uint16_t address = indirectAddressX(cpu, memory); 
            LDA(cpu, readMemory(memory, address));
            break;
        }
        case 0xB1: { // INDIRECT ADDRESSING (Y) - same as X except we add Y after doing the pointer combo
            uint16_t address = indirectAddressY(cpu, memory);
            LDA(cpu, readMemory(memory, address));
            break;
        }
        case 0xA2: { // LDX - IMMEDIATE
            uint8_t value = immediateAddress(cpu, memory);
            LDX(cpu, value);
            break;
        }
        case 0xA6: { // LDX ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            LDX(cpu, readMemory(memory, address));
            break;
        }
        case 0xAE: { // LDX ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            LDX(cpu, readMemory(memory, address));
            break;
        }
        case 0xB6: { // LDX ZERO PAGE Y
            uint8_t address = zeroPageAddress(cpu, memory);
            LDX(cpu, readMemory(memory, (uint8_t) (address + cpu->Y)));
            break;
        }
        case 0xBE: { // LDX ABSOLUTE Y
            uint16_t address = absoluteAddress(cpu, memory);
            LDX(cpu, readMemory(memory, (uint16_t)(address + cpu->Y)));
            break;
        }
        case 0xA0: { // LDY IMMEDIATE 
            uint8_t value = immediateAddress(cpu, memory);
            LDY(cpu, value);
            break;
        }
        case 0xA4: { // LDY ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            LDY(cpu, readMemory(memory, address));
            break;
        }
        case 0xAC: { // LDY ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            LDY(cpu, readMemory(memory, address));
            break;
        }
        case 0xB4: { // LDY ZERO PAGE X
            uint8_t address = zeroPageAddress(cpu, memory);
            LDY(cpu, readMemory(memory, (uint8_t) (address + cpu->X)));
            break;
        }
        case 0xBC: { // LDY ABSOLUTE X
            uint16_t address = absoluteAddress(cpu, memory);
            LDY(cpu, readMemory(memory, (uint16_t)(address + cpu->X)));
            break;
        }
        case 0X85: { // STA ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            STA(cpu, address, memory);
            break;
        }
        case 0x95: { // STA ZERO PAGE X
            uint8_t address = zeroPageAddress(cpu, memory);
            STA(cpu, (uint8_t) (address + cpu->X), memory);
            break;
        }
        case 0x8D: { // STA ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            STA(cpu, address, memory); // naming convention for my method some TRASH but im too far to change it now
            break;
        }
        case 0x9D: { // STA ABSOLUTE X
            uint16_t address = absoluteAddress(cpu, memory);
            STA(cpu, (uint16_t) (address + cpu->X), memory);
            break;
        }
        case 0x99: { // STA ABSOLUTE Y
            uint16_t address = absoluteAddress(cpu, memory);
            STA(cpu, (uint16_t) (address + cpu->Y), memory);
            break;
        }
        case 0x81: { // STA INDIRECT X
            uint16_t address = indirectAddressX(cpu, memory);
            STA(cpu, address, memory);
            break;
        }
        case 0x91: { // STA INDIRECT Y
            uint16_t address = indirectAddressY(cpu, memory);
            STA(cpu, address, memory);
            break;
        }
        case 0x86: { // STX ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            STX(cpu, address, memory);
            break;
        }
        case 0x96: { // STX ZERO PAGE (Y)
            uint8_t address = zeroPageAddress(cpu, memory);
            STX(cpu, (uint8_t)(address + cpu->Y), memory);
            break;    
        }
        case 0x8E: { // STX ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            STX(cpu, address, memory);
            break;    
        }
        case 0x84: { // STY ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            STY(cpu, address, memory);
            break;    
        }
        case 0x94: { // STY ZERO PAGE (X)
            uint8_t address = zeroPageAddress(cpu, memory);
            STY(cpu, (uint8_t)(address + cpu->X), memory);
            break;    
        }
        case 0x8C: { // STY ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            STY(cpu, address, memory);
            break;    
        }
        case 0xAA: { // TAX 
            TAX(cpu);
            break;    
        }
        case 0xA8: { // TAY
            TAY(cpu);
            break;
        }
        case 0x8A: { // TXA
            TXA(cpu);
            break;    
        }
        case 0x98: { // TYA
            TYA(cpu);
            break;    
        }
        case 0xE8: { // INX
            INX(cpu);
            break;    
        }
        case 0xC8: { // INY
            INY(cpu);
            break;
        }
        case 0xCA: { // DEX
            DEX(cpu);
            break;    
        }
        case 0x88: { // DEY
            DEY(cpu);
            break;
        }
        case 0x69: { // ADC IMMEDIATE
            uint8_t value = immediateAddress(cpu, memory);
            ADC(cpu, value);
            break;    
        }
        case 0x65: { // ADC ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            ADC(cpu, readMemory(memory, address));
            break;
        }
        case 0x75: { // ADC ZERO PAGE (X)
            uint8_t address = zeroPageAddress(cpu, memory);
            ADC(cpu, readMemory(memory, (uint8_t)(address + cpu->X)));
            break;
        }
        case 0x6D: { // ADC ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            ADC(cpu, readMemory(memory, address));
            break;    
        }
        case 0x7D: { // ADC ABSOLUTE (X)
            uint16_t address = absoluteAddress(cpu, memory);
            ADC(cpu, readMemory(memory, (uint16_t) (address + cpu->X)));
            break;
        }
        case 0x79: { // ADC ABSOLUTE (Y)
            uint16_t address = absoluteAddress(cpu, memory);
            ADC(cpu, readMemory(memory, (uint16_t) (address + cpu->Y)));
            break;    
        }
        case 0x61: { // ADC INDIRECT X
            uint16_t address = indirectAddressX(cpu, memory);
            ADC(cpu, readMemory(memory, address));
            break;    
        }
        case 0x71: { // ADC INDIRECT Y
            uint16_t address = indirectAddressY(cpu, memory);
            ADC(cpu, readMemory(memory, address));
            break;
        }
        case 0xE9: { // SBC IMMEDIATE
            uint8_t value = immediateAddress(cpu, memory);
            SBC(cpu, value);
            break;
        }
        case 0xE5: { // SBC ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            SBC(cpu, readMemory(memory, address));
            break;
        }
        case 0xF5: { // SBC ZERO PAGE (X)
            uint8_t address = zeroPageAddress(cpu, memory);
            SBC(cpu, readMemory(memory, (uint8_t)(address + cpu->X)));
            break;
        }
        case 0xED: { // SBC ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            SBC(cpu, readMemory(memory, address));
            break;
        }
        case 0xFD: { // SBC ABSOLUTE (X)
            uint16_t address = absoluteAddress(cpu, memory);
            SBC(cpu, readMemory(memory, (uint16_t) (address + cpu->X)));
            break;
        }
        case 0xF9: { // SBC ABSOLUTE (Y)
            uint16_t address = absoluteAddress(cpu, memory);
            SBC(cpu, readMemory(memory, (uint16_t) (address + cpu->Y)));
            break;
        }
        case 0xE1: { // SBC INDIRECT (X)
            uint16_t address = indirectAddressX(cpu, memory);
            SBC(cpu, readMemory(memory, address));
            break;
        }
        case 0xF1: { // SBC INDIRECT (Y)
            uint16_t address = indirectAddressY(cpu, memory);
            SBC(cpu, readMemory(memory, address));
            break;
        }
        case 0xE6: { // INC ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            INC(cpu, memory, address);
            break;
        }
        case 0xF6: { // INC ZERO PAGE (X)
            uint8_t address = zeroPageAddress(cpu, memory);
            INC(cpu, memory, (uint8_t)(address + cpu->X));
            break;
        }
        case 0xEE: { // INC ABSOLUTE 
            uint16_t address = absoluteAddress(cpu, memory);
            INC(cpu, memory, address);
            break;
        }
        case 0xFE: { // INC ABSOLUTE (X)
            uint16_t address = absoluteAddress(cpu, memory);
            INC(cpu, memory, (uint16_t)(address + cpu->X));
            break;
        }
        case 0xC6: { // DEC ZERO PAGE
            uint8_t address = zeroPageAddress(cpu, memory);
            DEC(cpu, memory, address);
            break;
        }
        case 0xD6: { // DEC ZERO PAGE (X)
            uint8_t address = zeroPageAddress(cpu, memory);
            DEC(cpu, memory, (uint8_t)(address + cpu->X));
            break;
        }
        case 0xCE: { // DEC ABSOLUTE
            uint16_t address = absoluteAddress(cpu, memory);
            DEC(cpu, memory, address);
            break;
        }
        case 0xDE: { // DEC ABSOLUTE (X)
            uint16_t address = absoluteAddress(cpu, memory);
            DEC(cpu, memory, (uint16_t)(address + cpu->X));
            break;
        }
        default:
            printf("No existing opcode here");
            break;
    }
}