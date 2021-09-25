#include "hack_assembler.h"

void init_table(SymbolTable *table, uint16_t table_sz, char *file_path, uint16_t ln_buff_sz){
	init_symbol_table(table, table_sz);
	get_symbol_table(file_path, table, ln_buff_sz);
}

void get_symbol_table(char *file_path, SymbolTable *table, uint16_t ln_buff_sz){
	FILE *fp;
	fp=fopen(file_path, "r");
	if(fp==NULL){
		fprintf(stderr, "The table does not exist\n");
		return;          
	}
	char *ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
	fgets(ln_buff, ln_buff_sz, fp);
	char *end="Value\0";
	while (fgets(ln_buff, ln_buff_sz, fp)){
		ln_buff[strcspn(ln_buff, "\r\n")]='\0';
		if(strcmp(ln_buff, end)==0)
			break;
		else{
			append_symbol_to_table(table, ln_buff);
			ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
		}
	}
	uint16_t i=0;
	char *ptr;
	while (fgets(ln_buff, ln_buff_sz, fp)){
		ln_buff[strcspn(ln_buff, "\r\n")]='\0';
		table->values[i++]=strtol(ln_buff, &ptr, 10);
		ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
	}
	free(ln_buff);
	fclose(fp);
}

void clear_white_space(char *ln){
	char *ptr=strstr(ln, "//");
	if(ptr!=NULL)
		*ptr='\0';
	uint16_t i;
	for(i=0; isspace(ln[i]); i++);
	memmove(ln, ln+i, strlen(ln)-i+1);
	i=0;
	do{
		for(; !isspace(ln[i])&&ln[i]; i++);
		if(ln[i]=='\0')
			break;
		memmove(ln+i, ln+i+1, strlen(ln)-i);
	}while(1);
}

void parse_labels(char *file_path, SymbolTable *symbols, uint16_t ln_buff_sz){
	FILE *fp;
	fp=fopen(file_path, "r");
	if(fp==NULL){
		fprintf(stderr, "The table does not exist\n");
		return;
	}
	char *ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
	uint16_t n_line=0;
	fgets(ln_buff, ln_buff_sz, fp);
	while (fgets(ln_buff, ln_buff_sz, fp)){
		clear_white_space(ln_buff);
		if(strlen(ln_buff)==0)
			continue;
		if(ln_buff[0]=='('&&ln_buff[strlen(ln_buff)-1]==')'){
			ln_buff[strlen(ln_buff)-1]='\0';
			memmove(ln_buff, ln_buff+1, strlen(ln_buff));
			if(append_symbol_to_table(symbols, ln_buff)==SYMBOL_OVERFLOW){
				fprintf(stderr, "Symbol Table overflow\n");
				exit(1);
			}
			set_value_on_table(symbols, ln_buff, n_line);
			ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
			continue;
		}
		++n_line;
	}
	free(ln_buff);
	fclose(fp);
}

void create_command(uint8_t i, uint8_t j, uint16_t init_mask, uint8_t shift_type, char *buffer, uint16_t n){
	uint16_t mask=init_mask;
	for(uint8_t k=i; k<=j; k++){
		if(n&mask)
			buffer[k]='1';
		else
			buffer[k]='0';
		if(shift_type==RIGHT)
			mask>>=1;
		else
			mask<<=1;
	}
}

void parse_command(char *assembly_path, char *hack_path, SymbolTable *tables, uint16_t ln_buff_sz, Memory *memory){
	FILE *assembly, *hack;
	assembly=fopen(assembly_path, "r");
	if(assembly==NULL){
		fprintf(stderr, "The table does not exist\n");
		return;
	}
	fclose(fopen(hack_path, "w"));
	hack=fopen(hack_path, "a");
	if(hack==NULL){
		fprintf(stderr, "The table does not exist\n");
		return;
	}
	char *ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
	char command[17];
	command[16]='\0';
	char comp[8], dest[5], jump[5], *null_pt="null\0";
	comp[7]='\0';
	dest[4]='\0';
	jump[4]='\0';
	char *ptr, *ptr2, *ptr3;
	uint16_t addr;
	uint8_t table_state;
	while (fgets(ln_buff, ln_buff_sz, assembly)){
		clear_white_space(ln_buff);
		if(ln_buff[0]=='\0')
			continue;
		else if(ln_buff[0]=='('&&ln_buff[strlen(ln_buff)-1]==')'){
			continue;
		}else if(ln_buff[0]=='@'){
			memmove(ln_buff, ln_buff+1, strlen(ln_buff));
			command[0]='0';
			if(isdigit(ln_buff[0])){
				create_command(1, 15, 0x4000, RIGHT, command, strtol(ln_buff, &ptr, 10));
			}else{
				table_state=append_symbol_to_table(&tables[SYMBOLS], ln_buff);
				switch(table_state){
					case SYMBOL_OVERFLOW:
						fprintf(stderr, "Symbol Table overflow\n");
						exit(1);
						break;
					case SYMBOL_ADDED:
						addr=create_variable(memory);
						if(addr==MEMORY_OVERFLOW){
							fprintf(stderr, "Memory overflow\n");
							exit(1);
						}
						set_value_on_table(&tables[SYMBOLS], ln_buff, addr);
						create_command(1, 15, 0x4000, RIGHT, command, addr);
						break;
					case SYMBOL_EXIST:
						get_value_from_table(&tables[SYMBOLS], ln_buff, &addr);
						create_command(1, 15, 0x4000, RIGHT, command, addr);
						free(ln_buff);
				}
			}
		}else{
			ptr=strchr(ln_buff, '=');
			ptr2=strchr(ln_buff, ';');
			ptr3=strchr(ln_buff, '\0');
			command[0]='1';
			command[1]='1';
			command[2]='1';
			memset(dest, '\0', sizeof(dest));
			memset(comp, '\0', sizeof(comp));
			memset(jump, '\0', sizeof(jump));
			if(ptr!=NULL){
				strncpy(dest, ln_buff, ptr-ln_buff);
				++ptr;
			}else{
				strncpy(dest, null_pt, 4);
				ptr=ln_buff;
			}
			get_value_from_table(&tables[DEST], dest, &addr);
			create_command(10, 12, 0x1, LEFT, command, addr);
			if(ptr2!=NULL){
				if(ptr2-ptr==0){
					fprintf(hack, "Sintax Error: %s", ln_buff);
					exit(1);
				}
				strncpy(comp, ptr, ptr2-ptr);
				++ptr2;
			}else{
				strncpy(comp, ptr, ptr3-ptr);
				ptr2=ptr3;
			}
			get_value_from_table(&tables[COMP], comp, &addr);
			create_command(3, 9, 0x1, LEFT, command, addr);
			if(ptr3-ptr2>0)
				strncpy(jump, ptr2, ptr3-ptr2);
			else
				strncpy(jump, null_pt, 4);
			get_value_from_table(&tables[JUMP], jump, &addr);
			create_command(13, 15, 0x1, LEFT, command, addr);
		}
		fprintf(hack, "%s\n", command);
		ln_buff=(char*)malloc(sizeof(char)*ln_buff_sz);
	}
	free(ln_buff);
	fclose(assembly);
	fclose(hack);
}

int main(){
	SymbolTable tables[4];
	Memory memory;
	init_table(&tables[SYMBOLS], 2048, "symbol.txt", 10);
	init_table(&tables[COMP], 28, "comp.txt", 10);
	init_table(&tables[DEST], 8, "dest.txt", 10);
	init_table(&tables[JUMP], 8, "jump.txt", 10);
	init_memory(&memory);
	char input_buffer[1024];
	char output_buffer[1024];
	char *hack_extension="hack\0";
	char *end="end";
	char *ptr;
	while(1){
		printf("Enter file path or end to finish execution: ");
		scanf("%s", input_buffer);
		if(strcmp(input_buffer, end)==0)
			break;
		parse_labels(input_buffer, &tables[SYMBOLS], 256);
		ptr=strchr(input_buffer, '.');
		strncpy(output_buffer, input_buffer, ptr-input_buffer+1);
		ptr=strchr(output_buffer, '.');
		strncpy(ptr+1, hack_extension, 5);
		parse_command(input_buffer, output_buffer, tables, 256, &memory);
		printf("Assembled!\n");
	}
	return 0;
}