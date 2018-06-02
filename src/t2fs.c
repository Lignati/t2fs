/*bernardo to do
algumas indirecoes
criar funcao que cria os blocos de records com tipo invalido
getcwd2
-----------------------------------------------------------------------
cleiton to do 
 write2 -> 
-----------------------------------------------------------------------
unsigned tasks(por favor alguem revisa essa lista pra ver se nao sobrou nada)
rmdir2
closedir2
rmdir2
delete2
close 2
indentify2 :P
*/

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
int tamanhoBloco; //quantidade de setores por bloco
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

/*funcao que inicia no heap(malloc) o veotr que contera o bloco atualmente carregado, deve estar na init(), acessar o bloco pela variavel
global ablocoAtual*/
void iniciaBloco(){
	int i;
	blocoAtual  = (char *) malloc (sizeof(char)*SECTOR_SIZE*tamanhoBloco);

}

/*funcao que abstrai a leitura de setores para blocos*/
void carregaBloco(int i){
	char buffer[SECTOR_SIZE];
	int n,j;
	n = i*tamanhoBloco;
	for(j = 0; j<tamanhoBloco;j++){
		read_sector (n+j, buffer);
		memcpy((void*)&blocoAtual[SECTOR_SIZE*j],            (void*)&buffer[0],SECTOR_SIZE);
	}
}

/*funcao que abstrai a escrita de setores para blocos*/
void escreveBloco(int i){
	char buffer[SECTOR_SIZE];
	int n,j;
	n = i*tamanhoBloco;
	for(j = 0; j<tamanhoBloco;j++){
		
		memcpy((void*)&buffer[0], (void*)&blocoAtual[SECTOR_SIZE*j],SECTOR_SIZE);
		write_sector (n+j, buffer);
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
	pos = n*32;

	//printf("leInode: bloco %d ... Pos %d\n", (blocoInodesInicial+((int)n/32)), pos );
	memcpy((void *)&(iNode.blocksFileSize),            (void*)&blocoAtual[pos]    ,4);
	memcpy((void *)&(iNode.bytesFileSize),             (void*)&blocoAtual[pos+4]  ,4);
	memcpy((void *)&(iNode.dataPtr[0]),                (void*)&blocoAtual[pos+8]  ,4);
	memcpy((void *)&(iNode.dataPtr[1]),                (void*)&blocoAtual[pos+12] ,4);
	memcpy((void *)&(iNode.singleIndPtr),              (void*)&blocoAtual[pos+16] ,4);
	memcpy((void *)&(iNode.doubleIndPtr),              (void*)&blocoAtual[pos+20] ,4);
	memcpy((void *)&(iNode.reservado),                 (void*)&blocoAtual[pos+24] ,8);	
		
	return iNode;
	

}
/*grava o Inode passado por parâmetro na posicao informada
Retorno 0 --> Sem erro*/

/* Ex: escreveInode(iNode, fileHandleList[handle].inodeNumber);	*/
int escreveInode(struct t2fs_inode Inode, int posicao){	
	int InodePorBlocos = ((int)tamanhoBlocoBytes/sizeof(Inode));
	int nBloco = ((int)posicao/InodePorBlocos);
	
//	printf("InodeporBloco %d --- nBloco %d\n", InodePorBlocos, nBloco);
//	printf("escreveInode: blocoInicial %d --- bloco %d ... Pos %d\n", blocoInodesInicial, nBloco, (posicao%InodePorBlocos)*sizeof(Inode) );

	carregaBloco(blocoInodesInicial+nBloco);
	memcpy((void*)&blocoAtual[(posicao%InodePorBlocos)*sizeof(Inode)], (void *)&(Inode),sizeof(Inode));
	escreveBloco(blocoInodesInicial+nBloco);
	//printInode(leInode(posicao));
	return 0;		
}


void printInode(struct t2fs_inode iNode){
	printf("\nSize in Blocks %d\n",iNode.blocksFileSize);
	printf("Size in Bytes %d\n",iNode.bytesFileSize);
	printf("Data ptr 1 %u\n",iNode.dataPtr[0]);
	printf("Data ptr 2 %u\n",iNode.dataPtr[1]);
	printf("Ptr Ind Simples %u\n",iNode.singleIndPtr);
	printf("Ptr Ind Dupla %u\n\n",iNode.doubleIndPtr);


}
//funcao de debug que imprime um diretorio de forma human friendly
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


/* Função que retorna o bloco que encontra-se no endereço passado por parametro.
Ex:
bloco = getBloco(iNode.singleIndPtr); */
char * getBloco(int i){
	char buffer[SECTOR_SIZE];
	char * bloco = (char *) malloc (sizeof(char)*tamanhoBlocoBytes);
	int n,j;
	n = i*tamanhoBloco;
	
	for(j = 0; j<tamanhoBloco;j++){
		read_sector (n+j, buffer);
		memcpy((void*)&bloco[SECTOR_SIZE*j],            (void*)&buffer[0],SECTOR_SIZE);
	}	
	return bloco;
}

/* Retorna o endereço direto do N-ésimo bloco do inode */
DWORD getBlocoN(struct t2fs_inode iNode, int blocoN){	
	char * bloco = (char *) malloc (tamanhoBlocoBytes);	
	char * blocoInd = (char *) malloc (tamanhoBlocoBytes);	
	int i, blocoIndirecao;
	int contBloc;	
	int PointerPerBloc = sizeof(bloco)/sizeof(DWORD); //quantidade de ponteiros que cabe em um bloco
	DWORD ender;
	
	//Blocos endereçados diretamente.
	if (blocoN == 0)
		return iNode.dataPtr[0];
	
	if (blocoN == 1)
		return iNode.dataPtr[1];

	contBloc = blocoN - 2;
	if (contBloc <= PointerPerBloc){ //blocoN esta no bloco de indireção simples
		if (iNode.singleIndPtr == INVALID_PTR)
			return -1;
		bloco = getBloco(iNode.singleIndPtr);
		memcpy((void *)&(ender), (void*)&bloco[sizeof(DWORD)*(contBloc)],sizeof(DWORD));	
		return ender;
	}
	else{		
		if (iNode.doubleIndPtr == INVALID_PTR)
			return -1;
		blocoInd = getBloco(iNode.doubleIndPtr);		
		contBloc = contBloc - PointerPerBloc;		
		
		blocoIndirecao = (int)contBloc/PointerPerBloc;
				
		memcpy((void *)&(ender), (void*)&blocoInd[sizeof(DWORD)*(blocoIndirecao)],sizeof(DWORD));	
		blocoInd = getBloco(ender);
		contBloc = contBloc%PointerPerBloc;
		memcpy((void *)&(ender), (void*)&blocoInd[sizeof(DWORD)*(contBloc)],sizeof(DWORD));	
		return ender;
	}
}

//inicia a list de files com valores de nao valido
void initFileHandleList(){
	int i;
	for(i =0;i<10;i++){
		fileHandleList[i].validade = NAO_VALIDO;
	}

}
//inicia a list de handles com valores de nao valido
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

//funcao auxiliar que bytes dentro de arquivos apontados por blocos de indices(indiretos);
void leBytesBloco(int * bytesRestantes, int size, char * buffer,int bloco, int * contador,int byteInicial){
	int j;
	carregaBloco(bloco);
	for(j = 0 ; j < tamanhoBlocoBytes && *contador < size && *contador < *bytesRestantes;*contador++,j++){
		buffer[*contador] = blocoAtual[j];
	} 
}

//funcao auxiliar que le blocos indiretos de arquivos de dados
void readArquivoIndirecao(int ptrIndirecao ,int blocoInicial,int * bytesRestantes, int size, char * buffer,int * contador,int handle){
	int k,bytesLidos,byteInicial;
	struct t2fs_record record;	
	for(k = 0; k < tamanhoBlocoBytes/sizeof(DWORD) && (*contador < *bytesRestantes) ;k++){
		carregaBloco(ptrIndirecao);
		if(blocoInicial == k + 2)
			byteInicial = fileHandleList[handle].seekPtr % tamanhoBlocoBytes;
		else 
			byteInicial = 0;
		 leBytesBloco(bytesRestantes,size,buffer,(k* sizeof(DWORD)),contador,byteInicial);
		

	}

}

//faz o parsing do caminho para craicao de arquivo
// retorna 0 se o path for o diretorio atual ex. file1
//retorn 1 se o path for absoluto
//retorna 2 se o path for apenas o diretorio raiz
//retorn 3 se o path for complexo e relativo
int createFilePathParser(char * filename,char * tempFileName, char * fileLastName){
	int i,j,simplePathFlag;
	i = 0; simplePathFlag = 0;
		
	while(filename[i] != '\0')
		i++;
	
	while(filename[i] != '/' && i>= 0){
		
		i --;
	}
	if(((i == 0) && (filename[i] == '/'))){
		simplePathFlag = 2;
		

	}
	if( i != -1 && !((i == 0) && (filename[i] == '/'))){	
		strcpy(tempFileName,filename);
		tempFileName[i] = '\0';
		simplePathFlag = 3;
		if(filename[0] == '/')
			return 1;
	}

	i++;
	j = 0;
	while(filename[i] != '\0'){
		fileLastName[j] = filename[i];
		i++; j++;
	}
	fileLastName[j] = '\0';
	return simplePathFlag;
}

//acha inode livre a a partir do bitmap, retorna numero do inode
findInodeLivre(){
	int valorBit,i;
	i = -1;
	do{
		i++;
		valorBit = getBitmap2 (BITMAP_INODE,i);
		printf("%d\n",i);
	}while(valorBit == 1);
	printf("retorno bitmap livre %d\n",i);

	return i;



}

//acha bloco livre a a partir do bitmap, retorna numero do inode
int findBlocoLivre(){
	int valorBit,i;
	i = -1;
	do{
		i++;
		valorBit = getBitmap2 (BITMAP_DADOS,i);
		printf("%d\n",i);
	}while(valorBit == 1);
	printf("retorno bitmap livre %d\n",i);

	return i;



}

//funcao auxiliar que aloca um bloco apontado por um ponteiro indireto
int createDataBlockSingleIndir(int numeroBlocoInd, int numeroInternoBloco){
	int numeroNovoBloco;
	numeroNovoBloco = findBlocoLivre();
	if(numeroNovoBloco < 0)
		return -1;
	setBitmap2(BITMAP_DADOS,numeroNovoBloco,1);
	carregaBloco(numeroBlocoInd);	
	memcpy((void*) &(blocoAtual[(numeroInternoBloco+2)*sizeof(DWORD)]),(void*)&numeroNovoBloco,sizeof(DWORD));
	escreveBloco(numeroBlocoInd);
	return numeroNovoBloco;

}
//funcao auxiliar que acha records em blocos indiretos
//retorna numero do bloco com records em branco para serem usados
int singleIndirRecordBlock(int inodeNumber){
	int blocoIndireto;
	int blocoNovoRecord;
	int i;
	blocoIndireto = leInode(inodeNumber).singleIndPtr;
	carregaBloco(blocoIndireto);
	

	for(i = 0; i < tamanhoBlocoBytes/sizeof(DWORD);i++){
			
			if(blocoAtual[i*sizeof(DWORD)] != INVALID_PTR)
				memcpy((void*)&blocoNovoRecord,(void *)&blocoAtual[i*sizeof(DWORD)],sizeof(struct t2fs_record));	
				blocoNovoRecord = findDirEntryInd(blocoNovoRecord) ;
				if(blocoNovoRecord > 0)
					return blocoNovoRecord;
			if(blocoAtual[i*sizeof(DWORD)] == INVALID_PTR){
				blocoNovoRecord = createDataBlockSingleIndir(inodeNumber,(i*sizeof(DWORD)+2));			
				emptyDir(blocoNovoRecord);
				return blocoNovoRecord;
			}	
	}
}
//funcao que aloca um bloco de dados
int createDataBlock(int numeroInode,int numeroInternoBloco){
	int numeroNovoBloco,i;
	int indiceInodeBloco;
	DWORD invalidPtrTemp;
	invalidPtrTemp = INVALID_PTR;
	struct t2fs_inode inode;

	numeroNovoBloco = findBlocoLivre();
	if(numeroNovoBloco < 0)
		return -1;
	setBitmap2(BITMAP_DADOS,numeroNovoBloco,1);


	inode = leInode(numeroInode);
	indiceInodeBloco = numeroInode % (tamanhoBlocoBytes/INODE_SIZE);
	if(numeroInternoBloco == 0){
		inode.dataPtr[0] = numeroNovoBloco;
	}
	if(numeroInternoBloco == 1){
		inode.dataPtr[1] = numeroNovoBloco;
	}
	//criaBlocoDeIndirecoes e inicia ele com ptrs nulos
	if(numeroInternoBloco == 2){
		inode.singleIndPtr = numeroNovoBloco;
		carregaBloco(numeroNovoBloco);
		for(i = 0; i < tamanhoBlocoBytes/sizeof(DWORD);i++){
			memcpy((void*) &(blocoAtual[i*sizeof(DWORD)]),(void*)&invalidPtrTemp,sizeof(DWORD));
			

		}
		escreveBloco(inode.singleIndPtr);
	}
	if(numeroInternoBloco < (tamanhoBlocoBytes/sizeof(struct t2fs_record) + 2) && (numeroInternoBloco >=2))
		createDataBlockSingleIndir(inode.singleIndPtr,numeroInternoBloco);


	//criaBlocoDuplaIndirecao
	if(numeroInternoBloco == (tamanhoBlocoBytes/sizeof(struct t2fs_record) + 2)){
		inode.doubleIndPtr = numeroNovoBloco;
		carregaBloco(numeroInode);
	}

	//recarrega inode na memoria de trabalho	
	leInode(numeroInode);
	memcpy((void*)&blocoAtual[INODE_SIZE * indiceInodeBloco],(void *)&(inode)    ,INODE_SIZE);
	
	escreveBloco(blocoInodesInicial+(((int)numeroInode/(tamanhoBlocoBytes/32))));
	
	

}
//cria uma entrada de diretorio dado o passado dir number 
//TYPEVAL_REGULAR como ultimo argumento para criar files
//TYPEVAL_DIRETORIO como ultimo argumento para criar diretorios
int createDirEntry(int dirInodeNumber,char * fileLastName,int numeroInode,int typeVal){
	struct t2fs_inode diretorioInode;
	struct t2fs_record record;
	int indiceInodeBloco,i;
	int singleIndirFreeRecord;
	diretorioInode = leInode(dirInodeNumber);
	if(diretorioInode.blocksFileSize == 0){
		leInode(dirInodeNumber);
		diretorioInode.blocksFileSize = 1;
		indiceInodeBloco = dirInodeNumber % (tamanhoBlocoBytes/INODE_SIZE);
		memcpy((void *)&(diretorioInode), (void*)&blocoAtual[INODE_SIZE * indiceInodeBloco],INODE_SIZE);
		escreveBloco(blocoInodesInicial+((int)dirInodeNumber/(tamanhoBlocoBytes/32)));
		createDataBlock(dirInodeNumber,0);
		leInode(dirInodeNumber);
		emptyDir(leInode(dirInodeNumber).dataPtr[0]);
		
	}
	if(diretorioInode.blocksFileSize > 0){
		carregaBloco(diretorioInode.dataPtr[0]);
		for(i = 0; i < numeroRecords; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_INVALIDO){
				if(typeVal == TYPEVAL_REGULAR)
					record.TypeVal     = TYPEVAL_REGULAR;
				if(typeVal == TYPEVAL_DIRETORIO)
					record.TypeVal = TYPEVAL_DIRETORIO;
				record.inodeNumber = numeroInode;
				strcpy(record.name,fileLastName);
				memcpy((void *)&blocoAtual[i*64],(void*)&record,sizeof(struct t2fs_record));
				escreveBloco(diretorioInode.dataPtr[0]);
				return 1;

			}
		}
	}
	if(diretorioInode.blocksFileSize == 1){
		leInode(dirInodeNumber);
		diretorioInode.blocksFileSize = 2;
		indiceInodeBloco = dirInodeNumber % (tamanhoBlocoBytes/INODE_SIZE);
		memcpy((void *)&(diretorioInode), (void*)&blocoAtual[INODE_SIZE * indiceInodeBloco],INODE_SIZE);
		escreveBloco(blocoInodesInicial+((int)dirInodeNumber/(tamanhoBlocoBytes/32)));
		createDataBlock(dirInodeNumber,1);
		leInode(dirInodeNumber);
		emptyDir(leInode(dirInodeNumber).dataPtr[1]);
	}
	if(diretorioInode.blocksFileSize > 1){	
		carregaBloco(diretorioInode.dataPtr[1]);
		for(i = 0; i < numeroRecords; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_INVALIDO){
				
				if(typeVal == TYPEVAL_REGULAR)
					record.TypeVal     = TYPEVAL_REGULAR;
				if(typeVal == TYPEVAL_DIRETORIO)
					record.TypeVal = TYPEVAL_DIRETORIO;
				record.inodeNumber = numeroInode;
				strcpy(record.name,fileLastName);
				escreveBloco(diretorioInode.dataPtr[1]);
				return 1;

			}
		}
	}
	if(diretorioInode.blocksFileSize == 2)
		createDataBlock(dirInodeNumber,2);

	if(diretorioInode.blocksFileSize > 2)
		singleIndirFreeRecord = singleIndirRecordBlock(dirInodeNumber);
		carregaBloco(singleIndirFreeRecord);
		for(i = 0; i < numeroRecords; i++) {

			memcpy((void*)&record,(void *)&blocoAtual[i*64],sizeof(struct t2fs_record));
			if(record.TypeVal == TYPEVAL_INVALIDO){
				if(typeVal == TYPEVAL_REGULAR)
					record.TypeVal     = TYPEVAL_REGULAR;
				if(typeVal == TYPEVAL_DIRETORIO)
					record.TypeVal = TYPEVAL_DIRETORIO;
				record.inodeNumber = numeroInode;
				strcpy(record.name,fileLastName);
				memcpy((void *)&blocoAtual[i*64],(void*)&record,sizeof(struct t2fs_record));
				escreveBloco(singleIndirFreeRecord);
				return 1;

			}
		}


	//tamanhoRestante = diretorioInode.bytesFileSize - (2 * tamanhoBlocoBytes); 
	printf("to do dupla  indirecao");
	return -1;
}


void emptyDir(){}
void findDirEntryInd(){}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////FUNCOES DE USUARIO/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//int identify2 (char *name, int size){}
FILE2 create2 (char *filename){

	int numeroInode,indiceInodeBloco,i,j,dirInodeTemp,dirInodeNumber;
	char tempFileName [59];
	char dirName[59];
	char fileLastName[59];
	char thisDir [] = ".";
	init();
	strcpy(tempFileName, filename);
	struct t2fs_inode novoInode;
	//verifica se o nome do arquivo ja existe no disco
	if(findFile(diretorioAtualInode,tempFileName) >= 0)
		return -3;
	if(findDir(diretorioAtualInode,tempFileName) >= 0)
		return -3;


	//to do
	numeroInode = findInodeLivre();
	if(numeroInode < 0)
		return -7;
	setBitmap2 (BITMAP_INODE, numeroInode,1);


	novoInode.blocksFileSize = 0;
	novoInode.bytesFileSize  = 0;	
	novoInode.dataPtr[0]     = INVALID_PTR;
	novoInode.dataPtr[1]     = INVALID_PTR;
	novoInode.singleIndPtr   = INVALID_PTR;
	novoInode.doubleIndPtr   = INVALID_PTR;
	printf("antes de ler lixo");
	//le propositalmente o lixo no local de memoria onde esta no inode para carregar o bloco certo na memoria
	leInode(numeroInode);
	indiceInodeBloco = numeroInode % (tamanhoBlocoBytes/INODE_SIZE);

	memcpy((void*)&blocoAtual[INODE_SIZE * indiceInodeBloco],(void *)&(novoInode)   ,INODE_SIZE);
	
	escreveBloco(blocoInodesInicial+((int)numeroInode/(tamanhoBlocoBytes/32)));
	
	switch(createFilePathParser(filename,dirName,fileLastName)){
		case 0:
			dirInodeNumber = findDir(diretorioAtualInode,thisDir);
			printf("numero do diretorio %d",dirInodeNumber);
			printf("\ncase 0\n");
		break;
		case 1:
			printf("\nnome do path %s, dirName:%s\n",filename,dirName);
			dirInodeNumber = findDir(diretorioRaizInode,dirName);
			printf("\ncase 1\n");
			break;
		case 2:
			printf("\ncase 2\n");
			dirInodeNumber = 0;
		break;
		case 3:
			dirInodeNumber = findDir(diretorioAtualInode,dirName);
		if(dirInodeNumber < 0)
			return -6;

	}
	
	if(createDirEntry(dirInodeNumber, fileLastName, numeroInode,TYPEVAL_REGULAR) < 0)
		return -5;
	return open2(filename);
}


int delete2 (char *filename){
	
	
	
}

FILE2 open2 (char *filename){
	int i,numeroInode;
	init();
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
//int close2 (FILE2 handle){}
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
	//verifica se o arquivo nao 'e vazio
	if(inode.bytesFileSize == 0){

		return 0;
	}





	if(blocoInicial == 0){
		if(i < tamanhoBlocoBytes)
			j = fileHandleList[handle].seekPtr % tamanhoBlocoBytes;
		else 
			j = 0;
	
		if(i<bytesRestantes && i <size) 
			carregaBloco(inode.dataPtr[0]);
		for(; j < tamanhoBlocoBytes && i < size && i < bytesRestantes;i++,j++){
			buffer[i] = blocoAtual[j];
		} 
	}
	if(blocoInicial >= 1){
		if(i < tamanhoBlocoBytes)
			j = fileHandleList[handle].seekPtr % tamanhoBlocoBytes;
		else 
			j = 0;
	
		if(i<bytesRestantes && i <size) 
			carregaBloco(inode.dataPtr[1]);
		for(j=0; j < tamanhoBlocoBytes && i < size &&  i < bytesRestantes;i++,j++){
			buffer[i] = blocoAtual[j];
		} 
	}
	//leitura bytes indirecao
	if (bytesRestantes > 0){
		//read 
		readArquivoIndirecao(inode.singleIndPtr,blocoInicial,&bytesRestantes,size,buffer,&i,handle);
	}
	fileHandleList[handle].seekPtr += i;
	return i;		

}

/*
retorno -1: handle inválido
retorno -2: Inconsistencia de tamanhos
*/
int write2 (FILE2 handle,char *buffer, int size) {
	int addrBloc, posIniWri;
	int blocIni = 0;
	int restSize = size;
	struct t2fs_inode iNode;
	
	init();
	
	if(fileHandleList[handle].validade == NAO_VALIDO)
		return -1;	
	iNode = leInode(fileHandleList[handle].inodeNumber);
	
	if (fileHandleList[handle].seekPtr >= iNode.blocksFileSize * tamanhoBlocoBytes ){
		posIniWri = 0;
		blocIni = iNode.blocksFileSize;
	}
	else{
		posIniWri = fileHandleList[handle].seekPtr;
		while (posIniWri > tamanhoBlocoBytes){
			printf("posIniWri(%d) > tamanhoBlocoBytes(%d)\n", posIniWri,tamanhoBlocoBytes  );
			posIniWri -= tamanhoBlocoBytes;	
			blocIni++;
		}		
	}
	printf("posIniWri: %d\nblocIni: %d\n", posIniWri, blocIni);
	
	while(restSize != 0){
		printf("while --- restSize %d\n", restSize);
		if(blocIni < iNode.blocksFileSize){ //escrevo em blocos já existentes
			printf("bloco existente\n");
			addrBloc = getBlocoN(iNode, blocIni);
			carregaBloco(addrBloc);
		}
		printf("restSize(%d) + posIniWri(%d)) > tamanhoBlocoBytes(%d)\n",restSize,posIniWri, tamanhoBlocoBytes  );
		if((restSize + posIniWri) > tamanhoBlocoBytes){	
			printf("yes\n");
			memcpy((void*)&blocoAtual[posIniWri],(void*)&buffer[0],tamanhoBlocoBytes - posIniWri);
			restSize = restSize - (tamanhoBlocoBytes - posIniWri);
			buffer = &buffer[tamanhoBlocoBytes - posIniWri];
			posIniWri = 0;
		}
		else{
			printf("no\n");
			printf("ultima Escrita\n");
			memcpy((void*)&blocoAtual[posIniWri],(void*)&buffer[0],restSize);
			restSize = 0;
			posIniWri = 0;
		}
				
		if(blocIni < iNode.blocksFileSize){
			escreveBloco(addrBloc);
		}
		else{
			createDataBlock(fileHandleList[handle].inodeNumber, blocIni);
		}	
		printf("restSize: %d\n", restSize);
		blocIni++;				
	}
	
	if(blocIni + 1 > iNode.blocksFileSize)
		iNode.blocksFileSize = blocIni;
	if(fileHandleList[handle].seekPtr + size > iNode.bytesFileSize )
		iNode.bytesFileSize = fileHandleList[handle].seekPtr + size;	
	fileHandleList[handle].seekPtr = fileHandleList[handle].seekPtr + size;
	escreveInode(iNode, fileHandleList[handle].inodeNumber);
	return size;
} 


int truncate2 (FILE2 handle) {
	
	struct t2fs_inode iNode;
	int i;
	iNode = leInode(fileHandleList[handle].inodeNumber);	
	int qtdBlocOld = iNode.blocksFileSize;
	int qtdBlocNew;
	init();
	if((fileHandleList[handle].seekPtr%tamanhoBlocoBytes) == 0)
		qtdBlocNew = fileHandleList[handle].seekPtr/tamanhoBlocoBytes;
	else 
		qtdBlocNew = (fileHandleList[handle].seekPtr/tamanhoBlocoBytes) + 1;
	
	
	for(i = qtdBlocNew; i < qtdBlocOld ; i++ ){
		setBitmap2(1,getBlocoN(iNode, i),0);
	}
	
	iNode.bytesFileSize = fileHandleList[handle].seekPtr;
	iNode.blocksFileSize = qtdBlocNew;
		
	fileHandleList[handle].seekPtr = fileHandleList[handle].seekPtr - 1;	

	escreveInode(iNode, fileHandleList[handle].inodeNumber);	
	//iNode = leInode(fileHandleList[handle].inodeNumber);	
	
}

/*
Return = -1 : FileHandle inválido
Return = -2 : offset resulta em uma posição fora do arquivo
*/
int seek2 (FILE2 handle,unsigned int offset){
	struct t2fs_inode iNode;
	init();
	if(fileHandleList[handle].validade == NAO_VALIDO)
		return -1;
	
	iNode = leInode(fileHandleList[handle].inodeNumber);	
	if(offset == -1){ //posiciona no final do arquivo
		fileHandleList[handle].seekPtr = iNode.bytesFileSize;
		return 0;
	}
	else{
		fileHandleList[handle].seekPtr = offset;
		return 0;		
	}	
}

int mkdir2 (char *pathname) {

	int numeroInode,indiceInodeBloco,i,j,dirInodeTemp,dirInodeNumber;
	char tempFileName [59];
	char dirName[59];
	char fileLastName[59];
	char thisDir [] = ".";
	init();
	strcpy(tempFileName, pathname);
	struct t2fs_inode novoInode;
	//verifica se o nome do arquivo ja existe no disco
	if(findDir(diretorioAtualInode,tempFileName) >= 0)
		return -3;
	if(findFile(diretorioAtualInode,tempFileName) >= 0)
		return -3;

	//to do
	init();
	numeroInode = findInodeLivre();
	if(numeroInode < 0)
		return -7;
	setBitmap2 (BITMAP_INODE, numeroInode,1);


	novoInode.blocksFileSize = 0;
	novoInode.bytesFileSize  = 0;	
	novoInode.dataPtr[0]     = INVALID_PTR;
	novoInode.dataPtr[1]     = INVALID_PTR;
	novoInode.singleIndPtr   = INVALID_PTR;
	novoInode.doubleIndPtr   = INVALID_PTR;
	printf("antes de ler lixo");
	//le propositalmente o lixo no local de memoria onde esta no inode para carregar o bloco certo na memoria
	leInode(numeroInode);
	indiceInodeBloco = numeroInode % (tamanhoBlocoBytes/INODE_SIZE);

	memcpy((void*)&blocoAtual[INODE_SIZE * indiceInodeBloco],(void *)&(novoInode)   ,INODE_SIZE);
	
	escreveBloco(blocoInodesInicial+((int)numeroInode/(tamanhoBlocoBytes/32)));
	
	switch(createFilePathParser(pathname,dirName,fileLastName)){
		case 0:
			dirInodeNumber = findDir(diretorioAtualInode,thisDir);
		break;
		case 1:
			dirInodeNumber = findDir(diretorioRaizInode,dirName);
			break;
		case 2:
			dirInodeNumber = 0;
		break;
		case 3:
			dirInodeNumber = findDir(diretorioAtualInode,dirName);



		if(dirInodeNumber < 0)
			return -6;

	}
	
	if(createDirEntry(dirInodeNumber, fileLastName, numeroInode,TYPEVAL_DIRETORIO) < 0)
		return -5;
	return opendir2(pathname);


}
//int rmdir2 (char *pathname) {}
int chdir2(char *pathname){
	int numeroInode;
	init();
	numeroInode = findDir(diretorioAtualInode,pathname);
	printf("novo path Inode %d\n",numeroInode);
	if(numeroInode < 0)
		return -1;

	diretorioAtualInode = leInode(numeroInode);	
	return 0;
}
//int getcwd2 (char *pathname, int size) {}
DIR2 opendir2 (char *pathname){
	int i,numeroInode;
	init();
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
//int readdir2 (DIR2 handle, DIRENT2 *dentry) {}

//int closedir2 (DIR2 handle) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////MAIN PARA TESTES/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
	char filePath    [] = "cleitonEhUmaFile";
	char novoDirPath [] = "dir1";
	char dirPath     [] = "bernardoEhUmDir";
	char aux1[59];
	char aux2[59];
	int i,size;
	char buffer [10000];
	char buffer3 [10000];
	char buffer2 [] = "_________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________Teste de escrita___________________________________					   ___________________________________________________________________________________________________________________________________________________________________________________________________________________________";
	char dir[80];
	char fileS[80];
	int loop;
	int c;
	int dirHandle;
	FILE2 fileHandle;
	struct t2fs_inode Inode;
	init();


	

	mkdir2(dirPath);

	printf("%d\n",chdir2(novoDirPath));
	create2(filePath);
	readAndPrintDir(diretorioAtualInode);

	
}

	/*
	file = open2(path);
	Inode = leInode(fileHandleList[0].inodeNumber);	
	read2(file, buffer, Inode.bytesFileSize);
	printf("Arquivo lido: %s\n\n", buffer);

	printf("Write -- Erro: %d\n", write2(file, buffer, 5));
	Inode = leInode(fileHandleList[0].inodeNumber);		
	fileHandleList[0].seekPtr = 0;
	
	read2(file, buffer2, Inode.bytesFileSize);
	printf("Arquivo lido: %s\n\n", buffer2);	
	
	printf("FIM\n");
	*/
/*
	//TESTE WRITE
	FILE2 file;
	char path [] = "/file3";
	file = open2(path);
	Inode = leInode(fileHandleList[0].inodeNumber);	
	printf("tam: %d\n", Inode.bytesFileSize);
	read2(file, buffer, Inode.bytesFileSize);
	for(i = 0; i < Inode.bytesFileSize; i++)
		printf("%c", buffer[i]);
	printf("\n");
	
	seek2(0,-1);
	printf("Write -- Erro: %d\n", write2(file, buffer2,16));
	Inode = leInode(fileHandleList[0].inodeNumber);		
	fileHandleList[0].seekPtr = 0;
	read2(file, buffer3, Inode.bytesFileSize);		
	for(i = 0; i < Inode.bytesFileSize; i++)
		printf("%c", buffer3[i]);
	printf("\n");	
	
	printf("tam: %d\n", Inode.bytesFileSize);	
		
	printf("FIM\n");	
*/

	/*
	//TESTE WRITE multiBlocos
	FILE2 file;
	char path [] = "/file3";
	file = open2(path);
	Inode = leInode(fileHandleList[0].inodeNumber);	
	printf("tam: %d\n", Inode.bytesFileSize);
	read2(file, buffer, Inode.bytesFileSize);
	for(i = 0; i < Inode.bytesFileSize; i++)
		printf("%c", buffer[i]);
	printf("\n");
	
	seek2(0,-1);
	printf("Write -- Erro: %d\n", write2(file, buffer2,500));
	Inode = leInode(fileHandleList[0].inodeNumber);		
	fileHandleList[0].seekPtr = 0;
	read2(file, buffer3, Inode.bytesFileSize);		
	for(i = 0; i < Inode.bytesFileSize; i++)
		printf("%c", buffer3[i]);
	printf("\n");	
	
	printf("tam: %d\n", Inode.bytesFileSize);	
		
	printf("FIM\n");
	*/
	
/*
	//teste truncate, seek e escreveInode
	char path [] = "/file3";
	file = open2(path);
	printf("ARQUIVO ABERTO COM HANDLE %d\n",file);
	Inode = leInode(fileHandleList[0].inodeNumber);
		
	loop = read2(file, buffer, Inode.bytesFileSize);	
	printf("Tamanho Lido %d\n",loop);
	printf("Arquivo lido: %s\n", buffer);
	
	fileHandleList[0].seekPtr = 0;	
	seek2 (file,6);
	printf("Seek realizado na pos %d\n", fileHandleList[0].seekPtr);
	printf("truncate -- qtd erros: %d\n", truncate2(file));
	
	fileHandleList[0].seekPtr = 0; //reposiciona no início do arq		
	Inode = leInode(fileHandleList[0].inodeNumber);	
	loop = read2(file, buffer2, Inode.bytesFileSize);
	printf("Tamanho Lido %d\n",loop);	
	printf("Arquivo lido: %s\n", buffer2);
	
	printf("\nFIM EXECUCAO\n");
*/

	
	
	
	/*
	printf("%d\n",create2(path));
	readAndPrintDir(leInode(0));
	printInode(leInode(13));
	return 0;
	*/



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
	
	/*
	createDataBlock(0,2);
	leInode(0);
	carregaBloco((leInode(0).singleIndPtr));
	for(i = 0;i<tamanhoBlocoBytes/sizeof(DWORD);i++)
		printf("%d -> %d\n",blocoAtual[i],i);
	*/
