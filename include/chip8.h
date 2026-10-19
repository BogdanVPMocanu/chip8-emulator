#ifndef CHIP8_H
#define CHIP8_H

#include "chip8_stack.h"

#include <stdint.h>

#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_NUM_REGISTERS 16

typedef struct {
	uint8_t memory[CHIP8_MEMORY_SIZE]; /* 4 kilobytes of RAM */
	uint32_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT]; /* 64x32 resolution (1 RGBA pixel = 4 bytes) */
	uint16_t pc; /* program counter */
	uint16_t i; /* index register */
	chip8_stack stack; /* stack for subroutines/functions */
	uint8_t delay_timer; /* decremented at a rate of 60 Hz until it reaches 0 */
	uint8_t sound_timer; /* decremented at a rate of 60 Hz until it reaches 0, gives off a beeping sound as long as it is not 0 */
	uint8_t v[CHIP8_NUM_REGISTERS]; /* general-purpose registers V0-VF; VF is used as a flag and should not hold regular data */
} chip8;

void chip8_initialize(chip8 *emulator);

#endif /* CHIP8_H */