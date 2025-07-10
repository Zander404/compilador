#include "parser.h"
#include "memory_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Static helper for realloc (if not using a REALLOC macro) */
static void* my_realloc_wrapper(void* ptr, size_t new_size) {
    void* new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL && new_size > 0) {
        perror("Erro ao realocar memória para stack");
        /* Handle fatal error if memory truly runs out, e.g., exit */
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}


/* Stack implementation for characters */
CharStack* stack_create() {
    CharStack* stack = (CharStack*)MALLOC(sizeof(CharStack));
    if (!stack) {
        perror("Erro ao alocar stack");
        exit(EXIT_FAILURE);
    }
    stack->capacity = 10; /* Initial capacity */
    stack->data = (char*)MALLOC(sizeof(char) * stack->capacity);
    if (!stack->data) {
        perror("Erro ao alocar dados da stack");
        FREE(stack);
        exit(EXIT_FAILURE);
    }
    stack->top = -1; /* Empty stack */
    return stack;
}

void stack_destroy(CharStack* stack) {
    if (stack) {
        FREE(stack->data);
        FREE(stack);
    }
}

void stack_push(CharStack* stack, char item) {
    if (stack->top + 1 >= stack->capacity) {
        stack->capacity *= 2; /* Double capacity */
        stack->data = (char*)my_realloc_wrapper(stack->data, sizeof(char) * stack->capacity);
    }
    stack->data[++(stack->top)] = item;
}

char stack_pop(CharStack* stack) {
    if (stack_is_empty(stack)) {
        /* Error: Popping from empty stack. This indicates imbalance. */
        fprintf(stderr, "ERRO: Tentativa de desempilhar de pilha vazia (símbolo de fechamento sem abertura correspondente).\n");
        return '\0'; /* Return null char to indicate error/empty */
    }
    return stack->data[(stack->top)--];
}

char stack_peek(CharStack* stack) {
    if (stack_is_empty(stack)) {
        return '\0';
    }
    return stack->data[stack->top];
}

int stack_is_empty(CharStack* stack) {
    return stack->top == -1;
}

/* Main function for symbol balancing check across all tokens */
int check_all_symbols_balance(TokenList* token_list) {
    CharStack* stack;
    size_t i; /* C90 */
    int is_balanced = 1; /* Assume balanced until an error is found */
    char popped_char; /* C90 */

    stack = stack_create();
    if (stack == NULL) {
        return 0; /* Failed to create stack */
    }

    for (i = 0; i < token_list->count; i++) {
        Token* token = token_list->tokens[i];
        if (token == NULL) continue;

        /* Check for opening delimiters */
        if (token->type == TK_DELIM) { /* Assuming TK_DELIM for all your structural delimiters */
            if (strcmp(token->word, "(") == 0 ||
                strcmp(token->word, "[") == 0 ||
                strcmp(token->word, "{") == 0) {
                stack_push(stack, token->word[0]); /* Push the actual char */
            }
            /* Check for closing delimiters */
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

    /* After iterating all tokens, check if stack is empty */
    if (!stack_is_empty(stack)) {
        fprintf(stderr, "[ERRO SINTÁTICO] Pilha não vazia no final do arquivo. Símbolo '%c' não fechado.\n", stack_peek(stack));
        is_balanced = 0;
    }

    stack_destroy(stack);
    return is_balanced;
}
