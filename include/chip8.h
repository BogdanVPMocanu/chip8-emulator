#ifndef CHIP8_H
#define CHIP8_H

#include "chip8_stack.h"

#include <stdint.h>

#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

typedef struct {
	uint8_t memory[CHIP8_MEMORY_SIZE]; /* 4 kilobytes of RAM */
	uint32_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT]; /* 64x32 resolution (1 RGBA pixel = 4 bytes) */
	uint16_t pc; /* program counter */
	uint16_t i; /* index register */
	chip8_stack stack; /* stack for subroutines/functions */
} chip8;

#endif /* CHIP8_H */