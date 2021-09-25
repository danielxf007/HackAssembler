#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

enum op_states{SYMBOL_ADDED, SYMBOL_EXIST, SYMBOL_OVERFLOW, EMPTY_SYMBOL, VALUE_SETTED, SYMBOL_NOT_FOUND, GOT_VALUE};

typedef struct{
	uint16_t sz, top_addr;
	char **symbols;
	uint16_t *values;
}SymbolTable;

void init_symbol_table(SymbolTable *table, uint16_t sz);
void clear_table(SymbolTable *table, uint16_t n_elements);
uint8_t is_symbol_on_table(SymbolTable *table, char *symbol);
uint8_t append_symbol_to_table(SymbolTable *table, char *symbol);
uint8_t set_value_on_table(SymbolTable *table, char *symbol, uint16_t value);
uint8_t get_value_from_table(SymbolTable *table, char *symbol, uint16_t *value_ptr);
void print_symbol_table(SymbolTable *table);