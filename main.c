#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "lexico.h"
#include "memory_manager.h" // Incluído gerenciador de memória

// main deve retornar int
int main(void){ // Adicionado void para conformidade se não usar argc, argv
  FILE *arq;
  char text[100]; // Buffer para leitura de linha
  // char palavra[100]; // Não parece ser utilizada
  int num_line = 1;
  // int hasPrincipalFunction = 0; // Não parece ser utilizada nesta função

  init_memory_manager(0); // Inicializa o gerenciador de memória com limite padrão (0 usa DEFAULT_MAX_MEMORY_KB)

  arq = fopen("./programa1.txt", "r");

  if (arq != NULL){
    while(fgets(text, sizeof(text), arq) != NULL) {
      text[strcspn(text, "\n")] = '\0'; // Remove newline
      checkLine(text, num_line);
      num_line++;
    }
    fclose(arq);
  }else{
    // Usar fprintf para stderr para mensagens de erro
    fprintf(stderr, "Erro: Arquivo './programa1.txt' nao encontrado.\n");
    report_memory_usage();    // Reporta uso mesmo em erro
    cleanup_memory_manager(); // Limpa memória alocada pelo gerenciador
    return 1; // Retorna código de erro
  }

  report_memory_usage();    // Reporta uso de memória ao final
  cleanup_memory_manager(); // Limpa toda a memória gerenciada
  return 0; // Retorno bem-sucedido
}
