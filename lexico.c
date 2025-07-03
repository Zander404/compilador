#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"

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

  fprintf(stderr, "[LINHA] %d | [ERROR] Funcao 'PRINCIPAL' MAL FORMULADA \n", num_line);
  return -1;
}



void checkVariable(const char* word, int num_line){
  Token token;
  token.line = num_line;

  printf("[VARIAVEL]: %s \n", word);
  return ;
}



void checkFunction(const char* word, int num_line){
  Token token;
  token.line = num_line;

  printf("[FUNCTION]: %s\n", word);
  return ;
}



void checkReservedWord(const char* word, int num_line){
  int num_of_reserved_words = NUM_RESERVED_WORDS;
  int i;

  Token token;
  token.line = num_line;

  token.word = malloc(strlen(word) + 1);
  if(token.word != NULL){
    strcpy(token.word, word);
  }
  printf("[PALAVRA RESERVADA]: %s \n", word);

  for(i = 0; i < num_of_reserved_words; i++){
    if (strcmp(reserved_words[i].word, word) == 0){
      token.type = reserved_words[i].type;
      return ;
    }
  }
  token.type = TK_IDENTIFICADOR;
  return;
}


void checkInteger(const char *word, int num_line){
  Token token;
  token.line = num_line;

  strcpy(token.word, word);
  printf("[INTEGER] %s", token.word);
  return ;
}


void checkDecimal(){

  return ;
}


void checkString(const char *word, int num_line){
  Token token;
  token.line = num_line;
  strcpy(token.word, word);
  token.type = TK_STRING;

  printf("[STRING]: %s \n", token.word);

  return ;
}



void checkOperator(const char *word, int num_line){
  Token token;
  int i;
  int num_of_valid_operators = NUM_VALID_OPERATORS;

  token.line = num_line;

  token.word = malloc(strlen(word) + 1);
  if(token.word != NULL){
    strcpy(token.word, word);
  }


  for(i = 0; i < num_of_valid_operators; i++){
    if(strcmp(VALID_OPERATORS[i].word, token.word) == 0){
      token.type = TK_OPERATOR;
      printf("[OPERATOR] %s \n", word);
      return ;
    }
  }
  printf("[INVALID OPERATOR] %s \n", token.word);
  return ;

}



void checkLine(const char *line, int num_line){
  int i = 0;
  int k = 0;
  char lexema[100];

  while(line[i] != '\0'){
    k = 0; 
    memset(lexema, 0, sizeof(lexema));

    while(isspace(line[i]) || line[i] == OPEN_PAREN || line[i] == CLOSE_PAREN || line[i] == OPEN_BRACE || line[i] == CLOSE_BRACE) { i++;}

    /* Verify if is a Variable */
    if (line[i]  == EXCLAMATION) {

      while( line[i] != COMMA && line[i] != '\0' && line[i] != SEMICOLON && line[i] != EQUAL && line[i] != LT && line[i] != GT && line[i] != CLOSE_PAREN){
        lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      checkVariable(lexema, num_line);
    }

    /* Verify if a Function */
    else if(line[i] == '_' && line[i+1] == '_'){
      lexema[k++] = line[i++];
      lexema[k++] = line[i++];
      while(line[i] != '\0' && isalnum(line[i])){
        lexema[k++] = line[i++];
      }
      checkFunction(lexema, num_line);

    }

    /* Verify if is a ReservedWord */
    else if(isalpha(line[i])){
      while((isalpha(line[i]))){
        lexema[k++] = line[i++];
      }

      lexema[k] = '\0';
      checkReservedWord(lexema, num_line);

    }

    /* Verify if is a String*/
    else if(line[i] == DQUOTE){
      lexema[k++] = line[i++];

      while(line[i] != DQUOTE && line[i] != '\0'){
        lexema[k++] = line[i++];
      }
      lexema[k++] = line[i++];

      lexema[k] = '\0';
      checkString(lexema, num_line);

    }
    else if(isdigit(line[i])){
      while(isdigit(line[i]) || line[i] == PERIOD){
        lexema[k++] == line[i++];
      }

      lexema[k] = '\0';
      checkDecimal(lexema, num_line);

    
    }

    k = 0; 
    memset(lexema, 0, sizeof(lexema));
 

    /* Check if is operator */
    if ( line [i] == EQUAL ||  line [i] == PLUS ||  line [i] == MINUS ||  line [i] == AMP ||  line [i] == ASTERISK ||  line [i] == CARET ||  line [i] == LT ||  line [i] == GT ||  line [i] == PIPE) {
      while(!isspace(line[i]) && !isalnum(line[i]) && line[i] != '\0'){
        lexema[k++] = line[i++];
      }
      lexema[k] = '\0';
      checkOperator(lexema, num_line);

    }


    i++;
  }

}





