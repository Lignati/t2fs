#include <stdio.h>
#include "../include/t2fs.h"
int main(){

	struct t2fs_inode iNode;
	char buffer[256] = "HEYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY   MEIO   DO   ARQUIVO    YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYFIM";
	char bufferRead[256];
	int i;
	//Verificar aqui!
	int tamanhoBlocoBytes = 1024;
	//Verificar aqui!
	HANDLE fileHandleList[10];
	//Verificar aqui!
	create2 ("ArquivoMuitoGrande");
	int handleNumber = open2 ("ArquivoMuitoGrande");
	int maxBlocos = tamanhoBlocoBytes/sizeof(DWORD) * tamanhoBlocoBytes/sizeof(DWORD);
		
	printf("Escrevendo o numero maximo de blocos: %d\n", maxBlocos);
	//iNode = leInode(fileHandleList[handleNumber].inodeNumber);	
	
	while(iNode.blocksFileSize < maxBlocos - 100){
		if(iNode.doubleIndPtr != INVALID_PTR)
			printf("doubleIndPtr --> iNode.blocksFileSize: %d\n", iNode.blocksFileSize);
		write2(handleNumber, buffer,256);
		//iNode = leInode(fileHandleList[handleNumber].inodeNumber);		
	}
	//iNode = leInode(fileHandleList[handleNumber].inodeNumber);	
	printf("Total de blocos utilizados %d\n", iNode.blocksFileSize);
	
	printf("Agora ler tudo...\nPress Enter");
	scanf("...", &i);
	seek2(handleNumber, 0);
	//iNode = leInode(fileHandleList[handleNumber].inodeNumber);
	
	while(fileHandleList[handleNumber].seekPtr < iNode.bytesFileSize - 1 ){
		read2(handleNumber, bufferRead, 256);
		for(i = 0; i < 256; i++)
			printf("%c", bufferRead[i]);
	}
	
	//iNode = leInode(fileHandleList[handleNumber].inodeNumber);
	printf("\n\n\n\nFIM da Leitura dos %d Blocos\n", iNode.blocksFileSize);	
	
}
