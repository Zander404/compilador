#ifndef SYNTACTIC_H
#define SYNTACTIC_H

#include "tokens.h"
#include "syntactic.h"


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


#define MAX_VARIABLES 128
extern Variable variable_table[MAX_VARIABLES];
extern int variable_count;


void validate_declaration(TokenList *token_list, VarList *var_list);
VarList* create_var_list();
void add_var_to_list(VarList *list, Variable *var);
void destroy_var_list(VarList *list);
void print_variables(VarList *list); 
#endif
