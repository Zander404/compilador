#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"
#include "symtab.h"

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
  (void)num_line;
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

static TokenType validate_and_get_variable_type(const char* variable_lexema, int line_num, BalanceCounters* counters){
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

static TokenType validate_and_get_function_name_type(const char* function_lexema, int line_num, BalanceCounters* counters){
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

static TokenType validate_and_get_reserved_or_identifier_type(const char* word_lexema, int line_num, BalanceCounters* counters){
  for(int i = 0; i < ACTUAL_NUM_RESERVED_WORDS; i++){
    if (strcmp(reserved_words[i].word, word_lexema) == 0){
      return reserved_words[i].type;
    }
  }
  if (strlen(word_lexema) == 0 || !isalpha((unsigned char)word_lexema[0])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Identificador '%s' mal formado (deve iniciar com letra e nao ser vazio).\n", line_num, word_lexema);
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

static TokenType validate_and_get_number_type(const char *number_lexema, int line_num, BalanceCounters* counters){
  int dot_count = 0;
  int has_digits = 0;
  if (strlen(number_lexema) == 0) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Tentativa de processar numero vazio.\n", line_num);
      counters->error_line = line_num; return TK_ERRO;
  }
  for(size_t i = 0; i < strlen(number_lexema); ++i) {
    if (number_lexema[i] == PERIOD) dot_count++;
    else if (!isdigit((unsigned char)number_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em numero '%s'.\n", line_num, number_lexema[i], number_lexema);
      counters->error_line = line_num; return TK_ERRO;
    } else has_digits = 1;
  }
  if (!has_digits && dot_count > 0) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto decimal '%s' sem digitos.\n", line_num, number_lexema);
      counters->error_line = line_num; return TK_ERRO;
  }
  if (dot_count > 1) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Multiplos pontos decimais em numero '%s'.\n", line_num, number_lexema);
    counters->error_line = line_num; return TK_ERRO;
  }
  if(dot_count == 1){
    char *dot_pos = strchr(number_lexema, PERIOD);
    int digit_before = 0; int digit_after = 0;
    if (dot_pos > number_lexema) { for(const char *p = number_lexema; p < dot_pos; p++) if(isdigit((unsigned char)*p)) digit_before = 1; }
    if (*(dot_pos + 1) != '\0') { for(const char *p = dot_pos + 1; *p != '\0'; p++) if(isdigit((unsigned char)*p)) digit_after = 1; }
    if(!digit_before && !digit_after && strlen(number_lexema) == 1) {
         fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto decimal isolado '%s'.\n", line_num, number_lexema);
         counters->error_line = line_num; return TK_ERRO;
    }
    return TK_LITERAL_DEC;
  } else {
    if (!has_digits) {
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Numero inteiro invalido '%s' (sem digitos).\n", line_num, number_lexema);
        counters->error_line = line_num; return TK_ERRO;
    }
    return TK_LITERAL_INT;
  }
}

static TokenType validate_and_get_operator_type(const char *operator_lexema, int line_num, BalanceCounters* counters){
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
    (void)line_num;
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
    (void)buff;
    if (len == 0 && c == EXCLAMATION) return 1;
    if (len > 0 ) {
      if (len == 1) return islower(c);
      return isalnum(c);
    }
    return 0;
}

int func_name_char_rule(int c, int len, const char* buff) {
    (void)buff;
    if (len == 0) return isalnum(c);
    return isalnum(c);
}

int word_char_rule(int c, int len, const char* buff) {
    (void)buff;
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

static void skip_spaces(const char **cursor) {
    while (**cursor != '\0' && isspace((unsigned char)**cursor)) {
        (*cursor)++;
    }
}

BalanceCounters checkLine(SymbolTable *table, const char *line_text, int line_num, int currently_in_string_global_status){
  const char *cursor = line_text;
  char *lexema_str = NULL;
  BalanceCounters current_line_counters;

  char *first_word = NULL;
  char *var_name_lex = NULL;
  char *dimensions_lex = NULL;

  init_balance_counters(&current_line_counters);
  current_line_counters.is_inside_string_at_line_end = currently_in_string_global_status;

  while(*cursor != '\0'){
    const char *cursor_at_iteration_start = cursor;

    if (!current_line_counters.is_inside_string_at_line_end) {
        skip_spaces(&cursor);
        if (*cursor == '/' && *(cursor + 1) == '/') {
            break;
        }
        if (cursor != cursor_at_iteration_start && *cursor == '\0') break;
        if (cursor != cursor_at_iteration_start && *cursor != '\0') continue;
    }
    if (*cursor == '\0') break;

    if (*cursor == DQUOTE) {
        if (current_line_counters.is_inside_string_at_line_end) {
            current_line_counters.is_inside_string_at_line_end = 0;
        } else {
            current_line_counters.is_inside_string_at_line_end = 1;
            const char* p_read = cursor + 1;
            while(*p_read != '\0' && *p_read != DQUOTE) {
                if (*p_read == '\\') {
                    p_read++;
                    if (*p_read == '\0') {
                        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Sequencia de escape '\\' incompleta no final da string/linha.\n", line_num);
                        current_line_counters.error_line = line_num; return current_line_counters;
                    }
                    switch (*p_read) {
                        case '"': case '\\': break;
                        default:
                            fprintf(stderr, "[LINHA %d] ERRO LEXICO: Sequencia de escape invalida '\\%c'.\n", line_num, *p_read);
                            current_line_counters.error_line = line_num; return current_line_counters;
                    }
                }
                p_read++;
            }
            cursor = p_read;
        }
        if (cursor == cursor_at_iteration_start) cursor++;
        continue;
    }

    if (current_line_counters.is_inside_string_at_line_end) {
        cursor++; continue;
    }

    int is_single_char_delim_token = 0;
    switch (*cursor) {
        case OPEN_PAREN: current_line_counters.parentheses++; is_single_char_delim_token=1; break;
        case CLOSE_PAREN: current_line_counters.parentheses--; is_single_char_delim_token=1; break;
        case OPEN_BRACE: current_line_counters.curly_braces++; is_single_char_delim_token=1; break;
        case CLOSE_BRACE: current_line_counters.curly_braces--; is_single_char_delim_token=1; break;
        case OPEN_BRACKET: current_line_counters.square_brackets++; is_single_char_delim_token=1; break;
        case CLOSE_BRACKET: current_line_counters.square_brackets--; is_single_char_delim_token=1; break;
        case SEMICOLON: is_single_char_delim_token=1; break;
        case COMMA: is_single_char_delim_token=1; break;
    }
    if (is_single_char_delim_token) {
        cursor++; continue;
    }

    const char* potential_declaration_start_cursor = cursor;
    if (isalpha((unsigned char)*potential_declaration_start_cursor)) {
        const char* lookahead_for_decl = potential_declaration_start_cursor;
        first_word = build_lexema_dynamically(&lookahead_for_decl, line_num, word_char_rule, NULL);

        const char* cursor_after_keyword = lookahead_for_decl;
        skip_spaces(&cursor_after_keyword);

        DataType dt = TYPE_NONE;
        SymbolCategory cat = SYM_NONE;
        int declaration_processed_successfully = 0;
        int is_declaration_keyword = 0;

        if (strcmp(first_word, "funcao") == 0) { cat = SYM_FUNCAO; dt = TYPE_FUNCAO_DEF; is_declaration_keyword = 1;}
        else if (strcmp(first_word, "inteiro") == 0) { cat = SYM_VARIAVEL; dt = TYPE_INTEIRO; is_declaration_keyword = 1;}
        else if (strcmp(first_word, "texto") == 0) { cat = SYM_VARIAVEL; dt = TYPE_TEXTO; is_declaration_keyword = 1;}
        else if (strcmp(first_word, "decimal") == 0) { cat = SYM_VARIAVEL; dt = TYPE_DECIMAL; is_declaration_keyword = 1;}
        else if (strcmp(first_word, "principal") == 0) {
             is_declaration_keyword = 1;
             const char* p_check = cursor_after_keyword;
             if (*p_check == OPEN_PAREN) {
                 p_check++; skip_spaces(&p_check);
                 if (*p_check == CLOSE_PAREN) {
                     p_check++; skip_spaces(&p_check);
                     if (*p_check == OPEN_BRACE) {
                        add_symbol(table, "principal", SYM_FUNCAO, TYPE_FUNCAO_DEF, NULL, line_num, "global");
                        cursor = p_check; // Avança cursor principal até o {
                        declaration_processed_successfully = 1;
                     }
                 }
             }
        }

        if (declaration_processed_successfully && strcmp(first_word, "principal")==0) { // Caso especial principal
             if(first_word) { managed_free(first_word); first_word = NULL; }
             continue;
        }

        if (cat == SYM_FUNCAO && dt == TYPE_FUNCAO_DEF && strcmp(first_word, "funcao")==0) {
            if (*cursor_after_keyword == '_' && *(cursor_after_keyword+1) == '_') {
                lookahead_for_decl = cursor_after_keyword + 2;
                var_name_lex = build_lexema_dynamically(&lookahead_for_decl, line_num, func_name_char_rule, "__");
                if (validate_and_get_function_name_type(var_name_lex, line_num, &current_line_counters) != TK_ERRO) {
                    add_symbol(table, var_name_lex, SYM_FUNCAO, TYPE_FUNCAO_DEF, NULL, line_num, "global");
                    cursor = lookahead_for_decl;
                    declaration_processed_successfully = 1;
                }
                if(var_name_lex) { managed_free(var_name_lex); var_name_lex = NULL; }
            } else {
                fprintf(stderr, "[LINHA %d] ERRO LEXICO: Nome de funcao esperado apos 'funcao' (deve ser __<alnum>...). Encontrado a partir de: '%.10s'\n", line_num, cursor_after_keyword);
                current_line_counters.error_line = line_num;
            }
        } else if (cat == SYM_VARIAVEL) {
            int first_var_in_list = 1;
            lookahead_for_decl = cursor_after_keyword;

            while(1) {
                if(current_line_counters.error_line > 0) {declaration_processed_successfully = 0; break;}
                skip_spaces(&lookahead_for_decl);

                if (!first_var_in_list) {
                    if (*lookahead_for_decl == ',') {
                        lookahead_for_decl++;
                        skip_spaces(&lookahead_for_decl);
                    } else if (*lookahead_for_decl == ';') {
                        declaration_processed_successfully = 1; break;
                    } else {
                        fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Esperado ',' ou ';' em declaracao de variavel. Encontrado '%c'\n", line_num, *lookahead_for_decl);
                        current_line_counters.error_line = line_num; declaration_processed_successfully = 0; break;
                    }
                }

                if (*lookahead_for_decl == '!') {
                    var_name_lex = build_lexema_dynamically(&lookahead_for_decl, line_num, var_char_rule, NULL);
                    if (validate_and_get_variable_type(var_name_lex, line_num, &current_line_counters) != TK_ERRO) {
                        dimensions_lex = NULL;
                        const char* dim_check_cursor = lookahead_for_decl;
                        skip_spaces(&dim_check_cursor);

                        if ((dt == TYPE_TEXTO || dt == TYPE_DECIMAL) && *dim_check_cursor == '[') {
                            const char* start_dim_lex = dim_check_cursor;
                            dim_check_cursor++;
                            while(*dim_check_cursor != '\0' && *dim_check_cursor != ']') dim_check_cursor++;
                            if (*dim_check_cursor == ']') {
                                dim_check_cursor++;
                                size_t dim_len = dim_check_cursor - start_dim_lex;
                                dimensions_lex = (char*)managed_malloc(dim_len + 1);
                                strncpy(dimensions_lex, start_dim_lex, dim_len);
                                dimensions_lex[dim_len] = '\0';
                                lookahead_for_decl = dim_check_cursor;
                            } else {
                                fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Colchete de dimensao '[' nao fechado para variavel '%s'.\n", line_num, var_name_lex);
                                current_line_counters.error_line = line_num;
                                if(dimensions_lex) { managed_free(dimensions_lex); dimensions_lex = NULL; }
                                if(var_name_lex) { managed_free(var_name_lex); var_name_lex = NULL; }
                                declaration_processed_successfully = 0; break;
                            }
                        }
                        // Consumir atribuição inicial, se houver
                        const char* assign_check_cursor = lookahead_for_decl;
                        skip_spaces(&assign_check_cursor);
                        if (*assign_check_cursor == '=') {
                            assign_check_cursor++; // Pula '='
                            skip_spaces(&assign_check_cursor);
                            // Simplificado: consome até a próxima vírgula ou ponto e vírgula
                            // Uma análise de expressão real seria necessária aqui.
                            while(*assign_check_cursor != '\0' && *assign_check_cursor != ',' && *assign_check_cursor != ';') {
                                // Se encontrar delimitadores de bloco de expressão, precisa tratar
                                if (*assign_check_cursor == '(') { // Ex: chamada de função na atribuição
                                    int paren_balance = 1;
                                    assign_check_cursor++;
                                    while(*assign_check_cursor != '\0' && paren_balance > 0) {
                                        if (*assign_check_cursor == '(') paren_balance++;
                                        else if (*assign_check_cursor == ')') paren_balance--;
                                        assign_check_cursor++;
                                    }
                                    if (paren_balance != 0) {
                                        fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Parenteses desbalanceados em expressao de atribuicao para '%s'.\n", line_num, var_name_lex);
                                        current_line_counters.error_line = line_num; declaration_processed_successfully=0; break;
                                    }
                                } else {
                                   assign_check_cursor++;
                                }
                            }
                            lookahead_for_decl = assign_check_cursor;
                        }
                        add_symbol(table, var_name_lex, SYM_VARIAVEL, dt, dimensions_lex, line_num, "global");
                        if(dimensions_lex) { managed_free(dimensions_lex); dimensions_lex = NULL; }
                    }
                    if(var_name_lex) { managed_free(var_name_lex); var_name_lex = NULL; }
                    if (current_line_counters.error_line > 0) {declaration_processed_successfully = 0; break;}
                    first_var_in_list = 0;

                    skip_spaces(&lookahead_for_decl);
                    if (*lookahead_for_decl == ';') { declaration_processed_successfully = 1; break;}
                    if (*lookahead_for_decl != ',') {
                        if (*lookahead_for_decl == '\0') {
                            fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Esperado ',' ou ';' apos declaracao da variavel. Encontrado 'FIM DE LINHA'\n", line_num);
                        } else {
                            fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Esperado ',' ou ';' apos declaracao da variavel. Encontrado '%c'\n", line_num, *lookahead_for_decl);
                        }
                        current_line_counters.error_line = line_num; declaration_processed_successfully = 0; break;
                    }
                    // Se for vírgula, o loop continua (lookahead_for_decl já está na vírgula ou após)
                } else if (first_var_in_list && *lookahead_for_decl == ';') {
                     fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Nome de variavel esperado apos tipo '%s'.\n", line_num, first_word);
                     current_line_counters.error_line = line_num; declaration_processed_successfully = 0; break;
                }
                else {
                    if(first_var_in_list && *lookahead_for_decl != '\0')
                        fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Nome de variavel esperado (iniciando com '!') apos tipo '%s'. Encontrado '%c'\n", line_num, first_word, *lookahead_for_decl);
                    else if (first_var_in_list && *lookahead_for_decl == '\0')
                         fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Nome de variavel esperado apos tipo '%s'. Fim de linha encontrado.\n", line_num, first_word);
                    else if (!first_var_in_list)
                         fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Nome de variavel esperado (iniciando com '!') apos ','. Encontrado '%c'\n", line_num, *lookahead_for_decl);

                    current_line_counters.error_line = line_num; declaration_processed_successfully = 0; break;
                }
            }
            if (declaration_processed_successfully) cursor = lookahead_for_decl;
        }

        if(first_word) { managed_free(first_word); first_word = NULL; }
        if(current_line_counters.error_line > 0) return current_line_counters;
        if(declaration_processed_successfully && cursor != cursor_at_iteration_start) continue;

        if (!declaration_processed_successfully && is_declaration_keyword) {
            // Foi uma palavra chave de declaração, mas a declaração falhou ou não foi completada.
            // O erro já deve ter sido setado. Retorna.
            return current_line_counters;
        }
        // Se não foi uma keyword de declaração que resultou em processamento, ou se foi 'principal' sem '(){'
        // restaurar o cursor para que o token seja processado pelas regras genéricas abaixo.
        if (!declaration_processed_successfully) {
             cursor = potential_declaration_start_cursor;
        }
    }

    if (*cursor == EXCLAMATION) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, var_char_rule, NULL);
      validate_and_get_variable_type(lexema_str, line_num, &current_line_counters);
      if(lexema_str) managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (*cursor == '_' && *(cursor+1) == '_') {
      const char* prefix_func = "__";
      cursor += 2;
      lexema_str = build_lexema_dynamically(&cursor, line_num, func_name_char_rule, prefix_func);
      validate_and_get_function_name_type(lexema_str, line_num, &current_line_counters);
      if(lexema_str) managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (isalpha((unsigned char)*cursor)) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, word_char_rule, NULL);
      validate_and_get_reserved_or_identifier_type(lexema_str, line_num, &current_line_counters);
      if(lexema_str) managed_free(lexema_str);
      if (current_line_counters.error_line > 0) return current_line_counters;
      continue;
    }
    else if (isdigit((unsigned char)*cursor) || (*cursor == PERIOD && isdigit((unsigned char)*(cursor+1)))) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, num_char_rule, NULL);
      validate_and_get_number_type(lexema_str, line_num, &current_line_counters);
      if(lexema_str) managed_free(lexema_str);
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
            (first_op_char == '<' && second_op_char == '!') ) {
             fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador binario invalido '%c%c' (Regra 3.2.3).\n", line_num, first_op_char, second_op_char);
             current_line_counters.error_line = line_num; return current_line_counters;
        }
        if ( (first_op_char == '=' && second_op_char == '=') ||
             (first_op_char == '<' && (second_op_char == '>' || second_op_char == '=')) ||
             (first_op_char == '>' && second_op_char == '=') ||
             (first_op_char == '&' && second_op_char == '&') ||
             (first_op_char == '|' && second_op_char == '|') ) {
            char temp_two_char_op[3] = {first_op_char, second_op_char, '\0'};
            int is_valid_two_char = 0;
            for(int i = 0; i < ACTUAL_NUM_VALID_OPERATORS; i++){
                if(strlen(VALID_OPERATORS[i].word) == 2 && strcmp(VALID_OPERATORS[i].word, temp_two_char_op) == 0){
                    is_valid_two_char = 1; break;
                }
            }
            if (is_valid_two_char) op_buffer[1] = second_op_char;
        }
        if(op_buffer[1] != '\0') cursor += 2;
        else cursor++;
        validate_and_get_operator_type(op_buffer, line_num, &current_line_counters);
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
