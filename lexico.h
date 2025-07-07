#ifndef LEXICO_H
#define LEXICO_H

#include "tokens.h"         // Para Token, TokenType
#include "memory_manager.h"
#include "token_list.h"     // Para TokenList

// Estrutura para retornar o estado do balanceamento de uma linha
typedef struct {
    int parentheses;
    int curly_braces;
    int square_brackets;
    int is_inside_string_at_line_end;
    int error_line;       // Se > 0, indica a linha do erro léxico ou de balanceamento detectado por checkLine
} BalanceCounters;

void init_balance_counters(BalanceCounters *counters);

// checkLine agora recebe a lista de tokens para popular.
BalanceCounters checkLine(TokenList *list, const char *line_text, int line_num, int currently_in_string_global_status);

// As funções checkXXX agora são internas a lexico.c (static) ou removidas,
// pois a lógica de criação de token será centralizada em checkLine.
// Se forem mantidas como auxiliares, não precisam ser expostas no .h se apenas checkLine as usa.
// Por ora, vamos comentar, pois checkLine vai chamar add_token_to_list diretamente.
// void checkReservedWord(TokenList *list, const char *word_lexema, int line_num);
// void checkVariable(TokenList *list, const char *variable_lexema, int line_num);
// void checkNumber(TokenList *list, const char *number_lexema, int line_num);
// void checkString(TokenList *list, const char *string_lexema, int line_num);
// void checkFunction(TokenList *list, const char *function_lexema, int line_num);
// void checkOperator(TokenList *list, const char *operator_lexema, int line_num);

void invalidToken(void); // Pode ser usada por checkLine para sinalizar erro antes de retornar.

int checkPrincipal(const char* line_content, int num_line); // Mantida se ainda usada externamente.


#endif // LEXICO_H
