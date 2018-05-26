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
int blocoInodesInicial;
int tamanhoBlocoBytes;
int tamanhoBloco;
int numeroRecords;
int initFlag = 0;
HANDLE fileHandleList[10];
HANDLE dirHandleList [10];

// funcao recebe duas strigs retrona na segunda o correspondente ao diretorio atual e na primeira o correspondente ao resto do caminho
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
		
	}

	shift = 0;

	if(stringTotal[i] == '\0'){
		strcpy(stringTotal,"\0");
		return;
	}
	i++;
	
	while(stringTotal[i] != '\0'){
		
		stringNovoPath[shift] = stringTotal[i];
		shift++;i++;
	}
	
	stringNovoPath[shift] = '\0';

	strcpy(stringTotal,stringNovoPath);

}
//carrega o super bloco e faz os calculos iniciais das variaveis globais, deve estar na init()	
void carregaSuperBloco(){
	char buffer[SECTOR_SIZE];

	read_sector (0, buffer);
	//copia do setor para a estrutura global
	memcpy((void *)&(superBloco.id),                  (void*)&buffer[0] ,4);
	memcpy((void *)&(superBloco.version),             (void*)&buffer[4] ,2);
	memcpy((void *)&(superBloco.superblockSize),      (void*)&buffer[6] ,2);
	memcpy((void *)&(superBloco.freeBlocksBitmapSize),(void*)&buffer[8] ,2);
	memcpy((void *)&(superBloco.freeInodeBitmapSize), (void*)&buffer[10],2);
	memcpy((void *)&(superBloco.inodeAreaSize),       (void*)&buffer[12],2);
	memcpy((void *)&(superBloco.blockSize),           (void*)&buffer[14],2);
	memcpy((void *)&(superBloco.diskSize),            (void*)&buffer[16],4);
	//inicia as varaiveis globais
	blocoInodesInicial  = superBloco.superblockSize + superBloco.freeBlocksBitmapSize + superBloco.freeInodeBitmapSize;
	tamanhoBlocoBytes   = SECTOR_SIZE * superBloco.blockSize;
	tamanhoBloco        = superBloco.blockSize;
	numeroRecords       = tamanhoBlocoBytes/RECORD_SIZE;
	



}

void iniciaBloco(){
	int i;
	blocoAtual  = (char *) malloc (sizeof(char)*SECTOR_SIZE*tamanhoBloco);

}
/*
funcao que inicia no heap(malloc) o veotr que contera o bloco atualmente carregado, deve estar na init(), acessar o bloco pela variavel
global ablocoAtual
*/
void carregaBloco(int i){
	char buffer[SECTOR_SIZE];
	int n,j;
	n = i*tamanhoBloco;
	for(j = 0; j<tamanhoBloco;j++){
		read_sector (n+j, buffer);
		memcpy((void*)&blocoAtual[SECTOR_SIZE*j],            (void*)&buffer[0],SECTOR_SIZE);
	}
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

//retorna uma estrutura inode e le o inode referente ao numero passado como parametro
struct t2fs_inode leInode(int n){
	struct t2fs_inode iNode;
	
	int pos;
	carregaBloco(blocoInodesInicial+((int)n/32));
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
	if(diretorioInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);

		for(i = 0; i < numeroRecords; i++) {
			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_DIRETORIO){
				printf("TypeVal: %X\n",record.TypeVal);
				printf("Name: %s\n",record.name);
				printf("iNodeNumber: %d\n",record.inodeNumber);
		
				printf("----------------------------------------------------------------------\n");
			}
		}
	}
	if(diretorioInode.blocksFileSize > 1){
		int i;
		carregaBloco(diretorioInode.dataPtr[1]);
		if(record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_DIRETORIO){
			for(i = 0; i < numeroRecords; i++) {
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
//funcao que percore os rescords dentro de um bloco e acha por nome de arquivo
struct t2fs_record  findRecords(int bloco,char* partialPath){
	int i;
	struct t2fs_record recordErro;
	struct t2fs_record record;
	recordErro.inodeNumber = INVALID_PTR;
	carregaBloco(bloco);
	for(i = 0; i < numeroRecords; i++) {
		memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
		if((record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_DIRETORIO) && strcmp(partialPath,record.name) == 0){
			return record;
		}
	}
	return recordErro;	

		

}
//funcao utilizada para percorrer bloco de indirecao, chama funcao que percorre os records dentro dos blocos apontados
struct t2fs_record  procuraRecordsIndirecao(int * tamanhoRestante,int blocoIndireto,char * partialPath){
	int i;
	struct t2fs_record record;	
	for(i = 0; i < tamanhoBlocoBytes/sizeof(DWORD);i++){
		carregaBloco(blocoIndireto);
		record =  findRecords(blocoAtual[i*sizeof(DWORD)],partialPath);
		if(record.inodeNumber >  INVALID_PTR )
			return record;
		
		*tamanhoRestante -= sizeof(sizeof(DWORD));

	}



}
//usado para achar em algum caminho(absoluto ou relativo) um arquivo regular, retorna o numero do inode do arquivo
int findFile(struct t2fs_inode diretorioInode,char * nome){
	struct t2fs_record record;
	int i,tamanhoRestante;
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
	if(diretorioInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);
		for(i = 0; i < numeroRecords; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_REGULAR && strcmp(partialPath,record.name) == 0){
				return record.inodeNumber;

			}
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findFile(leInode(record.inodeNumber),nome);
			}
		}
	}
	if(diretorioInode.blocksFileSize > 1){	
		carregaBloco(diretorioInode.dataPtr[1]);
		for(i = 0; i < numeroRecords; i++) {
			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_REGULAR && strcmp(partialPath,record.name) == 0){
				return record.inodeNumber;
			}
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findFile(leInode(record.inodeNumber),nome);
			}
		}
	}
	tamanhoRestante = diretorioInode.bytesFileSize - (2 * tamanhoBlocoBytes); 
	if(diretorioInode.blocksFileSize > 2){
		if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
			record = procuraRecordsIndirecao(&tamanhoRestante,diretorioInode.singleIndPtr,partialPath);
			if(record.TypeVal == TYPEVAL_REGULAR && strcmp(partialPath,record.name) == 0){
				return record.inodeNumber;
			}
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findFile(leInode(record.inodeNumber),nome);
			}
		}		
	}
	return -1;
}

//usado para achar em algum caminho(absoluto ou relativo) um arquivo diretorio, retorna o numero do inode do diretorio
int findDir(struct t2fs_inode diretorioInode,char * nome){
	struct t2fs_record record;
	int i,tamanhoRestante;
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
	printf("\n",record.name);
	if(diretorioInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);
		for(i = 0; i < numeroRecords; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));

			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){

				if(nome[0] == '\0'){
		
					return record.inodeNumber;
				}
				return findDir(leInode(record.inodeNumber),nome);
			}
		}
	}
	if(diretorioInode.blocksFileSize > 1){	
		carregaBloco(diretorioInode.dataPtr[1]);
		for(i = 0; i < numeroRecords; i++) {
			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
				return findDir(leInode(record.inodeNumber),nome);
			}
		}
	}
	tamanhoRestante = diretorioInode.bytesFileSize - (2 * tamanhoBlocoBytes); 
	if(diretorioInode.blocksFileSize > 2){
		if(record.TypeVal == TYPEVAL_DIRETORIO && strcmp(partialPath,record.name) == 0){
			record = procuraRecordsIndirecao(&tamanhoRestante,diretorioInode.singleIndPtr,partialPath);
			if(nome[0] == '\0'){
		
				return record.inodeNumber;
			}
			return findDir(leInode(record.inodeNumber),nome);
		}		
	}
	return -1;
}


/*
Função que retorna o bloco que encontra-se no endereço passado por parametro.
Ex:
bloco = getBloco(iNode.singleIndPtr);
*/
char * getBloco(int i){
char buffer[256];
char * bloco = (char *) malloc (sizeof(char)*1024);
int n;
n = (i*4);
read_sector (n, buffer);
memcpy((void*)&bloco[0], (void*)&buffer[0],256);
read_sector (n+1, buffer);
memcpy((void*)&bloco[SECTOR_SIZE*1],(void*)&buffer[0],256);
read_sector (n+2, buffer);
memcpy((void*)&bloco[SECTOR_SIZE*2],(void*)&buffer[0],256);
read_sector (n+3, buffer);
memcpy((void*)&bloco[SECTOR_SIZE*3],(void*)&buffer[0],256);
return bloco;
}
/*
Função que retorna uma lista do tipo list[iNode.blocksFileSize] contendo
todos os ponteiros diretos que formam o arquivo.
*/
DWORD* getListPointer(struct t2fs_inode iNode){
	//DWORD listBloc[iNode.blocksFileSize];
	DWORD * listBloc = (DWORD *) calloc (iNode.blocksFileSize, sizeof (DWORD));	
	char * bloco = (char *) malloc (sizeof(char)*1024);
	char * blocoInd = (char *) malloc (sizeof(char)*1024);
	int i;
	int contPoint = 0;
	int PointerPerBloc = sizeof(bloco)/sizeof(DWORD); //quantidade de ponteiros que cabe em um bloco
	DWORD * listPointerInd = (DWORD *) calloc (PointerPerBloc, sizeof (DWORD));	


	
	//Blocos endereçados diretamente.
	if (iNode.blocksFileSize == 1){
		listBloc[0] = iNode.dataPtr[0];
		contPoint++;
	}
	if (iNode.blocksFileSize == 2){
		listBloc[1] = iNode.dataPtr[1];
		contPoint++;
	}
	
	//Indireção simples.
	if (iNode.singleIndPtr != INVALID_PTR){
		bloco = getBloco(iNode.singleIndPtr);			
		for (i = 0; i <= PointerPerBloc && contPoint < iNode.blocksFileSize; i++){			
			memcpy((void *)&(listBloc[contPoint]), (void*)&bloco[4*(i)],4);			
			contPoint++;
		}				
	}
	int contBlocInd = 0;
	DWORD blocoDi;
	//Indireção Dupla
	if (iNode.doubleIndPtr != INVALID_PTR){
		//blocoInd é o bloco de indireção nível 1
		blocoInd = getBloco(iNode.doubleIndPtr);	
		while(contPoint < iNode.blocksFileSize){			
			//blocoDi é o endereço do bloco de nível 2 da indireção. Ou seja,
			//o bloco que contem os ponteiros dos blocos de dados
			memcpy((void *)&(blocoDi), (void*)&bloco[4*(contBlocInd)],4);
			contBlocInd++;		
			bloco = getBloco(blocoDi);
			for (i = 0; i <= PointerPerBloc && contPoint < iNode.blocksFileSize; i++){			
				memcpy((void *)&(listBloc[contPoint]), (void*)&bloco[4*(i)],4);			
				contPoint++;
			}
		}
		
	}
	
	
	
	
	for(i = 0; i< iNode.blocksFileSize; i++){
		printf("%d - ", listBloc[i]);
	}
	
	return listBloc;
	
	
}
void initFileHandleList(){
	int i;
	for(i =0;i<10;i++){
		fileHandleList[i].validade = NAO_VALIDO;
	}

}
void initDirHandleList(){
	int i;
	for(i =0;i<10;i++){

		dirHandleList[i].validade = NAO_VALIDO;
	}

}

//faz as inicializacoes da lib, 'e chamada no comeco de todas as funcoes de usuario
void init(){
	if (initFlag == 0){


		carregaSuperBloco();
		iniciaBloco();
		diretorioRaizInode  = leInode(0);
		diretorioAtualInode = leInode(0);
		initFlag = 1;
		initFileHandleList();
		initDirHandleList();
	}
}









/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////FUNCOES DE USUARIO/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FILE2 open2 (char *filename){
	int i,numeroInode;
	numeroInode = findFile(diretorioAtualInode,filename);
	if(numeroInode < 0)
		return -1;
	i = 0;

	while(i<10){
		if(fileHandleList[i].validade == NAO_VALIDO){
			fileHandleList[i].validade = VALIDO;
			fileHandleList[i].seekPtr  = 0;
			fileHandleList[i].inodeNumber    = numeroInode;
			return i;
		}
		i++;	

	}
	return -2;
}
DIR2 opendir2 (char *pathname){
	int i,numeroInode;
	numeroInode = findDir(diretorioAtualInode,pathname);
	if(numeroInode < 0)
		return -1;
	i = 0;
	while(i<10){
		if(dirHandleList[i].validade == NAO_VALIDO){
			dirHandleList[i].validade = VALIDO;
			dirHandleList[i].seekPtr  = 0;
			dirHandleList[i].inodeNumber    = numeroInode;
			return i;
		}
		i++;	

	}
	return -2;


}
int chdir2(char *pathname){
	int numeroInode;
	numeroInode = findDir(diretorioAtualInode,pathname);
	if(numeroInode < 0)
		return -1;

	diretorioAtualInode = leInode(numeroInode);	
	return 0;
}
int read2(FILE2 handle, char *buffer, int size){
	struct t2fs_inode inode;
	int bytesRestantes,blocoInicial,i,j;
	init();
	i = 0;
	
	if(fileHandleList[handle].validade == NAO_VALIDO)
		return -1;
	
	if(fileHandleList[handle].inodeNumber < 0)
		return -2;
	inode =  leInode(fileHandleList[handle].inodeNumber);
	bytesRestantes = inode.bytesFileSize - fileHandleList[handle].seekPtr ;
	blocoInicial = fileHandleList[handle].seekPtr/tamanhoBlocoBytes;
	printf("Seek ptr relativo %d\n",blocoInicial);
	if(blocoInicial == 0){
		if(i < tamanhoBlocoBytes)
			j = fileHandleList[handle].seekPtr % tamanhoBlocoBytes;
		else 
			j = 0;
		printf("Seek ptr relativo %d\n",j);
		carregaBloco(inode.dataPtr[0]);
		for(; j < tamanhoBlocoBytes && i < size && i < bytesRestantes;i++,j++){
			buffer[i] = blocoAtual[j];
		} 
	}
	if(blocoInicial == 1){
		if(i < tamanhoBlocoBytes)
			j = fileHandleList[handle].seekPtr % tamanhoBlocoBytes;
		else 
			j = 0;
		carregaBloco(inode.dataPtr[1]);
		for(j=0; j < tamanhoBlocoBytes && i < size &&  i < bytesRestantes;i++,j++){
			buffer[i] = blocoAtual[j];
		} 
	}
	if (blocoInicial > 1){
		printf("read2 leitura indirecao");
	}
	fileHandleList[handle].seekPtr += i;
	return i;		

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////MAIN PARA TESTES/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
	char path [] = "/dir1/dir11/file111";

	int i,size;
	char buffer [80];
	int loop;
	FILE2 file;
	init();
	size = 80;
	file = open2(path);
	printf("ARQUIVO ABERTO COM HANDLE %d\n",file);
	fileHandleList[0].seekPtr = 1;
	loop = read2(file, buffer, size);
	printf("Tamanho arquivo %d Tamanho Lido %d\n",leInode(6).bytesFileSize,loop);
	for(i = 0; i<loop;i++)
		printf("%c",buffer[i]);
	
	printf("\nFIM EXECUCAO\n");
	
	return 0;
}



	/*
	for (j = 0; j < 5; j++ ){
		if (getBitmap2(0,j) == 1){
			inode = leInode(j);
			printf("inode: %d ---> " , j);
			listBloc = getListPointer(inode);
			printf("fim getListPointer...\n");
			//if (inode.blocksFileSize > 2)
			//	printf("inode: %d", i);
			//printf("inode lido...\n");
			//printInode(leInode(i));
		}			
	}
	*/
	
