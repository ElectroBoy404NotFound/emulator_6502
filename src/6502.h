#pragma once

#include <stdint.h>

#include <stdio.h>

typedef struct {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint16_t pc;
    uint8_t status;
} CPU6502_Registers;

extern void CPU6502_write_memory(uint16_t address, uint8_t value);
extern uint8_t CPU6502_read_memory(uint16_t address);

void CPU6502_reset(CPU6502_Registers* registers);
uint8_t CPU6502_execute(CPU6502_Registers* registers);
