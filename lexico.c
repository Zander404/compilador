#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"
#include "token_list.h"

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
  (void)num_line; // Evita warning de não utilizado, já que o erro é pego em main
  int i = 0;
  int opened_brackets = 0;
  while(line_content[i] != '\0'){
    if(line_content[i] == OPEN_PAREN){
      opened_brackets++;
    } else if(line_content[i] == CLOSE_PAREN){
      opened_brackets--;
    }
    if (opened_brackets < 0) {
        return -1;
    }
    i++;
  }
  if(opened_brackets == 0) return 1;
  return -1;
}

static TokenType get_variable_token_type(const char* variable_lexema, int line_num, BalanceCounters* counters){
  if (strlen(variable_lexema) < 2 || variable_lexema[0] != '!' || !islower((unsigned char)variable_lexema[1])) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Variavel '%s' mal formada (deve ser !<minuscula>[alnum]*).\n", line_num, variable_lexema);
    counters->error_line = line_num;
    return TK_ERRO;
  }
  for (size_t i = 2; i < strlen(variable_lexema); ++i) {
    if (!isalnum((unsigned char)variable_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em nome de variavel '%s'.\n", line_num, variable_lexema[i], variable_lexema);
      counters->error_line = line_num;
      return TK_ERRO;
    }
  }
  return TK_VARIAVEL;
}

static TokenType get_function_token_type(const char* function_lexema, int line_num, BalanceCounters* counters){
  if (strlen(function_lexema) < 3 || function_lexema[0] != '_' || function_lexema[1] != '_' || !isalnum((unsigned char)function_lexema[2])) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Nome de funcao '%s' mal formado (deve ser __<alnum>[alnum]*).\n", line_num, function_lexema);
    counters->error_line = line_num;
    return TK_ERRO;
  }
  for (size_t i = 3; i < strlen(function_lexema); ++i) {
    if (!isalnum((unsigned char)function_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em nome de funcao '%s'.\n", line_num, function_lexema[i], function_lexema);
      counters->error_line = line_num;
      return TK_ERRO;
    }
  }
  return TK_IDENTIFICADOR;
}

static TokenType get_reserved_or_identifier_token_type(const char* word_lexema, int line_num, BalanceCounters* counters){
  for(int i = 0; i < ACTUAL_NUM_RESERVED_WORDS; i++){
    if (strcmp(reserved_words[i].word, word_lexema) == 0){
      return reserved_words[i].type;
    }
  }
  if (strlen(word_lexema) == 0 || !isalpha((unsigned char)word_lexema[0])) { // Identificador não pode ser vazio
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Identificador '%s' mal formado (deve iniciar com letra).\n", line_num, word_lexema);
      counters->error_line = line_num;
      return TK_ERRO;
  }
  for (size_t i = 1; i < strlen(word_lexema); ++i) {
      if (!isalnum((unsigned char)word_lexema[i])) {
          fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em identificador '%s'.\n", line_num, word_lexema[i], word_lexema);
          counters->error_line = line_num;
          return TK_ERRO;
      }
  }
  return TK_IDENTIFICADOR;
}

static TokenType get_number_token_type(const char *number_lexema, int line_num, BalanceCounters* counters){
  int dot_count = 0;
  int has_digits = 0;

  if (strlen(number_lexema) == 0) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Tentativa de processar numero vazio.\n", line_num);
      counters->error_line = line_num;
      return TK_ERRO;
  }

  for(size_t i = 0; i < strlen(number_lexema); ++i) {
    if (number_lexema[i] == PERIOD) {
      dot_count++;
    } else if (!isdigit((unsigned char)number_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em numero '%s'.\n", line_num, number_lexema[i], number_lexema);
      counters->error_line = line_num;
      return TK_ERRO;
    } else {
      has_digits = 1;
    }
  }

  if (!has_digits && dot_count > 0) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto decimal '%s' sem digitos.\n", line_num, number_lexema);
      counters->error_line = line_num;
      return TK_ERRO;
  }

  if (dot_count > 1) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Multiplos pontos decimais em numero '%s'.\n", line_num, number_lexema);
    counters->error_line = line_num;
    return TK_ERRO;
  }

  if(dot_count == 1){
    // Validação para Regra 2.3.3.2: "Haverá a necessidade de especificar a quantidade de caracteres antes e depois do símbolo separador"
    // Permitindo .5 e 5. por enquanto, pois atof aceita. Um "." isolado já é pego por !has_digits.
    char *dot_pos = strchr(number_lexema, PERIOD);
    int digit_before = 0;
    int digit_after = 0;
    if (dot_pos > number_lexema) { // Verifica se há algo antes do ponto
        for(const char *p = number_lexema; p < dot_pos; p++) if(isdigit(*p)) digit_before = 1;
    }
    if (*(dot_pos + 1) != '\0') { // Verifica se há algo depois do ponto
         for(const char *p = dot_pos + 1; *p != '\0'; p++) if(isdigit(*p)) digit_after = 1;
    }
    if(!digit_before && !digit_after && strlen(number_lexema) == 1) { // Só "."
         fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto decimal isolado '%s'.\n", line_num, number_lexema);
         counters->error_line = line_num;
         return TK_ERRO;
    }
    // Se a regra for estrita de ter dígitos dos dois lados:
    // if (!digit_before || !digit_after) {
    //      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Decimal '%s' deve ter digitos antes e depois do ponto.\n", line_num, number_lexema);
    //      counters->error_line = line_num;
    //      return TK_ERRO;
    // }
    return TK_LITERAL_DEC;
  } else { // dot_count == 0
    if (!has_digits) {
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Numero inteiro invalido '%s' (sem digitos).\n", line_num, number_lexema);
        counters->error_line = line_num;
        return TK_ERRO;
    }
    return TK_LITERAL_INT;
  }
}

static TokenType get_operator_token_type(const char *operator_lexema, int line_num, BalanceCounters* counters){
  for(int i = 0; i < ACTUAL_NUM_VALID_OPERATORS; i++){
    if(strcmp(VALID_OPERATORS[i].word, operator_lexema) == 0){
      return VALID_OPERATORS[i].type;
    }
  }
  fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador desconhecido ou malformado '%s'.\n", line_num, operator_lexema);
  counters->error_line = line_num;
  return TK_ERRO;
}

static char* build_lexema_dynamically(
    const char **line_cursor,
    int line_num,
    int (*char_belongs_to_lexema_rule)(int char_code, int current_lex_len, const char* current_lex_buffer),
    const char* prefix
) {
    (void)line_num; // Evitar warning de não utilizado
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
    (void)buff; // Evitar warning
    if (len == 0 && c == EXCLAMATION) return 1;
    if (len > 0 ) { // buff[0] deve ser '!' se len > 0 e esta regra for chamada corretamente
      if (len == 1) return islower(c);
      return isalnum(c);
    }
    return 0;
}

int func_name_char_rule(int c, int len, const char* buff) {
    (void)buff; // Evitar warning
    if (len == 0) return isalnum(c);
    return isalnum(c);
}

int word_char_rule(int c, int len, const char* buff) {
    (void)buff; // Evitar warning
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

BalanceCounters checkLine(TokenList *list, const char *line_text, int line_num, int currently_in_string_global_status){
  const char *cursor = line_text;
  char *lexema_str = NULL;
  BalanceCounters current_line_counters;

  init_balance_counters(&current_line_counters);
  current_line_counters.is_inside_string_at_line_end = currently_in_string_global_status;

  while(*cursor != '\0'){
    const char *cursor_at_iteration_start = cursor;

    if (!current_line_counters.is_inside_string_at_line_end) {
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

    if (*cursor == DQUOTE) {
        char aspas_lexema[2] = {'"', '\0'};
        add_token_to_list(list, TK_DELIM_ASPAS_DUPLAS, aspas_lexema, line_num);

        if (current_line_counters.is_inside_string_at_line_end) {
            current_line_counters.is_inside_string_at_line_end = 0;
        } else {
            current_line_counters.is_inside_string_at_line_end = 1;
            const char* string_content_start = cursor + 1;
            const char* string_content_end = string_content_start;
            while(*string_content_end != '\0' && *string_content_end != DQUOTE) {
                string_content_end++;
            }
            if (string_content_end > string_content_start) {
                size_t content_len = string_content_end - string_content_start;
                char* content_lexema = (char*)managed_malloc(content_len + 1);
                strncpy(content_lexema, string_content_start, content_len);
                content_lexema[content_len] = '\0';
                add_token_to_list(list, TK_LITERAL_STRING, content_lexema, line_num);
                managed_free(content_lexema);
            }
            cursor = string_content_end;
        }
        if (cursor == cursor_at_iteration_start) cursor++;
        continue;
    }

    if (current_line_counters.is_inside_string_at_line_end) {
        cursor++;
        continue;
    }

    TokenType current_token_type = TK_ERRO;
    char single_char_lexema[2] = {0,0};

    switch (*cursor) {
        case OPEN_PAREN: current_line_counters.parentheses++; current_token_type = TK_DELIM_ABRE_PAREN; break;
        case CLOSE_PAREN: current_line_counters.parentheses--; current_token_type = TK_DELIM_FECHA_PAREN; break;
        case OPEN_BRACE: current_line_counters.curly_braces++; current_token_type = TK_DELIM_ABRE_CHAVES; break;
        case CLOSE_BRACE: current_line_counters.curly_braces--; current_token_type = TK_DELIM_FECHA_CHAVES; break;
        case OPEN_BRACKET: current_line_counters.square_brackets++; current_token_type = TK_DELIM_ABRE_COLCH; break;
        case CLOSE_BRACKET: current_line_counters.square_brackets--; current_token_type = TK_DELIM_FECHA_COLCH; break;
        case SEMICOLON: current_token_type = TK_DELIM_PONTO_VIRGULA; break;
        case COMMA: current_token_type = TK_DELIM_VIRGULA; break;
    }

    if (current_token_type != TK_ERRO && strchr("(){}[],;", *cursor) ) {
        single_char_lexema[0] = *cursor;
        add_token_to_list(list, current_token_type, single_char_lexema, line_num);
        cursor++;
        continue;
    }

    if (*cursor == EXCLAMATION) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, var_char_rule, NULL);
      current_token_type = get_variable_token_type(lexema_str, line_num, &current_line_counters);
      if(current_token_type != TK_ERRO) add_token_to_list(list, current_token_type, lexema_str, line_num);
      managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (*cursor == '_' && *(cursor+1) == '_') {
      const char* prefix_func = "__";
      cursor += 2;
      lexema_str = build_lexema_dynamically(&cursor, line_num, func_name_char_rule, prefix_func);
      current_token_type = get_function_token_type(lexema_str, line_num, &current_line_counters);
      if(current_token_type != TK_ERRO) add_token_to_list(list, current_token_type, lexema_str, line_num);
      managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (isalpha((unsigned char)*cursor)) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, word_char_rule, NULL);
      current_token_type = get_reserved_or_identifier_token_type(lexema_str, line_num, &current_line_counters);
      if(current_token_type != TK_ERRO) add_token_to_list(list, current_token_type, lexema_str, line_num);
      managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (isdigit((unsigned char)*cursor) || (*cursor == PERIOD && isdigit((unsigned char)*(cursor+1)))) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, num_char_rule, NULL);
      current_token_type = get_number_token_type(lexema_str, line_num, &current_line_counters);
      if(current_token_type != TK_ERRO) add_token_to_list(list, current_token_type, lexema_str, line_num);
      managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (*cursor == PERIOD) {
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto '.' isolado ou mal utilizado.\n", line_num);
        current_line_counters.error_line = line_num;
        return current_line_counters;
    }
    else if (strchr("+-*/^=<>&|", *cursor)) {
        char op_buffer[3] = {0,0,0};
        char first_op_char = *cursor;
        char second_op_char = *(cursor + 1);

        op_buffer[0] = first_op_char;

        if ((first_op_char == '=' && (second_op_char == '<' || second_op_char == '>')) ||
            (first_op_char == '>' && second_op_char == '<') ||
            (first_op_char == '<' && second_op_char == '!') ) // Exemplo de outra combinação inválida
        {
             fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador binario invalido '%c%c' (Regra 3.2.3).\n", line_num, first_op_char, second_op_char);
             current_line_counters.error_line = line_num; return current_line_counters;
        }

        if ( (first_op_char == '=' && second_op_char == '=') ||
             (first_op_char == '<' && (second_op_char == '>' || second_op_char == '=')) ||
             (first_op_char == '>' && second_op_char == '=') ||
             (first_op_char == '&' && second_op_char == '&') ||
             (first_op_char == '|' && second_op_char == '|') )
        {
            char temp_two_char_op[3] = {first_op_char, second_op_char, '\0'};
            int is_valid_two_char = 0;
            for(int i = 0; i < ACTUAL_NUM_VALID_OPERATORS; i++){
                if(strlen(VALID_OPERATORS[i].word) == 2 && strcmp(VALID_OPERATORS[i].word, temp_two_char_op) == 0){
                    is_valid_two_char = 1;
                    break;
                }
            }
            if (is_valid_two_char) {
                op_buffer[1] = second_op_char;
            }
        }

        if(op_buffer[1] != '\0') cursor += 2;
        else cursor++;

        current_token_type = get_operator_token_type(op_buffer, line_num, &current_line_counters);
        if(current_token_type != TK_ERRO) add_token_to_list(list, current_token_type, op_buffer, line_num);

        if (current_line_counters.error_line > 0) return current_line_counters;
        continue;
    }

    if (cursor == cursor_at_iteration_start && *cursor != '\0') {
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere '%c' (ASCII: %d) nao reconhecido.\n", line_num, *cursor, (int)*cursor);
        current_line_counters.error_line = line_num;
        return current_line_counters;
    }
  }
  return current_line_counters;
}
