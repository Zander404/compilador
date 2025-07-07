#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para NULL, embora stddef.h seja mais comum para size_t

#define DEFAULT_MAX_MEMORY_KB 2048
#define INITIAL_ALLOCATION_CAPACITY 100 // Capacidade inicial para o array de registros

// Estrutura interna para rastrear uma alocação individual
typedef struct {
    void *ptr;
    size_t size;
    // Adicionar char file[256] e int line; aqui se quiser rastrear origem da alocação
} AllocationRecord;

// Variáveis estáticas para o gerenciador de memória
static AllocationRecord *allocation_records = NULL; // Array dinâmico de registros
static size_t allocation_count = 0;          // Número de alocações ativas
static size_t allocation_capacity = 0;       // Capacidade atual do array allocation_records
static size_t current_memory_usage = 0;      // Memória total atualmente alocada em bytes
static size_t peak_memory_usage = 0;         // Pico de memória utilizada em bytes
static size_t max_memory_bytes = 0;          // Limite máximo de memória em bytes

// Função auxiliar para redimensionar o array de registros de alocação se necessário
static int resize_allocation_records_if_needed() {
    if (allocation_count >= allocation_capacity) {
        size_t new_capacity = (allocation_capacity == 0) ? INITIAL_ALLOCATION_CAPACITY : allocation_capacity * 2;
        AllocationRecord *new_records = (AllocationRecord *)realloc(allocation_records, new_capacity * sizeof(AllocationRecord));

        if (new_records == NULL) {
            // Falha crítica: não foi possível nem alocar memória para rastreamento.
            // Usar fprintf para stderr, pois printf pode usar malloc internamente.
            fprintf(stderr, "ERRO FATAL: Falha ao realocar registros de memoria interna do gerenciador.\n");
            // Não chamar cleanup_memory_manager() aqui para evitar recursão se realloc falhar dentro dele.
            // Apenas sair. O SO limpará a memória do processo.
            exit(EXIT_FAILURE);
        }
        allocation_records = new_records;
        allocation_capacity = new_capacity;
    }
    return 0; // Sucesso
}

// Função auxiliar para encontrar o índice de um ponteiro nos registros
// Retorna -1 se não encontrado.
static int find_allocation_record_index(void *ptr) {
    for (size_t i = 0; i < allocation_count; ++i) {
        if (allocation_records[i].ptr == ptr) {
            return (int)i;
        }
    }
    return -1;
}

void init_memory_manager(size_t max_memory_kb) {
    if (max_memory_kb == 0) {
        max_memory_bytes = DEFAULT_MAX_MEMORY_KB * 1024;
    } else {
        max_memory_bytes = max_memory_kb * 1024;
    }

    allocation_records = (AllocationRecord *)malloc(INITIAL_ALLOCATION_CAPACITY * sizeof(AllocationRecord));
    if (allocation_records == NULL) {
        fprintf(stderr, "ERRO FATAL: Falha ao inicializar gerenciador de memoria (registros de alocacao).\n");
        exit(EXIT_FAILURE);
    }
    allocation_capacity = INITIAL_ALLOCATION_CAPACITY;
    allocation_count = 0;
    current_memory_usage = 0;
    peak_memory_usage = 0;
    // printf("Memory manager initialized. Limit: %zu KB\n", max_memory_bytes / 1024);
}

void* managed_malloc(size_t size) {
    if (allocation_records == NULL) {
        fprintf(stderr, "ERRO: Gerenciador de memoria nao inicializado. Chame init_memory_manager() primeiro.\n");
        exit(EXIT_FAILURE);
    }
    if (size == 0) return NULL; // Comportamento padrão do malloc

    if (current_memory_usage + size > max_memory_bytes) {
        fprintf(stderr, "ERRO Memoria Insuficiente: Tentativa de alocar %zu bytes. Limite maximo de %zu bytes seria excedido.\n", size, max_memory_bytes);
        // Não chamar cleanup_memory_manager() aqui, pois pode levar a loop se ele mesmo usar managed_malloc/free.
        // Apenas reportar e sair.
        report_memory_usage(); // Mostra o estado antes de sair
        exit(EXIT_FAILURE);
    }

    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "ERRO Memoria Insuficiente: malloc retornou NULL para %zu bytes.\n", size);
        report_memory_usage();
        exit(EXIT_FAILURE);
    }

    if (resize_allocation_records_if_needed() != 0) { // Se falhar, já deu exit
        free(ptr); // Libera o que foi alocado por malloc se o rastreamento falhar
        exit(EXIT_FAILURE); // resize_allocation_records_if_needed já imprimiu erro e saiu
    }

    allocation_records[allocation_count].ptr = ptr;
    allocation_records[allocation_count].size = size;
    allocation_count++;

    current_memory_usage += size;
    if (current_memory_usage > peak_memory_usage) {
        peak_memory_usage = current_memory_usage;
    }

    // Alerta de 90-99%
    if (current_memory_usage >= (size_t)(max_memory_bytes * 0.9) && current_memory_usage <= (size_t)(max_memory_bytes * 0.99) ) {
        // Usar stdout para alertas, stderr para erros
        printf("ALERTA: Memoria utilizada (%.2f KB) entre 90%% e 99%% do total disponivel (%.2f KB).\n",
               (double)current_memory_usage / 1024.0, (double)max_memory_bytes / 1024.0);
    }
     // Verificação de estouro de 100% (embora a primeira verificação de limite devesse pegar)
    if (current_memory_usage > max_memory_bytes) {
        fprintf(stderr, "ERRO Memoria Insuficiente: Uso de memoria (%zu bytes) excedeu o limite (%zu bytes) apos alocacao.\n", current_memory_usage, max_memory_bytes);
        report_memory_usage();
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* managed_realloc(void* ptr, size_t new_size) {
    if (allocation_records == NULL) {
        fprintf(stderr, "ERRO: Gerenciador de memoria nao inicializado antes de managed_realloc.\n");
        exit(EXIT_FAILURE);
    }

    if (ptr == NULL) { // realloc(NULL, size) é igual a malloc(size)
        return managed_malloc(new_size);
    }

    if (new_size == 0) { // realloc(ptr, 0) é igual a free(ptr)
        managed_free(ptr);
        return NULL;
    }

    int record_index = find_allocation_record_index(ptr);
    if (record_index == -1) {
        fprintf(stderr, "ERRO: Tentativa de realocar ponteiro %p nao gerenciado ou ja liberado.\n", ptr);
        // Não podemos continuar com segurança.
        exit(EXIT_FAILURE);
    }

    size_t old_size = allocation_records[record_index].size;
    long memory_diff = (long)new_size - (long)old_size;

    if (memory_diff > 0 && (current_memory_usage + memory_diff > max_memory_bytes)) {
        fprintf(stderr, "ERRO Memoria Insuficiente: Tentativa de realocar para %zu bytes (de %zu). Limite maximo de %zu bytes seria excedido.\n", new_size, old_size, max_memory_bytes);
        report_memory_usage();
        exit(EXIT_FAILURE);
    }

    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) { // new_size > 0, então falha de realloc
        fprintf(stderr, "ERRO Memoria Insuficiente: realloc retornou NULL ao tentar realocar de %zu para %zu bytes.\n", old_size, new_size);
        // O bloco original ptr NÃO é liberado por realloc se falhar.
        // Não chamar cleanup_memory_manager() aqui, pois o estado do bloco original pode ser importante.
        report_memory_usage();
        exit(EXIT_FAILURE);
    }

    allocation_records[record_index].ptr = new_ptr;
    allocation_records[record_index].size = new_size;

    current_memory_usage = current_memory_usage - old_size + new_size;
    if (current_memory_usage > peak_memory_usage) {
        peak_memory_usage = current_memory_usage;
    }

    if (current_memory_usage >= (size_t)(max_memory_bytes * 0.9) && current_memory_usage <= (size_t)(max_memory_bytes * 0.99)) {
        printf("ALERTA: Memoria utilizada (%.2f KB) entre 90%% e 99%% do total disponivel (%.2f KB).\n",
               (double)current_memory_usage / 1024.0, (double)max_memory_bytes / 1024.0);
    }
    if (current_memory_usage > max_memory_bytes) {
        fprintf(stderr, "ERRO Memoria Insuficiente: Uso de memoria (%zu bytes) excedeu o limite (%zu bytes) apos realocacao.\n", current_memory_usage, max_memory_bytes);
        report_memory_usage();
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

void managed_free(void* ptr) {
    if (ptr == NULL) return; // free(NULL) é seguro
    if (allocation_records == NULL) {
        // fprintf(stderr, "AVISO: managed_free chamado com gerenciador nao inicializado ou apos cleanup para ptr %p.\n", ptr);
        // Se o gerenciador não está ativo, apenas chama free normal.
        free(ptr);
        return;
    }

    int record_index = find_allocation_record_index(ptr);
    if (record_index == -1) {
        // Tentativa de liberar ponteiro não rastreado. Pode ser um erro no código cliente.
        // Por segurança, não tentamos liberar para evitar double-free de memória não gerenciada.
        // fprintf(stderr, "AVISO: Tentativa de liberar ponteiro %p nao rastreado por managed_free.\n", ptr);
        return;
    }

    current_memory_usage -= allocation_records[record_index].size;
    free(allocation_records[record_index].ptr);

    // Remove o registro do array trocando com o último e diminuindo o contador
    if (allocation_count > 0) { // Segurança extra
        allocation_records[record_index] = allocation_records[allocation_count - 1];
        allocation_count--;
    }
    // Opcional: reduzir a capacidade do array de registros se estiver muito vazio (shrink-to-fit)
}

void report_memory_usage(void) {
    printf("\n========================================\n");
    printf("RELATORIO DE USO DE MEMORIA:\n");
    printf("  Limite Maximo de Memoria: %.2f KB\n", (double)max_memory_bytes / 1024.0);
    printf("  Pico de Memoria Utilizada: %.2f KB (%.2f %% do limite)\n",
           (double)peak_memory_usage / 1024.0,
           max_memory_bytes > 0 ? ((double)peak_memory_usage / (double)max_memory_bytes) * 100.0 : 0.0);
    printf("  Memoria Atualmente em Uso: %.2f KB\n", (double)current_memory_usage / 1024.0);
    printf("  Numero de Alocacoes Ativas: %zu\n", allocation_count);
    printf("========================================\n");
}

void cleanup_memory_manager(void) {
    //printf("Limpando gerenciador de memoria. %zu alocacoes para liberar.\n", allocation_count);
    for (size_t i = 0; i < allocation_count; ++i) {
        if (allocation_records[i].ptr != NULL) {
            free(allocation_records[i].ptr); // Libera a memória alocada pelo programa
            allocation_records[i].ptr = NULL;
        }
    }

    if (allocation_records != NULL) {
        free(allocation_records); // Libera o array de registros em si
        allocation_records = NULL;
    }

    allocation_count = 0;
    allocation_capacity = 0;
    current_memory_usage = 0;
    // peak_memory_usage é mantido para o relatório final, não é resetado aqui.
    // printf("Gerenciador de memoria limpo.\n");
}
