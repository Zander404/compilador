#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include "tokens.h" // Para TokenType e Token struct
#include <stddef.h> // Para size_t

// Estrutura para a lista dinâmica de tokens
typedef struct {
    Token *tokens;    // Ponteiro para o array de tokens
    size_t size;      // Número atual de tokens na lista
    size_t capacity;  // Capacidade atual do array de tokens
} TokenList;

// Funções para gerenciar a lista de tokens
TokenList* create_token_list(size_t initial_capacity);
void add_token_to_list(TokenList *list, TokenType type, const char* lexema, int line_num);
void print_token_list(const TokenList *list); // Imprime a "tabela de tokens"
void free_token_list(TokenList *list);

// Função auxiliar para converter TokenType em string (para impressão)
const char* token_type_to_string(TokenType type);

#endif // TOKEN_LIST_H
