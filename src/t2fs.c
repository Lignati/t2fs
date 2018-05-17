#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/apidisk.h"
#include "../include/apidisk.h"
#include "../include/t2fs.h"


struct t2fs_superbloco superBloco;


void carregaSuperBloco(){
	char buffer[SECTOR_SIZE];

	read_sector (0, buffer);
	memcpy((void *)superBloco.id,                  (void*)&buffer[0] ,4);
	memcpy((void *)superBloco.version,             (void*)&buffer[4] ,sizeof(WORD));
	memcpy((void *)superBloco.superblockSize,      (void*)&buffer[6] ,sizeof(WORD));
	memcpy((void *)superBloco.freeBlocksBitmapSize,(void*)&buffer[8] ,sizeof(WORD));
	memcpy((void *)superBloco.freeInodeBitmapSize, (void*)&buffer[10],sizeof(WORD));
	memcpy((void *)superBloco.inodeAreaSize,       (void*)&buffer[12],sizeof(WORD));
	memcpy((void *)superBloco.blockSize,           (void*)&buffer[14],sizeof(WORD));
	memcpy((void *)superBloco.diskSize,            (void*)&buffer[18],sizeof(DWORD));



}
void printSuperBloco(){

	printf("%c %c %c %c \n",superBloco.id[0],superBloco.id[1],superBloco.id[2],superBloco.id[3]);


}
int main(){

	carregaSuperBloco();
	printSuperBloco();

	return 0;
}
