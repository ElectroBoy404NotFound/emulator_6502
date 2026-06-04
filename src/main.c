#include <stdio.h>

#include "6502.h"

uint8_t mem[64 * 1024];

uint8_t CPU6502_read_memory(uint16_t address) {
    // printf("Read from memory: Address: %04X\r\n", address);
    
    return mem[address];

    // if(address >= 0x7FFF && address <= 0xFFFF) {
    //     // printf("Reading from ROM at address: %04X  %02X\r\n", address - 0x8000, rom[address - 0x8000]);
    //     return rom[address - 0x8000];
    // }

    // return 0x00;
}

void CPU6502_write_memory(uint16_t address, uint8_t value) {
    // printf("Write to memory: Address: %04X, Value: %02X\r\n", address, value);
    mem[address] = value;
}

int main() {
    printf("Hello, World!\r\n");

    // Reset vector
    // mem[0xFFFC] = 0x00;
    // mem[0xFFFD] = 0x80;

    FILE* f = fopen("6502_functional_test.bin", "rb");
    fread(mem, 1, 64 * 1024, f);
    fclose(f);

    mem[0xFFFD] = 0x04;
    mem[0xFFFC] = 0x00;

    printf("%02X %02X\n", mem[0xFFFC], mem[0xFFFD]);

    CPU6502_Registers registers;
    CPU6502_reset(&registers);

    uint16_t last_pc = registers.pc;
    int i = 0;
    for(i = 0; ; i++) {
        // printf("EX PC: %04X, LST PC: %04X, OPCODE: %02X\r\n", registers.pc, last_pc, CPU6502_read_memory(registers.pc));
        uint16_t exit = CPU6502_execute(&registers);
        if(exit) {
            printf("Invalid OPCODE encountered at %04X during cycle %d. Exiting.\r\n", exit - 1, i);
            break;
        }

        // printf("PC: %04X, A: %02X, X: %02X, Y: %02X, SP: %02X, Status: %02X\r\n", registers.pc, registers.a, registers.x, registers.y, registers.sp, registers.status);
        if(last_pc == registers.pc) {
            printf("Looping at %04X during cycle %d. Exiting.\r\n", registers.pc, i);
            break;
        }

        // if(i % 100000) 
        //     printf("PC: %04X, A: %02X, X: %02X, Y: %02X, SP: %02X, Status: %02X\r\n", registers.pc, registers.a, registers.x, registers.y, registers.sp, registers.status);
        last_pc = registers.pc;
    }
    printf("Exited on cycle %d\r\n", i);

    return 0;
}