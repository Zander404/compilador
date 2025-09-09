#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tokens.h"
#include "lexico.h"
#include "memory_controller.h"
#include "syntactic.h"

/* Funções de validação semântica */
void check_variable_declaration(VarList *var_list, Token *token);
void check_variable_initialized(VarList *var_list, Token *token);
int check_type_compatibility(VarList *var_list, Token *var_token, TokenList *token_list, size_t start, size_t end);
void run_semantic_analysis(TokenList *token_list, VarList *var_list);
TokenType evaluate_expression_type(VarList *var_list, TokenList *token_list, size_t start, size_t end);

/* Representação da informação sobre a expressão ou variavel para checagem de tipo */
typedef struct {
    TokenType type;
} SemanticInfo;

int semantic_add_function_declaration(Token *name_token, TokenType return_type, VarList *params);
TokenType semantic_validate_function_call(Token *name_token, TokenList *args_tokens);
int semantic_add_variable_declaration(Token *var_token, TokenType declared_type);
int semantic_check_assignment_type(VarList *var_list, Token *var_token, Token *value_token);
int semantic_check_comparison_type(VarList *var_list, Token *left_operand_token, Token *operator_token, Token *right_operand_token);
TokenType semantic_get_variable_type(VarList *var_list, Token *var_token);
int semantic_check_variable_initialized(VarList *var_list, Token *var_token);

/* Funções auxiliares */
Variable *find_variable_in_list(VarList *var_list, const char *name);

const char *token_type_to_string(TokenType type);

#endif
