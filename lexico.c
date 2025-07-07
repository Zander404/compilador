#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"

// Global static variable `counters` to be used by checkXXX functions to report errors.
// This is not ideal. A better approach is for checkLine to manage its own BalanceCounters
// instance and for checkXXX functions to return an error status or set a flag in that instance.
// For now, to quickly adapt, checkLine will initialize this static `counters` and checkXXX will set its error_line field.
static BalanceCounters counters_lexico_internal;


void init_balance_counters(BalanceCounters *counters_param) {
    if (counters_param) {
        counters_param->parentheses = 0;
        counters_param->curly_braces = 0;
        counters_param->square_brackets = 0;
        counters_param->is_inside_string_at_line_end = 0;
        counters_param->error_line = 0;
    }
}

int checkPrincipal(const char* line_content, int num_line){
  int i = 0;
  int opened_brackets = 0;
  while(line_content[i] != '\0'){
    if(line_content[i] == OPEN_PAREN){
      opened_brackets++;
    } else if(line_content[i] == CLOSE_PAREN){
      opened_brackets--;
    }
    if (opened_brackets < 0) {
        // Error will be caught by main.c's global balance check
        return -1;
    }
    i++;
  }
  if(opened_brackets == 0) return 1;
  return -1;
}

void checkVariable(const char* variable_lexema, int line_num){
  if (strlen(variable_lexema) < 2 || variable_lexema[0] != '!' || !islower((unsigned char)variable_lexema[1])) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Variavel '%s' mal formada (deve ser !<minuscula>[alnum]*).\n", line_num, variable_lexema);
    counters_lexico_internal.error_line = line_num;
    return;
  }
  for (size_t i = 2; i < strlen(variable_lexema); ++i) {
    if (!isalnum((unsigned char)variable_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em nome de variavel '%s'.\n", line_num, variable_lexema[i], variable_lexema);
      counters_lexico_internal.error_line = line_num;
      return;
    }
  }
  printf("[VARIAVEL]: %s (Linha %d)\n", variable_lexema, line_num);
}

void checkFunction(const char* function_lexema, int line_num){
  if (strlen(function_lexema) < 3 || function_lexema[0] != '_' || function_lexema[1] != '_' || !isalnum((unsigned char)function_lexema[2])) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Nome de funcao '%s' mal formado (deve ser __<alnum>[alnum]*).\n", line_num, function_lexema);
    counters_lexico_internal.error_line = line_num;
    return;
  }
  for (size_t i = 3; i < strlen(function_lexema); ++i) {
    if (!isalnum((unsigned char)function_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em nome de funcao '%s'.\n", line_num, function_lexema[i], function_lexema);
      counters_lexico_internal.error_line = line_num;
      return;
    }
  }
  printf("[FUNCTION]: %s (Linha %d)\n", function_lexema, line_num);
}

void checkReservedWord(const char* word_lexema, int line_num){
  for(int i = 0; i < ACTUAL_NUM_RESERVED_WORDS; i++){
    if (strcmp(reserved_words[i].word, word_lexema) == 0){
      printf("[PALAVRA RESERVADA]: %s (Tipo: %d, Linha %d)\n", word_lexema, reserved_words[i].type, line_num);
      return;
    }
  }
  if (!isalpha((unsigned char)word_lexema[0])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Identificador '%s' mal formado (deve iniciar com letra).\n", line_num, word_lexema);
      counters_lexico_internal.error_line = line_num;
      return;
  }
  for (size_t i = 1; i < strlen(word_lexema); ++i) {
      if (!isalnum((unsigned char)word_lexema[i])) {
          fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em identificador '%s'.\n", line_num, word_lexema[i], word_lexema);
          counters_lexico_internal.error_line = line_num;
          return;
      }
  }
  printf("[IDENTIFICADOR]: %s (Linha %d)\n", word_lexema, line_num);
}

void checkNumber(const char *number_lexema, int line_num){
  int dot_count = 0;
  for(size_t i = 0; i < strlen(number_lexema); ++i) {
    if (number_lexema[i] == PERIOD) {
      dot_count++;
    } else if (!isdigit((unsigned char)number_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em numero '%s'.\n", line_num, number_lexema[i], number_lexema);
      counters_lexico_internal.error_line = line_num;
      return;
    }
  }
  if (dot_count > 1) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Multiplos pontos decimais em numero '%s'.\n", line_num, number_lexema);
    counters_lexico_internal.error_line = line_num;
    return;
  }
  if (strlen(number_lexema) == 1 && number_lexema[0] == PERIOD) {
       fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto decimal isolado '%s'.\n", line_num, number_lexema);
       counters_lexico_internal.error_line = line_num;
       return;
  }
  if(dot_count == 1){
    printf("[DECIMAL]: %s (Valor: %f, Linha %d)\n", number_lexema, atof(number_lexema), line_num);
  } else {
    printf("[INTEGER]: %s (Valor: %d, Linha %d)\n", number_lexema, atoi(number_lexema), line_num);
  }
}

void checkString(const char *string_lexema, int line_num){
  printf("[STRING]: %s (Linha %d)\n", string_lexema, line_num);
}

void checkOperator(const char *operator_lexema, int line_num){
  for(int i = 0; i < ACTUAL_NUM_VALID_OPERATORS; i++){
    if(strcmp(VALID_OPERATORS[i].word, operator_lexema) == 0){
      printf("[OPERATOR]: %s (Tipo: %d, Linha %d)\n", operator_lexema, VALID_OPERATORS[i].type, line_num);
      return;
    }
  }
  fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador invalido ou nao reconhecido '%s'.\n", line_num, operator_lexema);
  counters_lexico_internal.error_line = line_num;
}

static char* build_lexema_dynamically(
    const char **line_cursor,
    int line_num,
    int (*char_belongs_to_lexema_rule)(int char_code, int current_lex_len, const char* current_lex_buffer),
    const char* prefix
) {
    const char *current_pos = *line_cursor;
    size_t buffer_len = 32;
    char *lex_buffer = (char*)managed_malloc(buffer_len);
    size_t k = 0;
    if (prefix) {
        strcpy(lex_buffer, prefix);
        k = strlen(prefix);
    }

    while (*current_pos != '\0' && char_belongs_to_lexema_rule((unsigned char)*current_pos, k, lex_buffer)) {
        if (k >= buffer_len - 1) {
            buffer_len *= 2;
            char *new_buffer = (char*)managed_realloc(lex_buffer, buffer_len);
            lex_buffer = new_buffer;
        }
        lex_buffer[k++] = *current_pos;
        current_pos++;
    }
    lex_buffer[k] = '\0';
    *line_cursor = current_pos;
    return lex_buffer;
}

int var_char_rule(int c, int len, const char* buff) {
    if (len == 0 && c == EXCLAMATION) return 1;
    if (len > 0 && buff[0] == EXCLAMATION) {
      if (len == 1) return islower(c);
      return isalnum(c);
    }
    return 0;
}

int func_name_char_rule(int c, int len, const char* buff) {
    if (len == 0) return isalnum(c);
    return isalnum(c);
}

int word_char_rule(int c, int len, const char* buff) {
    if (len == 0) return isalpha(c);
    return isalnum(c);
}

int num_char_rule(int c, int len, const char* buff) {
    if (isdigit(c)) return 1;
    if (c == PERIOD) {
        for(int i=0; i < len; ++i) {
            if(buff[i] == PERIOD) return 0;
        }
        return 1;
    }
    return 0;
}


BalanceCounters checkLine(const char *line_text, int line_num, int currently_in_string_global_status){
  const char *cursor = line_text;
  char *lexema_str = NULL;

  init_balance_counters(&counters_lexico_internal); // Usa a global static, zerando para a linha atual
  counters_lexico_internal.is_inside_string_at_line_end = currently_in_string_global_status;

  while(*cursor != '\0'){
    const char *cursor_at_iteration_start = cursor;

    if (!counters_lexico_internal.is_inside_string_at_line_end) {
        if (isspace((unsigned char)*cursor)) {
            while(*cursor != '\0' && isspace((unsigned char)*cursor)) {
                cursor++;
            }
            if (cursor != cursor_at_iteration_start) {
                continue;
            }
        }
    }
    if (*cursor == '\0') break;

    // Primeiro, tratar o estado de estar dentro de uma string ou encontrar aspas
    if (*cursor == DQUOTE) {
        if (counters_lexico_internal.is_inside_string_at_line_end) { // Se já está em string, esta aspa fecha
            counters_lexico_internal.is_inside_string_at_line_end = 0;
        } else { // Se não está em string, esta aspa abre
            counters_lexico_internal.is_inside_string_at_line_end = 1;
        }
        printf("[DELIMITADOR_ASPAS]: \" (Linha %d)\n", line_num);
        cursor++;
        continue;
    }

    // Se estiver dentro de uma string, consumir caracteres como conteúdo da string
    if (counters_lexico_internal.is_inside_string_at_line_end) {
        // Aqui, estamos consumindo o conteúdo da string.
        // A lógica para construir o lexema da string para `checkString` é complexa
        // se ela cruza linhas ou contém escapes. Por ora, apenas avançamos.
        // `checkString` não será chamada com o conteúdo completo se cruzar linhas.
        // printf("DEBUG: String content char '%c'\n", *cursor);
        cursor++;
        continue;
    }

    // Se não estamos em string (e não é uma aspa), processar outros tokens e balanceamento
    switch (*cursor) {
        case OPEN_PAREN: counters_lexico_internal.parentheses++; break;
        case CLOSE_PAREN: counters_lexico_internal.parentheses--; break;
        case OPEN_BRACE: counters_lexico_internal.curly_braces++; break;
        case CLOSE_BRACE: counters_lexico_internal.curly_braces--; break;
        case OPEN_BRACKET: counters_lexico_internal.square_brackets++; break;
        case CLOSE_BRACKET: counters_lexico_internal.square_brackets--; break;
    }
    // A verificação de saldo < 0 foi movida para main.c

    if (strchr("(){}[],;", *cursor)) {
        char delim_char_str[2] = {*cursor, '\0'};
        printf("[DELIMITADOR]: %s (Linha %d)\n", delim_char_str, line_num);
        cursor++;
        if (counters_lexico_internal.error_line > 0) return counters_lexico_internal;
        continue;
    }

    if (*cursor == EXCLAMATION) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, var_char_rule, NULL);
      checkVariable(lexema_str, line_num);
      managed_free(lexema_str);
      if (counters_lexico_internal.error_line > 0) return counters_lexico_internal;
      continue;
    }
    else if (*cursor == '_' && *(cursor+1) == '_') {
      const char* prefix_func = "__";
      cursor += 2;
      lexema_str = build_lexema_dynamically(&cursor, line_num, func_name_char_rule, prefix_func);
      checkFunction(lexema_str, line_num);
      managed_free(lexema_str);
      if (counters_lexico_internal.error_line > 0) return counters_lexico_internal;
      continue;
    }
    else if (isalpha((unsigned char)*cursor)) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, word_char_rule, NULL);
      checkReservedWord(lexema_str, line_num);
      managed_free(lexema_str);
      if (counters_lexico_internal.error_line > 0) return counters_lexico_internal;
      continue;
    }
    else if (isdigit((unsigned char)*cursor) || (*cursor == PERIOD && isdigit((unsigned char)*(cursor+1)))) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, num_char_rule, NULL);
      checkNumber(lexema_str, line_num);
      managed_free(lexema_str);
      if (counters_lexico_internal.error_line > 0) return counters_lexico_internal;
      continue;
    }
    else if (strchr("+-*/^=<>&|", *cursor)) {
        char op_buffer[3] = {0,0,0};
        op_buffer[0] = *cursor;
        cursor++;
        if ( (op_buffer[0] == '=' && *cursor == '=') ||
             (op_buffer[0] == '<' && (*cursor == '>' || *cursor == '=')) ||
             (op_buffer[0] == '>' && *cursor == '=') ||
             (op_buffer[0] == '&' && *cursor == '&') ||
             (op_buffer[0] == '|' && *cursor == '|') )
        {
            if ((op_buffer[0] == '=' && (*cursor == '<' || *cursor == '>')) ) {
                 fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador invalido '%c%c' (Regra 3.2.3).\n", line_num, op_buffer[0], *cursor);
                 counters_lexico_internal.error_line = line_num; return counters_lexico_internal;
            }
            if (op_buffer[0] == '<' && *cursor == '>') {
                 op_buffer[1] = *cursor; cursor++;
            } else if (op_buffer[0] == '>' && *cursor == '<') {
                 fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador invalido '%c%c' (Regra 3.2.3).\n", line_num, op_buffer[0], *cursor);
                 counters_lexico_internal.error_line = line_num; return counters_lexico_internal;
            } else {
                 op_buffer[1] = *cursor; cursor++;
            }
        }
        checkOperator(op_buffer, line_num);
        if (counters_lexico_internal.error_line > 0) return counters_lexico_internal;
        continue;
    }

    if (cursor == cursor_at_iteration_start && *cursor != '\0') {
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere '%c' (ASCII: %d) nao reconhecido.\n", line_num, *cursor, (int)*cursor);
        counters_lexico_internal.error_line = line_num;
        return counters_lexico_internal;
    }
  }
  return counters_lexico_internal;
}

void invalidToken() {
    fprintf(stderr, "ERRO: Funcao invalidToken chamada - token invalido.\n");
    if (counters_lexico_internal.error_line == 0) counters_lexico_internal.error_line = -1;
}
