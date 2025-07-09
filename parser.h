#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <stddef.h>
#include "tokens.h" 
#include "lexico.h"

/* Definir a estrura para armazenar o "PROGRAMA" na memória */
typedef struct {
    char* data;    /* Vetor para armazenar os dados TOKEN na memória*/
    int top;       /* Indentificador de TOPO de pilha */
    int capacity;  /* Atual capacidade da pilha */
} CharStack;

/* Funções para controle de Pilha */
CharStack* stack_create();
void stack_destroy(CharStack* stack);
void stack_push(CharStack* stack, char item);
char stack_pop(CharStack* stack);
char stack_peek(CharStack* stack);
int stack_is_empty(CharStack* stack);

/* Declaração para Validar Duplo Balanceamento */ 
int check_all_symbols_balance(TokenList* token_list);

#endif
