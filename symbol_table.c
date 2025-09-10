#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "memory_controller.h"


/* --- VarList Functions --- */

VarList* create_var_list() {
    VarList *list = (VarList*)MALLOC(sizeof(VarList));
    if (list == NULL) {
        perror("Erro ao alocar TokenList");
        return NULL;
    }
    list->count = 0;
    list->capacity = 10;
    list->vars = (Variable**)MALLOC(sizeof(Variable*) * list->capacity);
    if (list->vars == NULL) {
        perror("Erro ao alocar array de Variaveis na lista");
        FREE(list);
        return NULL;
    }
    return list;
}

void add_var_to_list(VarList *list, Variable *var) {
    if (list == NULL || var == NULL) return;

    if (list->count == list->capacity) {
        list->capacity *= 2;

        Variable **new_vars = (Variable**)realloc(list->vars, sizeof(Variable*) * list->capacity);
        if (new_vars == NULL) {
            perror("Erro ao realocar lista de variveis");

            FREE(var->name);
            FREE(var);
            return;
        }
        list->vars = new_vars;
    }

    list->vars[list->count++] = var;
}

void destroy_var_list(VarList *list) {
    size_t i;
    if (list == NULL) return;

    for (i = 0; i < list->count; i++) {
        if (list->vars[i] != NULL) {
            FREE(list->vars[i]->name);
            FREE(list->vars[i]);
        }
    }
    FREE(list->vars);
    FREE(list);
}

void print_variables(VarList *list) {
    size_t i;
    if (list == NULL) {
        printf("Lista de Variaveis vazia ou nula.\n\n");
        return;
    }

    printf("\n--- Lista de Variaveis (%zu variaveis) ---\n", list->count);
    for (i = 0; i < list->count; i++) {
        Variable *t = list->vars[i];
        if (t == NULL) {
            printf("[%3zu] <NULL TOKEN>\n", i);
            continue;
        }
        if (!t->is_valid) {
            continue;
        }

        printf("[%3zu] Linha: %d, Tipo: %s (%s)", i, t->line, token_type_to_string(t->type), t->name);
        
       
        if (t->type == TIPO_INTEIRO) {
            printf(" (Valor Inteiro: %ld)", t->value.int_val);
        } else if (t->type == TIPO_DECIMAL) {
            printf(" (Valor Decimal: %lf)", t->value.dec_val);
        }else if( t->type == TIPO_TEXTO){
            printf(" (Valor Textual: %s)", (char*)t->value.int_val);
        }
        printf("\n");
    }
    printf("-------------------------------------------\n");
}

Variable *find_variable(VarList *list, const char *name) {
    if (!list) return NULL;
    size_t i = 0;
    for (i; i < list->count; i++) {
        if (list->vars[i] && strcmp(list->vars[i]->name, name) == 0) {
            return list->vars[i];
        }
    }
    return NULL; /* não encontrada */
}

/* --- Lista de Funções --- */
FunctionList* create_function_list() {
    FunctionList *list = (FunctionList*)MALLOC(sizeof(FunctionList));
    if (list == NULL) {
        perror("Erro ao alocar FunctionList");
        return NULL;
    }
    list->count = 0;
    list->capacity = 10;
    list->functions = (Function**)MALLOC(sizeof(Function*) * list->capacity);
    if (list->functions == NULL) {
        perror("Erro ao alocar array de Funções na lista");
        FREE(list);
        return NULL;
    }
    return list;
}

void add_function_to_list(FunctionList *list, Function *func) {
    if (list == NULL || func == NULL) return;

    if (list->count == list->capacity) {
        list->capacity *= 2;

        Function **new_funcs = (Function**)realloc(list->functions, sizeof(Function*) * list->capacity);
        if (new_funcs == NULL) {
            perror("Erro ao realocar lista de funções");
            FREE(func->name);
            destroy_var_list(func->params);
            FREE(func);
            return;
        }
        list->functions = new_funcs;
    }

    list->functions[list->count++] = func;
}

Function* find_function(FunctionList *list, const char *name) {
    if (!list) return NULL;
    size_t i = 0; 
    for (i; i < list->count; i++) {
        if (list->functions[i] && strcmp(list->functions[i]->name, name) == 0) {
            return list->functions[i];
        }
    }
    return NULL; /* não encontrada */
}

void destroy_function_list(FunctionList *list) {
    size_t i;
    if (list == NULL) return;

    for (i = 0; i < list->count; i++) {
        if (list->functions[i] != NULL) {
            FREE(list->functions[i]->name);
            destroy_var_list(list->functions[i]->params);
            FREE(list->functions[i]);
        }
    }
    FREE(list->functions);
    FREE(list);
}


