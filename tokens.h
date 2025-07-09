
#define NUM_RESERVED_WORDS 11
#define NUM_VALID_OPERATORS 12


#ifndef TOKENS_H
#define TOKENS_H

/* Definir os TIPOS para TOKENS */
typedef enum {
    TK_PRINCIPAL,
    TK_FUNCAO,
    TK_RETORNO,
    TK_LEIA,
    TK_ESCREVA,
    TK_SE,
    TK_SENAO,
    TK_PARA,


    TIPO_INTEIRO,
    TIPO_DECIMAL,
    TIPO_TEXTO,


    TK_IDENTIFICADOR,
    TK_VARIAVEL,
    TK_OPERATOR,
    TK_NUM_INT,
    TK_NUM_DEC,
    TK_STRING,


    TK_OPERATOR_SUM,
    TK_OPERATOR_MINUS,
    TK_OPERATOR_MULT,
    TK_OPERATOR_DIV,
    TK_OPERATOR_POT,
    TK_OPERATOR_EQUAL,


    TK_OPERATOR_MORE,
    TK_OPERATOR_LESS,


    TK_OPERATOR_SAME,
    TK_OPERATOR_DIFF,


    TK_OPERATOR_LT,
    TK_OPERATOR_LTE,
    TK_OPERATOR_GT,
    TK_OPERATOR_GTE,


    TK_OPERATOR_AND,
    TK_OPERATOR_OR,


    TK_DELIM,
    TK_ERROR

} TokenType;

/* STRUCT para o TOKEN */
typedef struct {
    TokenType type;
    char *word;
    int line;
    union {
        long int_val;
        double dec_val;
    } value;
} Token;

/* STRUCT das Palavras Reservadas */
typedef struct {
  const char* word;
  TokenType type;
} ReservedWord;

extern const ReservedWord reserved_words[];
extern const ReservedWord VALID_OPERATORS[];

#endif
