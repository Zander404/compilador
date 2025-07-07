#ifndef LEXICO_H
#define LEXICO_H

#include "tokens.h"
#include "memory_manager.h"

// Estrutura para retornar o estado do balanceamento de uma linha
typedef struct {
    int parentheses;      // Saldo de ( ) na linha: +1 para '(', -1 para ')'
    int curly_braces;     // Saldo de { } na linha: +1 para '{', -1 para '}'
    int square_brackets;  // Saldo de [ ] na linha: +1 para '[', -1 para ']'
    int is_inside_string_at_line_end; // 1 se a linha termina dentro de uma string, 0 caso contrário
    int error_line;       // > 0 se um erro de balanceamento imediato foi detectado nesta linha, 0 caso contrário
} BalanceCounters;

// Função para inicializar os contadores de balanceamento
void init_balance_counters(BalanceCounters *counters);

// A função checkLine agora recebe o estado global 'currently_in_string' da linha anterior
// e retorna os BalanceCounters para a linha atual.
BalanceCounters checkLine(const char *line_text, int line_num, int currently_in_string);

// As funções checkXXX são chamadas por checkLine e não precisam mais de Token local.
// Elas operam sobre o lexema fornecido.
void checkReservedWord(const char *word_lexema, int line_num);
void checkVariable(const char *variable_lexema, int line_num);
void checkNumber(const char *number_lexema, int line_num);
void checkString(const char *string_lexema, int line_num); // string_lexema inclui as aspas
void checkFunction(const char *function_lexema, int line_num);
void checkOperator(const char *operator_lexema, int line_num);

// Função de erro para token inválido (pode não ser mais necessária se checkLine tratar todos os erros)
void invalidToken(void);

// Declaração para checkPrincipal, se ainda for relevante para o léxico ou fases iniciais.
int checkPrincipal(const char* line_content, int num_line);


#endif // LEXICO_H
