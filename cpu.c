#include "cpu.h"

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
    // Really lazy implementation but wew're basically just flipping everything in value and calling ADC upon the cpu and flipped value
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