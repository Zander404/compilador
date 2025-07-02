#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"
#include "symbols.h"


int checkPrincipal(const char* line, int num_line){
  int i = 0;
  int opened = 0;

  while(line[i] != '\0'){
    if(line[i] == open_parethesis){
      opened ++;
    }else if(line[i] == close_parethesis){
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
  Token token;
  token.line = num_line;

  token.word = malloc(strlen(word) + 1);
  if(token.word != NULL){
    strcpy(token.word, word);
  }
  printf("[PALAVRA RESERVADA]: %s \n", word);

  int num_of_reserved_words = NUM_RESERVED_WORDS;
  int i;

  for(i = 0; i < num_of_reserved_words; i++){
    if (strcmp(reserved_words[i].word, word) == 0){
      token.type = reserved_words[i].type;
      return ;
    }
  }
  token.type = IDENTIFICADOR;
  return;
}

void checkInteger(){
  return ;
}

void checkDecimal(){

  return ;
}

void checkString(const char *word, int num_line){
  Token token;
  token.line = num_line;

  printf("[STRING]: %s \n", word);

  return ;
}



void checkLine(const char *line, int num_line){
  int i = 0;
  int k = 0;
  char lexema[100];

  while(line[i] != '\0'){
    k = 0; 
    memset(lexema, 0, sizeof(lexema));

    while(isspace(line[i]) || line[i] == open_parethesis || line[i] == close_parethesis || line[i] == open_curl_bracket || line[i] == close_curl_bracket) { i++;}



    /* Verify if is a Variable */
    if(line[i]  == exclamation) {

      while( line[i] != comma && line[i] != '\0' && line[i] != semicolon  ){

        /* if(isalnum(line[i]) || line[i] == '=' || line[i] == exclamation || line[i] == open_square_bracket || line[i] == close_square_bracket || line[i] == '+' ||){
          lexema[k++] = line[i];
        } */ 
        lexema[k++] =line[i];
      i++;
      }
      lexema[k] = '\0';
      checkVariable(lexema, num_line);
      
    }


    /* Verify if a Function */
    if(line[i] == '_' && line[i+1] == '_'){
      while(line[i] != '\0'){
        lexema[k++] = line[i++];
      }
      checkFunction(lexema, num_line);

    } 


    /* Verify if is a ReservedWord */
    if(isalpha(line[i])){
      while((isalpha(line[i]))){
        lexema[k++] = line[i++];
      }

      lexema[k] = '\0';
      checkReservedWord(lexema, num_line);

    } 

    /* Verify if is a String*/
    if(line[i] == double_quotes){
      lexema[k++] = line[i++];

      while(line[i] != double_quotes && line[i] != '\0'){
        lexema[k++] = line[i++];
      }
      lexema[k++] = line[i++];

      lexema[k] = '\0';
      checkString(lexema, num_line);

    }



    i++;
  }

}





