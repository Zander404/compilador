#ifndef TOKENS_H
#define TOKENS_H

// Mantendo as macros originais por enquanto, mas ACTUAL_... são preferíveis
// #define NUM_RESERVED_WORDS 21 // Esta contagem parece incorreta pela definição em tokens.c
// #define NUM_VALID_OPERATORS 12; // Ponto e vírgula aqui é um erro de sintaxe para #define

typedef enum {
    TK_PRINCIPAL,
    TK_FUNCAO,
    TK_RETORNO,
    TK_LEIA,
    TK_ESCREVA,
    TK_SE,
    TK_SENAO,
    TK_PARA,

    TIPO_INTEIRO,    // Usado como tipo de dado e como TokenType para palavras reservadas
    TIPO_DECIMAL,
    TIPO_TEXTO,

    TK_IDENTIFICADOR,
    TK_VARIAVEL,        // Pode ser útil distinguir de identificador geral
    TK_OPERATOR,        // Tipo genérico para operadores, tipos específicos abaixo são melhores
    TK_NUM_INT,         // Para literais inteiros
    TK_NUM_DEC,         // Para literais decimais
    TK_STRING,          // Para literais string

    // Tipos específicos de operadores (preferível sobre TK_OPERATOR genérico)
    TK_OPERATOR_SUM,
    TK_OPERATOR_MINUS,
    TK_OPERATOR_MULT,
    TK_OPERATOR_DIV,
    TK_OPERATOR_POT,    // ^ (Potência)
    TK_OPERATOR_EQUAL,  // = (Atribuição)

    TK_OPERATOR_SAME,   // == (Comparação Igual)
    TK_OPERATOR_DIFF,   // <> (Diferente)
    TK_OPERATOR_LT,     // < (Menor que)
    TK_OPERATOR_LTE,    // <= (Menor ou Igual que)
    TK_OPERATOR_GT,     // > (Maior que)
    TK_OPERATOR_GTE,    // >= (Maior ou Igual que)

    TK_OPERATOR_AND,    // && (Lógico E)
    TK_OPERATOR_OR,     // || (Lógico OU)

    TK_DELIM,           // Tipo genérico para delimitadores, se necessário
    TK_ERRO             // Para tokens de erro

} TokenType;

typedef struct {
  TokenType type;
  char *word; // O lexema do token (alocado dinamicamente)
  int line;
} Token;

// Estrutura para mapear strings de palavras reservadas/operadores a seus tipos
typedef struct {
  const char* word;
  TokenType type;
} ReservedWordMapping; // Nome mais genérico, já que é usado para operadores também

// Declarações externas dos arrays definidos em tokens.c
extern const ReservedWordMapping reserved_words[];
extern const ReservedWordMapping VALID_OPERATORS[];

// Declarações externas das constantes de tamanho real (definidas em tokens.c)
extern const int ACTUAL_NUM_RESERVED_WORDS;
extern const int ACTUAL_NUM_VALID_OPERATORS;

#endif // TOKENS_H
