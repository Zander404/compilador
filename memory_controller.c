#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "memory_controller.h"

/* Arquivo para lidar com o PSEUDO Controle de Memrória */
size_t total_allocated_memory  = 0;
size_t current_memory_in_use =0;
size_t max_memory_in_use = 0;

/* Implementar a estrutura de alocação de memória própria */
void* my_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr != NULL) {
        total_allocated_memory += size;
        current_memory_in_use += size; 
        if (current_memory_in_use > max_memory_in_use) {
            max_memory_in_use = current_memory_in_use;
        }
  
    } else {
        fprintf(stderr, "[MEM ERROR] MALLOC FALHOU: %zu bytes em %s:%d - Memória Insuficiente\n", size, file, line);
    }
    return ptr;
}

/* Implementação para liberar memória, a qual não está sendo usada */
void my_free(void* ptr, const char* file, int line) {
    if (ptr != NULL) {
        free(ptr);
    }
}

/* Copiar Dado para a memória */
char* my_strdup(const char* s, const char* file, int line) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* new_s = (char*)my_malloc(len, file, line);

    if (new_s != NULL) {
        memcpy(new_s, s, len);
    }
    return new_s;
}

/* Printar o USO de memória no terminal */
void print_memory_report() {
    printf("\n--- Relatório de Uso de Memória ---\n");
    printf("Memória Total Alocada (Acumulada): %zu bytes (%.2f KB)  %.2f%%\n",
           total_allocated_memory, ((double)total_allocated_memory / 1024.0), ( (double)total_allocated_memory*100.0)/MEMORY_SIZE);
    printf("Pico Máximo de Memória Utilizada: %zu bytes (%.2f KB)\n",
           max_memory_in_use, (double)max_memory_in_use / (1024.0));
    printf("----------------------------------\n");
}


/* Levar o arquivo do 'PROGRAMA' para a memória */
char* read_file_and_alloc(const char* filename, size_t* file_size_out){
  FILE* file = NULL;
  long file_size = 0;
  char* buffer = NULL;

  file = fopen(filename, "rb");
  
  /* Validar seu o arquivo existe */
  if(file == NULL){
    perror("Erro ao abrir o arquivo do programa");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);

  rewind(file);

  /* Verificar se o arquivo está vazio */
  if(file_size == -1L){
    perror("Erro ao obter o tamanho do arquivo");
    fclose(file);
    return NULL;
  }

  buffer = (char*)MALLOC(file_size + 1);
  if(buffer == NULL){
    fprintf(stderr, "Erro: Falha ao alocar memória para o arquivo '%s'\n", filename);
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, file_size, file);
  if(bytes_read != (size_t)file_size){
    perror("Erro ao ler o arquivo do programa completamente");
    FREE(buffer);
    fclose(file);
    return NULL;
  }

  buffer[file_size] = '\0';
  if(file_size_out != NULL){
    *file_size_out = file_size;
  }

  fclose(file);
  return buffer;
}

