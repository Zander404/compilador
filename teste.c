#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbols.h"

int main(void){
  FILE *arq;
  char text[100];
  arq = fopen("program.txt", "r");
  char palavra[100];

  if (arq != NULL){
    while(fgets(text, 100, arq) != NULL) {

      text[strcspn(text, "\n")] = '\0';


      if(strcmp(text, "principal") == 0){
        strcpy(palavra, principal);
        printf("%s", palavra);

      }
    }


  }else{
    printf("File Don't find");
  }

  fclose(arq);
}
