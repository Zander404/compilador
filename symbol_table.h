#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>
#include "tokens.h"
#include "lexico.h"

/* STRUCT para VAR */
typedef struct {
    char *name;
    TokenType type;
    int line;
    union {
        long int_val;
        double dec_val;
        char *str_val;
    } value;
    int initialized;
} Variable;

/* STRUCT */
typedef struct {
    Variable **vars;
    size_t count;
    size_t capacity;
} VarList;

// Represents a function in the symbol table
typedef struct {
    char *name;
    TokenType return_type;
    VarList *params; // List of parameters
} Function;

// List of functions
typedef struct {
    Function **functions;
    size_t count;
    size_t capacity;
} FunctionList;

// TokenList functions
TokenList* create_token_list();
void add_token_to_list(TokenList *list, Token *token);
void destroy_token_list(TokenList *list);
void destroy_token_list_without_tokens(TokenList *list);
void print_token_list(TokenList *list);
Token* create_new_token(TokenType type, const char* word, int num_line);

// VarList functions
VarList* create_var_list();
void add_var_to_list(VarList *list, Variable *var);
void destroy_var_list(VarList *list);
void print_variables(VarList *list);
Variable *find_variable(VarList *list, const char *name);

// FunctionList functions
FunctionList* create_function_list();
void add_function_to_list(FunctionList *list, Function *func);
Function* find_function(FunctionList *list, const char *name);
void destroy_function_list(FunctionList *list);
const char* token_type_to_string_name(TokenType type);

#endif
