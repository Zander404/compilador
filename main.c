#include <stdio.h>


void main(){
  FILE *fprt;
  char text[100];
  fprt = fopen("program.txt", "r");

  if (fprt != NULL){
    while(fgets(text, 100, fprt)){
      printf("%s", text);
    }


  }else{
    printf("File Don't find");
  }
  printf("Hello");
}
