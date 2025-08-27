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
VarList* create_var_list() {
    VarList *list = (VarList*)MALLOC(sizeof(VarList));
    if (list == NULL) {
        perror("Erro ao alocar TokenList");
        return NULL;
    }
    list->count = 0;
    list->capacity = 10;
    list->vars = (Variable**)MALLOC(sizeof(Variable*) * list->capacity);
    if (list->vars == NULL) {
        perror("Erro ao alocar array de Variaveis na lista");
        FREE(list);
        return NULL;
    }
    return list;
}


void add_var_to_list(VarList *list, Variable *var) {
    if (list == NULL || var == NULL) return;

    if (list->count == list->capacity) {
        list->capacity *= 2;

        Variable **new_vars = (Variable**)realloc(list->vars, sizeof(Variable*) * list->capacity);
        if (new_vars == NULL) {
            perror("Erro ao realocar lista de variveis");

            FREE(var->name);
            FREE(var);
            return;
        }
        list->vars = new_vars;
    }

    list->vars[list->count++] = var;
}


void destroy_var_list(VarList *list) {
    size_t i;
    if (list == NULL) return;

    for (i = 0; i < list->count; i++) {
        if (list->vars[i] != NULL) {
            FREE(list->vars[i]->name);
            FREE(list->vars[i]);
        }
    }
    FREE(list->vars);
    FREE(list);
}



static Variable* create_new_var(TokenType type, Token *token, Token *value, int num_line) {
    Variable *var = (Variable*)MALLOC(sizeof(Variable));
    if (var == NULL) {
        perror("Erro ao alocar nova variavel");
        return NULL;
    }

    var->line = num_line;
    var->type = type;
    var->name = STRDUP(token->word);

    // inicialização padrão (caso não tenha "value")
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
          printf("[ERRO] Tipo inválido ao criar variável.\n");
          break;
    }

    // se tem inicialização, sobrescreve
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
    }

    if (var->name == NULL) {
        perror("Erro ao alocar Nome para Nova Variable");
        FREE(var);
        return NULL;
    }
  
    return var;
}

/* Imprime tabela de variáveis */
void print_variables(VarList *list) {
    size_t i;
    if (list == NULL) {
        printf("Lista de Variaveis vazia ou nula.\n");
        return;
    }
    printf("\n--- Lista de Variaveis (%zu variaveis) ---\n", list->count);
    for (i = 0; i < list->count; i++) {
        Variable *t = list->vars[i];
        if (t == NULL) {
            printf("[%3zu] <NULL TOKEN>\n", i);
            continue;
        }
       
        printf("[%3zu] Linha: %d, Tipo: %s (%s)", i, t->line, token_type_to_string_name(t->type), t->name);
        
       
        if (t->type == TIPO_INTEIRO) {
            printf(" (Valor Inteiro: %ld)", t->value.int_val);
        } else if (t->type == TIPO_DECIMAL) {
            printf(" (Valor Decimal: %lf)", t->value.dec_val);
        }else if( t->type == TIPO_TEXTO){
            printf(" (Valor Textual: %s)", t->value.str_val);
        }
        printf("\n");
    }
    printf("-------------------------------------------\n");
}




void validate_declaration(TokenList *token_list, VarList *var_list){
  printf("Teste de Validar Declaração \n");
  if(token_list == NULL){
    printf("Lista de Tokens Vazia");
  }

  int line = -1;
  Variable *var; 
  size_t i = 0;
  Token *t = NULL;


  for(i; i < token_list->count; i++){
    t = token_list->tokens[i];
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

    if (t->type == TIPO_INTEIRO) {
        i++; // pula o token "inteiro"
        while (i < token_list->count) {
            Token *varToken = token_list->tokens[i];
            if (!varToken || varToken->type != TK_VARIAVEL) {
                printf("[ERRO] Esperado variável após tipo 'inteiro' (linha %d)\n", t->line);
                break;
            }

            Token *nextToken = token_list->tokens[i+1];

            if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
                Token *valueToken = token_list->tokens[i+2];
                if (valueToken && valueToken->type == TK_NUM_INT) {
                    printf("[OK] inteiro %s = %ld (linha %d)\n",
                          varToken->word, valueToken->value.int_val, t->line);

                    var = create_new_var(TIPO_INTEIRO, varToken, valueToken, t->line);
                    add_var_to_list(var_list, var);

                    i += 3; // pula var = valor
                } else {
                    printf("[ERRO] Esperado número inteiro após '=' (linha %d)\n", t->line);
                    i += 2;
                }
            } else {
                // declaração simples
                printf("[OK] inteiro %s (linha %d)\n", varToken->word, t->line);

                var = create_new_var(TIPO_INTEIRO, varToken, NULL, t->line);
                add_var_to_list(var_list, var);

                i++; // só pula a variável
            }

            // Agora verifica se tem vírgula (mais variáveis) ou ponto e vírgula (fim)
            Token *sep = token_list->tokens[i];
            if (sep && sep->type == TK_DELIM && strcmp(sep->word, ",") == 0) {
                i++; // continua para próxima variável
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
            Token *nextToken = token_list->tokens[i+2];

            // Caso simples: apenas "decimal !x;"
            if (nextToken && nextToken->type == TK_DELIM && strcmp(nextToken->word, ";") == 0) {
                printf("[OK] Declaração de decimal sem inicialização: %s (linha %d)\n",
                      varToken->word, t->line);

                var = create_new_var(TIPO_DECIMAL, varToken, NULL, t->line);
                add_var_to_list(var_list, var);

                i += 2;
                continue;
            }

            // Caso com array: decimal !x[5];
            Token *open_brackets = token_list->tokens[i+2];
            Token *valueToken    = token_list->tokens[i+3];
            Token *close_brackets= token_list->tokens[i+4];
            Token *semicolon     = token_list->tokens[i+5];

            if (open_brackets && open_brackets->type == TK_DELIM && strcmp(open_brackets->word, "[") == 0) {
                if (valueToken && valueToken->type == TK_NUM_DEC) {
                    if (close_brackets && close_brackets->type == TK_DELIM && strcmp(close_brackets->word, "]") == 0) {
                        if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                            printf("[OK] Declaração de array decimal: %s = [%lf] (linha %d)\n",
                                  varToken->word, valueToken->value.dec_val, t->line);

                            var = create_new_var(TIPO_DECIMAL, varToken, valueToken, t->line);
                            add_var_to_list(var_list, var);
                        } else {
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
                printf("[ERRO] Esperado '[' após variável decimal (linha %d)\n", t->line);
                i += 2;
                continue;
            }
        } else {
            printf("[ERRO] Esperado variável após tipo 'decimal' (linha %d)\n", t->line);
            continue;
        }
    }
    /* Validator de Texto */
    else if (t->type == TIPO_TEXTO) {
        Token *varToken = token_list->tokens[i+1];

        if (varToken && varToken->type == TK_VARIAVEL) {
            Token *nextToken = token_list->tokens[i+2];
            Token *valueToken = NULL;
            Token *semicolon = NULL;
            Variable *var;

            if (nextToken && nextToken->type == TK_OPERATOR_EQUAL) {
                valueToken = token_list->tokens[i+3];
                printf("%s", valueToken->word);
                semicolon = token_list->tokens[i+4];

                if (valueToken && valueToken->type == TK_STRING) {
                    if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                        // Cria e adiciona na lista
                        var = create_new_var(TIPO_TEXTO, varToken, valueToken, t->line);
                        add_var_to_list(var_list, var);

                        printf("[OK] Declaração de texto com atribuição: %s = %s (linha %d)\n",
                              varToken->word, valueToken->word, t->line);
                    } else {
                        printf("[ERRO] Falta ';' no final da declaração (linha %d)\n", t->line);
                    }
                } else {
                    printf("[ERRO] Valor inválido para texto (linha %d)\n", t->line);
                }

                i += 4; // pula token da variável, =, valor e ;
                continue;
            } else {
                semicolon = nextToken;
                if (semicolon && semicolon->type == TK_DELIM && strcmp(semicolon->word, ";") == 0) {
                    // Cria variável sem valor inicial
                    var = create_new_var(TIPO_TEXTO, varToken, NULL, t->line);
                    add_var_to_list(var_list, var);

                    printf("[OK] Declaração de texto sem atribuição: %s (linha %d)\n",
                          varToken->word, t->line);
                } else {
                    printf("[ERRO] Falta ';' no final da declaração (linha %d)\n", t->line);
                }
                i += 1; // pula token da variável
                continue;
            }
        } else {
            printf("[ERRO] Esperado variável após tipo 'texto' (linha %d)\n", t->line);
            continue;
        }
    }


        /* Validate LEIA */
    else if (t->type == TK_LEIA) {
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


    /* Validate ESCREVA */
    else  if (t->type == TK_ESCREVA) {
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
    

      /*Validate to SE */
        else  if (t->type == TK_SE) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO] Esperado '(' após 'SE' (linha %d)\n", t->line);
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
                    printf("[ERRO] Argumento inválido em 'SE' (linha %d)\n", arg->line);
                }
                expect_arg = 0;
            } else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expect_arg = 1;
                } else {
                    printf("[ERRO] Esperado ',' entre argumentos em 'SE' (linha %d)\n", arg->line);
                }
            }

            j++;
        }

        Token *semicolon = token_list->tokens[j];
        if (!semicolon || semicolon->type != TK_DELIM || strcmp(semicolon->word, ";") != 0) {
            printf("[ERRO] Esperado ';' ao final de 'SE' (linha %d)\n", t->line);
        } else {
            printf("[OK] SE válido (linha %d)\n", t->line);
        }

        i = j;
    }


    /* Validate to PARA */
        else  if (t->type == TK_PARA) {
        Token *open_paren = token_list->tokens[i+1];
        if (!open_paren || open_paren->type != TK_DELIM || strcmp(open_paren->word, "(") != 0) {
            printf("[ERRO] Esperado '(' após 'PARA' (linha %d)\n", t->line);
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
                    printf("[ERRO] Argumento inválido em 'PARA' (linha %d)\n", arg->line);
                }
                expect_arg = 0;
            } else {
                if (arg->type == TK_DELIM && strcmp(arg->word, ",") == 0) {
                    expect_arg = 1;
                } else {
                    printf("[ERRO] Esperado ',' entre argumentos em 'PARA' (linha %d)\n", arg->line);
                }
            }

            j++;
        }

        Token *semicolon = token_list->tokens[j];
        if (!semicolon || semicolon->type != TK_DELIM || strcmp(semicolon->word, ";") != 0) {
            printf("[ERRO] Esperado ';' ao final de 'PARA' (linha %d)\n", t->line);
        } else {
            printf("[OK] PARA válido (linha %d)\n", t->line);
        }

        i = j;
    }



    
  }



  return;
}
