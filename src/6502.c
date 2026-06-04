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

static uint16_t read_word_at(uint16_t address) {
    uint16_t low = CPU6502_read_memory(address);
    uint16_t high = CPU6502_read_memory(address + 1);

    return (high << 8) | low;
}

static uint16_t read_word_at_zp(uint8_t at) {
    uint16_t low = CPU6502_read_memory(at);
    uint16_t high = CPU6502_read_memory((uint8_t) (at + 1));

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

static inline void compare_reg(CPU6502_Registers* registers, uint8_t* reg, uint8_t cmp_val) {
    uint8_t result = *reg - cmp_val;

    if(*reg >= cmp_val)
        registers->status |= FLAG_C;
    else
        registers->status &= ~FLAG_C;

    if(result == 0)
        registers->status |= FLAG_Z;
    else
        registers->status &= ~FLAG_Z;

    if(result & 0x80)
        registers->status |= FLAG_N;
    else
        registers->status &= ~FLAG_N;

    printf("CMP EXPECT %02X HAVE %02X\r\n", cmp_val, *reg);
}

uint16_t CPU6502_execute(CPU6502_Registers* registers) {
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

        case 0x8D: { // STA abs
            registers->pc++;
            uint16_t address = read_word(registers);
            CPU6502_write_memory(address, registers->a);

            break;
        }
        case 0x9D: { // STA abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            CPU6502_write_memory(address, registers->a);

            break;
        }
        case 0x99: { // STA abs Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            CPU6502_write_memory(address, registers->a);

            break;
        }
        case 0x81: { // STA X ind
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;

            uint16_t address = read_word_at_zp(zp_address);
            CPU6502_write_memory(address, registers->a);

            break;
        }
        case 0x91: { // STA ind Y
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint16_t address = read_word_at_zp(zp_address);
            address += registers->y;
            CPU6502_write_memory(address, registers->a);

            break;
        }
        case 0x85: { // STA zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            CPU6502_write_memory(zp_address, registers->a);

            break;
        }
        case 0x95: { // STA zpg X
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            CPU6502_write_memory(zp_address, registers->a);

            break;
        }

        case 0x8E: { // STX abs
            registers->pc++;
            uint16_t address = read_word(registers);
            CPU6502_write_memory(address, registers->x);

            break;
        }
        case 0x86: { // STX zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            CPU6502_write_memory(zp_address, registers->x);

            break;
        }
        case 0x96: { // STX zpg Y
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->y;
            CPU6502_write_memory(zp_address, registers->x);

            break;
        }
        
        case 0x8C: { // STY abs
            registers->pc++;
            uint16_t address = read_word(registers);
            CPU6502_write_memory(address, registers->y);

            break;
        }
        case 0x84: { // STY zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            CPU6502_write_memory(zp_address, registers->y);

            break;
        }
        case 0x94: { // STY zpg X
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            CPU6502_write_memory(zp_address, registers->y);
            
            break;
        }

        case 0xAA: { // TAX
            set_x(registers, registers->a);
            break;
        }
        case 0xBA: { // TSX
            set_x(registers, registers->sp);
            break;
        }
        case 0x8A: { // TXA
            set_accumulator(registers, registers->x);
            break;
        }
        case 0x9A: { // TXS
            registers->sp = registers->x;
            break;
        }
        case 0xA8: { // TAY
            set_y(registers, registers->a);
            break;
        }
        case 0x98: { // TYA
            set_accumulator(registers, registers->y);
            break;
        }

        case 0xE8: { // INX
            set_x(registers, registers->x + 1);
            break;
        }
        case 0xC8: { // INY
            set_y(registers, registers->y + 1);
            break;
        }
        case 0xFE: { // INC abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            CPU6502_write_memory(address, CPU6502_read_memory(address) + 1);

            break;
        }
        case 0xCA: { // DEX
            set_x(registers, registers->x - 1);
            break;
        }
        case 0x88: { // DEY
            set_y(registers, registers->y - 1);
            break;
        }
        case 0xDE: { // DEC abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            CPU6502_write_memory(address, CPU6502_read_memory(address) - 1);

            break;
        }

        case 0x4C: { // JMP abs
            registers->pc++;
            uint16_t jump_address = read_word(registers);
            registers->pc = jump_address;
            registers->pc--;

            break;
        }
        case 0x6C: { // JMP ind
            registers->pc++;
            uint16_t address = read_word(registers);
            registers->pc = read_word_at(address);
            registers->pc--;

            break;
        }

        case 0x29: { // AND #
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            set_accumulator(registers, registers->a & value);

            break;
        }
        case 0x39: { // AND abs Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            set_accumulator(registers, registers->a & CPU6502_read_memory(address));

            break;
        }
        case 0x2D: { // AND abs
            registers->pc++;
            uint16_t address = read_word(registers);
            set_accumulator(registers, registers->a & CPU6502_read_memory(address));

            break;
        }
        case 0x3D: { // AND abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            set_accumulator(registers, registers->a & CPU6502_read_memory(address));

            break;
        }
        case 0x25: { // AND zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            set_accumulator(registers, registers->a & CPU6502_read_memory(zp_address));

            break;
        }
        case 0x35: { // AND zpg X
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            set_accumulator(registers, registers->a & CPU6502_read_memory(zp_address));

            break;
        }
        case 0x21: { // AND X ind
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;

            uint16_t address = read_word_at_zp(zp_address);
            set_accumulator(registers, registers->a & CPU6502_read_memory(address));

            break;
        }
        case 0x31: { // AND ind Y
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint16_t address = read_word_at_zp(zp_address);
            address += registers->y;
            set_accumulator(registers, registers->a & CPU6502_read_memory(address));

            break;
        }

        case 0x09: { // ORA #
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            set_accumulator(registers, registers->a | value);

            break;
        }
        case 0x19: { // ORA abs Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }
        case 0x0D: { // ORA abs
            registers->pc++;
            uint16_t address = read_word(registers);
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }
        case 0x1D: { // ORA abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }
        case 0x05: { // ORA zpg
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }
        case 0x15: { // ORA zpg X
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            address += registers->x;
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }
        case 0x01: { // ORA X ind
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            uint16_t address = read_word_at_zp(zp_address);
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }
        case 0x11: { // ORA ind Y
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint16_t address = read_word_at_zp(zp_address);
            address += registers->y;
            set_accumulator(registers, registers->a | CPU6502_read_memory(address));

            break;
        }

        case 0x49: { // EOR #
            registers->pc++;
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(registers->pc));

            break;
        }
        case 0x4D: { // EOR abs
            registers->pc++;
            uint16_t address = read_word(registers);
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(address));

            break;
        }
        case 0x5D: { // EOR abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(address));

            break;
        }
        case 0x59: { // EOR abs Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(address));

            break;
        }
        case 0x45: { // EOR zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(zp_address));

            break;
        }
        case 0x55: { // EOR zpg X
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(zp_address));

            break;
        }
        case 0x41: { // EOR X ind
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            uint16_t address = read_word_at_zp(zp_address);
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(address));

            break;
        }
        case 0x51: { // EOR ind Y
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint16_t address = read_word_at_zp(zp_address);
            address += registers->y;
            set_accumulator(registers, registers->a ^ CPU6502_read_memory(address));

            break;
        }

        case 0xC9: { // CMP #
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xD9: { // CMP abs Y
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->y;
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xDD: { // CMP abs X
            registers->pc++;
            uint16_t address = read_word(registers);
            address += registers->x;
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xCD: { // CMP abs
            registers->pc++;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xC5: { // CMP zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(zp_address);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xD5: { // CMP zpg X
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            uint8_t value = CPU6502_read_memory(zp_address);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xC1: { // CMP X ind
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            zp_address += registers->x;
            uint16_t address = read_word_at_zp(zp_address);
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->a, value);

            break;
        }
        case 0xD1: { // CMP ind Y
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint16_t address = read_word_at_zp(zp_address);
            address += registers->y;
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->a, value);

            break;
        }

        case 0xC0: { // CPY #
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            compare_reg(registers, &registers->y, value);

            break;
        }
        case 0xC4: { // CPY zpg
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->y, value);

            break;
        }
        case 0xCC: { // CPY abs
            registers->pc++;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->y, value);

            break;
        }

        case 0xE0: { // CPX #
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            compare_reg(registers, &registers->x, value);

            break;
        }
        case 0xE4: { // CPX zpg
            registers->pc++;
            uint8_t address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->x, value);

            break;
        }
        case 0xEC: { // CPX abs
            registers->pc++;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            compare_reg(registers, &registers->x, value);

            break;
        }
        
        case 0xD0: { // BNE rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if(!(registers->status & FLAG_Z)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0xF0: { // BEQ rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if(registers->status & FLAG_Z) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0x10: { // BPL rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if(!(registers->status & FLAG_N)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0x90: { // BCC rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if(!(registers->status & FLAG_C)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0x30: { // BMI rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if((registers->status & FLAG_N)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0xB0: { // BCS rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if((registers->status & FLAG_C)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0x50: { // BVC rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if(!(registers->status & FLAG_V)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }
        case 0x70: { // BVS rel
            uint16_t origin = registers->pc;
            registers->pc++;
            int8_t rel_address = (int8_t) CPU6502_read_memory(registers->pc);
            if((registers->status & FLAG_V)) {
                registers->pc = origin + rel_address;
                registers->pc++;
            }

            break;
        }

        case 0x48: { // PHA impl
            uint16_t stack_address = 0x0100 + registers->sp;
            CPU6502_write_memory(stack_address, registers->a);
            registers->sp--;

            break;
        }
        case 0x68: { // PLA impl
            registers->sp++;
            uint16_t stack_address = 0x0100 + registers->sp;

            set_accumulator(registers, CPU6502_read_memory(stack_address));

            break;
        }
        case 0x08: { // PHP impl
            uint16_t stack_address = 0x0100 + registers->sp;

            CPU6502_write_memory(stack_address, (registers->status) | (FLAG_B | FLAG_U));

            registers->sp--;

            break;
        }
        case 0x28: { // PLP impl
            registers->sp++;
            uint16_t stack_address = 0x0100 + registers->sp;

            registers->status = CPU6502_read_memory(stack_address) & ~(FLAG_B | FLAG_U);

            break;
        }
        
        case 0x20: { // JSR abs
            uint16_t stack_address = 0x0100 + registers->sp;
            uint16_t return_addr = registers->pc + 2;
            CPU6502_write_memory(stack_address, (uint8_t) (return_addr >> 8) & 0xFF);
            registers->sp--;
            CPU6502_write_memory(stack_address - 1, (uint8_t) return_addr & 0xff);
            registers->sp--;

            registers->pc++;
            uint16_t jmp_address = read_word(registers);
            registers->pc = jmp_address;
            registers->pc--;

            printf("JSR %02x\r\n", jmp_address);

            break;
        }
        case 0x60: { // RTS impl
            registers->sp++;
            uint16_t stack_address = 0x0100 + registers->sp;
            
            uint8_t low_addr = CPU6502_read_memory(stack_address);
            registers->sp++;
            uint8_t high_addr = CPU6502_read_memory(stack_address + 1);

            uint16_t jmp_address = (high_addr << 8) | low_addr;
            registers->pc = jmp_address;
            printf("RTS %02x\r\n", jmp_address);

            break;
        }

        case 0xD8: { // CLD impl
            registers->status &= ~FLAG_D;
            
            break;
        }
        case 0x18: { // CLC impl
            registers->status &= ~FLAG_C;
            
            break;
        }
        case 0x38: { // SEC impl
            registers->status |= FLAG_C;
            
            break;
        }
        case 0x58: { // CLI impl
            registers->status &= ~FLAG_I;
            
            break;
        }
        case 0x78: { // SEI impl
            registers->status |= FLAG_I;
            
            break;
        }
        case 0xF8: { // SED impl
            registers->status |= FLAG_D;
            
            break;
        }
        case 0xB8: { // CLV impl
            registers->status &= ~FLAG_V;
            
            break;
        }

        case 0x69: { // ADC #
            registers->pc++;
            uint8_t value = CPU6502_read_memory(registers->pc);
            uint16_t sum = registers->a + value + ((registers->status & FLAG_C) ? 1 : 0);
            if(sum > 0xFF)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            uint8_t result = sum & 0xFF;

            if(((registers->a ^ result) & (value ^ result) & 0x80))
                registers->status |= FLAG_V;
            else
                registers->status &= ~FLAG_V;

            set_accumulator(registers, result);

            break;
        }

        case 0x24: { // BIT zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(zp_address);
            uint8_t result = registers->a & value;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (value & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;
            
            if (value & 0x40)
                registers->status |= FLAG_V;
            else
                registers->status &= ~FLAG_V;
            
            break;
        }
        case 0x2C: { // BIT abs
            registers->pc++;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            uint8_t result = registers->a & value;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (value & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;
            
            if (value & 0x40)
                registers->status |= FLAG_V;
            else
                registers->status &= ~FLAG_V;
            
            break;
        }

        case 0xEA: { // NOP
            break;
        }

        case 0x00: { // BRK impl
            registers->pc++;
            registers->pc++;

            CPU6502_write_memory(0x0100 | registers->sp, (registers->pc >> 8) & 0xFF);
            printf("BRK Wrote %02X as PC HI at %04X\r\n", (registers->pc >> 8) & 0xFF, 0x0100 | registers->sp);
            registers->sp--;
            CPU6502_write_memory(0x0100 | registers->sp, registers->pc & 0xFF);
            printf("BRK Wrote %02X as PC LO at %04X\r\n", registers->pc & 0xFF, 0x0100 | registers->sp);
            registers->sp--;

            CPU6502_write_memory(0x0100 | registers->sp, (registers->status) | (FLAG_B | FLAG_U));
            registers->sp--;

            // registers->status = FLAG_I;
            registers->status |= FLAG_B | FLAG_U | FLAG_I;

            uint16_t lo = CPU6502_read_memory(0xFFFE);
            uint16_t hi = CPU6502_read_memory(0xFFFF);
            registers->pc = lo | (hi << 8);
            registers->pc--;
            break;
        }
        case 0x40: { // RTI impl
            registers->sp++;
            uint8_t status_reg = CPU6502_read_memory(0x0100 | registers->sp);
            registers->sp++;
            uint8_t low_addr = CPU6502_read_memory(0x0100 | registers->sp);
            registers->sp++;
            uint8_t high_addr = CPU6502_read_memory(0x0100 | registers->sp);

            registers->status = status_reg & ~(FLAG_B | FLAG_U);

            uint16_t jmp_address = (high_addr << 8) | low_addr;
            registers->pc = jmp_address;
            registers->pc--;

            break;
        }

        case 0x0A: { // ASL A
            uint8_t carry = (registers->a & 0x80) ? 1 : 0;
            uint8_t result = registers->a << 1;

            set_accumulator(registers, result);

            if (carry)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            break;
        }
        case 0x4A: { // LSR A
            uint8_t carry = (registers->a & 0x01) ? 1 : 0;
            uint8_t result = registers->a >> 1;

            set_accumulator(registers, result);

            if (carry)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            break;
        }
        case 0x2A: { // ROL A
            uint8_t org_carry = (registers->status & FLAG_C) ? 1 : 0;
            uint8_t carry = (registers->a & 0x80) ? 1 : 0;
            uint8_t result = registers->a << 1 | org_carry;

            set_accumulator(registers, result);

            if (carry)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            break;
        }
        case 0x6A: { // ROR A
            uint8_t org_carry = (registers->status & FLAG_C) ? 1 : 0;
            uint8_t carry = (registers->a & 0x01) ? 1 : 0;
            uint8_t result = (org_carry << 7) | registers->a >> 1;

            set_accumulator(registers, result);

            if (carry)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            break;
        }
        case 0x06: { // ASL zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(zp_address);
            uint8_t result = value << 1;
            if(value & 0x80)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (result & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;

            CPU6502_write_memory(zp_address, result);
            break;
        }
        case 0x46: { // LSR zpg
            registers->pc++;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(zp_address);
            uint8_t result = value >> 1;
            if(value & 0x01)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            registers->status &= ~FLAG_N;

            CPU6502_write_memory(zp_address, result);
            break;
        }
        case 0x26: { // ROL zpg
            registers->pc++;
            uint8_t org_carry = (registers->status & FLAG_C) ? 1 : 0;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(zp_address);
            uint8_t result = value << 1 | org_carry;
            if(value & 0x80)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (result & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;

            CPU6502_write_memory(zp_address, result);
            break;
        }
        case 0x66: { // ROR zpg
            registers->pc++;
            uint8_t org_carry = (registers->status & FLAG_C) ? 1 : 0;
            uint8_t zp_address = CPU6502_read_memory(registers->pc);
            uint8_t value = CPU6502_read_memory(zp_address);
            uint8_t result = (org_carry << 7) | value >> 1;
            if(value & 0x01)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (result & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;

            CPU6502_write_memory(zp_address, result);
            break;
        }
        case 0x0E: { // ASL abs
            registers->pc++;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            uint8_t result = value << 1;
            if(value & 0x80)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (result & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;

            CPU6502_write_memory(address, result);
            break;
        }
        case 0x4E: { // LSR abs
            registers->pc++;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            uint8_t result = value >> 1;
            if(value & 0x01)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            registers->status &= ~FLAG_N;

            CPU6502_write_memory(address, result);
            break;
        }
        case 0x2E: { // ROL abs
            registers->pc++;
            uint8_t org_carry = (registers->status & FLAG_C) ? 1 : 0;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            uint8_t result = value << 1 | org_carry;
            if(value & 0x80)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (result & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;

            CPU6502_write_memory(address, result);
            break;
        }
        case 0x6E: { // ROR abs
            registers->pc++;
            uint8_t org_carry = (registers->status & FLAG_C) ? 1 : 0;
            uint16_t address = read_word(registers);
            uint8_t value = CPU6502_read_memory(address);
            uint8_t result = (org_carry << 7) | value >> 1;
            if(value & 0x01)
                registers->status |= FLAG_C;
            else
                registers->status &= ~FLAG_C;

            if (result == 0)
                registers->status |= FLAG_Z;
            else
                registers->status &= ~FLAG_Z;

            if (result & 0x80)
                registers->status |= FLAG_N;
            else
                registers->status &= ~FLAG_N;

            CPU6502_write_memory(address, result);
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
