#include "symtab.h"
#include "memory_manager.h" // Para managed_malloc, managed_realloc, managed_free
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função auxiliar interna para duplicar string (usada para nome e dimensões)
static char* managed_strdup_for_symbol(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s);
    char *new_s = (char*)managed_malloc(len + 1);
    // managed_malloc já trata erros fatais e sai do programa se falhar.
    strcpy(new_s, s);
    return new_s;
}

SymbolTable* create_symbol_table(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = 16; // Capacidade inicial padrão
    }
    SymbolTable *table = (SymbolTable*)managed_malloc(sizeof(SymbolTable));
    // if (table == NULL) return NULL; // managed_malloc já trata

    table->entries = (SymbolEntry*)managed_malloc(initial_capacity * sizeof(SymbolEntry));
    // if (table->entries == NULL) { // managed_malloc já trata
    //     managed_free(table);
    //     return NULL;
    // }

    table->size = 0;
    table->capacity = initial_capacity;
    return table;
}

void add_symbol(SymbolTable *table, const char* name, SymbolCategory category, DataType data_type, const char* dimensions, int line_num) {
    if (table == NULL) {
        fprintf(stderr, "Erro: Tentativa de adicionar simbolo a uma tabela NULL.\n");
        return;
    }

    // Redimensionar se necessário
    if (table->size >= table->capacity) {
        size_t new_capacity = table->capacity * 2;
        if (new_capacity == 0) new_capacity = 16;

        SymbolEntry *new_entries_array = (SymbolEntry*)managed_realloc(table->entries, new_capacity * sizeof(SymbolEntry));
        // managed_realloc já trata erros fatais
        table->entries = new_entries_array;
        table->capacity = new_capacity;
    }

    // Adicionar a nova entrada de símbolo
    table->entries[table->size].name = managed_strdup_for_symbol(name);
    table->entries[table->size].category = category;
    table->entries[table->size].data_type = data_type;
    table->entries[table->size].dimensions = managed_strdup_for_symbol(dimensions); // Pode ser NULL
    table->entries[table->size].line_declared = line_num;

    // Inicializar campos futuros (se houver) para NULL ou valores padrão
    // table->entries[table->size].scope = NULL;
    // table->entries[table->size].parameters = NULL;
    // table->entries[table->size].num_parameters = 0;

    table->size++;
}

void print_symbol_table(const SymbolTable *table) {
    if (table == NULL) {
        printf("Tabela de simbolos esta NULL.\n");
        return;
    }
    printf("\n--- TABELA DE SIMBOLOS ---\n");
    printf("------------------------------------------------------------------------------\n");
    printf("| Linha | Categoria | Nome                 | Tipo Dado        | Dimensoes        |\n");
    printf("------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < table->size; ++i) {
        printf("| %-5d | %-9s | %-20s | %-16s | %-16s |\n",
               table->entries[i].line_declared,
               symbol_category_to_string(table->entries[i].category),
               table->entries[i].name ? table->entries[i].name : "N/A",
               data_type_to_string(table->entries[i].data_type),
               table->entries[i].dimensions ? table->entries[i].dimensions : "N/A");
    }
    printf("------------------------------------------------------------------------------\n");
    printf("Total de simbolos: %zu\n\n", table->size);
}

void free_symbol_table(SymbolTable *table) {
    if (table == NULL) {
        return;
    }
    for (size_t i = 0; i < table->size; ++i) {
        if (table->entries[i].name != NULL) {
            managed_free(table->entries[i].name);
        }
        if (table->entries[i].dimensions != NULL) {
            managed_free(table->entries[i].dimensions);
        }
        // Liberar outros campos alocados dinamicamente aqui se houver (ex: scope, parameters)
    }
    if (table->entries != NULL) {
        managed_free(table->entries);
    }
    managed_free(table);
}

const char* symbol_category_to_string(SymbolCategory category) {
    switch (category) {
        case SYM_NONE: return "NONE";
        case SYM_VARIAVEL: return "VARIAVEL";
        case SYM_FUNCAO: return "FUNCAO";
        default: return "DESCONHECIDO";
    }
}

const char* data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_NONE: return "NONE";
        case TYPE_INTEIRO: return "INTEIRO";
        case TYPE_TEXTO: return "TEXTO";
        case TYPE_DECIMAL: return "DECIMAL";
        case TYPE_FUNCAO: return "FUNCAO_DEF"; // Tipo para a entrada da função em si
        case TYPE_VOID: return "VOID"; // Para tipo de retorno de função, se aplicável
        default: return "DESCONHECIDO";
    }
}
