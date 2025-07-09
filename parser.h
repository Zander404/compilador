#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <stddef.h>
#include "tokens.h" 
#include "lexico.h"
/* Define the stack structure */
typedef struct {
    char* data;     /* Array to store stack elements (characters or TokenType values) */
    int top;        /* Index of the top element (-1 for empty) */
    int capacity;   /* Current allocated capacity of the stack */
} CharStack; /* Or TokenTypeStack if you push TokenTypes */

/* Function prototypes for stack operations */
CharStack* stack_create();
void stack_destroy(CharStack* stack);
void stack_push(CharStack* stack, char item);
char stack_pop(CharStack* stack);
char stack_peek(CharStack* stack);
int stack_is_empty(CharStack* stack);

/* Main function for symbol balancing check */
int check_all_symbols_balance(TokenList* token_list);

#endif /* PARSER_UTILS_H */
