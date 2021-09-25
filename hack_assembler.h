#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "memory_manager.h"
#include "symbol_table.h"
enum symbol_tables {SYMBOLS, COMP, DEST, JUMP};
enum shift_types {RIGHT, LEFT};

void get_symbol_table(char *file_path, SymbolTable *table, uint16_t ln_buff_sz);
void init_table(SymbolTable *table, uint16_t table_sz, char *file_path, uint16_t ln_buff_sz);
void clear_left_white_space(char *ln);
void clear_right_white_space(char *ln);
void clear_white_space(char *ln);
void parse_labels(char *file_path, SymbolTable *symbols, uint16_t ln_buff_sz);
void create_command(uint8_t i, uint8_t j, uint16_t init_mask, uint8_t shift_type, char *buffer, uint16_t n);
void parse_command(char *assembly_path, char *hack_path, SymbolTable *tables, uint16_t ln_buff_sz, Memory *memory);