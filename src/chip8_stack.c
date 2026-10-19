#include "chip8_stack.h"

#include <stdio.h>
#include <stdlib.h>

void chip8_stack_initialize(chip8_stack *stack) {
	/* Reset all fields (sp and array) to zero using a compound literal */
	*stack = (chip8_stack){0};
}

bool chip8_stack_is_empty(chip8_stack const *stack) {
	return stack->sp == 0;
}

bool chip8_stack_is_full(chip8_stack const *stack) {
	return stack->sp == CHIP8_STACK_SIZE;
}

void chip8_stack_push(chip8_stack *stack, uint16_t address) {
	if (chip8_stack_is_full(stack)) {
		fprintf(stderr, "CHIP8_STACK OVERFLOW\n");
		exit(EXIT_FAILURE);
	} else {
		stack->array[stack->sp++] = address;
	}
}

uint16_t chip8_stack_pop(chip8_stack *stack) {
	if (chip8_stack_is_empty(stack)) {
		fprintf(stderr, "CHIP8_STACK UNDERFLOW\n");
		exit(EXIT_FAILURE);
	} else {
		uint16_t address = stack->array[--stack->sp];
		stack->array[stack->sp] = 0;
		return address;
	}
}

uint16_t chip8_stack_peek(chip8_stack const *stack) {
	if (chip8_stack_is_empty(stack)) {
		fprintf(stderr, "CHIP8_STACK EMPTY\n");
		exit(EXIT_FAILURE);
	} else {
		return stack->array[stack->sp - 1];
	}
}