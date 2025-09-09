#ifndef SYNTACTIC_H
#define SYNTACTIC_H

#include "tokens.h"
#include "symbol_table.h"
#include "lexico.h"


#define MAX_VARIABLES 128


void validate_declaration(TokenList *token_list, VarList *var_list);
#endif
