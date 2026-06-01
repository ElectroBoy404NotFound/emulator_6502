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

static uint16_t read_word(CPU6502_Registers* r) {
    uint16_t low = CPU6502_read_memory(r->pc);

    r->pc++;
    uint16_t high = CPU6502_read_memory(r->pc);

    return low | (high << 8);
}

static uint16_t read_word_at_zp(uint8_t at) {
    uint16_t low = CPU6502_read_memory(at);
    uint16_t high = CPU6502_read_memory(at + 1);

    return low | (high << 8);
}

static inline void set_accumulator(CPU6502_Registers* registers, uint8_t val) {
    registers->a = val;

    if(registers->a == 0)
        registers->status |= FLAG_Z;
    else
        registers->status &= ~FLAG_Z;
    
    if(registers->a & 0b10000000)
        registers->status |= FLAG_N;
    else
        registers->status &= ~FLAG_N;
}

static inline void set_x(CPU6502_Registers* registers, uint8_t val) {
    registers->x = val;

    if(registers->x == 0)
        registers->status |= FLAG_Z;
    else
        registers->status &= ~FLAG_Z;
    
    if(registers->x & 0b10000000)
        registers->status |= FLAG_N;
    else
        registers->status &= ~FLAG_N;
}

static inline void set_y(CPU6502_Registers* registers, uint8_t val) {
    registers->y = val;

    if(registers->y == 0)
        registers->status |= FLAG_Z;
    else
        registers->status &= ~FLAG_Z;
    
    if(registers->y & 0b10000000)
        registers->status |= FLAG_N;
    else
        registers->status &= ~FLAG_N;
}

uint8_t CPU6502_execute(CPU6502_Registers* registers) {
    uint8_t opcode = CPU6502_read_memory(registers->pc);

    switch (opcode) {
        case 0xA9: { // LDA Immediate
            registers->pc++;
            set_accumulator(registers, CPU6502_read_memory(registers->pc));

            break;
        }
        case 0xAD: { // LDA abs
            registers->pc++;
            set_accumulator(registers, CPU6502_read_memory(read_word(registers)));

            break;
        }
        case 0xA5: { // LDA zpg
            registers->pc++;
            set_accumulator(registers, CPU6502_read_memory(CPU6502_read_memory(registers->pc)));

            break;
        }
        case 0xBD: { // LDA abs, X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            set_accumulator(registers, CPU6502_read_memory(address));

            break;
        }
        case 0xB5: { // LDA zpg, X
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            address += registers->x;
            set_accumulator(registers, CPU6502_read_memory(address));

            break;
        }
        case 0xB9: { // LDA abs, Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            set_accumulator(registers, CPU6502_read_memory(address));

            break;
        }
        case 0xA1: { // LDA X ind
            registers->pc++;
            uint8_t zp = CPU6502_read_memory(registers->pc);
            zp += registers->x;

            uint16_t address = read_word_at_zp(zp);
            set_accumulator(registers, CPU6502_read_memory(address));

            break;
        }
        case 0xB1: { // LDA ind Y
            registers->pc++;
            uint8_t zp = CPU6502_read_memory(registers->pc);
            uint16_t address = read_word_at_zp(zp);
            address += registers->y;
            set_accumulator(registers, CPU6502_read_memory(address));

            break;
        }

        case 0xA2: { // LDX Imm
            registers->pc++;
            set_x(registers, CPU6502_read_memory(registers->pc));

            break;
        }
        case 0xA6: { // LDX zpg
            registers->pc++;
            set_x(registers, CPU6502_read_memory(CPU6502_read_memory(registers->pc)));
            
            break;
        }
        case 0xB6: { // LDX zpg Y
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            address += registers->y;
            set_x(registers, CPU6502_read_memory(address));
            
            break;
        }
        case 0xAE: { // LDX abs
            registers->pc++;
            uint16_t address = read_word(registers);
            set_x(registers, CPU6502_read_memory(address));

            break;
        }
        case 0xBE: { // LDX abs Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            set_x(registers, CPU6502_read_memory(address));

            break;
        }

        case 0xA0: { // LDY Imm
            registers->pc++;
            set_y(registers, CPU6502_read_memory(registers->pc));

            break;
        }
        case 0xA4: { // LDY zpg
            registers->pc++;
            set_y(registers, CPU6502_read_memory(CPU6502_read_memory(registers->pc)));
            
            break;
        }
        case 0xB4: { // LDY zpg X
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            address += registers->x;
            set_y(registers, CPU6502_read_memory(address));
            
            break;
        }
        case 0xAC: { // LDY abs
            registers->pc++;
            uint16_t address = read_word(registers);
            set_y(registers, CPU6502_read_memory(address));

            break;
        }
        case 0xBC: { // LDY abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            set_y(registers, CPU6502_read_memory(address));

            break;
        }

        case 0xEA: { // NOP
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
