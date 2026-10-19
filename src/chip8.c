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

void chip8_initialize(chip8 *emulator) {
	*emulator = (chip8){0};
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