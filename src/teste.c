#include <string.h>
#include <stdio.h>

void caminhoParcial(char * stringTotal, char * stringParcial){
	int i,shift;
	char stringNovoPath[59];
	
	i = 0;

		
	if(stringTotal[0] == '/'){
		i = 1;
		
		
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
		return;
	}
	
	while(stringTotal[i] != '\0'){
		
		stringNovoPath[shift] = stringTotal[i];
		shift++;i++;
	}
	stringNovoPath[shift] = '\0';
	
	strcpy(stringTotal,stringNovoPath);
	
}
int main () {
	char tmp[59];
	char path[] = "/dir1/dir12/dir123/teste";
	caminhoParcial(path, tmp);
	printf("%s\n", path);
	printf("%s\n", tmp);

	caminhoParcial(path, tmp);
	printf("%s\n", path);
	printf("%s\n", tmp);

	caminhoParcial(path, tmp);
	printf("%s\n", path);
	printf("%s\n", tmp);

	caminhoParcial(path, tmp);
	printf("%s\n", path);
	printf("%s\n", tmp);

	
}

