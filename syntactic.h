#ifndef SYNTACTIC_H
#define SYNTACTIC_H

#include "tokens.h"
#include "symbol_table.h"
#include "lexico.h"


#define MAX_VARIABLES 128
extern Variable variable_table[MAX_VARIABLES];
extern int variable_count;


void validate_declaration(TokenList *token_list, VarList *var_list);
VarList* create_var_list();
void add_var_to_list(VarList *list, Variable *var);
void destroy_var_list(VarList *list);
void print_variables(VarList *list); 
#endif
