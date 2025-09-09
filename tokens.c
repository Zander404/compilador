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


const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TK_PRINCIPAL: return "TK_PRINCIPAL";
        case TK_FUNCAO: return "TK_FUNCAO";
        case TK_RETORNO: return "TK_RETORNO";
        case TK_LEIA: return "TK_LEIA";
        case TK_ESCREVA: return "TK_ESCREVA";
        case TK_SE: return "TK_SE";
        case TK_SENAO: return "TK_SENAO";
        case TK_PARA: return "TK_PARA";
        case TIPO_INTEIRO: return "TIPO_INTEIRO";
        case TIPO_DECIMAL: return "TIPO_DECIMAL";
        case TIPO_TEXTO: return "TIPO_TEXTO";
        case TK_IDENTIFICADOR: return "TK_IDENTIFICADOR";
        case TK_VARIAVEL: return "TK_VARIAVEL";
        case TK_OPERATOR: return "TK_OPERATOR";
        case TK_NUM_INT: return "TK_NUM_INT";
        case TK_NUM_DEC: return "TK_NUM_DEC";
        case TK_STRING: return "TK_STRING";
        case TK_OPERATOR_SUM: return "TK_OPERATOR_SUM";
        case TK_OPERATOR_MINUS: return "TK_OPERATOR_MINUS";
        case TK_OPERATOR_MULT: return "TK_OPERATOR_MULT";
        case TK_OPERATOR_MORE: return "TK_OPERATOR_MORE";
        case TK_OPERATOR_LESS: return "TK_OPERATOR_LESS";
        case TK_OPERATOR_DIV: return "TK_OPERATOR_DIV";
        case TK_OPERATOR_POT: return "TK_OPERATOR_POT";
        case TK_OPERATOR_EQUAL: return "TK_OPERATOR_EQUAL";
        case TK_OPERATOR_SAME: return "TK_OPERATOR_SAME";
        case TK_OPERATOR_DIFF: return "TK_OPERATOR_DIFF";
        case TK_OPERATOR_LT: return "TK_OPERATOR_LT";
        case TK_OPERATOR_LTE: return "TK_OPERATOR_LTE";
        case TK_OPERATOR_GT: return "TK_OPERATOR_GT";
        case TK_OPERATOR_GTE: return "TK_OPERATOR_GTE";
        case TK_OPERATOR_AND: return "TK_OPERATOR_AND";
        case TK_OPERATOR_OR: return "TK_OPERATOR_OR";
        case TK_DELIM: return "TK_DELIM";
        case TK_ERROR: return "TK_ERROR";
        default: return "UNKNOWN_TYPE";
    }
}


