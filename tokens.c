#include "tokens.h" // Contém ReservedWordMapping e TokenType

// Definição das palavras reservadas
const ReservedWordMapping reserved_words[] = {
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
  // Outras palavras reservadas, se houver, devem ser adicionadas aqui.
};
// Definição da constante para o número real de palavras reservadas
const int ACTUAL_NUM_RESERVED_WORDS = sizeof(reserved_words) / sizeof(reserved_words[0]);


// Definição dos operadores válidos
const ReservedWordMapping VALID_OPERATORS[] = {
  {"+", TK_OPERATOR_SUM},
  {"-", TK_OPERATOR_MINUS},
  {"*", TK_OPERATOR_MULT},
  {"/", TK_OPERATOR_DIV},
  {"^", TK_OPERATOR_POT},
  {"=", TK_OPERATOR_EQUAL},    // Atribuição

  {"==", TK_OPERATOR_SAME},   // Comparação
  {"<>", TK_OPERATOR_DIFF },
  {"<",  TK_OPERATOR_LT },
  {"<=", TK_OPERATOR_LTE },
  {">",  TK_OPERATOR_GT },
  {">=", TK_OPERATOR_GTE },

  {"&&", TK_OPERATOR_AND},
  {"||", TK_OPERATOR_OR}
};
// Definição da constante para o número real de operadores válidos
const int ACTUAL_NUM_VALID_OPERATORS = sizeof(VALID_OPERATORS) / sizeof(VALID_OPERATORS[0]);

// A função token_type_to_string não é necessária neste arquivo se não for usada aqui.
// Se for necessária por outros módulos, deve ser declarada em tokens.h e definida aqui.
// Por enquanto, vamos assumir que não é necessária em tokens.c.
// Se lexico.c ou token_list.c precisarem dela, ela deve ser global (não static)
// e declarada no .h apropriado.
// Considerando a estrutura original, ela não existia.
// Se for reintroduzida para a lista de tokens, token_list.c seria um bom lugar.
// Se for para debug geral de TokenType, tokens.c/tokens.h é o lugar.
// Para o objetivo atual de apenas integrar o memory_manager no código original,
// não vamos adicionar token_type_to_string aqui.
