#include "romloader.h"

bool readROM(const char* filepath, Memory* memory){
    FILE *fptr = fopen(filepath, "rb");

    if (fptr == NULL){
        printf("Can't open file. Path probably wrong.");
        return false;
    }

    uint8_t header[16]; // The NES ROM has a header that tells us some important information - very vague comment i know
    fread(header, 1, 16, fptr); // Reads the header file - we read 16 bytes total here.

    uint8_t programSize = header[4]; // Either our byte tells us our program data is 16 KB or 32KB.

    if (programSize == 1 || programSize == 2){
        fread(&memory->memory[0x8000], 1, 16384, fptr); // 1024 * 16 = 16 KB. we also start at 0x8000 as per memory map
        if (programSize == 1){
            memcpy(&memory->memory[0xC000], &memory->memory[0x8000], 16384); // copy what we had in the first 16kb to the next 16kb
        }
        else{
            fread(&memory->memory[0xC000], 1, 16384, fptr);
        }
    }
    else{
        printf("Unsupported type - or it is supported and I have not implemented it yet ... sorry!");
        return false;
    }


    fclose(fptr);
    return true;
}