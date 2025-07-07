#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h> // Para size_t

// Estrutura para rastrear uma alocação individual
// Não será exposta no header, apenas usada internamente em memory_manager.c
// typedef struct {
//     void *ptr;
//     size_t size;
// } AllocationRecord; // Movida para .c pois é detalhe de implementação

// Interface do Gerenciador de Memória
void init_memory_manager(size_t max_memory_kb);
void* managed_malloc(size_t size);
void* managed_realloc(void* ptr, size_t new_size);
void managed_free(void* ptr);
void report_memory_usage(void);
void cleanup_memory_manager(void); // Libera todas as alocações rastreadas e recursos do gerenciador

#endif // MEMORY_MANAGER_H
