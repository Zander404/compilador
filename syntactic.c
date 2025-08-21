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
#include "ascii_table.h"
#include "tokens.h"


void validate_declaration(TokenList *token_list){
  printf("Teste de Validar Declaração \n");
  if(token_list == NULL){
    printf("Lista de Tokens Vazia");
  }

  int line = -1;
  size_t i = 0;

  for(i; i < token_list->count; i++){
    Token *t = token_list->tokens[i];
    if (t == NULL) {
            continue;
    }

    if (t->line != line) {
      if (line != -1) {
        printf("\n");
      }

      line = t->line;
      printf("Linha %d: ", line);
    }

    /* Para Função */
    if (t->type == TK_FUNCAO){
      Token *next = token_list->tokens[i+1];
      continue;
    }

  /* Validator de Inteiros */ 
  if (t->type == TIPO_INTEIRO) {
    Token *varToken = token_list->tokens[i+1];

    if (varToken && varToken->type == TK_VARIAVEL) {
        Token *nextToken = token_list->tokens[i+2];

        if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
            Token *valueToken = token_list->tokens[i+3];

            if (valueToken && valueToken->type == TK_NUM_INT) {
                printf("[OK] Declaração de inteiro com atribuição: %s = %ld (linha %d)\n",
                       varToken->word, valueToken->value.int_val, t->line);
                i += 3;
                continue;
            } else {
                printf("[ERRO] Esperado número inteiro após '=' (linha %d)\n", t->line);
                i += 2;
                continue;
            }
        } else {
            /* declaração sem atribuição */
            printf("[OK] Declaração de inteiro: %s (linha %d)\n", varToken->word, t->line);
            i += 1; /* pula token da variável */
            continue;
        }
    } else {
        printf("[ERRO] Esperado variável após tipo 'inteiro' (linha %d)\n", t->line);
        continue;
      }
    }


    /* Validator de Decimal */
    if (t->type == TIPO_DECIMAL) {
        Token *varToken = token_list->tokens[i+1];

        if (varToken && varToken->type == TK_VARIAVEL) {
            Token *nextToken = token_list->tokens[i+2];
            Token *valueToken = NULL;
            Token *open_brackets = NULL;
            Token *close_brackets = NULL;
            Token *semicolon = NULL;

              open_brackets = token_list->tokens[i+2];
              valueToken = token_list->tokens[i+3];
              close_brackets = token_list->tokens[i+4];
              semicolon = token_list->tokens[i+5];

              if (open_brackets && open_brackets->type == TK_DELIM && strcmp(open_brackets->word, "[") == 0) {
                  if (valueToken && valueToken->type == TK_NUM_DEC) {
                      if (close_brackets && close_brackets->type == TK_DELIM && strcmp(close_brackets->word, "]") == 0) {
                          if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                              printf("[OK] Declaração de array decimal: %s = [%lf] (linha %d)\n",
                                    varToken->word, valueToken->value.dec_val, t->line);
                          } else {
                              printf("%c", semicolon->word);
                              printf("[ERRO] Falta ';' no final da declaração (linha %d)\n", t->line);
                          }
                          i += 5;
                          continue;
                      } else {
                          printf("[ERRO] Falta ']' no final do array (linha %d)\n", t->line);
                          i += 4;
                          continue;
                      }
                  } else {
                      printf("[ERRO] Valor inválido dentro do array (linha %d)\n", t->line);
                      i += 3;
                      continue;
                  }
              } else {
                  printf("[ERRO] Esperado '[' após (linha %d)\n", t->line);
                  i += 2;
                  continue;
              }
        } else {
            printf("[ERRO] Esperado variável após tipo 'decimal' (linha %d)\n", t->line);
            continue;
        }
    }


    /* Validator de Texto */
    if (t->type == TIPO_TEXTO) {
        Token *varToken = token_list->tokens[i+1];

        if (varToken && varToken->type == TK_VARIAVEL) {
            Token *nextToken = token_list->tokens[i+2];
            Token *valueToken = NULL;
            Token *semicolon = NULL;

            if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
                valueToken = token_list->tokens[i+3];
                semicolon = token_list->tokens[i+4];

                if (valueToken && valueToken->type == TK_STRING) {
                    if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                        printf("[OK] Declaração de texto com atribuição: %s = \"%s\" (linha %d)\n",
                              varToken->word, valueToken->value.str_val, t->line);
                    } else {
                        printf("[ERRO] Falta ';' no final da declaração (linha %d)\n", t->line);
                    }
                } else {
                    printf("[ERRO] Valor inválido para texto (linha %d)\n", t->line);
                }

                i += 4;
                continue;
            } else {
                semicolon = nextToken;
                if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                    printf("[OK] Declaração de texto sem atribuição: %s (linha %d)\n",
                          varToken->word, t->line);
                } else {
                    printf("[ERRO] Falta ';' no final da declaração (linha %d)\n", t->line);
                }
                i += 1;
                continue;
            }
        } else {
            printf("[ERRO] Esperado variável após tipo 'texto' (linha %d)\n", t->line);
            continue;
        }
    }
  
    /* Validate Escreva */
    if (t->type == TK_ESCREVA) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO] Esperado '(' após 'escreva' (linha %d)\n", t->line);
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
                if (arg->type == TK_STRING || arg->type == TK_NUM_INT || arg->type == TK_NUM_DEC || arg->type == TK_VARIAVEL) {
                }else {
                    printf("[ERRO] Argumento inválido em 'escreva' (linha %d)\n", arg->line);
                }
                expect_arg = 0;
            } else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expect_arg = 1;
                } else {
                    printf("[ERRO] Esperado ',' entre argumentos em 'escreva' (linha %d)\n", arg->line);
                }
            }

            j++;
        }

        Token *semicolon = token_list->tokens[j];
        if (!semicolon || semicolon->type != TK_DELIM || strcmp(semicolon->word, ";") != 0) {
            printf("[ERRO] Esperado ';' ao final de 'escreva' (linha %d)\n", t->line);
        } else {
            printf("[OK] escreva válido (linha %d)\n", t->line);
        }

        i = j;
    }
  

    /* Validate Escreva */
    if (t->type == TK_LEIA) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO] Esperado '(' após 'escreva' (linha %d)\n", t->line);
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
                }else {
                    printf("[ERRO] Argumento inválido em 'leia' (linha %d)\n", arg->line);
                }
                expect_arg = 0;
            } else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expect_arg = 1;
                } else {
                    printf("[ERRO] Esperado ',' entre argumentos em 'leia' (linha %d)\n", arg->line);
                }
            }

            j++;
        }

        Token *semicolon = token_list->tokens[j];
        if (!semicolon || semicolon->type != TK_DELIM || strcmp(semicolon->word, ";") != 0) {
            printf("[ERRO] Esperado ';' ao final de 'leia' (linha %d)\n", t->line);
        } else {
            printf("[OK] leia válido (linha %d)\n", t->line);
        }

        i = j;
    }








    
  }



  return;
}
