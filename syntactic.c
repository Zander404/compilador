#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h> 
#include <limits.h>
#include <float.h>


#include "memory_controller.h"
#include "lexico.h"
#include "syntactic.h"
#include "semantic.h" 
#include "ascii_table.h"
#include "tokens.h"
#include "symbol_table.h"



Variable list[MAX_VARIABLES];
int variable_count = 0;

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;   /* tamanho da string + '\0' */
    char *copy = (char *)malloc(len);
    if (copy != NULL) {
        memcpy(copy, s, len);
    }
    return copy;
}



/* --- Funções para Lidar com a Pilha de TOKEN_LIST --- */



static Variable* create_new_var(TokenType type, Token *token, Token *value, int num_line) {
    Variable *var = (Variable*)MALLOC(sizeof(Variable));
    if (var == NULL) {
        perror("Erro ao alocar nova variavel");
        return NULL;
    }

    var->line = num_line;
    var->type = type;
    var->name = STRDUP(token->word);
    var->initialized = 1;

    /* inicialização padrão (caso não tenha "valor") */
    switch (var->type) {
      case TIPO_INTEIRO:
          var->value.int_val = 0;
          break;
      case TIPO_DECIMAL:
          var->value.dec_val = 0.0;
          break;
      case TIPO_TEXTO:
          var->value.str_val = NULL;
          break;
      default:
          printf("[ERRO SEMÂNTICO] Tipo inválido ao criar variável.\n");
          break;
    }

    /* se tem inicialização, sobrescreve */
    if (value) {
        switch (var->type) {
          case TIPO_INTEIRO:
              var->value.int_val = value->value.int_val;
              break;
          case TIPO_DECIMAL:
              var->value.dec_val = value->value.dec_val;
              break;
          case TIPO_TEXTO:
              if (value->value.str_val) {
                  var->value.str_val = value->value.str_val;
              }
              break;
          default:
              break;
        }
       var->initialized = 1;
    }

    if (var->name == NULL) {
        perror("Erro ao alocar Nome para Nova Variable");
        FREE(var);
        return NULL;
    }
  
    return var;
}

/* Imprime tabela de variáveis */

void set_variable_int(VarList *list, const char *name, long value) {
    Variable *var = find_variable(list, name);
    if (!var) {
        printf("[ERRO SEMÂNTICO] Variável '%s' não declarada!\n", name);
        return;
    }
    var->value.int_val = value;
    var->initialized = 1;
}

void set_variable_dec(VarList *list, const char *name, double value) {
    Variable *var = find_variable(list, name);
    if (!var) {
        printf("[ERRO SEMÂNTICO] Variável '%s' não declarada!\n", name);
        return;
    }
    var->value.dec_val = value;
    var->initialized = 1;
}

void set_variable_str(VarList *list, const char *name, const char *value) {
    Variable *var = find_variable(list, name);
    if (!var) {
        printf("[ERRO SEMÂNTICO] Variável '%s' não declarada!\n", name);
        return;
    }
    var->value.str_val = strdup(value);
    var->initialized = 1;
}

void validate_declaration(TokenList *token_list, VarList *var_list){

  int line = -1;
  Variable *var; 
  size_t i = 0;
  Token *t = NULL;
  int se_ativo = 0;  /* marca se existe um SE aberto */



  for(i; i < token_list->count; i++){
    t = token_list->tokens[i];
    if (t == NULL) {
      continue;
    }

    if (t->line != line) {
      line = t->line;
    }

    /* Bloco para Chamada de funções */
    else if (t->type == TK_IDENTIFICADOR) {
        Token *name_token = t;
        Token *open_paren = token_list->tokens[i+1];

        if (open_paren && open_paren->type == TK_DELIM && strcmp(open_paren->word, "(") == 0) {
            size_t j = i + 2;
            TokenList *args_tokens = create_token_list();
            int expecting_arg = 1;

            while (j < token_list->count) {
                Token *arg = token_list->tokens[j];

                if (arg->type == TK_DELIM && strcmp(arg->word, ")") == 0) {
                    j++; 
                    break;
                }

                if (expecting_arg) {
                    add_token_to_list(args_tokens, arg);
                    expecting_arg = 0;
                } else {
                    if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                        expecting_arg = 1;
                    } else {
                        printf("[ERRO SINTATICO] Esperado ',' entre argumentos da função (linha %d)\\n", arg->line);
                        destroy_token_list_without_tokens(args_tokens);
                        return;
                    }
                }
                j++;
            }

            /* Validar Semanticamente a chamada de função */
            TokenType function_return_type = semantic_validate_function_call(name_token, args_tokens);
            destroy_token_list_without_tokens(args_tokens);

            if (function_return_type == TK_ERROR) {
                i = j;
                continue;
            }

            Token *assign_op = token_list->tokens[j];
            if (assign_op && assign_op->type == TK_OPERATOR_EQUAL && strcmp(assign_op->word, "=") == 0) {
                Token *var_to_assign = token_list->tokens[i-1];
                if (var_to_assign && var_to_assign->type == TK_VARIAVEL) {
                    Token *return_value_token = create_new_token(function_return_type, "", name_token->line);
                    if (semantic_check_assignment_type(var_list, var_to_assign, return_value_token) != 0) {
                    }
                    FREE(return_value_token->word);
                    FREE(return_value_token); 
                } else {
                    printf("[ERRO SINTATICO] Esperado variável antes da atribuição de função (linha %d)\n", name_token->line);
                }
                i = j + 1; 
                continue;
            } else {
                i = j;
                continue;
            }
        }
    }
    /* Para Função */
    if (t->type == TK_FUNCAO) {
        Token *name = token_list->tokens[i+1];
        if (!name || name->type != TK_IDENTIFICADOR) {
            printf("[ERRO SINTÁTICO] Nome de função inválido (linha %d)\n", t->line);
            continue;
        }

        Token *open_paren = token_list->tokens[i+2];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO SINTATICO] Esperado '(' após nome da função (linha %d)\n", t->line);
            continue;
        }

        size_t j = i + 3;
        int expecting_param = 1;
        VarList *function_params = create_var_list(); /* Criar uma lista para armazenar os paramentro da função */

        /* valida parâmetros */
        while (j < token_list->count) {
            Token *arg = token_list->tokens[j];

            if (arg->type == TK_DELIM && strcmp(arg->word, ")") == 0) {
                j++;
                break;
            }

            if (expecting_param) {
                if (arg->type == TK_VARIAVEL) {
                    Variable *param_var = create_new_var(TK_UNKNOWN, arg, NULL, arg->line); /* Setar como TK_UNKNOWN como Placeholder */
                    add_var_to_list(function_params, param_var);
                } else {
                    printf("[ERRO SINTATICO] Esperado parâmetro na lista da função (linha %d)\n", arg->line);
                }
                expecting_param = 0;
            } else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expecting_param = 1;
                } else {
                    printf("[ERRO SINTATICO] Esperado ',' entre parâmetros (linha %d)\n", arg->line);
                }
            }
            j++;
        }

        /* Depois de fazer o parsin dos parametros, chamar a função semantica para adionar a declaração de função */ 
        if (semantic_add_function_declaration(name, TK_UNKNOWN, function_params) != 0) {
            printf("[ERRO SEMANTICO] Erro ao declarar função '%s' (linha %d)\n", name->word, name->line);
        }
        destroy_var_list(function_params); /* Limpar a lista temporaria */

        /* Corpo da Função */
        Token *open_brace = token_list->tokens[j];
        if (!open_brace || open_brace->type != TK_DELIM || strcmp(open_brace->word, "{") != 0) {
            printf("[ERRO SINTATICO] Esperado '{' no início do corpo da função (linha %d)\n", t->line);
            continue;
        }

        int found_close = 0;
        j++;
        while (j < token_list->count) {
            if (token_list->tokens[j]->type == TK_DELIM &&
                strcmp(token_list->tokens[j]->word, "}") == 0) {
                found_close = 1;
                j++;
                break;
            }
            j++;
        }

        if (!found_close) {
            printf("[ERRO SINTATICO] Esperado '}' ao final do corpo da função (linha %d)\n", t->line);
        }

        i = j;
    }

    else if (t->type == TIPO_INTEIRO) {
        i++; 
        while (i < token_list->count) {
            Token *varToken = token_list->tokens[i];
            if (!varToken || varToken->type != TK_VARIAVEL) {
                printf("[ERRO] Esperado variável após tipo 'inteiro' (linha %d)\n", t->line);
                break;
            }

            /* Adionar a declaração de varivavel */
            if (semantic_add_variable_declaration(varToken, TIPO_INTEIRO) != 0) {
                printf("[ERRO SEMANTICO] Erro ao declarar variável '%s' (linha %d)\n", varToken->word, varToken->line);
            }

            Token *nextToken = token_list->tokens[i+1];

            if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
                Token *rhs_start_token = token_list->tokens[i+2];

                /* Checar se é a chamada de função */ 
                if (rhs_start_token && rhs_start_token->type == TK_IDENTIFICADOR) {
                    Token *open_paren = token_list->tokens[i+3];
                    if (open_paren && open_paren->type == TK_DELIM && strcmp(open_paren->word, "(") == 0) {
                        Token *name_token = rhs_start_token;
                        size_t func_call_start_idx = i + 2;
                        size_t func_call_end_idx = func_call_start_idx;

                        /* Encontrar o fim da chamada de função  */  
                        int paren_balance = 0;
                        size_t k = func_call_start_idx + 1;
                        while (k < token_list->count) {
                            if (token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, "(") == 0) {
                                paren_balance++;
                            } else if (token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, ")") == 0) {
                                paren_balance--;
                            }
                            if (paren_balance == 0 && token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, ")") == 0) {
                                func_call_end_idx = k;
                                break;
                            }
                            k++;
                        }

                        if (paren_balance != 0) {
                            printf("[ERRO SINTATICO] Parênteses desbalanceados na chamada de função (linha %d)\n", name_token->line);
                            i = k; 
                            continue;
                        }

                        /* Extrair os argumentos para avaliação semantica */  
                        TokenList *args_tokens = create_token_list();
                        size_t arg_start_idx = func_call_start_idx + 2; 
                        size_t arg_end_idx = func_call_end_idx - 1; 
                        
                        size_t arg_idx = arg_start_idx;

                        for (arg_idx; arg_idx <= arg_end_idx; arg_idx++) {
                            add_token_to_list(args_tokens, token_list->tokens[arg_idx]);
                        }

                        /* Validar Semanticamente a chamada de função */
                        TokenType function_return_type = semantic_validate_function_call(name_token, args_tokens);
                        destroy_token_list_without_tokens(args_tokens); /* Limpar a lista Temporaria */

                        if (function_return_type == TK_ERROR) {
                            i = func_call_end_idx;
                            continue;
                        }

                        /* Criar variavel com inicialização */
                        var = create_new_var(TIPO_INTEIRO, varToken, NULL, t->line);
                        add_var_to_list(var_list, var);

                        Token *dummy_return_type_token = create_new_token(function_return_type, "", name_token->line);
                        if (semantic_check_assignment_type(var_list, varToken, dummy_return_type_token) != 0) {
                        }
                        FREE(dummy_return_type_token->word);
                        FREE(dummy_return_type_token);

                        i = func_call_end_idx + 1;
                        if (i < token_list->count && token_list->tokens[i]->type == TK_DELIM && strcmp(token_list->tokens[i]->word, ";") == 0) {
                            break; 
                        }
                        continue;
                    }
                }

                /* Logica para adiconar token de Inteiro */ 
                Token *valueToken = token_list->tokens[i+2];
                if (valueToken && valueToken->type == TK_NUM_INT) {
                    var = create_new_var(TIPO_INTEIRO, varToken, valueToken, t->line);
                    add_var_to_list(var_list, var);

                    /* Semantica: Checar o tipo de atribuição de tipo */ 
                    if (semantic_check_assignment_type(var_list, varToken, valueToken) != 0) {
                        printf("[ERRO SEMANTICO] Atribuição de tipo incompatível para '%s' (linha %d)\n", varToken->word, varToken->line);
                    }

                    i += 3;
                } else {
                    printf("[ERRO] Esperado número inteiro após '=' (linha %d)\n", t->line);
                    i += 2;
                }
            } else {

                var = create_new_var(TIPO_INTEIRO, varToken, NULL, t->line);
                add_var_to_list(var_list, var);

                i++;
            }

            /* Verificar para virgula ou ponto e virgula */   
            Token *sep = token_list->tokens[i];
            if (sep && sep->type == TK_DELIM && strcmp(sep->word, ",") == 0) {
                i++;
                continue;
            } else if (sep && sep->type == TK_DELIM && strcmp(sep->word, ";") == 0) {
                break;
            } else {
                printf("[ERRO] Esperado ',' ou ';' após declaração (linha %d)\n", t->line);
                break;
            }
        }
        continue;
    }

    /* Validator de Decimal */
    else if (t->type == TIPO_DECIMAL) {
        Token *varToken = token_list->tokens[i+1];
        if (varToken && varToken->type == TK_VARIAVEL) {
            /* Semantic: Adicionar o token a lista */
            if (semantic_add_variable_declaration(varToken, TIPO_DECIMAL) != 0) {
                printf("[ERRO SEMANTICO] Erro ao declarar variável \'%s\' (linha %d)\
", varToken->word, varToken->line);
            }
            Token *nextToken = token_list->tokens[i+2];

            /* Caso simples: apenas "decimal !x;" */
            if (nextToken && nextToken->type == TK_DELIM && strcmp(nextToken->word, ";") == 0) {
                var = create_new_var(TIPO_DECIMAL, varToken, NULL, t->line);
                add_var_to_list(var_list, var);

                i += 2;
                continue;
            }
            /* Bloco para atribuição de valor */ 
            else if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
                Token *rhs_start_token = token_list->tokens[i+2];

                if (rhs_start_token && rhs_start_token->type == TK_IDENTIFICADOR) {
                    Token *open_paren = token_list->tokens[i+3];
                    if (open_paren && open_paren->type == TK_DELIM && strcmp(open_paren->word, "(") == 0) {
                        Token *name_token = rhs_start_token;
                        size_t func_call_start_idx = i + 2;
                        size_t func_call_end_idx = func_call_start_idx;

                        int paren_balance = 0;
                        size_t k = func_call_start_idx + 1;
                        while (k < token_list->count) {
                            if (token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, "(") == 0) {
                                paren_balance++;
                            } else if (token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, ")") == 0) {
                                paren_balance--;
                            }
                            if (paren_balance == 0 && token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, ")") == 0) {
                                func_call_end_idx = k;
                                break;
                            }
                            k++;
                        }

                        if (paren_balance != 0) {
                            printf("[ERRO SINTATICO] Parênteses desbalanceados na chamada de função (linha %d)\n", name_token->line);
                            i = k;
                            continue;
                        }

                        /* Extrair argumetos para validação_semantica_de_chamada_de_função */   
                        TokenList *args_tokens = create_token_list();
                        size_t arg_start_idx = func_call_start_idx + 2; /* After '(' */ 
                        size_t arg_end_idx = func_call_end_idx - 1; /* Before ')' */
                        
                        size_t arg_idx = arg_start_idx;

                        for (arg_idx; arg_idx <= arg_end_idx; arg_idx++) {
                            add_token_to_list(args_tokens, token_list->tokens[arg_idx]);
                        }

                        /* Validação da chamada de funcão semantica */ 
                        TokenType function_return_type = semantic_validate_function_call(name_token, args_tokens);
                        destroy_token_list_without_tokens(args_tokens); /* Limpar a lista de token */ 

                        if (function_return_type == TK_ERROR) {
                            i = func_call_end_idx;
                            continue;
                        }

                        Token *return_value_token = create_new_token(function_return_type, "", name_token->line); 
                        var = create_new_var(TIPO_DECIMAL, varToken, return_value_token, t->line);
                        add_var_to_list(var_list, var);

                        if (semantic_check_assignment_type(var_list, varToken, return_value_token) != 0) {
                        }
                        FREE(return_value_token->word);
                        FREE(return_value_token); 

                        i = func_call_end_idx + 1;
                        if (i < token_list->count && token_list->tokens[i]->type == TK_DELIM && strcmp(token_list->tokens[i]->word, ";") == 0) {
                            break; 
                        }
                        continue;
                    }
                }

                /* Logica original para tratamento de atribuição de literais de decimal */  
                Token *valueToken = token_list->tokens[i+2];
                if (valueToken && (valueToken->type == TK_NUM_DEC || valueToken->type == TK_NUM_INT)) {
                    var = create_new_var(TIPO_DECIMAL, varToken, valueToken, t->line);
                    add_var_to_list(var_list, var);

                    /* Semantica: Avaliara a atribuição */ 
                    if (semantic_check_assignment_type(var_list, varToken, valueToken) != 0) {
                        printf("[ERRO SEMANTICO] Atribuição de tipo incompatível para '%s' (linha %d)\n", varToken->word, varToken->line);
                    }

                    i += 3;
                } else {
                    printf("[ERRO] Esperado número decimal ou inteiro após '=' (linha %d)\n", t->line);
                    i += 2;
                }
            }

            /* Caso com array: decimal !x[5]; */
            Token *open_brackets = token_list->tokens[i+2];
            Token *valueToken    = token_list->tokens[i+3];
            Token *close_brackets= token_list->tokens[i+4];
            Token *semicolon     = token_list->tokens[i+5];

            if (open_brackets && open_brackets->type == TK_DELIM && strcmp(open_brackets->word, "[") == 0) {
                if (valueToken && valueToken->type == TK_NUM_DEC) {
                    if (close_brackets && close_brackets->type == TK_DELIM && strcmp(close_brackets->word, "]") == 0) {
                        if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {

                            var = create_new_var(TIPO_DECIMAL, varToken, valueToken, t->line);
                            add_var_to_list(var_list, var);

                            if (semantic_check_assignment_type(var_list, varToken, valueToken) != 0) {
                                printf("[ERRO SEMANTICO] Atribuição de tipo incompatível para '%s' (linha %d)\n", varToken->word, varToken->line);
                            }
                        } else {
                            printf("[ERRO SINTATICO] Falta ';' no final da declaração (linha %d)\n", t->line);
                        }
                        i += 5;
                        continue;
                    } else {
                        printf("[ERRO SINTATICO] Falta ']' no final do array (linha %d)\n", t->line);
                        i += 4;
                        continue;
                    }
                } else {
                    printf("[ERRO SEMANTICO] Valor inválido dentro do array (linha %d)\n", t->line);
                    i += 3;
                    continue;
                }
            } else {
                printf("[ERRO SINTATICO] Esperado '[' após variável decimal (linha %d)\n", t->line);
                i += 2;
                continue;
            }
        } else {
            printf("[ERRO SINTATICO] Esperado variável após tipo 'decimal' (linha %d)\n", t->line);
            continue;
        }
    }
    /* Validator de Texto */
    else if (t->type == TIPO_TEXTO) {
        Token *varToken = token_list->tokens[i+1];

        if (varToken && varToken->type == TK_VARIAVEL) {
            /* Adicionar o token a lista */
            if (semantic_add_variable_declaration(varToken, TIPO_TEXTO) != 0) {
                printf("[ERRO SEMANTICO] Erro ao declarar variável '%s' (linha %d)\n", varToken->word, varToken->line);
            }
            Token *nextToken = token_list->tokens[i+2];
            Token *valueToken = NULL;
            Token *semicolon = NULL;
            Variable *var;

            if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
                Token *rhs_start_token = token_list->tokens[i+2];

                /* Checar se a função foi chamada */
                if (rhs_start_token && rhs_start_token->type == TK_IDENTIFICADOR) {
                    Token *open_paren = token_list->tokens[i+3];
                    if (open_paren && open_paren->type == TK_DELIM && strcmp(open_paren->word, "(") == 0) {
                        Token *name_token = rhs_start_token;
                        size_t func_call_start_idx = i + 2;
                        size_t func_call_end_idx = func_call_start_idx;

                        int paren_balance = 0;
                        size_t k = func_call_start_idx + 1; 
                        while (k < token_list->count) {
                            if (token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, "(") == 0) {
                                paren_balance++;
                            } else if (token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, ")") == 0) {
                                paren_balance--;
                            }
                            if (paren_balance == 0 && token_list->tokens[k]->type == TK_DELIM && strcmp(token_list->tokens[k]->word, ")") == 0) {
                                func_call_end_idx = k;
                                break;
                            }
                            k++;
                        }

                        if (paren_balance != 0) {
                            printf("[ERRO SINTATICO] Parênteses desbalanceados na chamada de função (linha %d)\n", name_token->line);
                            i = k; 
                            continue;
                        }

                        /* Extrair os argumentos da chamada de função */
                        TokenList *args_tokens = create_token_list();
                        size_t arg_start_idx = func_call_start_idx + 2;
                        size_t arg_end_idx = func_call_end_idx - 1;

                        size_t arg_idx = arg_start_idx;
                        for (arg_idx; arg_idx <= arg_end_idx; arg_idx++) {
                            add_token_to_list(args_tokens, token_list->tokens[arg_idx]);
                        }

                        /* Validar semanticamente a chamada de função */
                        TokenType function_return_type = semantic_validate_function_call(name_token, args_tokens);
                        destroy_token_list_without_tokens(args_tokens); 

                        if (function_return_type == TK_ERROR) {
                            i = func_call_end_idx;
                            continue;
                        }

                        /* Criar um dummy */
                        Token *return_value_token = create_new_token(function_return_type, "", name_token->line);
                        var = create_new_var(TIPO_TEXTO, varToken, return_value_token, t->line);
                        add_var_to_list(var_list, var);

                        if (semantic_check_assignment_type(var_list, varToken, return_value_token) != 0) {
                        }
                        FREE(return_value_token->word); 
                        FREE(return_value_token); 

                        i = func_call_end_idx + 1; 
                        if (i < token_list->count && token_list->tokens[i]->type == TK_DELIM && strcmp(token_list->tokens[i]->word, ";") == 0) {
                            break; 
                        }
                        continue;
                    }
                }

                /* Logica original para atribuição de texto */ 
                Token *valueToken = token_list->tokens[i+3];
                
                semicolon = token_list->tokens[i+4];

                if (valueToken && valueToken->type == TK_STRING) {
                    if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                        /* Cria e adiciona na lista */
                        var = create_new_var(TIPO_TEXTO, varToken, valueToken, t->line);
                        add_var_to_list(var_list, var);

                        /* Checar Tipagem Semantica */
                        if (semantic_check_assignment_type(var_list, varToken, valueToken) != 0) {
                            printf("[ERRO SEMANTICO] Atribuição de tipo incompatível para '%s' (linha %d)\n", varToken->word, varToken->line);
                        }
                    } else {
                        printf("[ERRO SINTATICO] Falta ';' no final da declaração (linha %d)\n", t->line);
                    }
                } else {
                    printf("[ERRO SEMÂNTICO] Valor inválido para texto (linha %d)\n", t->line);
                }

                i += 4;
                continue;
            } else {
                semicolon = nextToken;
                if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                   
                    var = create_new_var(TIPO_TEXTO, varToken, NULL, t->line);
                    add_var_to_list(var_list, var);

                } else {
                    printf("[ERRO SINTATICO] Falta ';' no final da declaração (linha %d)\n", t->line);
                }
                i += 1; 
                continue;
            }
        } else {
            printf("[ERRO SINTATICO] Esperado variável após tipo 'texto' (linha %d)\n", t->line);
            continue;
        }
    }


        /* Validate LEIA */
    else if (t->type == TK_LEIA) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO SINTATICO] Esperado '(' após 'escreva' (linha %d)\n", t->line);
            continue;
        }

        size_t j = i + 2;
        int expect_arg = 1;

        while (j < token_list->count) {
            Token *arg = token_list->tokens[j];

            if (arg->type == TK_DELIM && strcmp(arg->word, ")") == 0) {
                j++;
                break;
            }

            if (expect_arg) {
                if (arg->type == TK_VARIAVEL) {
                    /* Semantica: Checar se a variavel foi declarada */ 
                    if (semantic_get_variable_type(var_list, arg) == TK_UNKNOWN) {
                        printf("[ERRO SEMANTICO] Variável '%s' não declarada em 'leia' (linha %d)\n", arg->word, arg->line);
                    }
                }else {
                    printf("[ERRO SEMÂNTICO] Argumento inválido em 'leia' (linha %d)\n", arg->line);
                }
                expect_arg = 0;
            } else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expect_arg = 1;
                } else {
                    printf("[ERRO SINTATICO] Esperado ',' entre argumentos em 'leia' (linha %d)\n", arg->line);
                }
            }

            j++;
        }

        Token *semicolon = token_list->tokens[j];
        if (!semicolon || semicolon->type != TK_DELIM || strcmp(semicolon->word, ";") != 0) {
            printf("[ERRO SINTATICO] Esperado ';' ao final de 'leia' (linha %d)\n", t->line);
        }

        i = j;
    }


   
    /* Validate e execute ESCREVA */
    else if (t->type == TK_ESCREVA) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO SINTATICO] Esperado '(' após 'escreva' (linha %d)\n", t->line);
            continue;
        }

        size_t j = i + 2;
        int expect_arg = 1;

        while (j < token_list->count) {
            Token *arg = token_list->tokens[j];

            if (arg->type == TK_DELIM && strcmp(arg->word, ")") == 0) {
                j++;
                break;
            }

            if (expect_arg) {
                if (arg->type == TK_STRING) {
                } 
                else if (arg->type == TK_NUM_INT) {
                }
                else if (arg->type == TK_NUM_DEC) {
                }
                else if (arg->type == TK_VARIAVEL) {
                    /* Semantica: Verificar se a variavel foi declarada */
                    TokenType var_type = semantic_get_variable_type(var_list, arg);
                    if (var_type == TK_UNKNOWN) {
                        printf("[ERRO SEMANTICO] Variável '%s' não declarada em 'escreva' (linha %d)\n", arg->word, arg->line);
                    } else {
                        /* Semantic: Checar se a variavel foi declarada */
                        if (semantic_check_variable_initialized(var_list, arg) != 0) {
                            printf("[ERRO SEMANTICO] Variável '%s' usada sem inicialização em 'escreva' (linha %d)\n", arg->word, arg->line);
                        }
                    }
                }
                else {
                    printf("[ERRO SEMANTICO] Argumento inválido em 'escreva' (linha %d)\n", arg->line);
                }
                expect_arg = 0;
            } 
            else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expect_arg = 1;
                } else {
                    printf("[ERRO SINTATICO] Esperado ',' entre argumentos em 'escreva' (linha %d)\n", arg->line);
                }
            }

          j++;
      }

      Token *semicolon = token_list->tokens[j];
      if (!semicolon || semicolon->type != TK_DELIM || strcmp(semicolon->word, ";") != 0) {
          printf("[ERRO SINTATICO] Esperado ';' ao final de 'escreva' (linha %d)\n", t->line);
      } 

      i = j;
  }
   

    /* Validate to SE */
    else if (t->type == TK_SE) {
          Token *open_paren = token_list->tokens[i+1];
          if (!open_paren || strcmp(open_paren->word, "(") != 0) {
              printf("[ERRO SINTATICO] Esperado '(' após SE (linha %d)\n", t->line);
              continue;
          }

          int found_comparison = 0;
          int found_and_or = 0;

          /* percorre até achar ')' */
          size_t j;
          for (j = i+2; j < token_list->count; j++) {
              Token *arg = token_list->tokens[j];

              if (strcmp(arg->word, ")") == 0) {
                  if (!found_comparison) {
                      printf("[ERRO SINTATICO] Condição inválida em SE (linha %d)\n", arg->line);
                  }
                  break;
              }

              /* ===== Verificação de variáveis ===== */
              if (arg->type == TK_VARIAVEL) {
                  Variable *var = find_variable(var_list, arg->word);
                  if (!var) {
                      printf("[ERRO SEMANTICO] Variável '%s' não declarada (linha %d)\n", arg->word, arg->line);
                  } else if (!var->initialized) {
                      printf("[ERRO SEMANTICO] Variável '%s' usada sem inicialização (linha %d)\n", arg->word, arg->line);
                  }
              }

              /* ===== Comparadores ===== */
              if (strcmp(arg->word, "==") == 0 || strcmp(arg->word, "<>" ) == 0 ||
                  strcmp(arg->word, "<") == 0 || strcmp(arg->word, ">") == 0 ||
                  strcmp(arg->word, "<=") == 0 || strcmp(arg->word, ">=") == 0) {
                  found_comparison = 1;
                  /* Semantic: Check tipagem para comparação  */
                  Token *left_operand = token_list->tokens[j-1];
                  Token *right_operand = token_list->tokens[j+1];
                  if (semantic_check_comparison_type(var_list, left_operand, arg, right_operand) != 0) {
                      printf("[ERRO SEMANTICO] Tipos incompatíveis na comparação (linha %d)\n", arg->line);
                  }
              }

              /* ===== AND / OR ===== */
              if (strcmp(arg->word, "&&") == 0 || strcmp(arg->word, "||") == 0) {
                  if (!found_comparison) {
                      printf("[ERRO SINTATICO] Operador lógico '%s' sem comparação anterior (linha %d)\n", arg->word, arg->line);
                  }
                  found_and_or = 1;
                  found_comparison = 0; /* espera nova comparação depois do && */ 
              }
          }

          /* ===== Checar abertura do bloco ===== */
          Token *open_brace = NULL;
          for (j = i; j < token_list->count; j++) {
              if (strcmp(token_list->tokens[j]->word, "{") == 0) {
                  open_brace = token_list->tokens[j];
                  break;
              }
          }
          if (!open_brace) {
              printf("[ERRO SINTATICO] Bloco esperado após SE (linha %d)\n", t->line);
          }

          /* marca que já houve um SE */ 
          se_ativo = 1;
      }

      /* SENAO só se existir SE anterior */
      else if (t->type == TK_SENAO) {
          if (!se_ativo) {
              printf("[ERRO SINTATICO] 'SENAO' sem 'SE' correspondente (linha %d)\n", t->line);
          } else {
              se_ativo = 0; /* fecha o SE ativo */
          }
      }

       /* Validate PARA */
    else if (t->type == TK_PARA) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO SINTATICO] Esperado '(' após 'PARA' (linha %d)\n", t->line);
            continue;
        }

        size_t j = i + 2;

        /* ============================
           x1 -> inicialização
           ============================ */
        int expect_assign = 1;
        int found_x1 = 0;

        while (j < token_list->count) {
            Token *arg = token_list->tokens[j];

            if (arg->type == TK_DELIM && strcmp(arg->word, ";") == 0) {
                j++;
                break;
            }

            if (expect_assign) {
                if (arg->type == TK_VARIAVEL) {
                    Token *next = token_list->tokens[j+1];
                    if (next && next->type == TK_OPERATOR_EQUAL && strcmp(next->word, "=") == 0) {
                        /* !a = ... */
                        found_x1 = 1;
                        /* Semantic: Checar o tipo da atribuição */ 
                        Token *var_to_assign = arg;
                        Token *value_assigned = token_list->tokens[j+2];
                        if (semantic_check_assignment_type(var_list, var_to_assign, value_assigned) != 0) {
                            printf("[ERRO SEMANTICO] Atribuição de tipo incompatível em PARA (linha %d)\n", arg->line);
                        }
                        j += 2;
                        continue;
                    } else {
                        printf("[ERRO SINTATICO] Esperado '=' após variável em inicialização do PARA (linha %d)\n", arg->line);
                    }
                }
            }

            if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                expect_assign = 1;
            }

            j++;
        }

        if (!found_x1) {
            printf("[ERRO SINTATICO] Inicialização inválida em PARA (linha %d)\n", t->line);
        }

        /* ============================
           x2 -> condição
           ============================ */
        int found_x2 = 0;
        while (j < token_list->count) {
            Token *arg = token_list->tokens[j];

            if (arg->type == TK_DELIM && strcmp(arg->word, ";") == 0) {
                j++; 
                break;
            }

       
            if (arg->type == TK_VARIAVEL || arg->type == TK_NUM_INT || arg->type == TK_NUM_DEC) {
                found_x2 = 1;
            }
            /* Semantic: Checar para operadores de comparçaõ e fazer checagem de tipo */
            if (strcmp(arg->word, "==") == 0 || strcmp(arg->word, "<>" ) == 0 ||
                strcmp(arg->word, "<") == 0 || strcmp(arg->word, ">") == 0 ||
                strcmp(arg->word, "<=") == 0 || strcmp(arg->word, ">=") == 0) {
                Token *left_operand = token_list->tokens[j-1];
                Token *right_operand = token_list->tokens[j+1];
                if (semantic_check_comparison_type(var_list, left_operand, arg, right_operand) != 0) {
                    printf("[ERRO SEMANTICO] Tipos incompatíveis na condição PARA (linha %d)\n", arg->line);
                }
            }

            j++;
        }

        if (!found_x2) {
            printf("[ERRO SEMANTICO] Condição inválida em PARA (linha %d)\n", t->line);
        }

        /* ============================
           x3 -> incremento
           ============================ */
        int found_x3 = 0;
        while (j < token_list->count) {
            Token *arg = token_list->tokens[j];

            if (arg->type == TK_DELIM && strcmp(arg->word, ")") == 0) {
                j++;
                break;
            }

            /* Incremento pode ser: */
            /* !a++ , ++!a , !a-- , --!a , ou !a = !a + 1 */
            if (arg->type == TK_VARIAVEL) {
                Token *next = token_list->tokens[j+1];
                if (next && next->type == TK_OPERATOR_EQUAL &&
                    (strcmp(next->word, "++") == 0 || strcmp(next->word, "--") == 0)) {
                    found_x3 = 1;

                    /* Semantica: Checar se variavel e numerica para incrementar/decrementar */ 
                    TokenType var_type = semantic_get_variable_type(var_list, arg);
                    if (var_type != TIPO_INTEIRO && var_type != TIPO_DECIMAL) {
                        printf("[ERRO SEMANTICO] Operador de incremento/decremento inválido para tipo \'%s\' (linha %d)\n", arg->word, arg->line);
                    }
                    j += 2;
                    continue;
                }
                else if (next && next->type == TK_OPERATOR_EQUAL && strcmp(next->word, "=") == 0) {
                    /* atribuição matemática */
                    found_x3 = 1;
                    /* Checar a tipagem de atribuição */
                    Token *var_to_assign = arg;
                    Token *value_assigned = token_list->tokens[j+2]; /* Asumindo simples atribuição, como a = b */
                    if (semantic_check_assignment_type(var_list, var_to_assign, value_assigned) != 0) {
                        printf("[ERRO SEMANTICO] Atribuição de tipo incompatível em incremento PARA (linha %d)\n", arg->line);
                    }
                }
            }
            else if (arg->type == TK_OPERATOR_EQUAL &&
                     (strcmp(arg->word, "++") == 0 || strcmp(arg->word, "--") == 0)) {
                /* ++!a ou --!a */
                found_x3 = 1;

                /* Semantica: Checagem se variavel é numerica para incremento/decremento */
                Token *var_to_check = token_list->tokens[j-1];
                TokenType var_type = semantic_get_variable_type(var_list, var_to_check);
                if (var_type != TIPO_INTEIRO && var_type != TIPO_DECIMAL) {
                    printf("[ERRO SEMANTICO] Operador de incremento/decremento inválido para tipo \'%s\' (linha %d)\n", var_to_check->word, var_to_check->line);
                }
            }

            j++;
        }

        if (!found_x3) {
            printf("[ERRO SINTATICO] Incremento inválido em PARA (linha %d)\n", t->line);
        }

        /* ============================
           fim do PARA
           ============================ */
        Token *next = token_list->tokens[j];
        if (next && next->type == TK_DELIM && strcmp(next->word, "{") == 0) {
            /* esperar '}' */
            int found_close = 0;
            j++;
            while (j < token_list->count) {
                if (token_list->tokens[j]->type == TK_DELIM &&
                    strcmp(token_list->tokens[j]->word, "}") == 0) {
                    found_close = 1;
                    j++;
                    break;
                }
                j++;
            }
            if (!found_close) {
                printf("[ERRO SINTATICO] Esperado '}' ao final do bloco PARA (linha %d)\n", t->line);
            }
        }

        i = j;
    }        
  }
  return;
}
