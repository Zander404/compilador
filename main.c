#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "lexico.h"
#include "memory_manager.h"

static int global_curly_brace_balance = 0;
static int global_square_bracket_balance = 0;
static int global_parenthesis_balance = 0;
static int global_in_string_literal = 0;

// void update_global_balance_counters(BalanceCounters counters) { // Removida pois main atualiza diretamente
//     global_curly_brace_balance += counters.curly_braces;
//     global_square_bracket_balance += counters.square_brackets;
//     global_parenthesis_balance += counters.parentheses;
//     if (counters.is_inside_string_at_line_end) {
//         global_in_string_literal = 1;
//     } else {
//         if (global_in_string_literal && !counters.is_inside_string_at_line_end) {
//              global_in_string_literal = 0;
//         }
//     }
// }

int main(void){
  FILE *arq;
  char text_buffer[256];
  int num_line = 1;
  BalanceCounters line_counters;
  const char* input_filename = "./test_lex_reserved_words.txt"; // MODIFICADO PARA TESTE

  init_memory_manager(0);
  // init_balance_counters(&line_counters); // checkLine vai zerar os contadores da linha internamente.

  printf("Processando arquivo: %s\n", input_filename);
  arq = fopen(input_filename, "r");

  if (arq != NULL){
    while(fgets(text_buffer, sizeof(text_buffer), arq) != NULL) {
      text_buffer[strcspn(text_buffer, "\n")] = '\0';

      line_counters = checkLine(text_buffer, num_line, global_in_string_literal);

      global_curly_brace_balance += line_counters.curly_braces;
      global_square_bracket_balance += line_counters.square_brackets;
      global_parenthesis_balance += line_counters.parentheses;
      // Atualiza o estado global da string com base no que a linha retornou
      global_in_string_literal = line_counters.is_inside_string_at_line_end;

      if (line_counters.error_line > 0) {
          // Erro já foi impresso por checkLine ou por uma das funções checkXXX.
          // Não precisa imprimir duplicado aqui, mas o programa deve parar.
          fprintf(stderr, "Analise interrompida devido a erro na linha %d.\n", line_counters.error_line);
          report_memory_usage();
          cleanup_memory_manager();
          return 1;
      }
      num_line++;
    }
    fclose(arq);
  }else{
    fprintf(stderr, "Erro: Arquivo '%s' nao encontrado.\n", input_filename);
    cleanup_memory_manager();
    return 1;
  }

  if (global_curly_brace_balance != 0) {
      fprintf(stderr, "ERRO SINTATICO FINAL: Chaves desbalanceadas ao final do arquivo (saldo: %d).\n", global_curly_brace_balance);
  }
  if (global_square_bracket_balance != 0) {
      fprintf(stderr, "ERRO SINTATICO FINAL: Colchetes desbalanceados ao final do arquivo (saldo: %d).\n", global_square_bracket_balance);
  }
  if (global_parenthesis_balance != 0) {
      fprintf(stderr, "ERRO SINTATICO FINAL: Parenteses desbalanceados ao final do arquivo (saldo: %d).\n", global_parenthesis_balance);
  }
  if (global_in_string_literal) {
      fprintf(stderr, "ERRO SINTATICO FINAL: String literal nao fechada ao final do arquivo.\n");
  }

  if (global_curly_brace_balance != 0 || global_square_bracket_balance != 0 || global_parenthesis_balance != 0 || global_in_string_literal) {
      report_memory_usage();
      cleanup_memory_manager();
      return 1;
  }

  printf("Analise lexica e de balanceamento concluida com sucesso para o arquivo %s.\n", input_filename);
  report_memory_usage();
  cleanup_memory_manager();

  return 0;
}
