int main(){
	char dir[] = "/AAA";	
	char dir2[] = "/AAA/BBB";	
	char dir3[] = "./CCC";	
	char * buffer;
	int i;
	
	printf("criando um novo diretório (ERRO %d)\n", mkdir2(dir));
	
	printf("Diretório Atual(ERRO %d):", getcwd2 (buffer,4));	
	for (i = 0; i< 4; i++)
		printf("%c", buffer[i]);
	printf("\n");	
	
	printf("Diretório Atual debug %s:\n", currentPathName);		
	
	printf("Alterando diretório atual(ERRO %d)\n", chdir2(dir));
	printf("Diretório Atual(ERRO %d):", getcwd2 (buffer,4));
	for (i = 0; i< 4; i++)
		printf("%c", buffer[i]);
	printf("\n");	
	printf("Diretório Atual debug %s:\n", currentPathName);	

	printf("FIM TESTES\n");
	
}