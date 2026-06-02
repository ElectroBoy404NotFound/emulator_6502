#include <stdio.h>

#include "6502.h"

uint8_t mem[65536];

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
    printf("Write to memory: Address: %04X, Value: %02X\r\n", address, value);
}

int main() {
    printf("Hello, World!\r\n");

    // Reset vector
    mem[0xFFFC] = 0x00;
    mem[0xFFFD] = 0x80;

    uint16_t pc = 0x8000;

    /* LDA #$42 */
    mem[pc++] = 0xA9;
    mem[pc++] = 0x42;

    /* LDX #$05 */
    mem[pc++] = 0xA2;
    mem[pc++] = 0x05;

    /* LDY #$07 */
    mem[pc++] = 0xA0;
    mem[pc++] = 0x07;

    /* LDA $1234 */
    mem[pc++] = 0xAD;
    mem[pc++] = 0x34;
    mem[pc++] = 0x12;

    /* LDA $20 */
    mem[pc++] = 0xA5;
    mem[pc++] = 0x20;

    /* LDA $20,X */
    mem[pc++] = 0xB5;
    mem[pc++] = 0x20;

    /* LDA $1230,X */
    mem[pc++] = 0xBD;
    mem[pc++] = 0x30;
    mem[pc++] = 0x12;

    /* LDA $1230,Y */
    mem[pc++] = 0xB9;
    mem[pc++] = 0x30;
    mem[pc++] = 0x12;

    /* LDA ($10,X) */
    mem[pc++] = 0xA1;
    mem[pc++] = 0x10;

    /* LDA ($20),Y */
    mem[pc++] = 0xB1;
    mem[pc++] = 0x20;

    /* NOP */
    mem[pc++] = 0xEA;

    /* STA $1234 */
    mem[pc++] = 0x8D;
    mem[pc++] = 0x34;
    mem[pc++] = 0x12;

    /* JMP $F100 */
    mem[pc++] = 0x4C;
    mem[pc++] = 0x00;
    mem[pc++] = 0xF1;

    pc = 0xF100;
    /* LDA #$67 */
    mem[pc++] = 0xA9;
    mem[pc++] = 0x67;

    /* JMP ($F200) */
    mem[pc++] = 0x6C;
    mem[pc++] = 0x00;
    mem[pc++] = 0xF2;

    pc = 0xE000;
    /* LDA #$AA */
    mem[pc++] = 0xA9;
    mem[pc++] = 0xAA;

    /* 0xF200: 0x00E0 */
    mem[0xF200] = 0x00;
    mem[0xF201] = 0xE0;

    /* Absolute */
    mem[0x1234] = 0x11;

    /* Zero Page */
    mem[0x20] = 0x22;

    /* Zero Page,X */
    mem[0x25] = 0x33;

    /* Absolute,X */
    mem[0x1235] = 0x44;

    /* Absolute,Y */
    mem[0x1237] = 0x55;

    /* ($10,X) */
    mem[0x15] = 0x40;
    mem[0x16] = 0x12;
    mem[0x1240] = 0x66;

    /* ($20),Y */
    mem[0x20] = 0x50;
    mem[0x21] = 0x12;
    mem[0x1257] = 0x77;

    CPU6502_Registers registers;
    CPU6502_reset(&registers);

    for(int i = 0; i < 20; i++) {
        uint16_t exit = CPU6502_execute(&registers);
        if(exit) {
            printf("Invalid OPCODE encountered at %04X. Exiting.\r\n", exit - 1);
            break;
        }

        printf("PC: %04X, A: %02X, X: %02X, Y: %02X, SP: %02X, Status: %02X\r\n", registers.pc, registers.a, registers.x, registers.y, registers.sp, registers.status);
    }

    return 0;
}