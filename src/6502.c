#include "6502.h"

void CPU6502_reset(CPU6502_Registers* registers) {
    registers->a = 0;
    registers->x = 0;
    registers->y = 0;
    registers->sp = 0xFF;
    registers->pc = 0x0000;
    registers->status = 0b00100000;

    registers->pc = (CPU6502_read_memory(0xFFFD) << 8) | CPU6502_read_memory(0xFFFC);
}

uint8_t CPU6502_execute(CPU6502_Registers* registers) {
    uint8_t opcode = CPU6502_read_memory(registers->pc);

    switch (opcode) {
        case 0xA9: {
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            registers->a = value;

            if(value == 0)
                registers->status |= 0b00000010;
            else
                registers->status &= ~0b00000010;
            
            if(value & 0b10000000)
                registers->status |= 0b10000000;
            else
                registers->status &= ~0b10000000;

            break;
        }
        case 0xEA: {
            printf("NOP executed\r\n");
            break;
        }
        default:
            printf("Unknown opcode: %02X at address: %04X\r\n", opcode, registers->pc);
            printf("PC: %04X, A: %02X, X: %02X, Y: %02X, SP: %02X, Status: %02X\r\n", registers->pc, registers->a, registers->x, registers->y, registers->sp, registers->status);
            return registers->pc + 1;
            break;
    }
    registers->pc++;
    return 0;
}
