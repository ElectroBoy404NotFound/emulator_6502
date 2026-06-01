#include <stdio.h>

#include "6502.h"

uint8_t rom[32 * 1024];

uint8_t CPU6502_read_memory(uint16_t address) {
    // printf("Read from memory: Address: %04X\r\n", address);
    
    if(address >= 0x7FFF && address <= 0xFFFF) {
        // printf("Reading from ROM at address: %04X  %02X\r\n", address - 0x8000, rom[address - 0x8000]);
        return rom[address - 0x8000];
    }

    return 0x00;
}

void CPU6502_write_memory(uint16_t address, uint8_t value) {
    printf("Write to memory: Address: %04X, Value: %02X\r\n", address, value);
}

int main() {
    printf("Hello, World!\r\n");

    // Initialize memory with a simple program
    rom[0x7FFC] = 0x00; // Low byte of reset
    rom[0x7FFD] = 0x80; // High byte of reset

    rom[0x0000] = 0xA9; // LDA Immediate
    rom[0x0001] = 0x42; // Value to load into A
    rom[0x0002] = 0xEA; // NOP

    CPU6502_Registers registers;
    CPU6502_reset(&registers);
    
    for(int i = 0; i < 10; i++) {
        uint16_t exit = CPU6502_execute(&registers);
        if(exit) {
            printf("Invalid OPCODE encountered at %04X. Exiting.\r\n", exit - 1);
            break;
        }
    }

    return 0;
}