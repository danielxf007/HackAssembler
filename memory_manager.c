#include "memory_manager.h"

void init_memory(Memory *memory){
	memory->storage=(uint16_t*)calloc(STORAGE_SZ, sizeof(uint16_t));
	memory->sz=STORAGE_SZ;
	memory->top_addr=INIT_MEMORY_ADDR;
}

void reset_memory(Memory *memory){
	memset(memory->storage, 0, (memory->sz)*sizeof(uint16_t));
	memory->top_addr=INIT_MEMORY_ADDR;
}

uint16_t create_variable(Memory *memory){
	uint16_t addr=MEMORY_OVERFLOW;
	if(memory->top_addr<memory->sz)
		addr=(memory->top_addr)++;
	return addr;
}