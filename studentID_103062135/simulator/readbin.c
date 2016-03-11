#include <stdio.h>
#include <stdlib.h>

void toBinary(char *buffer, long lSize);

int main () {
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( "iimage.bin" , "rb" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);
  printf("%ld\n", lSize);
  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */
  /*int i;
  for(i = 0; i < lSize; i++){
    printf("%02X", (unsigned char)buffer[i]);
    if((i+1)%4==0)printf("\n");
  }*/

  toBinary(buffer, lSize);
  // terminate
  fclose (pFile);
  free (buffer);
  return 0;
}

void toBinary(char *buffer, long lSize){
  	int i;
	unsigned int result[100];
  	for(i = 0; i < lSize; i++){
		//printf("%X%X ", (unsigned char)buffer[i]/16, (unsigned char)buffer[i]%16);
		int a = (unsigned char)buffer[i]/16, b = (unsigned char)buffer[i]%16;
		printf("%d%d%d%d %d%d%d%d ", (a&8)>0, (a&4)>0, (a&2)>0, (a&1), (b&8)>0, (b&4)>0, (b&2)>0, (b&1));
		if((i+1)%4 == 0)printf("\n");
  	}
  	return;  
}
