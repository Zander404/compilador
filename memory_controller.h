#ifndef MEMORY_CONTROLLER_H
#define MEMORY_CONTROLLER_H

#include <stddef.h>
#include <stdio.h>

#define MEMORY_SIZE ( 2 * 1024 * 1024)


extern size_t total_allocated_memory;
extern size_t total_freed_memory;

void* my_malloc(size_t size, const char* file, int line);
void my_free(void* ptr, const char* file, int line);
char* my_strdup(const char* s, const char* file, int line);

#define MALLOC(size) my_malloc(size, __FILE__, __LINE__)
#define FREE(ptr) my_free(ptr, __FILE__, __LINE__)
#define STRDUP(s) my_strdup(s, __FILE__, __LINE__)

char* load_file_to_memory(const char* filename, char* buffer, size_t buffer_size);
void print_memory_report();

#endif
