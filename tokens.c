#include "tokens.h"

const ReservedWordMapping reserved_words[] = {
  {"principal", TK_PRINCIPAL},
  {"funcao", TK_FUNCAO},
  {"retorno", TK_RETORNO},
  {"leia", TK_LEIA},
  {"escreva", TK_ESCREVA},
  {"se", TK_SE},
  {"senao", TK_SENAO},
  {"para", TK_PARA},
  {"inteiro", TK_TIPO_INTEIRO},
  {"decimal", TK_TIPO_DECIMAL},
  {"texto", TK_TIPO_TEXTO}
};
const int ACTUAL_NUM_RESERVED_WORDS = sizeof(reserved_words) / sizeof(reserved_words[0]);

const ReservedWordMapping VALID_OPERATORS[] = {
  {"+", TK_OP_SOMA},
  {"-", TK_OP_SUBTRACAO},
  {"*", TK_OP_MULTIPLICACAO},
  {"/", TK_OP_DIVISAO},
  {"^", TK_OP_POTENCIA},
  {"=", TK_OP_ATRIBUICAO},

  {"==", TK_OP_IGUAL},
  {"<>", TK_OP_DIFERENTE},
  {"<",  TK_OP_MENOR},
  {"<=", TK_OP_MENOR_IGUAL},
  {">",  TK_OP_MAIOR},
  {">=", TK_OP_MAIOR_IGUAL},

  {"&&", TK_OP_LOGICO_E},
  {"||", TK_OP_LOGICO_OU}
};
const int ACTUAL_NUM_VALID_OPERATORS = sizeof(VALID_OPERATORS) / sizeof(VALID_OPERATORS[0]);
