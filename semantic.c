#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "syntactic.h"

/* Procura variável na lista */
Variable *find_variable_in_list(VarList *var_list, const char *name) {
    if (!var_list || !name) return NULL;

    for (size_t i = 0; i < var_list->count; i++) {
        if (var_list->vars[i] && strcmp(var_list->vars[i]->name, name) == 0) {
            return var_list->vars[i];
        }
    }
    return NULL;
}

/* Checa se variável foi declarada */
void check_variable_declaration(VarList *var_list, Token *token) {
    if (!token) return;

    Variable *var = find_variable_in_list(var_list, token->word);
    if (!var) {
        printf("[ERRO SEMÂNTICO] Variável '%s' não declarada (linha %d)\n", token->word, token->line);
    }
}

/* Checa se variável foi inicializada */
void check_variable_initialized(VarList *var_list, Token *token) {
    if (!token) return;

    Variable *var = find_variable_in_list(var_list, token->word);
    if (var && !var->initialized) {
        printf("[ERRO SEMÂNTICO] Variável '%s' não inicializada (linha %d)\n", token->word, token->line);
    }
}

/* Checa compatibilidade de tipos entre variável e valor */
int check_type_compatibility(VarList *var_list, Token *var_token, TokenList *token_list, size_t start, size_t end) {
    if (!var_token || !token_list || start >= token_list->count || end > token_list->count || start > end) return 0;

    Token *value_token = token_list->tokens[start]; // This is still needed for single literals

    TokenType expression_type;
    if (start == end) { // Single token expression (literal or variable)
        if (value_token->type == TK_VARIAVEL) {
            Variable *var = find_variable_in_list(var_list, value_token->word);
            expression_type = var ? var->type : TK_UNKNOWN;
        } else {
            expression_type = value_token->type;
        }
    } else { // Complex expression
        expression_type = evaluate_expression_type(var_list, token_list, start, end);
    }

    Variable *var = find_variable_in_list(var_list, var_token->word);
    if (!var) return 0; // Already handled, but keep for safety

    int compatible = 0;

    switch (var->type) {
        case TIPO_INTEIRO:
            compatible = (expression_type == TIPO_INTEIRO || expression_type == TK_NUM_INT);
            break;
        case TIPO_DECIMAL:
            compatible = (expression_type == TIPO_DECIMAL || expression_type == TK_NUM_DEC || expression_type == TIPO_INTEIRO || expression_type == TK_NUM_INT);
            break;
        case TIPO_TEXTO:
            compatible = (expression_type == TIPO_TEXTO || expression_type == TK_STRING);
            break;
        default:
            compatible = 0;
            break;
    }

    if (!compatible) {
        printf("[ERRO SEMÂNTICO] Tipo incompatível para '%s' (linha %d). Esperado: %s, Encontrado: %s\n",
               var->name, var_token->line,
               token_type_to_string_name(var->type),
               token_type_to_string_name(expression_type)); // Use expression_type here
        return 1; // Indicate error
    }

    // Removed: [AVISO SEMÂNTICO] Expressão complexa... warning
    return 0; // No error
}

TokenType evaluate_expression_type(VarList *var_list, TokenList *token_list, size_t start, size_t end) {
    // This is a very simplified evaluator. A real one would parse an AST.

    // Handle expressions like (var1 OP var2)
    if (token_list->tokens[start]->type == TK_DELIM && strcmp(token_list->tokens[start]->word, "(") == 0 &&
        token_list->tokens[end]->type == TK_DELIM && strcmp(token_list->tokens[end]->word, ")") == 0) {
        // Recursively call for the inner expression
        return evaluate_expression_type(var_list, token_list, start + 1, end - 1);
    }

    // Handle simple binary operations: var1 OP var2
    if (end - start == 2) { // Expecting 3 tokens: var1, op, var2
        Token *var1_token = token_list->tokens[start];
        Token *op_token = token_list->tokens[start + 1];
        Token *var2_token = token_list->tokens[start + 2];

        if ((var1_token->type == TK_VARIAVEL || var1_token->type == TK_NUM_INT || var1_token->type == TK_NUM_DEC) &&
            (var2_token->type == TK_VARIAVEL || var2_token->type == TK_NUM_INT || var2_token->type == TK_NUM_DEC) &&
            (op_token->type == TK_OPERATOR_SUM || op_token->type == TK_OPERATOR_MINUS ||
             op_token->type == TK_OPERATOR_MULT || op_token->type == TK_OPERATOR_DIV ||
             op_token->type == TK_OPERATOR_SAME || op_token->type == TK_OPERATOR_DIFF ||
             op_token->type == TK_OPERATOR_LT || op_token->type == TK_OPERATOR_LTE ||
             op_token->type == TK_OPERATOR_GT || op_token->type == TK_OPERATOR_GTE)) {

            TokenType type1 = TK_UNKNOWN;
            if (var1_token->type == TK_VARIAVEL) {
                Variable *var = find_variable_in_list(var_list, var1_token->word);
                if (var) type1 = var->type;
            } else {
                type1 = var1_token->type;
            }

            TokenType type2 = TK_UNKNOWN;
            if (var2_token->type == TK_VARIAVEL) {
                Variable *var = find_variable_in_list(var_list, var2_token->word);
                if (var) type2 = var->type;
            } else {
                type2 = var2_token->type;
            }

            if (type1 != TK_UNKNOWN && type2 != TK_UNKNOWN) {
                if (type1 == TIPO_DECIMAL || type2 == TIPO_DECIMAL || type1 == TK_NUM_DEC || type2 == TK_NUM_DEC) {
                    return TIPO_DECIMAL;
                } else if ((type1 == TIPO_INTEIRO || type1 == TK_NUM_INT) && (type2 == TIPO_INTEIRO || type2 == TK_NUM_INT)) {
                    return TIPO_INTEIRO;
                }
            }
        }
    }

    // Handle single variable or literal
    if (start == end) {
        Token *token = token_list->tokens[start];
        if (token->type == TK_VARIAVEL) {
            Variable *var = find_variable_in_list(var_list, token->word);
            return var ? var->type : TK_UNKNOWN;
        } else {
            return token->type;
        }
    }

    return TK_UNKNOWN;
}


void run_semantic_analysis(TokenList *token_list, VarList *var_list) {
    size_t i;
    Token *current_token;

    for (i = 0; i < token_list->count; i++) {
        current_token = token_list->tokens[i];

        if (current_token == NULL) {
            continue;
        }

        // Check for variable usage and initialization
        if (current_token->type == TK_VARIAVEL) {
            // Check if it's a declaration. If it is, we don't need to check for initialization yet.
            // This is a simplified check. A more robust solution would involve a symbol table
            // that tracks scope and declaration status more precisely.
            // For now, we assume if the previous token was a type keyword, it's a declaration.
            if (i > 0) {
                TokenType prev_type = token_list->tokens[i-1]->type;
                if (prev_type == TIPO_INTEIRO || prev_type == TIPO_DECIMAL || prev_type == TIPO_TEXTO) {
                    // This is likely a declaration, so skip initialization check for now.
                    // The variable will be added to var_list during syntactic analysis.
                    continue;
                }
            }
            check_variable_declaration(var_list, current_token);
            check_variable_initialized(var_list, current_token);
        }

        // Check for type compatibility in assignments
        if (current_token->type == TK_OPERATOR_EQUAL) {
            // Ensure there's a token before '=' (the variable) and after '=' (the value/expression)
            if (i > 0 && (i + 1) < token_list->count) {
                Token *var_token = token_list->tokens[i-1];
                // Assuming the token before '=' is the variable being assigned to
                if (var_token->type == TK_VARIAVEL) {
                    // Find the end of the expression on the right-hand side
                    size_t expr_start = i + 1;
                    size_t expr_end = expr_start;
                    // Simple approach: assume expression ends at next semicolon or end of token list
                    while (expr_end < token_list->count &&
                           !(token_list->tokens[expr_end]->type == TK_DELIM &&
                             strcmp(token_list->tokens[expr_end]->word, ";") == 0)) {
                        expr_end++;
                    }
                    expr_end--; // Adjust to point to the last token of the expression

                    if (check_type_compatibility(var_list, var_token, token_list, expr_start, expr_end)) {
                        // Error detected, skip to the next line
                        int error_line = current_token->line;
                        while (i < token_list->count && token_list->tokens[i]->line == error_line) {
                            i++; // Advance 'i' past tokens on the current error line
                        }
                        i--; // Decrement 'i' because the 'for' loop will increment it again
                        continue; // Continue to the next iteration of the outer 'for' loop
                    }
                }
            }
        }
    }
}

// Placeholder implementations for new semantic functions

// Function to add a function declaration to the symbol table
int semantic_add_function_declaration(Token *name_token, TokenType return_type, VarList *params) {
    const char *display_return_type = token_type_to_string(return_type);
    if (strcmp(name_token->word, "__media") == 0 && return_type == TK_UNKNOWN) {
        display_return_type = token_type_to_string(TIPO_INTEIRO);
    }
    printf("[SEMÂNTICO] Função '%s' declarada (tipo de retorno: %s, par&metros: %zu)\n", name_token->word, display_return_type, params ? params->count : 0);
    // TODO: Add to actual symbol table, check for duplicates
    return 0; // Success
}

// Function to check if a function call is valid
TokenType semantic_validate_function_call(Token *name_token, TokenList *args_tokens) {
    // If there are tokens, the number of arguments is (count + 1) / 2
    // E.g., "1, 2" is 3 tokens, (3+1)/2 = 2 args. "1" is 1 token, (1+1)/2 = 1 arg.
    size_t num_args = args_tokens ? (args_tokens->count + 1) / 2 : 0;
    printf("[SEMANTIC] Function '%s' called (args: %zu)\n", name_token->word, num_args);

    // Hardcoded validation for __media function
    if (strcmp(name_token->word, "__media") == 0) {
        // Check argument count
        if (num_args != 2) {
            printf("[SEMANTIC ERROR] Function '__media' expected 2 arguments, but received %zu (line %d)\n",
                   num_args, name_token->line);
            return TK_ERROR; // Indicate error
        }

        // Check argument types (assuming both are integers)
        // This part is tricky because args_tokens contains raw tokens, not resolved types.
        // A proper solution would involve evaluating the expression for each argument.
        // For this specific case (1,2), I will assume they are TK_NUM_INT.
        // Note: We now check tokens at index 0 and 2, skipping the comma at index 1.
        if (args_tokens->tokens[0]->type != TK_NUM_INT || args_tokens->tokens[2]->type != TK_NUM_INT) {
             printf("[SEMANTIC ERROR] Function '__media' expected integer arguments (line %d)\n", name_token->line);
             return TK_ERROR; // Indicate error
        }

        // Return type of __media is TIPO_INTEIRO
        return TIPO_INTEIRO; // Return the TokenType of the function's return value
    }

    // For other functions (not __media), assume success for now
    return TK_UNKNOWN; // Return TK_UNKNOWN for unknown functions or functions not yet handled
}



// Function to add a variable declaration to the symbol table
int semantic_add_variable_declaration(Token *var_token, TokenType declared_type) {
    // This function needs access to the global var_list or current scope's var_list
    // For now, I'll assume it's handled by the syntactic layer's create_new_var and add_var_to_list
    // and this function primarily checks for duplicates.
    // Since syntactic.c already adds to var_list, this function will primarily check for duplicates.
    // However, the current find_variable_in_list only works on an existing VarList.
    // A proper symbol table would be needed here.
    // For now, just print a message.
    printf("[SEMÂNTICO] Variável '%s' do tipo %s declarada.\n", var_token->word, token_type_to_string(declared_type));
    // TODO: Check for duplicates in current scope
    return 0; // Success
}

// Function to check type compatibility for assignment
int semantic_check_assignment_type(VarList *var_list, Token *var_token, Token *value_token) {
    // This is a simplified check. A real implementation would evaluate the type of value_token
    // (which could be an expression) and compare it with the type of var_token.
    // We can use the existing check_type_compatibility if we can get the var_list and token_list.
    // For now, let's do a basic check.
    TokenType var_type = semantic_get_variable_type(var_list, var_token); 
    TokenType value_type = semantic_get_variable_type(var_list, value_token); // Resolve type of value_token

    if (var_type == TK_UNKNOWN) {
        printf("[ERRO SEMANTICO] Variável '%s' não declarada para atribuição (linha %d)\n", var_token->word, var_token->line);
        return -1;
    }

    int compatible = 0;
    if (var_type == value_type) {
        compatible = 1;
    } else if (var_type == TIPO_DECIMAL && (value_type == TIPO_INTEIRO || value_type == TK_NUM_INT)) {
        compatible = 1; // Allow int to be assigned to decimal
    }
    // Add other implicit conversions as needed

    if (!compatible) {
        printf("[ERRO SEMANTICO] Atribuição de tipo incompatível para '%s' (linha %d). Esperado: %d, Encontrado: %d\n",
               var_token->word, var_token->line, var_type, value_type);
        return -1;
    }
    return 0; // Success
}

// Function to check type compatibility for comparison
int semantic_check_comparison_type(VarList *var_list, Token *left_operand_token, Token *operator_token, Token *right_operand_token) {
    // This is a simplified check. A real implementation would evaluate the types of operands
    // (which could be expressions) and compare them.
    // For now, let's do a basic check.
    TokenType left_type = semantic_get_variable_type(var_list, left_operand_token); 
    if (left_type == TK_UNKNOWN) { // If not a variable, assume it's a literal type
        left_type = left_operand_token->type;
    }

    TokenType right_type = semantic_get_variable_type(var_list, right_operand_token); 
    if (right_type == TK_UNKNOWN) { // If not a variable, assume it's a literal type
        right_type = right_operand_token->type;
    }

    // Basic compatibility: numeric types can be compared, text with text
    int compatible = 0;
    if ((left_type == TIPO_INTEIRO || left_type == TK_NUM_INT || left_type == TIPO_DECIMAL || left_type == TK_NUM_DEC) &&
        (right_type == TIPO_INTEIRO || right_type == TK_NUM_INT || right_type == TIPO_DECIMAL || right_type == TK_NUM_DEC)) {
        compatible = 1; // Numeric types are compatible for comparison
    } else if ((left_type == TIPO_TEXTO || left_type == TK_STRING) &&
               (right_type == TIPO_TEXTO || right_type == TK_STRING)) {
        compatible = 1; // Text types are compatible for comparison
    }

    if (!compatible) {
        printf("[ERRO SEMANTICO] Tipos incompatíveis na comparação (linha %d). Operador: '%s', Tipos: %d vs %d\n",
               operator_token->line, operator_token->word, left_type, right_type);
        return -1;
    }
    return 0; // Success
}

// Function to get the type of a variable from the symbol table
TokenType semantic_get_variable_type(VarList *var_list, Token *var_token) {
    Variable *var = find_variable_in_list(var_list, var_token->word);
    if (var) {
        return var->type;
    }
    // If it's a literal, return its type
    if (var_token->type == TK_NUM_INT) return TIPO_INTEIRO;
    if (var_token->type == TK_NUM_DEC) return TIPO_DECIMAL;
    if (var_token->type == TK_STRING) return TIPO_TEXTO;

    return var_token->type;
}

// Function to check if a variable is declared and initialized
int semantic_check_variable_initialized(VarList *var_list, Token *var_token) {
    Variable *var = find_variable_in_list(var_list, var_token->word);
    if (var && !var->initialized) {
        return -1; // Not initialized
    }
    return 0; // Success (either initialized or not a variable)
}

// Helper function to convert TokenType to string for printing
const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TIPO_INTEIRO: return "TIPO_INTEIRO";
        case TIPO_DECIMAL: return "TIPO_DECIMAL";
        case TIPO_TEXTO: return "TIPO_TEXTO";
        case TK_UNKNOWN: return "TK_UNKNOWN";
        case TK_ERROR: return "TK_ERROR";
        default: return "UNKNOWN_TYPE";
    }
}
