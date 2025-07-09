#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "memory_controller.h"


size_t total_allocated_memory  = 0;
size_t total_freed_memory = 0;
size_t current_memory_in_use =0;
size_t max_memory_in_use = 0;


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

void my_free(void* ptr, const char* file, int line) {
    if (ptr != NULL) {
        free(ptr);
    }
}

char* my_strdup(const char* s, const char* file, int line) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* new_s = (char*)my_malloc(len, file, line);

    if (new_s != NULL) {
        memcpy(new_s, s, len);
    }
    return new_s;
}

void print_memory_report() {
    printf("\n--- Relatório de Uso de Memória ---\n");
    printf("Memória Total Alocada (Acumulada): %zu bytes (%.2f KB)  %.2f\%\n",
           total_allocated_memory, ((double)total_allocated_memory / 1024.0), ( (double)total_allocated_memory*100.0)/MEMORY_SIZE);
    printf("Pico Máximo de Memória Utilizada: %zu bytes (%.2f KB)\n",
           max_memory_in_use, (double)max_memory_in_use / (1024.0));
    printf("----------------------------------\n");
}



char* load_file_to_memory(const char* filename, char* buffer, size_t buffer_size){
  FILE* file = NULL;
  long file_size = 0;

  file = fopen(filename, "rb");

  if(file == NULL){
    perror("Erro ao abrir o arquivo do programa");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);

  rewind(file);

  if(file_size == -1L){
    perror("Erro ao obter o tamanho do arquivo");
    fclose(file);
    return NULL;
  }


  if((size_t)file_size >= buffer_size){
    fprintf(stderr, "Erro: Arquivo '%s' (tamanho %ld bytes) excede a pseudo mémoria disponível (%zu bytes). \n", filename, file_size, buffer_size);
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, file_size, file);
  if(bytes_read != (size_t)file_size){
    perror("Erro ao ler o arquivo do programa completamente");
    fclose(file);
    return NULL;
  }

  buffer[file_size] = '\0';
  fclose(file);
  return buffer;

}

