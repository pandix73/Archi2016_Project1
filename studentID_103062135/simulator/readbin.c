#include <stdio.h>
#include <stdlib.h>

#define $sp 29
// R
#define R 0
#define add 32   
#define addu 33  
#define sub 34   
#define and 36   
#define or 37    
#define xor 38   
#define nor 39   
#define nand 40  
#define slt 42   
#define sll 0    
#define srl 2    
#define sra 3    
#define jr 8     
// I
#define addi 8   
#define addiu 9  
#define lw 35    
#define lh 33    
#define lhu 37   
#define lb 32    
#define lbu 36   
#define sw 43    
#define sh 41    
#define sb 40    
#define lui 15   
#define andi 12  
#define ori 13   
#define nori 14  
#define slti 10  
#define beq 4    
#define bne 5    
#define bgtz 7   
// J
#define j 2      
#define jal 3    
// S
#define halt 63  

FILE *i_file, *d_file;
int i_size, d_size;
char *i_buffer, *d_buffer;
size_t i_result, d_result;
unsigned int PC, i_memory[1024];
unsigned int sp, d_memory[1024];
unsigned int reg[32];

void read_i_memory(int load_num){
	int i, opcode;
	for(i = 0; i < load_num; i++){
		opcode = i_memory[i] >> 26;
		switch(opcode){

			case R: 
				printf("R\n");
				int funct = i_memory[i] << 26 >> 26;
				int rs = i_memory[i] << 6 >> 26;
				int rt = i_memory[i] << 11 >> 26;
				int rd = i_memory[i] << 16 >> 26;
				int shamt = i_memory[i] << 21 >> 26;
				switch(funct){
					case add:
						printf("add\n");
						reg[rd] = reg[rs] + reg[rt];
						break;
					case addu:
						printf("addu\n");
						reg[rd] = reg[rs] + reg[rt];
						break;
					case sub:
						printf("sub\n");
						reg[rd] = reg[rs] - reg[rt];
						break;
					case and:
						printf("and\n");
						reg[rd] = reg[rs] & reg[rt];
						break;
					case or:
						printf("or\n");
						reg[rd] = reg[rs] | reg[rt];
						break;
					case xor:
						printf("xor\n");
						reg[rd] = reg[rs] ^ reg[rt];
						break;
					case nor:
						printf("nor\n");
						reg[rd] = ~(reg[rs] | reg[rt]);
						break;
					case nand:
						printf("nand\n");
						reg[rd] = ~(reg[rs] & reg[rt]);
						break;
					case slt:
						printf("slt\n");
						reg[rd] = (reg[rs] < reg[rt]);
						break;
					case sll:
						printf("all\n");
						reg[rd] = reg[rt] << shamt;
						break;
					case srl:
						printf("srl\n");
						reg[rd] = reg[rt] >> shamt;
						break;
					case sra:
						printf("sra\n");
						reg[rd] = reg[rt] >> shamt;
						break;
					case jr:
						printf("jr\n");
						PC = reg[rs];
						break;
				}
				break;

			case j:
				break;

			case jal:
				break;

			case halt: 
				break;

			default:
				break;
		}
	}
}

void make_i_memory(char *buffer){
	int i;
	unsigned int load_num = 0;
	for(i = 0; i < 4; i++)
		PC = PC * 256 + (unsigned char)buffer[i];
	for(i = 4; i < 8; i++)
		load_num = load_num * 256 + (unsigned char)buffer[i];
	for(i = 0; i < load_num * 4; i++)
		i_memory[i/4] = i_memory[i/4] * 256 + (unsigned char)buffer[i+8];
	read_i_memory(load_num);
	return;  
}

void make_d_memory(char *buffer){
	int i;
	unsigned int load_num = 0;
	for(i = 0; i < 4; i++)
		sp = sp * 256 + (unsigned char)buffer[i];
	for(i = 4; i < 8; i++)
		load_num = load_num * 256 + (unsigned char)buffer[i];
	for(i = 0; i < load_num * 4; i++)
		d_memory[i/4] = d_memory[i/4] * 256 + (unsigned char)buffer[i+8];
	return;  
}

int main () {

	i_file = fopen("iimage.bin", "rb");
	d_file = fopen("dimage.bin", "rb");

	if (i_file == NULL || d_file == NULL) {fputs ("File error",stderr); exit (1);}

	// obtain file size:
	fseek (i_file , 0 , SEEK_END);
	fseek (d_file , 0 , SEEK_END);
	i_size = ftell(i_file);
	d_size = ftell(d_file);
	rewind(i_file);
	rewind(d_file);

	// allocate memory to contain the whole file:
	i_buffer = (char*)malloc(sizeof(char)*i_size);
	d_buffer = (char*)malloc(sizeof(char)*d_size);

	if (i_buffer == NULL || d_buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

	// copy the file into the buffer:
	i_result = fread(i_buffer, 1, i_size, i_file);
	d_result = fread(d_buffer, 1, d_size, d_file);

	if (i_result != i_size || d_result != d_size) {fputs ("Reading error",stderr); exit (3);}

	/* the whole file is now loaded in the memory buffer. */

	make_i_memory(i_buffer);
	make_d_memory(d_buffer);

	// terminate
	fclose(i_file);
	fclose(d_file);
	free(i_buffer);
	free(d_buffer);
	return 0;
}


