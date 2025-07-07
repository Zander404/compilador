#include "symtab.h"
#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função auxiliar interna para duplicar string usando memória gerenciada
static char* managed_strdup_for_symtab(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s);
    char *new_s = (char*)managed_malloc(len + 1);
    if (new_s == NULL) { // managed_malloc já encerra em falha, mas para ser explícito
        fprintf(stderr, "Falha fatal de alocacao em managed_strdup_for_symtab\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_s, s);
    return new_s;
}

SymbolTable* create_symbol_table(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = 16;
    }
    SymbolTable *table = (SymbolTable*)managed_malloc(sizeof(SymbolTable));
    // Erro de alocação para 'table' já é tratado por managed_malloc

    table->entries = (SymbolEntry*)managed_malloc(initial_capacity * sizeof(SymbolEntry));
    // Erro de alocação para 'table->entries' já é tratado por managed_malloc

    table->size = 0;
    table->capacity = initial_capacity;
    return table;
}

void add_symbol(SymbolTable *table,
                const char* name,
                SymbolCategory category,
                DataType data_type,
                const char* dimensions,
                int line_num,
                const char* scope) {
    if (table == NULL) {
        fprintf(stderr, "Erro: Tentativa de adicionar simbolo a uma tabela NULL (symtab.c).\n");
        return;
    }

    if (table->size >= table->capacity) {
        size_t new_capacity = table->capacity * 2;
        if (new_capacity == 0) new_capacity = 16;

        SymbolEntry *new_entries_array = (SymbolEntry*)managed_realloc(table->entries, new_capacity * sizeof(SymbolEntry));
        // Erro de realocação já é tratado por managed_realloc
        table->entries = new_entries_array;
        table->capacity = new_capacity;
    }

    table->entries[table->size].name = managed_strdup_for_symtab(name);
    table->entries[table->size].category = category;
    table->entries[table->size].data_type = data_type;
    table->entries[table->size].dimensions = managed_strdup_for_symtab(dimensions);
    table->entries[table->size].line_declared = line_num;
    table->entries[table->size].scope = managed_strdup_for_symtab(scope);

    table->size++;
}

void print_symbol_table(const SymbolTable *table) {
    if (table == NULL) {
        printf("Tabela de simbolos esta NULL.\n");
        return;
    }
    printf("\n--- TABELA DE SIMBOLOS ---\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("| Linha | Categoria | Nome                 | Tipo Dado        | Dimensoes        | Escopo           |\n");
    printf("----------------------------------------------------------------------------------------\n");
    for (size_t i = 0; i < table->size; ++i) {
        printf("| %-5d | %-9s | %-20s | %-16s | %-16s | %-16s |\n",
               table->entries[i].line_declared,
               symbol_category_to_string(table->entries[i].category),
               table->entries[i].name ? table->entries[i].name : "N/A",
               data_type_to_string(table->entries[i].data_type),
               table->entries[i].dimensions ? table->entries[i].dimensions : "N/A",
               table->entries[i].scope ? table->entries[i].scope : "global"); // Default scope
    }
    printf("----------------------------------------------------------------------------------------\n");
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
        if (table->entries[i].scope != NULL) {
            managed_free(table->entries[i].scope);
        }
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
        case TYPE_FUNCAO_DEF: return "FUNCAO_DEF";
        case TYPE_VOID: return "VOID";
        default: return "DESCONHECIDO";
    }
}
