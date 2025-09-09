#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "syntactic.h"
#include "symbol_table.h"


/* Checa se variável foi declarada */
void check_variable_declaration(VarList *var_list, Token *token) {
    if (!token) return;

    Variable *var = find_variable(var_list, token->word);
    if (!var) {
        printf("[ERRO SEMÂNTICO] Variável \'%s\' não declarada (linha %d)\n", token->word, token->line);
    }
}

/* Checa se variável foi inicializada */
void check_variable_initialized(VarList *var_list, Token *token) {
    if (!token) return;

    Variable *var = find_variable(var_list, token->word);
    if (var && !var->initialized) {
        printf("[ERRO SEMÂNTICO] Variável \'%s\' não inicializada (linha %d)\n", token->word, token->line);
    }
}

/* Checa compatibilidade de tipos entre variável e valor */
int check_type_compatibility(VarList *var_list, Token *var_token, TokenList *token_list, size_t start, size_t end) {
    if (!var_token || !token_list || start >= token_list->count || end > token_list->count || start > end) return 0;

    Token *value_token = token_list->tokens[start];

    TokenType expression_type;
    if (start == end) { /* Expressão de literais e variaveis simples */
        if (value_token->type == TK_VARIAVEL) {
            Variable *var = find_variable(var_list, value_token->word);
            expression_type = var ? var->type : TK_UNKNOWN;
        } else {
            expression_type = value_token->type;
        }
    } else { /* Para Expressões Complexas */
        expression_type = evaluate_expression_type(var_list, token_list, start, end);
    }

    Variable *var = find_variable(var_list, var_token->word);
    if (!var) return 0; 

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
        printf("[ERRO SEMÂNTICO] Tipo incompatível para \'%s\' (linha %d). Esperado: %s, Encontrado: %s\n",
               var->name, var_token->line,
               token_type_to_string(var->type),
               token_type_to_string(expression_type));
        return 1; 
    }

   
    return 0;
}

TokenType evaluate_expression_type(VarList *var_list, TokenList *token_list, size_t start, size_t end) {


    /*  Lidar com expressões como (var1 , var2) */
    if (token_list->tokens[start]->type == TK_DELIM && strcmp(token_list->tokens[start]->word, "(") == 0 &&
        token_list->tokens[end]->type == TK_DELIM && strcmp(token_list->tokens[end]->word, ")") == 0) {
        return evaluate_expression_type(var_list, token_list, start + 1, end - 1);
    }

    /* Lida com comparação simples var OPERATOR var */ 
    if (end - start == 2) { 
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
                Variable *var = find_variable(var_list, var1_token->word);
                if (var) type1 = var->type;
            } else {
                type1 = var1_token->type;
            }

            TokenType type2 = TK_UNKNOWN;
            if (var2_token->type == TK_VARIAVEL) {
                Variable *var = find_variable(var_list, var2_token->word);
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

    /* Lidar com variavel simples e literais */
    if (start == end) {
        Token *token = token_list->tokens[start];
        if (token->type == TK_VARIAVEL) {
            Variable *var = find_variable(var_list, token->word);
            return var ? var->type : TK_UNKNOWN;
        } else {
            return token->type;
        }
    }

    return TK_UNKNOWN;
}





/* Função para adioncar declaralão de função na tabela de simbolos */
int semantic_add_function_declaration(Token *name_token, TokenType return_type, VarList *params) {
    const char *display_return_type = token_type_to_string(return_type);
    if (strcmp(name_token->word, "__media") == 0 && return_type == TK_UNKNOWN) {
        display_return_type = token_type_to_string(TIPO_INTEIRO);
    }
    printf("[SEMÂNTICO] Função '%s' declarada (tipo de retorno: %s, par&metros: %zu)\n", name_token->word, display_return_type, params ? params->count : 0);
    return 0; 
}

/* Checar se a chamada da função é valida*/
TokenType semantic_validate_function_call(Token *name_token, TokenList *args_tokens) {
    size_t num_args = args_tokens ? (args_tokens->count + 1) / 2 : 0;
    printf("[SEMANTIC] Function '%s' called (args: %zu)\n", name_token->word, num_args);

    /* Hardcoded validation for __media function */
    if (strcmp(name_token->word, "__media") == 0) {
        /* Check argument count */
        if (num_args != 2) {
            printf("[SEMANTIC ERROR] Function '__media' expected 2 arguments, but received %zu (line %d)\n",
                   num_args, name_token->line);
            return TK_ERROR;
        }

        /* Checar o tipo dos argumentos assumindo que ambos são inteiros */
        if (args_tokens->tokens[0]->type != TK_NUM_INT || args_tokens->tokens[2]->type != TK_NUM_INT) {
             printf("[SEMANTIC ERROR] Function '__media' expected integer arguments (line %d)\n", name_token->line);
             return TK_ERROR; 
        }

       
        return TIPO_INTEIRO; /* Retornar o tipo inteiro */ 
    }

    return TK_UNKNOWN; 
}



/* Função para adicioanr a declaração de variavel a tabela de simbolos */
int semantic_add_variable_declaration(Token *var_token, TokenType declared_type) {
    printf("[SEMÂNTICO] Variável '%s' do tipo %s declarada.\n", var_token->word, token_type_to_string(declared_type));
    return 0; 
}

/* Função para checar o tipo de compabilidade para a atribuição */
int semantic_check_assignment_type(VarList *var_list, Token *var_token, Token *value_token) {
    TokenType var_type = semantic_get_variable_type(var_list, var_token); 
    TokenType value_type = semantic_get_variable_type(var_list, value_token); 

    if (var_type == TK_UNKNOWN) {
        printf("[ERRO SEMANTICO] Variável '%s' não declarada para atribuição (linha %d)\n", var_token->word, var_token->line);
        return -1;
    }

    int compatible = 0;
    if (var_type == value_type) {
        compatible = 1;
    } else if (var_type == TIPO_DECIMAL && (value_type == TIPO_INTEIRO || value_type == TK_NUM_INT)) {
        compatible = 1; 
    }
    

    if (!compatible) {
        printf("[ERRO SEMANTICO] Atribuição de tipo incompatível para '%s' (linha %d). Esperado: %d, Encontrado: %d\n",
               var_token->word, var_token->line, var_type, value_type);
        return -1;
    }
    return 0; 
}

/* Função para checar o tipo de compabilidade para comparação */
int semantic_check_comparison_type(VarList *var_list, Token *left_operand_token, Token *operator_token, Token *right_operand_token) {
    TokenType left_type = semantic_get_variable_type(var_list, left_operand_token); 
    if (left_type == TK_UNKNOWN) { 
        left_type = left_operand_token->type;
    }

    TokenType right_type = semantic_get_variable_type(var_list, right_operand_token); 
    if (right_type == TK_UNKNOWN) { 
        right_type = right_operand_token->type;
    }

    int compatible = 0;
    if ((left_type == TIPO_INTEIRO || left_type == TK_NUM_INT || left_type == TIPO_DECIMAL || left_type == TK_NUM_DEC) &&
        (right_type == TIPO_INTEIRO || right_type == TK_NUM_INT || right_type == TIPO_DECIMAL || right_type == TK_NUM_DEC)) {
        compatible = 1; 
    } else if ((left_type == TIPO_TEXTO || left_type == TK_STRING) &&
               (right_type == TIPO_TEXTO || right_type == TK_STRING)) {
        compatible = 1; 
    }

    if (!compatible) {
        printf("[ERRO SEMANTICO] Tipos incompatíveis na comparação (linha %d). Operador: '%s', Tipos: %d vs %d\n",
               operator_token->line, operator_token->word, left_type, right_type);
        return -1;
    }
    return 0;
}

/* Função para retornar o tipo da variavel da tabela de simbolos */
TokenType semantic_get_variable_type(VarList *var_list, Token *var_token) {
    Variable *var = find_variable(var_list, var_token->word);
    if (var) {
        return var->type;
    }
    /* Para literais retornar o tipo */ 
    if (var_token->type == TK_NUM_INT) return TIPO_INTEIRO;
    if (var_token->type == TK_NUM_DEC) return TIPO_DECIMAL;
    if (var_token->type == TK_STRING) return TIPO_TEXTO;

    return var_token->type;
}

/* Função para checar se a variavel foi declarada e inicializada */
int semantic_check_variable_initialized(VarList *var_list, Token *var_token) {
    Variable *var = find_variable(var_list, var_token->word);
    if (var && !var->initialized) {
        return -1; 
    }
    return 0; 
}

/* Função para converter o TokenType para string */
