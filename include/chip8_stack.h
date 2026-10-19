#ifndef CHIP8_STACK_H
#define CHIP8_STACK_H

#include <stdint.h>
#include <stdbool.h>

#define CHIP8_STACK_SIZE 16

typedef struct {
	uint8_t sp; /* index of the next free slot: 0 = empty, CHIP8_STACK_SIZE = full */
	uint16_t array[CHIP8_STACK_SIZE];
} chip8_stack;

void chip8_stack_initialize(chip8_stack *stack);
bool chip8_stack_is_empty(chip8_stack const *stack);
bool chip8_stack_is_full(chip8_stack const *stack);
void chip8_stack_push(chip8_stack *stack, uint16_t address);
uint16_t chip8_stack_pop(chip8_stack *stack);
uint16_t chip8_stack_peek(chip8_stack const *stack);

#endif /* CHIP8_STACK_H */