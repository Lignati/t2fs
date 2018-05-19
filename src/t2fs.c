#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"


struct t2fs_superbloco superBloco;
char * blocoAtual;


void carregaSuperBloco(){
	char buffer[SECTOR_SIZE];

	read_sector (0, buffer);
	memcpy((void *)&(superBloco.id),                  (void*)&buffer[0] ,4);
	memcpy((void *)&(superBloco.version),             (void*)&buffer[4] ,2);
	memcpy((void *)&(superBloco.superblockSize),      (void*)&buffer[6] ,2);
	memcpy((void *)&(superBloco.freeBlocksBitmapSize),(void*)&buffer[8] ,2);
	memcpy((void *)&(superBloco.freeInodeBitmapSize), (void*)&buffer[10],2);
	memcpy((void *)&(superBloco.inodeAreaSize),       (void*)&buffer[12],2);
	memcpy((void *)&(superBloco.blockSize),           (void*)&buffer[14],2);
	memcpy((void *)&(superBloco.diskSize),            (void*)&buffer[16],4);



}
void iniciaBloco(){
	blocoAtual = (char *) malloc (sizeof(char)*1024);


}
void carregaBloco(int i){
	char buffer[256];
	int n;
	n = (i*4);
	read_sector (n, buffer);
	memcpy((void*)&blocoAtual[0],            (void*)&buffer[0],256);
	read_sector (n+1, buffer);
	memcpy((void*)&blocoAtual[SECTOR_SIZE*1],(void*)&buffer[0],256);	
	read_sector (n+2, buffer);
	memcpy((void*)&blocoAtual[SECTOR_SIZE*2],(void*)&buffer[0],256);
	read_sector (n+3, buffer);
	memcpy((void*)&blocoAtual[SECTOR_SIZE*3],(void*)&buffer[0],256);
	

}

void printSuperBloco(){
	
	printf("ID:%c %c %c %c \n",superBloco.id[0],superBloco.id[1],superBloco.id[2],superBloco.id[3]);
	printf("Version:%X \n",superBloco.version);
	printf("SuperBlockBitmap:%d \n",superBloco.superblockSize);
	printf("FreeBlockBitMap:%d \n",superBloco.freeBlocksBitmapSize);
	printf("FreeiNodeBitmap:%d \n",superBloco.freeInodeBitmapSize);
	printf("iNodeArea:%d \n",superBloco.inodeAreaSize);
	printf("BlockSize:%d \n",superBloco.blockSize);
	printf("DiskSize:%d \n",superBloco.diskSize);


}
void printfOcupacaoDebug(int opt){
	char buffer[SECTOR_SIZE];
	int i;

	printf("iNodes\n");
	for(i =0; i<256*4 ;i++){
		printf("%4d -> %d ",i,getBitmap2 (0,i));
		if (i%12 == 11 )
			printf("\n");
	}
	printf("\n");
	printf("Dados\n");
	for(i =0; i<256*4 ;i++){
		printf("%4d -> %d ",i,getBitmap2 (1,i));
		if (i%12 == 11 )
			printf("\n");
	}
	
	
	
	



}
struct t2fs_inode leInode(int n){
	struct t2fs_inode iNode;
	int pos;
	pos = n*32;

	memcpy((void *)&(iNode.blocksFileSize),            (void*)&blocoAtual[pos]    ,4);
	memcpy((void *)&(iNode.bytesFileSize),             (void*)&blocoAtual[pos+4]  ,4);
	memcpy((void *)&(iNode.dataPtr[0]),                (void*)&blocoAtual[pos+8]  ,4);
	memcpy((void *)&(iNode.dataPtr[1]),                (void*)&blocoAtual[pos+12] ,4);
	memcpy((void *)&(iNode.singleIndPtr),              (void*)&blocoAtual[pos+16] ,4);
	memcpy((void *)&(iNode.doubleIndPtr),              (void*)&blocoAtual[pos+20] ,4);
	memcpy((void *)&(iNode.reservado),                 (void*)&blocoAtual[pos+24] ,8);	
	
	return iNode;
	

}
void printInode(struct t2fs_inode iNode){
	printf("Size in Blocks %d\n",iNode.blocksFileSize);
	printf("Size in Bytes %d\n",iNode.bytesFileSize);
	printf("Data ptr 1 %u\n",iNode.dataPtr[0]);
	printf("Data ptr 2 %u\n",iNode.dataPtr[1]);
	printf("Ptr Ind Simples %u\n",iNode.singleIndPtr);
	printf("Ptr Ind Dupla %u\n",iNode.doubleIndPtr);


}
int main(){
	int a = sizeof(char);
	int i,j;
	iniciaBloco();
	carregaSuperBloco();
	printSuperBloco();
	


	carregaBloco(3);
	printInode(leInode(0));
	carregaBloco(67);
	for(i = 0; i< 1024; i++){
		printf("%c",blocoAtual[i]);
	}

	return 0;
}
