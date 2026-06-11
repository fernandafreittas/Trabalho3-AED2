# Nome do executável final
EXEC = trabalho3.exe

# Compilador e flags de compilação
CC = gcc
CFLAGS = -Wall -g

# Todos os arquivos fonte que precisam ser compilados juntos
SRC = Main/main.c TADS/arvoreAVL.c TADS/tabelaHash.c TADS/registro.c

# Regra principal (o que roda quando você digita apenas 'make')
all: $(EXEC)

# Como construir o executável
$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

# Regra para limpar a "sujeira" gerada (digite 'make clean')
clean:
	del $(EXEC) banco_dados.bin dados.bin 2>nul || rm -f $(EXEC) banco_dados.bin dados.bin