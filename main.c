#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbols.h"
#include "lexico.h"

void main(){
  FILE *arq;
  char text[100];
  arq = fopen("./programa1.txt", "r");
  char palavra[100];
  int num_line = 1;
  int hasPrincipalFunction = 0;

  if (arq != NULL){
    while(fgets(text, sizeof(text), arq) != NULL) {
      text[strcspn(text, "\n")] = '\0';
      checkLine(text, num_line);
      num_line++;
    }
    fclose(arq);


  }else{
    printf("File Don't find");
  }

  return;
}
