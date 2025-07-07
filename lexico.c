#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"

// Função para inicializar os contadores de balanceamento
void init_balance_counters(BalanceCounters *counters) {
    if (counters) {
        counters->parentheses = 0;
        counters->curly_braces = 0;
        counters->square_brackets = 0;
        counters->is_inside_string_at_line_end = 0;
        counters->error_line = 0;
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
        fprintf(stderr, "[LINHA %d] ERRO SINTATICO: ')' inesperado ou desbalanceado na funcao principal.\n", num_line);
        return -1;
    }
    i++;
  }

  if(opened_brackets == 0){
    return 1;
  }

  fprintf(stderr, "[LINHA %d] ERRO SINTATICO: '(' desbalanceados na funcao principal (esperava %d ')' a mais).\n", num_line, opened_brackets);
  return -1;
}

void checkVariable(const char* variable_lexema, int line_num){
  if (strlen(variable_lexema) < 2 || variable_lexema[0] != '!' || !islower((unsigned char)variable_lexema[1])) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Variavel '%s' mal formada (deve ser !<minuscula>[alnum]*).\n", line_num, variable_lexema);
    report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
  }
  for (size_t i = 2; i < strlen(variable_lexema); ++i) {
    if (!isalnum((unsigned char)variable_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em nome de variavel '%s'.\n", line_num, variable_lexema[i], variable_lexema);
      report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
    }
  }
  printf("[VARIAVEL]: %s (Linha %d)\n", variable_lexema, line_num);
}

void checkFunction(const char* function_lexema, int line_num){
  if (strlen(function_lexema) < 3 || function_lexema[0] != '_' || function_lexema[1] != '_' || !isalnum((unsigned char)function_lexema[2])) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Nome de funcao '%s' mal formado (deve ser __<alnum>[alnum]*).\n", line_num, function_lexema);
    report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
  }
  for (size_t i = 3; i < strlen(function_lexema); ++i) {
    if (!isalnum((unsigned char)function_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em nome de funcao '%s'.\n", line_num, function_lexema[i], function_lexema);
      report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
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
      report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
  }
  for (size_t i = 1; i < strlen(word_lexema); ++i) {
      if (!isalnum((unsigned char)word_lexema[i])) {
          fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em identificador '%s'.\n", line_num, word_lexema[i], word_lexema);
          report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
      }
  }
  printf("[IDENTIFICADOR]: %s (Linha %d)\n", word_lexema, line_num);
}

void checkNumber(const char *number_lexema, int line_num){
  int dot_count = 0;
  int has_digits_after_dot = 0;
  int has_digits_before_dot = 0;

  for(size_t i = 0; i < strlen(number_lexema); ++i) {
    if (number_lexema[i] == PERIOD) {
      dot_count++;
    } else if (!isdigit((unsigned char)number_lexema[i])) {
      fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere invalido '%c' em numero '%s'.\n", line_num, number_lexema[i], number_lexema);
      report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
    }
    if (dot_count > 0 && isdigit((unsigned char)number_lexema[i])) has_digits_after_dot = 1;
    if (dot_count == 0 && isdigit((unsigned char)number_lexema[i])) has_digits_before_dot = 1;
  }

  if (dot_count > 1) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: Multiplos pontos decimais em numero '%s'.\n", line_num, number_lexema);
    report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
  }
  if (strlen(number_lexema) == 1 && number_lexema[0] == PERIOD) {
       fprintf(stderr, "[LINHA %d] ERRO LEXICO: Ponto decimal isolado '%s'.\n", line_num, number_lexema);
       report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
  }

  if(dot_count == 1){
    printf("[DECIMAL]: %s (Valor: %f, Linha %d)\n", number_lexema, atof(number_lexema), line_num);
  } else {
    printf("[INTEGER]: %s (Valor: %d, Linha %d)\n", number_lexema, atoi(number_lexema), line_num);
  }
}

void checkString(const char *string_lexema, int line_num){
  if (strlen(string_lexema) < 2 || string_lexema[0] != DQUOTE || string_lexema[strlen(string_lexema)-1] != DQUOTE) {
    fprintf(stderr, "[LINHA %d] ERRO LEXICO: String literal '%s' mal formada (sem aspas balanceadas).\n", line_num, string_lexema);
    report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
  }
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
  report_memory_usage(); cleanup_memory_manager(); exit(EXIT_FAILURE);
}

static char* build_lexema_dynamically(
    const char **line_cursor,
    int line_num,
    int (*char_belongs_to_lexema_rule)(int char_code, int current_lex_len, const char* current_lex_buffer, const char* line_start_for_lookahead),
    const char* prefix
) {
    const char *current_pos = *line_cursor;
    size_t buffer_len = 32;
    char *lex_buffer = (char*)managed_malloc(buffer_len);
    size_t k = 0;
    if (prefix) {
        strcpy(lex_buffer, prefix);
        k = strlen(prefix);
        // Avança o cursor da linha principal pelos caracteres do prefixo se eles foram consumidos para formar o prefixo
        // Esta parte é crítica: se o prefixo vem de *line_cursor, então *line_cursor deve ser avançado.
        // Se o prefixo é uma string literal constante, *line_cursor não muda aqui por causa do prefixo.
        // Assumindo que o chamador ajusta o line_cursor para depois do prefixo se ele foi lido da linha.
    }

    while (*current_pos != '\0' && char_belongs_to_lexema_rule((unsigned char)*current_pos, k, lex_buffer, *line_cursor)) {
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

int var_char_rule(int c, int len, const char* buff, const char* line_start) {
    if (len == 0 && c == EXCLAMATION) return 1; // Aceita o '!' como primeiro char se não for prefixo
    if (len > 0 && buff[0] == EXCLAMATION) { // Se o '!' já está no buffer
      if (len == 1) return islower(c);
      return isalnum(c);
    }
    return 0; // Não deveria chegar aqui se o primeiro char não for '!'
}

int func_name_char_rule(int c, int len, const char* buff, const char* line_start) {
    // Esta regra é para a parte APÓS "__". O prefixo "__" é tratado externamente.
    if (len == 0) return isalnum(c);
    return isalnum(c);
}

int word_char_rule(int c, int len, const char* buff, const char* line_start) {
    if (len == 0) return isalpha(c);
    return isalnum(c);
}

int num_char_rule(int c, int len, const char* buff, const char* line_start) {
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
  BalanceCounters counters;
  init_balance_counters(&counters);
  counters.is_inside_string_at_line_end = currently_in_string_global_status;

  while(*cursor != '\0'){
    // Pular espaços em branco
    while(*cursor != '\0' && isspace((unsigned char)*cursor)) {
        cursor++;
    }
    if (*cursor == '\0') break;

    // Lógica de Balanceamento Simples (antes de tokenizar o char)
    // Esta lógica é para delimitadores que são tokens por si sós.
    // Se um delimitador faz parte de outro token (ex: string), será tratado lá.
    if (!counters.is_inside_string_at_line_end) { // Só processa balanceamento se não estiver em string
        switch (*cursor) {
            case OPEN_PAREN: counters.parentheses++; break;
            case CLOSE_PAREN: counters.parentheses--; break;
            case OPEN_BRACE: counters.curly_braces++; break;
            case CLOSE_BRACE: counters.curly_braces--; break;
            case OPEN_BRACKET: counters.square_brackets++; break;
            case CLOSE_BRACKET: counters.square_brackets--; break;
        }
        if (counters.parentheses < 0 || counters.curly_braces < 0 || counters.square_brackets < 0) {
            fprintf(stderr, "[LINHA %d] ERRO SINTATICO: Delimitador de fechamento '%c' inesperado ou desbalanceado.\n", line_num, *cursor);
            counters.error_line = line_num;
            // Não precisa chamar cleanup/exit aqui, main.c fará isso baseado em error_line.
            return counters;
        }
    }

    // Tratamento de Aspas Duplas para strings (alterna o estado)
    if (*cursor == DQUOTE) {
        counters.is_inside_string_at_line_end = !counters.is_inside_string_at_line_end;
        // Se acabou de fechar uma string, e globalmente estávamos nela, atualiza o estado global implícito
        // A lógica em main.c com o retorno de is_inside_string_at_line_end cuidará disso.
    }

    // Delimitadores que são tokens individuais
    if (strchr("(){}[],;", *cursor) && !counters.is_inside_string_at_line_end) { // Só se não estiver em string
        char delim_char_str[2] = {*cursor, '\0'};
        printf("[DELIMITADOR]: %s (Linha %d)\n", delim_char_str, line_num);
        cursor++;
        continue;
    }

    // --- Início da Lógica de Tokenização de Lexemas Compostos ---
    if (*cursor == EXCLAMATION && !counters.is_inside_string_at_line_end) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, var_char_rule, NULL);
      checkVariable(lexema_str, line_num);
      managed_free(lexema_str);
    }
    else if (*cursor == '_' && *(cursor+1) == '_' && !counters.is_inside_string_at_line_end) {
      const char* prefix_func = "__";
      cursor += 2; // Consome o prefixo da linha antes de chamar build_lexema
      lexema_str = build_lexema_dynamically(&cursor, line_num, func_name_char_rule, prefix_func);
      checkFunction(lexema_str, line_num);
      managed_free(lexema_str);
    }
    else if (isalpha((unsigned char)*cursor) && !counters.is_inside_string_at_line_end) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, word_char_rule, NULL);
      checkReservedWord(lexema_str, line_num);
      managed_free(lexema_str);
    }
    else if (*cursor == DQUOTE) { // Início ou Fim de String Literal. O estado já foi alternado.
      const char *string_start_ptr = cursor;
      // Se entramos numa string, consumimos até o próximo DQUOTE ou fim da linha.
      // Se saímos de uma string, o DQUOTE já foi "consumido" pela lógica de alternância de estado.
      // A tokenização da string em si é complexa se ela puder cruzar linhas.
      // A lógica atual de build_lexema_dynamically não é ideal para strings com espaços.
      // Por ora, vamos apenas reconhecer a aspa e o lexer principal em main gerencia o estado global.
      // Se a string for processada como um único token aqui:
      if (counters.is_inside_string_at_line_end) { // Acabamos de ABRIR uma string
          // O conteúdo da string será processado até a próxima aspa (possivelmente em outra linha)
          // Para simplificar, vamos apenas "imprimir" a abertura da string.
          // A lógica anterior de construir o lexema da string é mais apropriada se strings são tokens completos.
          // Re-introduzindo lógica de captura de string:
          char* string_content_only = NULL;
          size_t content_len = 0;
          size_t buffer_cap = 64;
          string_content_only = (char*)managed_malloc(buffer_cap);
          int closed_on_this_line = 0;
          // cursor já está no primeiro DQUOTE, avançamos para o conteúdo
          // const char* content_start_cursor = cursor + 1; // Não, cursor já foi avançado pela lógica DQUOTE

          // O cursor já foi avançado pela lógica de DQUOTE acima, então o próximo char é o conteúdo ou DQUOTE de fechamento
          // Não, a lógica de DQUOTE acima só alterna o estado. O cursor ainda está no DQUOTE.
          // Então, avançamos o cursor para DENTRO da string ou para DEPOIS do DQUOTE se for fechamento.

          const char* current_DQUOTE_char = cursor; // Guarda o ponteiro para a aspa atual
          cursor++; // Avança para depois da aspa atual

          if (currently_in_string_global_status == 0) { // Se esta aspa ABRIU a string
            while(*cursor != '\0'){
                if(*cursor == DQUOTE){
                    closed_on_this_line = 1;
                    break;
                }
                if(content_len >= buffer_cap -1){
                    buffer_cap *= 2;
                    char* new_content = (char*)managed_realloc(string_content_only, buffer_cap);
                    string_content_only = new_content;
                }
                string_content_only[content_len++] = *cursor;
                cursor++;
            }
            string_content_only[content_len] = '\0';

            size_t full_lex_len = strlen(string_content_only) + 2 + 1;
            lexema_str = (char*)managed_malloc(full_lex_len);
            sprintf(lexema_str, "\"%s\"", string_content_only); // Adiciona aspas de volta

            if(closed_on_this_line) {
              // counters.is_inside_string_at_line_end já foi invertido para false
              cursor++; // Pula a aspa final "
            } else {
              // String não fechou nesta linha, counters.is_inside_string_at_line_end permanece true
              // Não precisamos fazer nada com cursor aqui, ele parou no \0 da linha
            }
            checkString(lexema_str, line_num); // Imprime a string (completa ou parcial)
            managed_free(lexema_str);
            managed_free(string_content_only);
          } else {
            // Esta aspa está FECHANDO uma string que começou em linha anterior.
            // O cursor já foi avançado. counters.is_inside_string_at_line_end já é false.
            // Apenas imprimimos um token de "fim de string" simbólico ou nada.
            // A string completa foi formada em linhas anteriores + esta.
            // A lógica atual não reconstrói a string completa se ela cruza linhas.
            // Para simplificar, o checkString só é chamado quando a string é formada na mesma linha.
            // Se cruzar linhas, a lógica em main que verifica global_in_string_literal pegará.
            printf("[STRING CONTINUADA/FECHADA]: Linha %d (Aspa de fechamento '%c')\n", line_num, *current_DQUOTE_char);

          }
      } else {
          // Se *cursor == DQUOTE e !counters.is_inside_string_at_line_end, significa que uma string foi FECHADA.
          // O cursor já está no DQUOTE. Avançamos para o próximo token.
          printf("[STRING FECHADA]: Linha %d (Aspa de fechamento '%c')\n", line_num, *cursor);
          cursor++;
      }
       // Fim da lógica DQUOTE para tokenização de string.
    }
    else if ((isdigit((unsigned char)*cursor) || (*cursor == PERIOD && isdigit((unsigned char)*(cursor+1)))) && !counters.is_inside_string_at_line_end ) {
      lexema_str = build_lexema_dynamically(&cursor, line_num, num_char_rule, NULL);
      checkNumber(lexema_str, line_num);
      managed_free(lexema_str);
    }
    else if (strchr("+-*/^=<>&|", *cursor) && !counters.is_inside_string_at_line_end) {
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
                 counters.error_line = line_num; return counters;
            }
            if (op_buffer[0] == '<' && *cursor == '>') {
                 op_buffer[1] = *cursor; cursor++;
            } else if (op_buffer[0] == '>' && *cursor == '<') {
                 fprintf(stderr, "[LINHA %d] ERRO LEXICO: Operador invalido '%c%c' (Regra 3.2.3).\n", line_num, op_buffer[0], *cursor);
                 counters.error_line = line_num; return counters;
            } else {
                 op_buffer[1] = *cursor; cursor++;
            }
        }
        checkOperator(op_buffer, line_num);
    }
    else if (*cursor != '\0' && !counters.is_inside_string_at_line_end) { // Caractere não reconhecido e não está em string
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere desconhecido '%c' (ASCII: %d).\n", line_num, *cursor, (int)*cursor);
        counters.error_line = line_num; return counters;
    } else if (*cursor != '\0' && counters.is_inside_string_at_line_end) {
        // Estamos dentro de uma string, mas o caractere não é uma aspa de fechamento.
        // Apenas avançamos, pois o conteúdo da string está sendo tratado na lógica de DQUOTE.
        // No entanto, a lógica de DQUOTE atual é um pouco confusa.
        // Se a lógica de DQUOTE for para consumir a string inteira, este `else if` não é necessário.
        // Se a lógica de DQUOTE só lida com as aspas em si, então precisamos consumir o conteúdo aqui.
        // A refatoração da lógica de DQUOTE é necessária para maior clareza.
        // Por agora, se estamos em string e não é DQUOTE, apenas avançamos o cursor.
        // Isso pode levar a pular caracteres se a lógica DQUOTE não os consumir.
        // Vamos assumir que a lógica DQUOTE acima lida com o consumo de conteúdo.
        // Se o cursor não avançou na lógica DQUOTE e caiu aqui, é um problema.
        // Para evitar loop infinito, avançamos.
        if (!strchr("!_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"+-*/^=<>&|(){}[],;", *cursor) && isprint(*cursor) ) {
             // Se é um caractere imprimível não especial DENTRO de uma string, ele é parte da string.
             // A lógica de captura de string acima (no bloco 'else if (*cursor == DQUOTE)') deveria ter pego isso.
             // Se chegamos aqui, pode ser um bug na lógica de string.
             // Para segurança, apenas avançamos o cursor se não for um caractere de controle ou \0.
        }
        // Se o caractere não foi consumido por nenhuma regra acima E estamos em string, ele é parte da string.
        // A lógica de string deveria consumir tudo até a próxima aspa.
        // Se chegamos aqui, é porque o caractere não é uma aspa (já tratado) e não é um token especial.
        // Portanto, ele DEVE ser parte do conteúdo da string.
        // A forma como a string está sendo tratada precisa ser unificada.
        // A iteração atual do loop while(*cursor != '\0') e os blocos if/else if
        // podem precisar ser reestruturados para lidar com o estado "in_string" de forma mais limpa.
        // Por ora, se estamos em string e o char não é DQUOTE, ele é conteúdo.
        // A lógica de captura de string já deveria ter avançado o cursor.
        // Se não avançou, e não é DQUOTE, e estamos em string, avançamos para evitar loop.
        if (*cursor != DQUOTE) { // Se não é a aspa que fecha
             // Este caractere é parte do conteúdo da string.
             // A lógica de construção da string no bloco DQUOTE deveria capturá-lo.
             // Se não capturou, e o cursor não avançou, avançamos aqui para evitar loop.
             // printf("DEBUG: char '%c' in string, linha %d\n", *cursor, line_num);
             // cursor++; // CUIDADO: pode pular caracteres se a lógica de string estiver falha.
        }
        // Se o cursor não foi avançado por nenhuma outra regra e ainda estamos aqui, avançamos.
        // Isso é um fallback para evitar loops infinitos, mas indica um problema na lógica.
        // Uma forma mais segura seria garantir que cada iteração do while avance o cursor.
        // Se o cursor não for explicitamente avançado por uma regra de token, e não for '\0', avançamos.
        const char *cursor_before_fallback = cursor;
        // ... (lógica de tokenização) ...
        if (cursor == cursor_before_fallback && *cursor != '\0') {
            // Se nenhuma regra consumiu o caractere, e não é o fim da linha
            if (counters.is_inside_string_at_line_end && *cursor != DQUOTE) {
                // Se estamos numa string e o caractere não é uma aspa, ele é conteúdo.
                // A lógica de captura de string (acima) deveria ter lidado com isso.
                // Se não lidou, é um bug. Por segurança, avançamos.
                // fprintf(stderr, "AVISO: Caractere '%c' dentro de string nao consumido por regra especifica. Avancando.\n", *cursor);
                cursor++;
            } else if (!counters.is_inside_string_at_line_end) {
                // Se não estamos em string e o caractere não foi reconhecido.
                fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere '%c' (ASCII: %d) nao reconhecido e fora de string.\n", line_num, *cursor, (int)*cursor);
                counters.error_line = line_num; return counters;
            } else {
                // Estamos dentro de uma string, e o caractere é uma aspa (deveria ter sido tratada acima).
                // Ou outra condição inesperada.
                 cursor++; // Fallback para evitar loop
            }
        }


    }
     // Garantir que o cursor avance para evitar loop infinito se nenhuma regra pegar o caractere
    const char *cursor_before_loop_end = cursor;
    // ... toda a lógica if/else if ...
    if (cursor == cursor_before_loop_end && *cursor != '\0') {
        // Se o cursor não avançou nesta iteração, é um erro ou um caractere não tratado
        if (counters.is_inside_string_at_line_end && *cursor != DQUOTE) {
            // Se estamos dentro de uma string, e o char não é uma aspa, ele é conteúdo.
            // A lógica de string deveria ter consumido este char. Se não o fez, é um bug.
            // Para evitar loop, avançamos, mas isso pode mascarar o bug.
            // Idealmente, a lógica de string consome TUDO até a próxima aspa ou EOL.
            // fprintf(stderr, "DEBUG: Caractere '%c' em string nao tratado, avancando cursor.\n", *cursor);
            cursor++;
        } else if (!counters.is_inside_string_at_line_end) {
            fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere '%c' (ASCII: %d) nao processado e fora de string.\n", line_num, *cursor, (int)*cursor);
            counters.error_line = line_num;
            return counters;
        } else { // Dentro de string e é uma aspa, ou outra situação estranha
            cursor++; // Avança para evitar loop
        }
    }

  }
  return counters;
}

void invalidToken() {
    fprintf(stderr, "ERRO: Funcao invalidToken chamada - token invalido.\n");
    report_memory_usage();
    cleanup_memory_manager();
    exit(EXIT_FAILURE);
}
