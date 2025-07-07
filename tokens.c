#include "tokens.h"
#include <stdio.h>

const ReservedWordMapping reserved_words[] = {
  {"principal", TK_PRINCIPAL},
  {"funcao", TK_FUNCAO},
  {"retorno", TK_RETORNO},
  {"leia", TK_LEIA},
  {"escreva", TK_ESCREVA},
  {"se", TK_SE},
  {"senao", TK_SENAO},
  {"para", TK_PARA},
  // Os tipos de dados também são palavras reservadas
  {"inteiro", TIPO_INTEIRO},
  {"decimal", TIPO_DECIMAL},
  {"texto", TIPO_TEXTO}
};
const int ACTUAL_NUM_RESERVED_WORDS = sizeof(reserved_words) / sizeof(reserved_words[0]);

const ReservedWordMapping VALID_OPERATORS[] = {
  {"+", TK_OP_SOMA},
  {"-", TK_OP_SUBTRACAO},
  {"*", TK_OP_MULTIPLICACAO},
  {"/", TK_OP_DIVISAO},
  {"^", TK_OP_POTENCIA},
  {"=", TK_OP_ATRIBUICAO}, // Atribuição '='
  {"==", TK_OP_IGUAL},    // Comparação '=='
  {"<>", TK_OP_DIFERENTE},
  {"<",  TK_OP_MENOR},
  {"<=", TK_OP_MENOR_IGUAL},
  {">",  TK_OP_MAIOR},
  {">=", TK_OP_MAIOR_IGUAL},
  {"&&", TK_OP_LOGICO_E},
  {"||", TK_OP_LOGICO_OU}
};
const int ACTUAL_NUM_VALID_OPERATORS = sizeof(VALID_OPERATORS) / sizeof(VALID_OPERATORS[0]);

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TK_ERRO: return "ERRO";
        case TK_PRINCIPAL: return "PRINCIPAL";
        case TK_FUNCAO: return "FUNCAO";
        case TK_RETORNO: return "RETORNO";
        case TK_LEIA: return "LEIA";
        case TK_ESCREVA: return "ESCREVA";
        case TK_SE: return "SE";
        case TK_SENAO: return "SENAO";
        case TK_PARA: return "PARA";
        case TIPO_INTEIRO: return "TIPO_INTEIRO"; // Mantendo consistência com o enum
        case TIPO_DECIMAL: return "TIPO_DECIMAL";
        case TIPO_TEXTO: return "TIPO_TEXTO";
        case TK_IDENTIFICADOR: return "IDENTIFICADOR";
        case TK_VARIAVEL: return "VARIAVEL";
        case TK_LITERAL_INT: return "LITERAL_INT";
        case TK_LITERAL_DEC: return "LITERAL_DEC";
        case TK_LITERAL_STRING: return "LITERAL_STRING";
        case TK_OP_ATRIBUICAO: return "OP_ATRIBUICAO";
        case TK_OP_SOMA: return "OP_SOMA";
        case TK_OP_SUBTRACAO: return "OP_SUBTRACAO";
        case TK_OP_MULTIPLICACAO: return "OP_MULTIPLICACAO";
        case TK_OP_DIVISAO: return "OP_DIVISAO";
        case TK_OP_POTENCIA: return "OP_POTENCIA";
        case TK_OP_IGUAL: return "OP_IGUAL";
        case TK_OP_DIFERENTE: return "OP_DIFERENTE";
        case TK_OP_MENOR: return "OP_MENOR";
        case TK_OP_MENOR_IGUAL: return "OP_MENOR_IGUAL";
        case TK_OP_MAIOR: return "OP_MAIOR";
        case TK_OP_MAIOR_IGUAL: return "OP_MAIOR_IGUAL";
        case TK_OP_LOGICO_E: return "OP_LOGICO_E";
        case TK_OP_LOGICO_OU: return "OP_LOGICO_OU";
        case TK_DELIM_ABRE_PAREN: return "DELIM_ABRE_PAREN";
        case TK_DELIM_FECHA_PAREN: return "DELIM_FECHA_PAREN";
        case TK_DELIM_ABRE_COLCH: return "DELIM_ABRE_COLCH";
        case TK_DELIM_FECHA_COLCH: return "DELIM_FECHA_COLCH";
        case TK_DELIM_ABRE_CHAVES: return "DELIM_ABRE_CHAVES";
        case TK_DELIM_FECHA_CHAVES: return "DELIM_FECHA_CHAVES";
        case TK_DELIM_PONTO_VIRGULA: return "DELIM_PONTO_VIRGULA";
        case TK_DELIM_VIRGULA: return "DELIM_VIRGULA";
        case TK_DELIM_PONTO: return "DELIM_PONTO";
        case TK_DELIM_ASPAS_DUPLAS: return "DELIM_ASPAS_DUPLAS";
        case TK_EOF: return "EOF";
        default: return "TIPO_DESCONHECIDO";
    }
}
