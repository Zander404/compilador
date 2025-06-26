#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexico.h"
#include "ascii_table.h"

void checkPrincipal(){
    printf("principal \n");
    return ;
}


void checkFuncao(){
   printf("funcao \n");

    // Check Function
    // if ( c == '_' && line[j+1] == '_'){
    //   int k = 0;
    //   token[k++] = line[j++];
    //   token[k++] = line[j++];
    //
    //
    //   while(isalnum(line[j])){
    //     token[k++] = line[j++];
    //   }
    //
    //   token[k++] = '\0';
    //   j--;
    //   printf("[FUNCTION] %s (line %d)", token, num_line);
    //   continue;
    //
    // }
      return;
}

void checkLeia(){
  printf("leia \n");
  return ;
}

void checkEscreva(){
  printf("escreva \n");
  return ;
}

void checkSe(){
  printf("SE \n");
  return ;
}

void checkSenao(){
  printf("SENAO \n");
  return ;
}

void checkPara(){
  printf("PARA \n");
  return ;
}

void checkInteiro(){
  printf("INTEIRO \n");
  return ;
}

void checkDecimal(){
   printf("DECIMAL \n");
    
    // Check Integer or  Decimal
    //
    //
    // // Check Operators
    // if(strchr("=<>!&|+-*^", c)){
    //   token[0] = c;
    //   token[1] = '\0';
    //
    //   if((c == '=' && line[j+1] == '=') || (c == '<' && line[j+1] == '=') || (c == '>' && line[j+1] == '=') || (c == '<' && line[j+1] == '=') || (c == '<' && line[j+1] == '>')){
    //     token[1] = line[j+1];
    //     token[2] = '\0';
    //     j++;
    //   } 
    //
    // }
    //
    //
}

void checkTexto(){
   printf("TEXTO \n");
     // Check STRING
    //
    //
    // // Check Variable
    // if (c == '!'){
    //   int k = 0;
    //   token[k++] = c;
    //   j++;
    //   while (isalnum(line[j])){
    //     token[k++] = line[j++];
    //   }
    //   token[k] = '\0';
    //   j--;
    //   printf("[VARIABLE]  %s (line %d) \n", token, num_line);
    //   continue;
    // }

    return;
  }



void checkVariavel(){
  printf("VARIAVEL \n");
  return ;
}




void checkLine(const char *line, int num_line){
  int i = 0;
  char token[100];

  while(line[i] != '\0'){

    if(isspace(line[i])){
      i++;
      continue;
    }

    // ANALISADOR DE PALAVRA RESERVADA
    if(isalnum(line[i]) && line[i] != exclamation){
      int k = 0;
      while(isalnum(line[i])){
        token[k++] = line[i++]; 
      }

      token[k] = '\0';
    
      printf("WORD: %s , LINHA: %d \n", token, num_line);

      if(strcmp(token, "principal") == 0){
        checkPrincipal();
        break;

      }else if (strcmp(token, "funcao") == 0 ){
        checkFuncao();
        break;


      }else if( strcmp(token, "leia") == 0){
        checkLeia(); 
        break;

      }else if( strcmp(token, "escreva") == 0){
        checkEscreva();
        break;

      }else if (strcmp(token, "se") == 0 ){
        checkSe();
        break;

      }else if( strcmp(token, "senao") == 0){
        checkSenao();
        break;

      }else if( strcmp(token, "para") == 0){
        checkPara();
        break;

      }else if (strcmp(token, "inteiro") == 0 ){
        checkInteiro();
        break;

      }else if (strcmp(token, "decimal") == 0 ){
        checkDecimal();
        break;

      }else if (strcmp(token, "texto") == 0 ){
        checkTexto();
        break;

      }
    // ANALISADOR DE VARIAVEL
    }else if( line[i] == exclamation){
        checkVariavel();
        break;
    }
    
    i++;
    
  }
}





