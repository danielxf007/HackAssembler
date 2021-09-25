#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define STORAGE_SZ 16384
#define INIT_MEMORY_ADDR 16
enum memory_states {MEMORY_OVERFLOW=0x1000};

typedef struct{
	uint16_t *storage, sz, top_addr;
}Memory;

void init_memory(Memory *memory);
void reset_memory(Memory *memory);
uint16_t create_variable(Memory *memory);