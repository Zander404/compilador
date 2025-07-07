#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h" // Mantido para TokenType se usado por symtab ou lexico internamente
#include "lexico.h"
#include "memory_manager.h"
#include "symtab.h"     // Incluído para usar a tabela de símbolos

static int global_curly_brace_balance = 0;
static int global_square_bracket_balance = 0;
static int global_parenthesis_balance = 0;
static int global_in_string_literal = 0;

int main(void){
  FILE *arq;
  char text_buffer[256];
  int num_line = 1;
  BalanceCounters line_counters;
  const char* input_filename = "./test_balance_square_mismatch.txt"; // Testando colchetes desbalanceados (mismatch)
  SymbolTable *symbol_table = NULL;

  init_memory_manager(0);
  symbol_table = create_symbol_table(50);
  if (symbol_table == NULL) {
      fprintf(stderr, "Falha ao criar a tabela de simbolos. Encerrando.\n");
      cleanup_memory_manager();
      return 1;
  }

  printf("Processando arquivo: %s\n", input_filename);
  arq = fopen(input_filename, "r");

  if (arq != NULL){
    while(fgets(text_buffer, sizeof(text_buffer), arq) != NULL) {
      text_buffer[strcspn(text_buffer, "\n")] = '\0';

      line_counters = checkLine(symbol_table, text_buffer, num_line, global_in_string_literal);

      global_curly_brace_balance += line_counters.curly_braces;
      global_square_bracket_balance += line_counters.square_brackets;
      global_parenthesis_balance += line_counters.parentheses;
      global_in_string_literal = line_counters.is_inside_string_at_line_end;

      if (global_parenthesis_balance < 0) {
          fprintf(stderr, "[LINHA %d] ERRO SINTATICO: ')' inesperado ou desbalanceado.\n", num_line);
          line_counters.error_line = num_line;
      } else if (global_curly_brace_balance < 0) {
          fprintf(stderr, "[LINHA %d] ERRO SINTATICO: '}' inesperado ou desbalanceado.\n", num_line);
          line_counters.error_line = num_line;
      } else if (global_square_bracket_balance < 0) {
          fprintf(stderr, "[LINHA %d] ERRO SINTATICO: ']' inesperado ou desbalanceado.\n", num_line);
          line_counters.error_line = num_line;
      }

      if (line_counters.error_line > 0) {
          fprintf(stderr, "Analise interrompida devido a erro na linha %d.\n", num_line);
          print_symbol_table(symbol_table);
          free_symbol_table(symbol_table);
          report_memory_usage();
          cleanup_memory_manager();
          return 1;
      }
      num_line++;
    }
    fclose(arq);
  }else{
    fprintf(stderr, "Erro: Arquivo '%s' nao encontrado.\n", input_filename);
    free_symbol_table(symbol_table);
    cleanup_memory_manager();
    return 1;
  }

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
      print_symbol_table(symbol_table);
      free_symbol_table(symbol_table);
      report_memory_usage();
      cleanup_memory_manager();
      return 1;
  }

  printf("Analise lexica e de balanceamento concluida para o arquivo %s.\n", input_filename);
  if (strstr(input_filename, "invalid") != NULL || strstr(input_filename, "error") != NULL || strstr(input_filename, "mismatch") != NULL || strstr(input_filename, "unclosed") != NULL) {
    if (!final_balance_error && line_counters.error_line == 0) {
        printf("AVISO: Esperava-se um erro lexico/sintatico que interrompesse ou um erro de balanceamento final para '%s', mas nenhum foi detectado.\n", input_filename);
    }
  }
  print_symbol_table(symbol_table);

  free_symbol_table(symbol_table);
  report_memory_usage();
  cleanup_memory_manager();

  return 0;
}
