#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tokens.h"
#include "lexico.h"
#include "memory_controller.h"
#include "syntactic.h"

// Funções de validação semântica
void check_variable_declaration(VarList *var_list, Token *token);
void check_variable_initialized(VarList *var_list, Token *token);
int check_type_compatibility(VarList *var_list, Token *var_token, TokenList *token_list, size_t start, size_t end);
void run_semantic_analysis(TokenList *token_list, VarList *var_list);
TokenType evaluate_expression_type(VarList *var_list, TokenList *token_list, size_t start, size_t end);

// Represents information about an expression or variable for type checking
typedef struct {
    TokenType type; // The resolved type of the expression/variable
    // Add more fields as needed, e.g., value, pointer to symbol table entry
} SemanticInfo;

// Function to add a function declaration to the symbol table
// Returns 0 on success, -1 on error (e.g., duplicate function)
int semantic_add_function_declaration(Token *name_token, TokenType return_type, VarList *params);

// Function to check if a function call is valid
// Checks existence, number of arguments, and argument types
TokenType semantic_validate_function_call(Token *name_token, TokenList *args_tokens);

// Function to add a variable declaration to the symbol table
// Returns 0 on success, -1 on error (e.g., duplicate variable)
int semantic_add_variable_declaration(Token *var_token, TokenType declared_type);

// Function to check type compatibility for assignment
// Returns 0 on success, -1 on error (e.g., type mismatch)
int semantic_check_assignment_type(VarList *var_list, Token *var_token, Token *value_token);

// Function to check type compatibility for comparison
// Returns 0 on success, -1 on error (e.g., incompatible types)
int semantic_check_comparison_type(VarList *var_list, Token *left_operand_token, Token *operator_token, Token *right_operand_token);

// Function to get the type of a variable from the symbol table
// Returns the TokenType of the variable, or TK_UNKNOWN if not found
TokenType semantic_get_variable_type(VarList *var_list, Token *var_token);

// Function to check if a variable is declared and initialized
int semantic_check_variable_initialized(VarList *var_list, Token *var_token);

// Funções auxiliares
Variable *find_variable_in_list(VarList *var_list, const char *name);

#endif
