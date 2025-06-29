
/* Funções e Modulos */
#define principal  "PRINCIPAL"
#define funcao 
#define retorno


/* Palavras Reservadas */
#define leia
#define escreva
#define se
#define senao
#define para


/* Tipos de Dados */
#define inteiro
#define texto
#define decimal


typedef struct {
  char type[20];
  char value[100];
  int line;

} Token;


Token* check_line(const char* linha, int line_number, int* total_tokens);

Token* validate_token(char* type, char* value, int line);



