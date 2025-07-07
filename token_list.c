#include "token_list.h"
#include "memory_manager.h" // Para managed_malloc, managed_realloc, managed_free
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função auxiliar para duplicar uma string usando memória gerenciada
// Usada internamente por add_token_to_list
static char* managed_strdup_for_token(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s);
    char *new_s = (char*)managed_malloc(len + 1);
    // managed_malloc já trata erro fatal (imprime mensagem e sai)
    // if (new_s == NULL) {
    //     fprintf(stderr, "Falha ao alocar memoria para lexema do token via managed_strdup_for_token.\n");
    //     // Não é necessário chamar cleanup_memory_manager() ou exit() aqui, pois managed_malloc já o faz.
    //     return NULL; // Em teoria, não alcançável se managed_malloc sai em falha.
    // }
    strcpy(new_s, s);
    return new_s;
}

TokenList* create_token_list(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = 16; // Uma capacidade inicial padrão razoável
    }
    TokenList *list = (TokenList*)managed_malloc(sizeof(TokenList));
    // if (list == NULL) return NULL; // managed_malloc já trata

    list->tokens = (Token*)managed_malloc(initial_capacity * sizeof(Token));
    // if (list->tokens == NULL) { // managed_malloc já trata
    //     managed_free(list);
    //     return NULL;
    // }

    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

void add_token_to_list(TokenList *list, TokenType type, const char* lexema, int line_num) {
    if (list == NULL) {
        fprintf(stderr, "Erro: Tentativa de adicionar token a uma lista NULL.\n");
        // Considerar finalizar se isso for um erro irrecuperável.
        // cleanup_memory_manager(); exit(EXIT_FAILURE);
        return;
    }

    // Redimensionar se necessário
    if (list->size >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        if (new_capacity == 0) new_capacity = 16; // Caso comece com capacidade 0 (embora create_token_list evite isso)

        Token *new_tokens_array = (Token*)managed_realloc(list->tokens, new_capacity * sizeof(Token));
        // managed_realloc já trata erro fatal
        // if (new_tokens_array == NULL) {
        //     fprintf(stderr, "Erro: Falha ao realocar lista de tokens.\n");
        //     // A lista original (list->tokens) ainda é válida se realloc falhar.
        //     // O programa provavelmente deve terminar aqui, pois não pode adicionar mais tokens.
        //     // free_token_list(list); // Libera o que pode
        //     // cleanup_memory_manager(); exit(EXIT_FAILURE);
        //     return;
        // }
        list->tokens = new_tokens_array;
        list->capacity = new_capacity;
    }

    // Adicionar o novo token
    list->tokens[list->size].type = type;
    list->tokens[list->size].word = managed_strdup_for_token(lexema);
    // managed_strdup_for_token já usa managed_malloc e trata erros.
    // if (list->tokens[list->size].word == NULL && lexema != NULL) {
    //    fprintf(stderr, "Erro: Falha ao duplicar lexema para token na linha %d.\n", line_num);
        // O programa provavelmente já terminou via managed_strdup se houve falha.
        // Se não terminou, precisamos decidir como tratar. Talvez não adicionar o token.
    //    return;
    // }
    list->tokens[list->size].line = line_num;
    list->size++;
}

void print_token_list(const TokenList *list) {
    if (list == NULL) {
        printf("Lista de tokens esta NULL.\n");
        return;
    }
    printf("\n--- TABELA DE TOKENS GERADOS ---\n");
    printf("---------------------------------------------------\n");
    printf("| Linha | Tipo               | Lexema             |\n");
    printf("---------------------------------------------------\n");
    for (size_t i = 0; i < list->size; ++i) {
        printf("| %-5d | %-18s | %-18s |\n",
               list->tokens[i].line,
               token_type_to_string(list->tokens[i].type),
               list->tokens[i].word ? list->tokens[i].word : "NULL");
    }
    printf("---------------------------------------------------\n");
    printf("Total de tokens: %zu\n\n", list->size);
}

void free_token_list(TokenList *list) {
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->size; ++i) {
        if (list->tokens[i].word != NULL) {
            managed_free(list->tokens[i].word);
            list->tokens[i].word = NULL;
        }
    }
    if (list->tokens != NULL) {
        managed_free(list->tokens);
        list->tokens = NULL;
    }
    managed_free(list);
}

// Função auxiliar para converter TokenType em string
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TK_ERRO: return "ERRO";
        case TK_PRINCIPAL: return "PRINCIPAL";
        case TK_FUNCAO: return "FUNCAO";
        case TK_RETORNO: return "RETORNO";
        case TK_LEIA: return "LEIA";
        case TK_ESCREVA: return "ESCREVA";
        case TK_SE: return "SE";
        case TK_SENAO: return "SENAO";
        case TK_PARA: return "PARA";
        case TK_TIPO_INTEIRO: return "TIPO_INTEIRO";
        case TK_TIPO_DECIMAL: return "TIPO_DECIMAL";
        case TK_TIPO_TEXTO: return "TIPO_TEXTO";
        case TK_IDENTIFICADOR: return "IDENTIFICADOR";
        case TK_VARIAVEL: return "VARIAVEL"; // Pode ser subsumido por IDENTIFICADOR
        case TK_LITERAL_INT: return "LITERAL_INT";
        case TK_LITERAL_DEC: return "LITERAL_DEC";
        case TK_LITERAL_STRING: return "LITERAL_STRING";
        case TK_OP_ATRIBUICAO: return "OP_ATRIBUICAO"; // =
        case TK_OP_SOMA: return "OP_SOMA";         // +
        case TK_OP_SUBTRACAO: return "OP_SUBTRACAO";    // -
        case TK_OP_MULTIPLICACAO: return "OP_MULTIPLICACAO";// *
        case TK_OP_DIVISAO: return "OP_DIVISAO";      // /
        case TK_OP_POTENCIA: return "OP_POTENCIA";     // ^
        case TK_OP_IGUAL: return "OP_IGUAL";        // ==
        case TK_OP_DIFERENTE: return "OP_DIFERENTE";    // <>
        case TK_OP_MENOR: return "OP_MENOR";        // <
        case TK_OP_MENOR_IGUAL: return "OP_MENOR_IGUAL";  // <=
        case TK_OP_MAIOR: return "OP_MAIOR";        // >
        case TK_OP_MAIOR_IGUAL: return "OP_MAIOR_IGUAL";  // >=
        case TK_OP_LOGICO_E: return "OP_LOGICO_E";     // &&
        case TK_OP_LOGICO_OU: return "OP_LOGICO_OU";    // ||
        case TK_DELIM_ABRE_PAREN: return "DELIM_ABRE_PAREN";    // (
        case TK_DELIM_FECHA_PAREN: return "DELIM_FECHA_PAREN";   // )
        case TK_DELIM_ABRE_COLCH: return "DELIM_ABRE_COLCH";   // [
        case TK_DELIM_FECHA_COLCH: return "DELIM_FECHA_COLCH";  // ]
        case TK_DELIM_ABRE_CHAVES: return "DELIM_ABRE_CHAVES";  // {
        case TK_DELIM_FECHA_CHAVES: return "DELIM_FECHA_CHAVES"; // }
        case TK_DELIM_PONTO_VIRGULA: return "DELIM_PONTO_VIRGULA"; // ;
        case TK_DELIM_VIRGULA: return "DELIM_VIRGULA";       // ,
        case TK_DELIM_PONTO: return "DELIM_PONTO";         // .
        case TK_DELIM_ASPAS_DUPLAS: return "DELIM_ASPAS_DUPLAS"; // "
        case TK_EOF: return "EOF";
        default: return "DESCONHECIDO";
    }
}
