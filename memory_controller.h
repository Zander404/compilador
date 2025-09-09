#ifndef MEMORY_CONTROLLER_H
#define MEMORY_CONTROLLER_H

#include <stddef.h>
#include <stdio.h>

/* Definir o Tamanho da Memoria (2048 Kb) */
#define MEMORY_SIZE ( 2 * 1024 * 1024)


extern size_t total_allocated_memory;

/* Definir a estrutura própria para o armazenamento das informações em nossa memória */
void* my_malloc(size_t size, const char* file, int line);
void my_free(void* ptr, const char* file, int line);
char* my_strdup(const char* s, const char* file, int line);

#define MALLOC(size) my_malloc(size, __FILE__, __LINE__)
#define FREE(ptr) my_free(ptr, __FILE__, __LINE__)
#define STRDUP(s) my_strdup(s, __FILE__, __LINE__)

char* read_file_and_alloc(const char* filename, size_t* file_size_out);
void print_memory_report();

#endif
