#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"
#include "memory_manager.h"

int checkPrincipal(const char* line, int num_line){
  int i = 0;
  int opened = 0;

  while(line[i] != '\0'){
    if(line[i] == OPEN_PAREN){
      opened ++;
    }else if(line[i] == CLOSE_PAREN){
      opened --;
    }
    i++;
  }

  if(opened == 0){
    return 1;
  }

  fprintf(stderr, "[LINHA %d] | [ERROR] Funcao 'PRINCIPAL' MAL FORMULADA \n", num_line);
  return -1;
}

void checkVariable(const char* word, int num_line){
  printf("[VARIAVEL]: %s (Linha %d)\n", word, num_line);
}

void checkFunction(const char* word, int num_line){
  printf("[FUNCTION]: %s (Linha %d)\n", word, num_line);
}

void checkReservedWord(const char* word, int num_line){
  int i;
  Token token;
  token.line = num_line;

  token.word = (char*)managed_malloc(strlen(word) + 1);
  if(token.word == NULL){
    fprintf(stderr, "[LINHA %d] ERRO FATAL: Falha ao alocar memoria para palavra reservada '%s'.\n", num_line, word);
    exit(EXIT_FAILURE);
  }
  strcpy(token.word, word);

  printf("[PALAVRA RESERVADA]: %s (Linha %d)\n", word, num_line);

  int found = 0;
  for(i = 0; i < ACTUAL_NUM_RESERVED_WORDS; i++){
    if (strcmp(reserved_words[i].word, word) == 0){
      token.type = reserved_words[i].type;
      found = 1;
      break;
    }
  }
  if (!found) {
    token.type = TK_IDENTIFICADOR;
  }

  if (token.word != NULL) {
    managed_free(token.word);
  }
}

void checkNumber(const char *word, int num_line){
  Token token;
  token.line = num_line;

  token.word = (char*)managed_malloc(strlen(word) + 1);
  if (token.word == NULL) {
      fprintf(stderr, "[LINHA %d] ERRO FATAL: Falha ao alocar memoria para numero '%s'.\n", num_line, word);
      exit(EXIT_FAILURE);
  }
  strcpy(token.word, word);

  if(strchr(token.word, PERIOD) != NULL){
    printf("[DECIMAL]: %s (Valor: %f, Linha %d)\n", token.word, atof(token.word), num_line);
    token.type = TIPO_DECIMAL;
  }else{
    printf("[INTEGER]: %s (Valor: %d, Linha %d)\n", token.word, atoi(token.word), num_line);
    token.type = TIPO_INTEIRO;
  }

  if (token.word != NULL) {
    managed_free(token.word);
  }
}

void checkString(const char *word, int num_line){
  Token token;
  token.line = num_line;

  token.word = (char*)managed_malloc(strlen(word) + 1);
   if (token.word == NULL) {
      fprintf(stderr, "[LINHA %d] ERRO FATAL: Falha ao alocar memoria para string '%s'.\n", num_line, word);
      exit(EXIT_FAILURE);
  }
  strcpy(token.word, word);
  token.type = TK_STRING;

  printf("[STRING]: %s (Linha %d)\n", token.word, num_line);

  if (token.word != NULL) {
    managed_free(token.word);
  }
}

void checkOperator(const char *word, int num_line){
  Token token;
  int i;
  int num_of_valid_operators = ACTUAL_NUM_VALID_OPERATORS;

  token.line = num_line;

  token.word = (char*)managed_malloc(strlen(word) + 1);
  if(token.word == NULL){
    fprintf(stderr, "[LINHA %d] ERRO FATAL: Falha ao alocar memoria para operador '%s'.\n", num_line, word);
    exit(EXIT_FAILURE);
  }
  strcpy(token.word, word);

  int found = 0;
  for(i = 0; i < num_of_valid_operators; i++){
    if(strcmp(VALID_OPERATORS[i].word, token.word) == 0){
      token.type = VALID_OPERATORS[i].type;
      printf("[OPERATOR]: %s (Linha %d)\n", token.word, num_line);
      found = 1;
      break;
    }
  }
  if (!found) {
    printf("[INVALID_OPERATOR]: %s (Linha %d)\n", token.word, num_line);
  }

  if (token.word != NULL) {
    managed_free(token.word);
  }
}

void checkLine(const char *line, int num_line){
  int i = 0;
  int k = 0;
  char lexema[100];
  int i_before_processing;

  while(line[i] != '\0'){
    k = 0;
    memset(lexema, 0, sizeof(lexema));
    i_before_processing = i;

    // Pula espaços iniciais
    while(isspace((unsigned char)line[i])) {
        if (line[i] == '\0') break;
        i++;
    }
    if (line[i] == '\0') break;

    // Trata delimitadores de um caractere PRIMEIRO
    // Parênteses, Chaves, Colchetes, Ponto e Vírgula, Vírgula
    if (line[i] == OPEN_PAREN) { printf("[DELIMITADOR_ABRE_PAREN]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == CLOSE_PAREN) { printf("[DELIMITADOR_FECHA_PAREN]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == OPEN_BRACE) { printf("[DELIMITADOR_ABRE_CHAVES]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == CLOSE_BRACE) { printf("[DELIMITADOR_FECHA_CHAVES]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == OPEN_BRACKET) { printf("[DELIMITADOR_ABRE_COLCH]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == CLOSE_BRACKET) { printf("[DELIMITADOR_FECHA_COLCH]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == SEMICOLON) { printf("[DELIMITADOR_PONTO_VIRGULA]: %c (Linha %d)\n", line[i], num_line); i++; continue; }
    if (line[i] == COMMA) { printf("[DELIMITADOR_VIRGULA]: %c (Linha %d)\n", line[i], num_line); i++; continue; }


    // Se não for espaço nem delimitador de um char conhecido, tenta os lexemas mais longos
    i_before_processing = i; // Reseta i_before_processing APÓS pular espaços e delimitadores simples

    if (line[i]  == EXCLAMATION) {
      while( line[i] != COMMA && line[i] != PLUS && line[i] != MINUS &&
             line[i] != SPACE && line[i] != '\0' && line[i] != SEMICOLON &&
             line[i] != EQUAL && line[i] != LT && line[i] != GT &&
             line[i] != CLOSE_PAREN && line[i] != OPEN_BRACKET &&
             line[i] != OPEN_BRACE && line[i] != CLOSE_BRACE && /* Adicionado chaves e colchetes aqui */
             k < (int)(sizeof(lexema) -1) ) {
        lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      if (k > 0) checkVariable(lexema, num_line);
    }
    else if(line[i] == '_' && line[i+1] == '_'){
      lexema[k++] = line[i++];
      lexema[k++] = line[i++];
      while(line[i] != '\0' && isalnum((unsigned char)line[i]) && k < (int)(sizeof(lexema) -1)){
        lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      if (k > 0) checkFunction(lexema, num_line);
    }
    else if(isalpha((unsigned char)line[i])){
      while(isalpha((unsigned char)line[i]) && k < (int)(sizeof(lexema) -1) ){
        lexema[k++] = line[i++];
      }
      while(isalnum((unsigned char)line[i]) && k < (int)(sizeof(lexema) -1)) {
          lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      if (k > 0) checkReservedWord(lexema, num_line);
    }
    else if(line[i] == DQUOTE){
      lexema[k++] = line[i++];
      while(line[i] != DQUOTE && line[i] != '\0' && k < (int)(sizeof(lexema) -1)){
        lexema[k++] = line[i++];
      }
      if(line[i] == DQUOTE && k < (int)(sizeof(lexema) -1)) {
        lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      if (k > 0) checkString(lexema, num_line);
    }
    else if (isdigit((unsigned char)line[i]) || (line[i] == PERIOD && isdigit((unsigned char)line[i+1])) ){
      int has_dot = 0;
      while ( (isdigit((unsigned char)line[i]) || (line[i] == PERIOD && !has_dot)) && k < (int)(sizeof(lexema) -1)){
          if (line[i] == PERIOD) has_dot = 1;
          lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      if (k > 0) checkNumber(lexema, num_line);
    }
    else if (strchr("+-*/^=<>&|", line[i])) {
      lexema[k++] = line[i++];
      if ( (lexema[0] == '=' && line[i] == '=') ||
           (lexema[0] == '<' && (line[i] == '=' || line[i] == '>')) ||
           (lexema[0] == '>' && line[i] == '=') ||
           (lexema[0] == '&' && line[i] == '&') ||
           (lexema[0] == '|' && line[i] == '|') )
      {
          if (k < (int)(sizeof(lexema) -1)) lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      if (k > 0) checkOperator(lexema, num_line);
    }
    else if (line[i] != '\0') {
        fprintf(stderr, "[LINHA %d] ERRO LEXICO: Caractere '%c' (ASCII: %d) nao reconhecido.\n", num_line, line[i], (int)line[i]);
        i++;
    }

    if (line[i] != '\0' && i == i_before_processing) {
         if (k==0) {
            // Se o caracter não foi pulado como espaço/delimitador inicial,
            // E não formou nenhum lexema conhecido,
            // E não foi pego pelo erro de "caractere não reconhecido" acima (o que não deveria acontecer),
            // então força o avanço para evitar loop.
            // Mas o "else if (line[i] != '\0')" acima já deveria ter tratado isso.
            // Este é um fallback extremo.
            //fprintf(stderr, "DEBUG: Forcando avanco em char '%c' na linha %d. Posicao i: %d\n", line_line[i], num_line, i);
            i++;
         }
    }
  }
}
