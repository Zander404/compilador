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

int main(void){
  FILE *arq;
  char text_buffer[256];
  int num_line = 1;
  BalanceCounters line_counters;
  const char* input_filename = "./test_spaces_issue.txt";

  init_memory_manager(0);

  printf("Processando arquivo: %s\n", input_filename);
  arq = fopen(input_filename, "r");

  if (arq != NULL){
    while(fgets(text_buffer, sizeof(text_buffer), arq) != NULL) {
      text_buffer[strcspn(text_buffer, "\n")] = '\0';

      line_counters = checkLine(text_buffer, num_line, global_in_string_literal);

      // Atualiza contadores globais COM BASE NOS DELTAS DA LINHA
      global_curly_brace_balance += line_counters.curly_braces;
      global_square_bracket_balance += line_counters.square_brackets;
      global_parenthesis_balance += line_counters.parentheses;
      global_in_string_literal = line_counters.is_inside_string_at_line_end;

      // Verifica desbalanceamento imediato (contador global ficou < 0)
      if (global_parenthesis_balance < 0) {
          fprintf(stderr, "[LINHA %d] ERRO SINTATICO: ')' inesperado ou desbalanceado.\n", num_line);
          line_counters.error_line = num_line; // Sinaliza para sair
      } else if (global_curly_brace_balance < 0) {
          fprintf(stderr, "[LINHA %d] ERRO SINTATICO: '}' inesperado ou desbalanceado.\n", num_line);
          line_counters.error_line = num_line; // Sinaliza para sair
      } else if (global_square_bracket_balance < 0) {
          fprintf(stderr, "[LINHA %d] ERRO SINTATICO: ']' inesperado ou desbalanceado.\n", num_line);
          line_counters.error_line = num_line; // Sinaliza para sair
      }

      // Verifica se checkLine reportou algum outro erro léxico
      if (line_counters.error_line > 0) {
          // A mensagem de erro específica já foi impressa por checkLine ou pela verificação acima
          fprintf(stderr, "Analise interrompida devido a erro na linha %d.\n", num_line);
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

  // Verificações finais de balanceamento global (contadores > 0 ou string não fechada)
  int final_balance_error = 0;
  if (global_curly_brace_balance != 0) {
      fprintf(stderr, "ERRO SINTATICO FINAL: Chaves desbalanceadas ao final do arquivo (saldo: %d).\n", global_curly_brace_balance);
      final_balance_error = 1;
  }
  if (global_square_bracket_balance != 0) {
      fprintf(stderr, "ERRO SINTATICO FINAL: Colchetes desbalanceados ao final do arquivo (saldo: %d).\n", global_square_bracket_balance);
      final_balance_error = 1;
  }
  if (global_parenthesis_balance != 0) {
      fprintf(stderr, "ERRO SINTATICO FINAL: Parenteses desbalanceados ao final do arquivo (saldo: %d).\n", global_parenthesis_balance);
      final_balance_error = 1;
  }
  if (global_in_string_literal) {
      fprintf(stderr, "ERRO SINTATICO FINAL: String literal nao fechada ao final do arquivo.\n");
      final_balance_error = 1;
  }

  if (final_balance_error) {
      report_memory_usage();
      cleanup_memory_manager();
      return 1;
  }

  printf("Analise lexica e de balanceamento concluida com sucesso para o arquivo %s.\n", input_filename);
  report_memory_usage();
  cleanup_memory_manager();

  return 0;
}
