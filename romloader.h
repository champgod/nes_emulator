/* The purpose of this file is to just give us a general sense of how we read .nes files. 
After all what I am creating is an NES emulator, so now that the CPU has been implemented 
it would be wise to test the instruction set using a test rom. I ripped an old nestest rom
from a GitHub repository. But yeah this class will go over what each method does and whatnot. */

#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "memory.h"

bool readROM(const char* filepath, Memory* memory);

#endif
