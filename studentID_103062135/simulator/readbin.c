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
unsigned int sp, d_data[256];
char d_memory[1024];
unsigned int reg[32];

void read_d_memory(int load_num){
	int i;
	for(i = 0; i < load_num; i++){
		//change 12 34 56 78  to  78 56 34 12
		d_data[i] = d_data[i] << 24 | d_data[i] >> 8 << 24 >> 8 | d_data[i] >> 16 << 24 >> 16 | d_data[i] >> 24;
	}
	for(i = 0; i < d_data[1]; i++){
		d_memory[i * 4] = 
	}
}

void read_i_memory(int load_num){
	unsigned int i, opcode;
	int funct, rs, rt, rd, shamt;
	short C;
	for(i = 0; i < load_num; i++){
		//change 12 34 56 78  to  78 56 34 12
		i_memory[i] = i_memory[i] << 24 | i_memory[i] >> 8 << 24 >> 8 | i_memory[i] >> 16 << 24 >> 16 | i_memory[i] >> 24;
	}
	// PC  printf("%u\n", i_memory[0]);	
	for(i = 2; i < load_num; i++){
		opcode = i_memory[i] >> 26;
		printf("%08X ", i_memory[i]);
		//printf("\n");
		switch(opcode){
			case R: 
				funct = i_memory[i] << 26 >> 26;
				rs = i_memory[i] << 6 >> 27;
				rt = i_memory[i] << 11 >> 27;
				rd = i_memory[i] << 16 >> 27;
				shamt = i_memory[i] << 21 >> 27;
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
						printf("sll\n");
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
						//PC_now = 4*i + PC
						//PC_now = reg[rs]
						printf("jr\n");
						i = reg[rs] >> 2 - PC >> 2 - 1;
						break;
				}
				break;

			case j:
				//PC_now = (PC_now+4)>>28<<28 | C<<2
				//PC_now = 4*i + PC
				printf("j\n");
				C = i_memory[i] << 6 >> 6;
				i = (((4 * i + PC + 4) >> 28 << 28 | C << 2) - PC) >> 2 - 1;
				break;
			case jal:
				//PC_now = 4*i + PC
				printf("jal\n");
				C = i_memory[i] << 6 >> 6;
				reg[31] = (PC + 4 * i) + 4;
				break;
			case halt: 
				printf("halt\n");
				break;
			default:
				rs = i_memory[i] << 6 >> 27;
				rt = i_memory[i] << 11 >> 27;
				C = i_memory[i] << 16 >> 16;
				int addr = reg[rs] + C;
				switch(opcode){
					case addi:
						printf("addi\n");
						reg[rt] = reg[rs] + C;
						break;
					case addiu:
						printf("addiu\n");
						reg[rt] = reg[rs] + (unsigned short)C;
						break;
					case lw:
						printf("lw\n");
						reg[rt] = d_memory[addr] << 24 | d_memory[addr+1] << 16 + d_memory[addr+2] << 8 | d_memory[addr+3];
						break;
					case lh:
						printf("lh\n");
						reg[rt] = d_memory[addr] << 8 + d_memory[addr+1];
						break;
					case lhu:
						printf("lhu\n");
						reg[rt] = (unsigned char)d_memory[addr] << 8 + (unsigned char)d_memory[addr+1];
						break;
					case lb:
						printf("lb\n");
						reg[rt] = d_memory[reg[rs] + C];
						break;
					case lbu:
						printf("lbu\n");
						reg[rt] = (unsigned char)d_memory[reg[rs] + C];
						break;
					case sw:
						printf("sw\n");
						d_memory[addr] = reg[rt] >> 24;
						d_memory[addr+1] = reg[rt] << 8 >> 24;
						d_memory[addr+2] = reg[rt] << 16 >> 24;
						d_memory[addr+3] = reg[rt] << 24 >> 24;
						break;
					case sh:
						printf("sh\n");
						d_memory[addr] = reg[rt] << 16 >> 24;
						d_memory[addr+1] = reg[rt] << 24 >> 24;
						break;
					case sb:
						printf("sb\n");
						d_memory[reg[rs] + C] = reg[rt] << 24 >> 24;
						break;
					case lui:
						printf("lui\n");
						reg[rt] = C << 16;
						break;
					case andi:
						printf("andi\n");
						reg[rt] = reg[rs] & (unsigned short)C;
						break;
					case ori:
						printf("ori\n");
						reg[rt] = reg[rs] | (unsigned short)C;
						break;
					case nori:
						printf("nori\n");
						reg[rt] = ~(reg[rs] | (unsigned short)C);
						break;
					case slti:
						reg[rt] = (reg[rs] < C) ? 1 : 0;
						printf("slti\n");
						break;
					case beq:
						//PC_now = 4*i + PC
						//PC_now + 4 + 4*C = 4*(i + 1 + C) + PC
						if(reg[rs] == reg[rt])
							i = i + C; // add 1 after each cycle
						printf("beq\n");
						break;
					case bne:
						printf("bne\n");
						if(reg[rs] != reg[rt])
							i = i + C;
						break;
					case bgtz:
						printf("bgtz\n");
						if(reg[rs] > 0)
							i = i + C;
						break;
					default:
						break;
				}
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
	printf("%d\n", load_num);
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
	i_result = fread(i_memory, 4, i_size/4, i_file);
	d_result = fread(d_data  , 4, d_size/4, d_file);

	//if (i_result != i_size || d_result != d_size) {fputs ("Reading error",stderr); exit (3);}

	/* the whole file is now loaded in the memory buffer. */

	//make_i_memory(i_buffer);
	//make_d_memory(d_buffer);
	
	read_i_memory(i_size/4);
	read_d_memory(d_size/4); 
	
	// terminate
	fclose(i_file);
	fclose(d_file);
	free(i_buffer);
	free(d_buffer);
	return 0;
}


