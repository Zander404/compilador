#ifndef LEXICO_H
#define LEXICO_H

#include "tokens.h"         // Para TokenType
#include "memory_manager.h"
#include "symtab.h"         // Para SymbolTable

// Estrutura para retornar o estado do balanceamento de uma linha
typedef struct {
    int parentheses;
    int curly_braces;
    int square_brackets;
    int is_inside_string_at_line_end;
    int error_line;
} BalanceCounters;

void init_balance_counters(BalanceCounters *counters);

// checkLine agora recebe a tabela de símbolos para popular com declarações.
BalanceCounters checkLine(SymbolTable *table, const char *line_text, int line_num, int currently_in_string_global_status);

// A função invalidToken não é mais necessária, pois checkLine retorna error_line
// void invalidToken(void);

int checkPrincipal(const char* line_content, int num_line);


#endif // LEXICO_H
