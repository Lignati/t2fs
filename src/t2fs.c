#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"

struct t2fs_inode diretorioAtualInode;
struct t2fs_superbloco superBloco;
struct t2fs_inode diretorioRaizInode;
char * blocoAtual;

void caminhoParcial(char * stringTotal, char * stringParcial){
	int i,shift;
	char stringNovoPath[59];
	
	i = 0;
		
	if(stringTotal[0] == '/'){
		i++;
		
		
	}
	else{
		while(stringTotal[i] != '\0' && stringTotal[i] != '/'){
			stringParcial[i] = stringTotal[i];
			i++;
		}
		stringParcial[i] = '\0'; 
		i++;
	}

	shift = 0;

	if(stringTotal[i] == '\0'){
		strcpy(stringTotal,"\0");
		return;
	}
	
	while(stringTotal[i] != '\0'){
		
		stringNovoPath[shift] = stringTotal[i];
		shift++;i++;
	}
	
	stringNovoPath[shift] = '\0';

	strcpy(stringTotal,stringNovoPath);

}

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
	int i;
	blocoAtual  = (char *) malloc (sizeof(char)*1024);

}

void carregaBloco(int i){
	char buffer[256];
	int n;
	n = (i*4);
	//printf("Em carregaBloco...\n");
	read_sector (n, buffer);
	//printf("leu primeiro Setor...");
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

void printOcupacaoDebug(){
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
	carregaBloco(3+((int)n/32));
	//printf("bloco carregado\n");
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
	printf("\nSize in Blocks %d\n",iNode.blocksFileSize);
	printf("Size in Bytes %d\n",iNode.bytesFileSize);
	printf("Data ptr 1 %u\n",iNode.dataPtr[0]);
	printf("Data ptr 2 %u\n",iNode.dataPtr[1]);
	printf("Ptr Ind Simples %u\n",iNode.singleIndPtr);
	printf("Ptr Ind Dupla %u\n\n",iNode.doubleIndPtr);


}

struct t2fs_inode readAndPrintDir(struct t2fs_inode diretorioInode){
	struct t2fs_record record;
	int i;
	if(diretorioAtualInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);
		for(i = 0; i < RECORD_NUMBER; i++) {
			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_DIRETORIO){
				printf("TypeVal: %X\n",record.TypeVal);
				printf("Name: %s\n",record.name);
				printf("iNodeNumber: %d\n",record.inodeNumber);
		
				printf("----------------------------------------------------------------------\n");
			}
		}
	}
	if(diretorioAtualInode.blocksFileSize > 1){
		int i;
		carregaBloco(diretorioInode.dataPtr[1]);
		if(record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_DIRETORIO){
			for(i = 0; i < RECORD_NUMBER; i++) {
				memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
				printf("TypeVal: %X\n",record.TypeVal);
				printf("Name: %s\n",record.name);
				printf("iNodeNumber: %d\n",record.inodeNumber);
		
				printf("----------------------------------------------------------------------\n");
			}
		}
	}
	if(diretorioInode.blocksFileSize > 2){

		printf("leitura de indirecao\n");

	}
}

int findFile(struct t2fs_inode diretorioInode,char * nome){
	struct t2fs_record record;
	int i;
	char partialPath[59];
	if(nome[0] == '/'){
		diretorioInode = diretorioRaizInode;
		i = 1;
		while(nome[i] != '\0'){	
			nome[i-1] = nome[i];
			i++;	
		}
		nome[i-1] = '\0';
	}
	caminhoParcial(nome,partialPath);
	if(diretorioAtualInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);
		printf("bloco ptr:%d\n",diretorioInode.dataPtr[0]);
		for(i = 0; i < RECORD_NUMBER; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			printf("%s\n",record.name);
			if(record.TypeVal == TYPEVAL_REGULAR && strcmp(partialPath,record.name) == 0){
				return record.inodeNumber;

			}
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findFile(leInode(record.inodeNumber),nome);
			}
		}
	}
	if(diretorioAtualInode.blocksFileSize > 1){	
		carregaBloco(diretorioInode.dataPtr[1]);
		for(i = 0; i < RECORD_NUMBER; i++) {
			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_REGULAR && strcmp(partialPath,record.name) == 0){
				return record.inodeNumber;
			}
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findFile(leInode(record.inodeNumber),nome);
			}
		}
	}
	if(diretorioInode.blocksFileSize > 2){
		printf("leitura de indirecao)");
	}
	return -1;
}

int findDir(struct t2fs_inode diretorioInode,char * nome){
	struct t2fs_record record;
	int i;
	char partialPath[59];
	

	if(nome[0] == '/'){
		diretorioInode = diretorioRaizInode;
		i = 1;
		while(nome[i] != '\0'){	
			nome[i-1] = nome[i];
			i++;	
		}
		nome[i-1] = '\0';
	}
	caminhoParcial(nome,partialPath);
	printf("Nome:%s PartialPath %s \n",nome,partialPath);
	printf("i\n",record.name);
	readAndPrintDir(diretorioInode);
	if(diretorioAtualInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);
		for(i = 0; i < RECORD_NUMBER; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));

			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){

				if(nome[0] == '\0'){
		
					return record.inodeNumber;
				}
				printf("Inode number %d \n",record.inodeNumber);
				return findDir(leInode(record.inodeNumber),nome);
			}
		}
	}
	if(diretorioAtualInode.blocksFileSize > 1){	
		carregaBloco(diretorioInode.dataPtr[1]);
		for(i = 0; i < RECORD_NUMBER; i++) {
			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findDir(leInode(record.inodeNumber),nome);
			}
		}
	}
	if(diretorioInode.blocksFileSize > 2){
		printf("leitura de indirecao)");
	}
	return -1;
}


/*
Função que retorna uma lista do tipo list[iNode.blocksFileSize] contendo
todos os ponteiros diretos que formam o arquivo.
*/
DWORD* getPonteiros(struct t2fs_inode iNode){
	//DWORD listBloc[iNode.blocksFileSize];
	DWORD * listBloc = (DWORD *) calloc (iNode.blocksFileSize, sizeof (DWORD));
	char * bloco = (char *) malloc (sizeof(char)*1024);
	int i;
	//Blocos endereçados diretamente.
	if (iNode.blocksFileSize == 1)
		listBloc[0] = iNode.dataPtr[0];
	if (iNode.blocksFileSize == 2)
		listBloc[1] = iNode.dataPtr[1];
	
	//Indireção simples.
	if (iNode.singleIndPtr != INVALID_PTR){
		bloco = getBloco(iNode.singleIndPtr);		
		for (i = 2; i < iNode.blocksFileSize; i++){
			memcpy((void *)&(listBloc[i]), (void*)&bloco[4*(i-2)],4);			
		}				
	}
	
	//Indireção Dupla
	if (iNode.doubleIndPtr != INVALID_PTR){
	}
	
	
	
	
	for(i = 0; i< iNode.blocksFileSize; i++){
		printf("%d - ", listBloc[i]);
	}
	
	return listBloc;
	
	
}











/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////FUNCOES DE USUARIO/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FILE2 open2 (char *filename){

	
	return findFile(diretorioAtualInode,filename);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////MAIN PARA TESTES/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
	char string1 [52]= "aaaaaaaaaa/bbbbbbbbbbbbb)";
	char string2 [52];
	char file [] = "/dir1";
	int a = sizeof(char);
	int i,j;	
	struct t2fs_record record;
	struct t2fs_inode inode;
	DWORD * listBloc;

	
	iniciaBloco();
	carregaSuperBloco();
	//printSuperBloco();
	
	
	
	for (j = 0; j < 5; j++ ){
		if (getBitmap2(0,j) == 1){
			inode = leInode(j);
			printf("inode: %d ---> " , j);
			listBloc = getPonteiros(inode);
			printf("fim getPonteiros...\n");
			//if (inode.blocksFileSize > 2)
			//	printf("inode: %d", i);
			//printf("inode lido...\n");
			//printInode(leInode(i));
		}			
	}
	
	

	
	
	
	
	
	//carregaBloco(3);
	//diretorioRaizInode = leInode(0);
	
	//diretorioAtualInode = leInode(1);
	//printInode(diretorioAtualInode);
	//diretorioAtualInode = leInode(1);
	//readAndPrintDir(diretorioRaizInode);
	//printf("%d\n",findDir(diretorioRaizInode,file));
	

	printf("FIM EXECUCAO\n");
	return 0;
}
