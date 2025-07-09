#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "lexico.h"
#include "parser.h"
#include "memory_controller.h"


int main(){
  char* memory_buffer = NULL;
  TokenList *token_list = NULL;
  int balance_status = 0; 
  
  /* Alocar um vetor de memoria com o tamanho da nossa memoria definida em memory_controller.h */
  memory_buffer = (char*)malloc(MEMORY_SIZE);
  if(memory_buffer == NULL){
    perror("Erro ao alocar 2MB de memória");
    return EXIT_FAILURE;
  }


  printf("Memória de %d MB alocada com sucesso. \n", MEMORY_SIZE / (1024 * 1024));
  
  /* Carregar o programa para a memoria */
  if(load_file_to_memory("./programa1.txt", memory_buffer, MEMORY_SIZE) == NULL){
    FREE(memory_buffer);
    return EXIT_FAILURE;
  }
  printf("Arquivo 'programa1.txt' carregando para a memória .\n");


  /* Criar a Tabela de Token */
  token_list = create_token_list();
  if(token_list == NULL){
    FREE(memory_buffer);
    return EXIT_FAILURE;
  }
  printf("Lista de Tokens criado com sucesso. \n");


  char *current_pos = memory_buffer;
  int current_line_num = 1;
  
  /* Ler Linha a Linha e fazer a analise dos tokens */
  while (*current_pos != '\0') {
    char *new_line_pos = strchr(current_pos, '\n');
    size_t line_length;
    char temp_line_buffer[256];

    if(new_line_pos != NULL){
      line_length = new_line_pos - current_pos;
    }else {
      line_length = strlen(current_pos);
    }
    
    if(line_length >= sizeof(temp_line_buffer)){
      fprintf(stderr, "AVISO: Linha %d muito longa, truncado para 255 caracteres. \n", current_line_num);
      line_length = sizeof(temp_line_buffer) - 1;
    }
    strncpy(temp_line_buffer, current_pos, line_length);
    temp_line_buffer[line_length] = '\0';

    checkLine(temp_line_buffer, current_line_num, token_list );
  
    current_line_num++;

    if(new_line_pos != NULL){
      current_pos = new_line_pos + 1;
    } else {
      current_pos += line_length;
    }
  }

  /* Analisar se o Programa está com os Delimitadores '()', '[]', '{}', estão balanceados */
  printf("\n--- Verificando Balanceamento de Símbolos ---\n");
  balance_status = check_all_symbols_balance(token_list);
  if (balance_status) {
    printf("Balanceamento de símbolos: OK.\n");
  } else {
    printf("Balanceamento de símbolos: ERRO ENCONTRADO.\n");
  }
  printf("-----------------------------------------\n");

  /*Liberar a Memória, quando a execução terminar */ 
  if(memory_buffer != NULL){ FREE(memory_buffer); printf("Memória do programa liberada.\n"); }
  if (token_list != NULL) { destroy_token_list(token_list); printf("Lista de tokens liberada.\n"); }
  print_memory_report();

  /* Se o tudo correto e balanceado emitir sinal de SUCESSO */
  return balance_status ? EXIT_SUCCESS : EXIT_FAILURE;
}
