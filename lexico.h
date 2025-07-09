#ifndef LEXICO_H
#define LEXICO_H

#include "tokens.h"


/* STRUCT */
typedef struct {
    Token **tokens;
    size_t count;   
    size_t capacity; 
} TokenList;


/* Controle de Pilha do TOKEN_LIST */
TokenList* create_token_list();
void add_token_to_list(TokenList *list, Token *token);
void destroy_token_list(TokenList *list);
void print_token_list(TokenList *list);




/* Funções do controle LEXICO */
void checkLine(const char *line, int num_line, TokenList *list);

void checkVariable(const char* word, int num_line, TokenList *list);
void checkFunction(const char* word, int num_line, TokenList *list);
void checkReservedWord(const char* word, int num_line, TokenList *list);
void checkNumber(const char *word, int num_line, TokenList *list);
void checkString(const char *word, int num_line, TokenList *list);
void checkOperator(const char *word, int num_line, TokenList *list);
void invalidToken();

#endif
