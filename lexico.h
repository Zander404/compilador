#ifndef LEXICO_H
#define LEXICO_H

#include "tokens.h"     // Para TokenType (enumerações de tipo)
#include "memory_manager.h"
#include "symtab.h"     // Para SymbolTable

// Estrutura para retornar o estado do balanceamento de uma linha
typedef struct {
    int parentheses;
    int curly_braces;
    int square_brackets;
    int is_inside_string_at_line_end;
    int error_line;       // Se > 0, indica a linha do erro léxico ou de sintaxe simples detectado por checkLine
} BalanceCounters;

// Inicializa os campos de uma struct BalanceCounters.
void init_balance_counters(BalanceCounters *counters);

// Processa uma linha de texto, atualiza os contadores de balanceamento,
// e popula a tabela de símbolos com declarações encontradas.
// Retorna a struct BalanceCounters com os deltas da linha e status de erro.
BalanceCounters checkLine(SymbolTable *table,        // Tabela de símbolos a ser populada
                          const char *line_text,     // Texto da linha a ser analisada
                          int line_num,              // Número da linha atual
                          int currently_in_string_global_status); // Estado da string da linha anterior

// Função para verificar a formatação de 'principal()' - pode ser chamada por checkLine ou main.
// Retorna 1 se OK, -1 se mal formulada.
// (Esta função pode se tornar obsoleta se a lógica de 'principal' for totalmente integrada em checkLine para a tabela de símbolos)
int checkPrincipal(const char* line_content, int num_line);


#endif // LEXICO_H
