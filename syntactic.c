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
                          i += 5; // pula todos os tokens do array
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

                i += 4; // pula tokens processados
                continue;
            } else {
                // declaração sem atribuição
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
  }

  return;
}
