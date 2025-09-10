#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>
#include "lexico.h"

/* STRUCT para VAR */
#include <stdbool.h>

typedef struct {
    char *name;
    TokenType type;
    int line;
    int initialized; 
    bool is_valid;
    union {
        long long int_val;
        double dec_val;
        char *text_val;
    } value;
} Variable;

/* STRUCT de VarList */
typedef struct {
    Variable **vars;
    size_t count;
    size_t capacity;
} VarList;

/* Struct de Função */
typedef struct {
    char *name;
    TokenType return_type;
    VarList *params; /* Lista de Variaveis */
} Function;

/* Struct para a Lista de Função*/
typedef struct {
    Function **functions;
    size_t count;
    size_t capacity;
} FunctionList;


/* Funções para VarList */
VarList* create_var_list();
void add_var_to_list(VarList *list, Variable *var);
void destroy_var_list(VarList *list);
void print_variables(VarList *list);
Variable *find_variable(VarList *list, const char *name);

/* Função para Lista de FunctionList */ 
FunctionList* create_function_list();
void add_function_to_list(FunctionList *list, Function *func);
Function* find_function(FunctionList *list, const char *name);
void destroy_function_list(FunctionList *list);

#endif
