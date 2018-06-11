#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 
CC=gcc
LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/
TES_DIR=./teste/
TEST_ARCHIVE=diretorios.c
EXEMP_DIR=./exemplo/
LIB_NAME = libt2fs.a

all:
	$(CC) -c $(SRC_DIR)t2fs.c -o $(LIB_DIR)libt2fs.o 
	ar crs $(LIB_DIR)libt2fs.a $(LIB_DIR)libt2fs.o $(LIB_DIR)apidisk.o  $(LIB_DIR)bitmap2.o
#Compilação de arquivo DENTRO DA PASTA TESTE com a biblioteca libt2fs.a, pela variavel TEST_ARCHIVE.
	#$(CC) -o prog $(TES_DIR)$(TEST_ARCHIVE) $(LIB_DIR)$(LIB_NAME)
#Compilação de arquivo DENTRO DA PASTA EXEMPLO com a libt2fs.2. Descomentar linha abaixo se quiser mostrar os erros que não foram solucionados ainda.
#	$(CC) -o progExemploCechin $(EXEMP_DIR)main.c $(LIB_DIR)$(LIB_NAME)

clean:
	rm -rf $(TES_DIR)*.o
	rm $(LIB_DIR)libt2fs.o
	rm $(LIB_DIR)libt2fs.a
