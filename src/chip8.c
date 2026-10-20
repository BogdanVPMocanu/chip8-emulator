#include "chip8.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static const uint8_t font[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

static void clear_screen(chip8 *emulator) {
	for (uint16_t i = 0; i < CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT; i++) {
		emulator->display[i] = CHIP8_PIXEL_OFF;
	}
}

void chip8_initialize(chip8 *emulator) {
	*emulator = (chip8){0};
	clear_screen(emulator);
	emulator->pc = CHIP8_PROGRAM_START;
	chip8_stack_initialize(&emulator->stack);
	memcpy(emulator->memory + CHIP8_FONT_ADDRESS, font, sizeof(font));
}

bool chip8_load_rom(chip8 *emulator, const char *rom_path) {
	FILE *rom = fopen(rom_path, "rb");
	if (rom == NULL) {
		fprintf(stderr, "FAILED TO OPEN %s: %s\n", rom_path, strerror(errno));
		return false;
	}

	if (fseek(rom, 0, SEEK_END) != 0) {
		fprintf(stderr, "FAILED TO SEEK FILE %s: %s\n", rom_path, strerror(errno));
		fclose(rom);
		return false;
	}

	long rom_size = ftell(rom);
	if (rom_size == -1) {
		fprintf(stderr, "FAILED TO GET FILE %s SIZE: %s\n", rom_path, strerror(errno));
		fclose(rom);
		return false;
	} else if (rom_size > CHIP8_PROGRAM_SIZE) {
		fprintf(stderr, "FILE %s IS TOO BIG: %ld bytes! Maximum file size is %d bytes\n", rom_path, rom_size, CHIP8_PROGRAM_SIZE);
		fclose(rom);
		return false;
	} else if (rom_size == 0) {
		fprintf(stderr, "FILE %s IS EMPTY!\n", rom_path);
		fclose(rom);
		return false;
	}

	rewind(rom);

	long bytes_read = (long) fread(emulator->memory + CHIP8_PROGRAM_START, 1, rom_size, rom);
	if (bytes_read != rom_size) {
		fprintf(stderr, "FAILED TO READ FROM FILE: %s\n", rom_path);
		fclose(rom);
		return false;
	}

	fclose(rom);

	return true;
}

static uint16_t fetch(chip8 *emulator) {
	uint16_t instruction = 0;
	uint16_t upper_byte = emulator->memory[emulator->pc];
	emulator->pc++;
	uint16_t lower_byte = emulator->memory[emulator->pc];
	emulator->pc++;

	instruction = instruction | (upper_byte << 8);
	instruction = instruction | lower_byte;

	return instruction;
}

bool chip8_cycle(chip8 *emulator) {
	if (emulator->pc > CHIP8_MEMORY_SIZE - 2) {
		fprintf(stderr, "PC OUT OF BOUNDS: 0x%X\n", emulator->pc);
		return false;
	}

	uint16_t curr_instruction = fetch(emulator);

	uint8_t first_nibble = curr_instruction >> 12; /* instruction type */
	uint8_t x = (curr_instruction >> 8) & 0xF; /* second nibble */
	uint8_t y = (curr_instruction >> 4) & 0xF; /* third nibble */
	uint8_t n = curr_instruction & 0xF; /* fourth nibble */
	uint8_t nn = curr_instruction & 0xFF; /* second byte */
	uint16_t nnn = curr_instruction & 0xFFF; /* second, third and fourth nibble */

	switch (first_nibble) {
		case 0x0:
			if (nnn == 0x0E0) { /* clear screen */
				clear_screen(emulator);
			} else {
				fprintf(stderr, "Unknown instruction: 0x%04X\n", curr_instruction);
				return false;
			}
			break;
		case 0x1: /* jump */
			emulator->pc = nnn;
			break;
		case 0x6: /* set register */
			emulator->v[x] = nn;
			break;
		case 0x7: /* add value to register */
			emulator->v[x] += nn;
			break;
		case 0xA: /* set index register */
			emulator->i = nnn;
			break;
		case 0xD: { /* display/draw */
			uint8_t px = emulator->v[x] % CHIP8_DISPLAY_WIDTH;
			uint8_t py = emulator->v[y] % CHIP8_DISPLAY_HEIGHT;
			emulator->v[0xF] = 0;
			for (uint8_t row = 0; row < n; row++) {
				if (py + row >= CHIP8_DISPLAY_HEIGHT) {
					break;
				}
				uint8_t sprite_data = emulator->memory[emulator->i + row];
				for (uint8_t col = 0; col < 8; col++) {
					if (px + col >= CHIP8_DISPLAY_WIDTH) {
						break;
					}
					uint8_t sprite_pixel = (sprite_data >> (7 - col)) & 0x1;
					uint16_t pixel_idx = (py + row) * CHIP8_DISPLAY_WIDTH + px + col;
					if (sprite_pixel == 0) {
						continue;
					} else if (emulator->display[pixel_idx] == CHIP8_PIXEL_ON) {
						emulator->display[pixel_idx] = CHIP8_PIXEL_OFF;
						emulator->v[0xF] = 1;
					} else {
						emulator->display[pixel_idx] = CHIP8_PIXEL_ON;
					}
				}
			}
			break;
		}
		default:
			fprintf(stderr, "Unknown instruction: 0x%04X\n", curr_instruction);
			return false;
	}

	return true;
}