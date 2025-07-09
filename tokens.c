#include "tokens.h"

/* Definir as palavras reservadas para validação */
const ReservedWord reserved_words[] = {
  {"principal", TK_PRINCIPAL},
  {"funcao", TK_FUNCAO},
  {"retorno", TK_RETORNO},
  {"leia", TK_LEIA},
  {"escreva", TK_ESCREVA},
  {"se", TK_SE},
  {"senao", TK_SENAO},
  {"para", TK_PARA},
  {"inteiro", TIPO_INTEIRO},
  {"decimal", TIPO_DECIMAL},
  {"texto", TIPO_TEXTO}
};

/* Definir Operadores Validos para validação */
const ReservedWord VALID_OPERATORS[] = {
  {"+", TK_OPERATOR_SUM},
  {"-", TK_OPERATOR_MINUS},
  {"*", TK_OPERATOR_MULT},
  {"/", TK_OPERATOR_DIV},
  {"^", TK_OPERATOR_POT},
  {"=", TK_OPERATOR_EQUAL},

  {"++", TK_OPERATOR_MORE},
  {"--", TK_OPERATOR_LESS},

  
  {"==", TK_OPERATOR_SAME},
  {"<>", TK_OPERATOR_DIFF },
  {"<",  TK_OPERATOR_LT },
  {"<=", TK_OPERATOR_LTE },
  {">",  TK_OPERATOR_GT },
  {">=", TK_OPERATOR_GTE },

  {"&&", TK_OPERATOR_AND},
  {"||", TK_OPERATOR_OR}
};
