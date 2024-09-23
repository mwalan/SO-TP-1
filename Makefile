# Diretório dos arquivos de código fonte
SOURCE_DIR = data
# Diretório dos arquivos objeto
OBJECT_DIR = objects

# Nome do compilador C
CC = gcc

# Flags de compilação
CFLAGS = -Wall -g

# Nome do executável
EXECUTABLE = main

# Obter todos os arquivos .c no diretório de origem
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)

# Obter os nomes dos arquivos .o a partir dos nomes dos arquivos .c e ajustar o caminho para o diretório de objetos
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCES))

# Regra padrão: compila o executável
all: $(EXECUTABLE)

# Regra para compilar o executável a partir dos arquivos objeto
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Regra para compilar cada arquivo .c em um arquivo .o no diretório de objetos
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Criar o diretório de objetos se ele não existir
$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

# Regra para limpar todos os arquivos objeto e o executável
clean:
	rm -rf $(OBJECT_DIR) $(EXECUTABLE)

# Regra "phony" para evitar conflitos com arquivos ou diretórios existentes
.PHONY: all clean