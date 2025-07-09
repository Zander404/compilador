#include "parser.h"
#include "memory_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/* Criar a pilha para Armazenar Simbolos para Duplo Balanceamento () [] {} */
CharStack* stack_create() {
    CharStack* stack = (CharStack*)MALLOC(sizeof(CharStack));
    if (!stack) {
        perror("Erro ao alocar stack");
        exit(EXIT_FAILURE);
    }
    stack->capacity = 10;
    stack->data = (char*)MALLOC(sizeof(char) * stack->capacity);
    if (!stack->data) {
        perror("Erro ao alocar dados da stack");
        FREE(stack);
        exit(EXIT_FAILURE);
    }
    stack->top = -1;
    return stack;
}

/* Destruir Stack */
void stack_destroy(CharStack* stack) {
    if (stack) {
        FREE(stack->data);
        FREE(stack);
    }
}

/* Adicionar a Pilha */
void stack_push(CharStack* stack, char item) {
    if (stack->top + 1 >= stack->capacity) {
        stack->capacity *= 2;
        stack->data = (char*)my_realloc_wrapper(stack->data, sizeof(char) * stack->capacity);
    }
    stack->data[++(stack->top)] = item;
}

/* Remover Elemento da Pilha */
char stack_pop(CharStack* stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "ERRO: Tentativa de desempilhar de pilha vazia (símbolo de fechamento sem abertura correspondente).\n");
        return '\0'; 
    }
    return stack->data[(stack->top)--];
}

/* Olhar o Caracter no TOPO da Pilha */
char stack_peek(CharStack* stack) {
    if (stack_is_empty(stack)) {
        return '\0';
    }
    return stack->data[stack->top];
}

int stack_is_empty(CharStack* stack) {
    return stack->top == -1;
}

/* Função para executar a analise se os simbolos estão Balanciados */
int check_all_symbols_balance(TokenList* token_list) {
    CharStack* stack;
    size_t i;
    int is_balanced = 1;
    char popped_char;

    stack = stack_create();
    if (stack == NULL) {
        return 0;
    }

    for (i = 0; i < token_list->count; i++) {
        Token* token = token_list->tokens[i];
        if (token == NULL) continue;

        /* Verificar se o simbolo é do tipo DELIMITADOR e um dos seguintes simbolos '(', '[', '{' e adicionar-lo a pilha  */
        if (token->type == TK_DELIM) { 
            if (strcmp(token->word, "(") == 0 ||
                strcmp(token->word, "[") == 0 ||
                strcmp(token->word, "{") == 0) {
                stack_push(stack, token->word[0]);
            }

            /* Se for um simbolo de fechamento, remover o simbolo de abertura correspondente */
            else if (strcmp(token->word, ")") == 0) {
                if (stack_is_empty(stack)) {
                    fprintf(stderr, "[ERRO SINTÁTICO] Linha %d: Parêntese ')' sem abertura correspondente.\n", token->line);
                    is_balanced = 0; break;
                }
                popped_char = stack_pop(stack);
                if (popped_char != '(') {
                    fprintf(stderr, "[ERRO SINTÁTICO] Linha %d: Parêntese ')' inesperado. Esperado '%c'.\n", token->line, popped_char == '[' ? ']' : popped_char == '{' ? '}' : '?');
                    is_balanced = 0; break;
                }
            }
            else if (strcmp(token->word, "]") == 0) {
                if (stack_is_empty(stack)) {
                    fprintf(stderr, "[ERRO SINTÁTICO] Linha %d: Colchete ']' sem abertura correspondente.\n", token->line);
                    is_balanced = 0; break;
                }
                popped_char = stack_pop(stack);
                if (popped_char != '[') {
                    fprintf(stderr, "[ERRO SINTÁTICO] Linha %d: Colchete ']' inesperado. Esperado '%c'.\n", token->line, popped_char == '(' ? ')' : popped_char == '{' ? '}' : '?');
                    is_balanced = 0; break;
                }
            }
            else if (strcmp(token->word, "}") == 0) {
                if (stack_is_empty(stack)) {
                    fprintf(stderr, "[ERRO SINTÁTICO] Linha %d: Chave '}' sem abertura correspondente.\n", token->line);
                    is_balanced = 0; break;
                }
                popped_char = stack_pop(stack);
                if (popped_char != '{') {
                    fprintf(stderr, "[ERRO SINTÁTICO] Linha %d: Chave '}' inesperada. Esperado '%c'.\n", token->line, popped_char == '(' ? ')' : popped_char == '[' ? ']' : '?');
                    is_balanced = 0; break;
                }
            }
        }
    }

    /* Verificar se no fim da executção a pilha está vazia, se positivo DESTRUIR PILHA, caso negativo EMITIR ERRO */
    if (!stack_is_empty(stack)) {
        fprintf(stderr, "[ERRO SINTÁTICO] Pilha não vazia no final do arquivo. Símbolo '%c' não fechado.\n", stack_peek(stack));
        is_balanced = 0;
    }

    stack_destroy(stack);
    return is_balanced;
}
