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

FILE *i_file, *d_file, *error, *snap;
int i_size, d_size;
char *i_buffer, *d_buffer;
size_t i_result, d_result;
unsigned int PC, i_memory[1024];
unsigned int sp, d_data[256];
unsigned char d_memory[1024];
unsigned int reg[32];

void read_d_memory(int load_num){
	int i;
	for(i = 0; i < load_num; i++){
		//change 12 34 56 78  to  78 56 34 12
		d_data[i] = d_data[i] << 24 | d_data[i] >> 8 << 24 >> 8 | d_data[i] >> 16 << 24 >> 16 | d_data[i] >> 24;
	}
	reg[29] = d_data[0];
	for(i = 0; i < d_data[1]; i++){
		d_memory[i*4] = d_data[i+2] >> 24;
		d_memory[i*4 + 1] = d_data[i+2] << 8 >> 24;
		d_memory[i*4 + 2] = d_data[i+2] << 16 >> 24;
		d_memory[i*4 + 3] = d_data[i+2] << 24 >> 24;
	}
}

void read_i_memory(int load_num){
	unsigned int i, opcode;
	int funct, rs, rt, rd, shamt, C_26;
	short C;
	for(i = 0; i < load_num; i++){
		//change 12 34 56 78  to  78 56 34 12
		i_memory[i] = i_memory[i] << 24 | i_memory[i] >> 8 << 24 >> 8 | i_memory[i] >> 16 << 24 >> 16 | i_memory[i] >> 24;
	}
	PC = i_memory[0];
	int cycle = 0;

	fprintf(snap, "cycle %d\n", cycle++);
	for(i = 0; i < 32; i++){
		fprintf(snap, "$%02d: 0x%08X\n", i, reg[i]);
	}fprintf(snap, "PC: 0x%08X\n\n\n", PC);
	
	for(i = 2; i < load_num; i = ((PC-i_memory[0]) >> 2) + 2){
		opcode = i_memory[i] >> 26;
		//printf("%08X ", i_memory[i]);
		PC = PC + 4;

		printf("cycle %d ", cycle);
		int w0error = 0;
		
		switch(opcode){
			case R: 
				funct = i_memory[i] << 26 >> 26;
				rs = i_memory[i] << 6 >> 27;
				rt = i_memory[i] << 11 >> 27;
				rd = i_memory[i] << 16 >> 27;
				shamt = i_memory[i] << 21 >> 27;
				int s_sign = 0;
				int t_sign = 0;

				if(rd == 0 && funct != jr && (i_memory[i] << 11 >> 11) != 0){
					fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
					w0error = 1;
				}

				switch(funct){
					case add:
						s_sign = reg[rs] >> 31;
						t_sign = reg[rt] >> 31;
						reg[rd] = reg[rs] + reg[rt];
						printf("add %u = %u + %u\n", reg[rd], reg[rs], reg[rt]);
						if(s_sign == t_sign && s_sign != reg[rd] >> 31)
							fprintf(error, "In cycle %d: Number Overflow\n", cycle);
						break;
					case addu:
						printf("addu\n");
						reg[rd] = reg[rs] + reg[rt];
						break;
					case sub:
						s_sign = reg[rs] >> 31;
						t_sign = (~reg[rt] + 1) >> 31;
						reg[rd] = reg[rs] + (~reg[rt] + 1);
						printf("sub %d = %d - %d\n", reg[rd], reg[rs], reg[rt]);
						if(s_sign == t_sign && s_sign != reg[rd] >> 31)
							fprintf(error, "In cycle %d: Number Overflow\n", cycle);
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
						reg[rd] = ((int)reg[rs] < (int)reg[rt]);
						printf("slt %d = %d < %d\n", reg[rd], reg[rs], reg[rt]);
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
						reg[rd] = (int)reg[rt] >> shamt;
						break;
					case jr:
						printf("jr\n");
						PC = reg[rs];
						break;
				}
				break;
			case j:
				printf("j");
				C_26 = i_memory[i] << 6 >> 6;
				PC = PC >> 28 << 28 | (unsigned int)C_26 << 2;
				printf(" %08X\n", PC);
				break;
			case jal:
				printf("jal\n");
				C_26 = i_memory[i] << 6 >> 6;
				reg[31] = PC;
				PC = PC >> 28 << 28 | (unsigned int)C_26 << 2;
				break;
			case halt: 
				printf("halt\n");
				return;
				break;
			default:
				rs = i_memory[i] << 6 >> 27;
				rt = i_memory[i] << 11 >> 27;
				C = i_memory[i] << 16 >> 16;
				int addr = reg[rs] + C;
				int halterror = 0;

				if(opcode <= 37 && opcode >= 8 && rt == 0){
					fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
					w0error = 1;
				}
				
				if(opcode >= 32 || opcode == 8){
					if((reg[rs] >> 31) == ((unsigned short)C >> 15) && (reg[rs] >> 31) != ((unsigned int)addr >> 31)){
						fprintf(error, "In cycle %d: Number Overflow\n", cycle);
					}
				}
				
				printf("%d\n", addr);
				if(opcode >= 32){
					if(((opcode == lw || opcode == sw) && (addr > 1020 || addr < 0)) || 
					   ((opcode == lh || opcode == lhu || opcode == sh) && (addr > 1022 || addr < 0)) || 
					   ((opcode == lb || opcode == lbu || opcode == sb) && (addr > 1023 || addr < 0))){
						fprintf(error, "In cycle %d: Address Overflow\n", cycle);
						printf("addr overflow\n");
						halterror = 1;
					}
				}
				
				if(opcode >= 32){
					if(((opcode == lw || opcode == sw) && (addr % 4) != 0) || 
					   ((opcode == lh || opcode == lhu || opcode == sh) && (addr % 2) != 0)){
						fprintf(error, "In cycle %d: Misalignment Error\n", cycle);
						halterror = 1;	
					}
				}

				if(halterror == 1)return;

				switch(opcode){
					case addi:
						//printf("addi %d = %d + %hd\n", rt, rs, C);
						reg[rt] = addr;
						printf("addi %d(%d) = %d(%d) + %hd(%d)\n", reg[rt],reg[rt] >> 31, reg[rs],reg[rs] >> 31, C,(unsigned short)C >> 15);
						break;
					case addiu:
						printf("addiu\n");
						reg[rt] = reg[rs] + (unsigned short)C;
						break;
					case lw:
						printf("lw\n");
						reg[rt] = d_memory[addr] << 24 | d_memory[addr+1] << 16 | d_memory[addr+2] << 8 | d_memory[addr+3];
						break;
					case lh:
						printf("lh\n");
						reg[rt] = (char)d_memory[addr] << 8 | (unsigned char)d_memory[addr+1];
						break;
					case lhu:
						printf("lhu %d %d %hd\n", rs, rt, C);
						reg[rt] = (unsigned char)d_memory[addr] << 8 | (unsigned char)d_memory[addr+1];
						break;
					case lb:
						printf("lb\n");
						reg[rt] = (char)d_memory[reg[rs] + C];
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
						d_memory[addr] = reg[rt] << 24 >> 24;
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
						reg[rt] = ((int)reg[rs] < C) ? 1 : 0;
						printf("slti\n");
						break;
					case beq:
						if(reg[rs] == reg[rt])
							PC += C << 2;
						printf("beq\n");
						break;
					case bne:
						printf("bne %d %d\n", rs, rt);
						if(reg[rs] != reg[rt])
							PC += C << 2;
						break;
					case bgtz:
						printf("bgtz\n");
						if(reg[rs] > 0)
							PC += C << 2;
						break;
				}
				break;
		}

		if(w0error == 1){
			reg[0] = 0;
			w0error = 0;
		}

		fprintf(snap, "cycle %d\n", cycle++);
		int reg_n;
		for(reg_n = 0; reg_n < 32; reg_n++){
			fprintf(snap, "$%02d: 0x%08X\n", reg_n, reg[reg_n]);
		}fprintf(snap, "PC: 0x%08X\n\n\n", PC);
		
		while(PC < i_memory[0]){
			PC += 4;
			fprintf(snap, "cycle %d\n", cycle++);
			for(reg_n = 0; reg_n < 32; reg_n++){
				fprintf(snap, "$%02d: 0x%08X\n", reg_n, reg[reg_n]);
			}fprintf(snap, "PC: 0x%08X\n\n\n", PC);
		}

	}
}


int main () {

	i_file = fopen("iimage.bin", "rb");
	d_file = fopen("dimage.bin", "rb");
	error = fopen("error_dump.rpt", "w");
	snap = fopen("snapshot.rpt", "w");

	if (i_file == NULL || d_file == NULL) {fputs ("File error",stderr); exit (1);}

	// obtain file size:
	fseek (i_file , 0 , SEEK_END);
	fseek (d_file , 0 , SEEK_END);
	i_size = ftell(i_file);
	d_size = ftell(d_file);
	rewind(i_file);
	rewind(d_file);

	// copy the file into the buffer:
	i_result = fread(i_memory, 4, i_size/4, i_file);
	d_result = fread(d_data  , 4, d_size/4, d_file);

	//if (i_result != i_size || d_result != d_size) {fputs ("Reading error",stderr); exit (3);}

	/* the whole file is now loaded in the memory buffer. */
	
	read_d_memory(d_size/4); 
	read_i_memory(i_size/4);
		
	// terminate
	fclose(i_file);
	fclose(d_file);
	fclose(error);
	free(i_buffer);
	free(d_buffer);
	return 0;
}


