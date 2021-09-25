#include "symbol_table.h"

void init_symbol_table(SymbolTable *table, uint16_t sz){
	table->sz=sz;
	table->top_addr=0;
	table->symbols=(char**)calloc(sz, sizeof(char*));
	table->values=(uint16_t*)calloc(sz, sizeof(uint16_t));
}
	
void clear_table(SymbolTable *table, uint16_t n_elements){
	for(uint16_t i=0; i<n_elements; i++)
		--(table->top_addr);
}

uint8_t is_symbol_on_table(SymbolTable *table, char *symbol){
	for(uint16_t i=0; i<table->sz; i++){
		if(table->symbols[i]!=NULL&&strcmp(table->symbols[i], symbol)==0)
			return SYMBOL_EXIST;
	}
	return SYMBOL_NOT_FOUND;
}

uint8_t append_symbol_to_table(SymbolTable *table, char *symbol){
	if(table->top_addr==table->sz)
		return SYMBOL_OVERFLOW;
	if(strlen(symbol)==0)
		return EMPTY_SYMBOL;
	if(is_symbol_on_table(table, symbol)==SYMBOL_EXIST)
		return SYMBOL_EXIST;
	for(uint16_t i=0; i<table->sz; i++){
		if(table->symbols[i]!=NULL&&strcmp(table->symbols[i], symbol)==0)
			return SYMBOL_EXIST;
	}
	table->symbols[(table->top_addr)++]=symbol;
	table->values[table->top_addr]=0;
	return SYMBOL_ADDED;
}

uint8_t set_value_on_table(SymbolTable *table, char *symbol, uint16_t value){
	for(uint16_t i=0; i<table->sz; i++){
		if(strcmp(table->symbols[i], symbol)==0){
			table->values[i]=value;
			return VALUE_SETTED;
		}
	}
	return SYMBOL_NOT_FOUND;
}

uint8_t get_value_from_table(SymbolTable *table, char *symbol, uint16_t *value_ptr){
	for(uint16_t i=0; i<table->sz; i++){
		if(strcmp(table->symbols[i], symbol)==0){
			*value_ptr=table->values[i];
			return GOT_VALUE;
		}
	}
	return SYMBOL_NOT_FOUND;
}

void print_symbol_table(SymbolTable *table){
	for(uint16_t i=0; i<table->sz; i++)
		printf("|%s|%d|\n", table->symbols[i], table->values[i]);
	printf("%c", '\n');
}