#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TK_ERRO = 0,
    TK_PRINCIPAL,       // Palavra reservada "principal"
    TK_FUNCAO,          // Palavra reservada "funcao"
    TK_RETORNO,         // Palavra reservada "retorno"
    TK_LEIA,            // Palavra reservada "leia"
    TK_ESCREVA,         // Palavra reservada "escreva"
    TK_SE,              // Palavra reservada "se"
    TK_SENAO,           // Palavra reservada "senao"
    TK_PARA,            // Palavra reservada "para"

    // Tipos de dados como palavras reservadas (usados em declarações)
    // Mantendo os nomes originais TIPO_ para consistência com o código legado,
    // mas TK_TIPO_ seria mais uniforme.
    TIPO_INTEIRO,       // Palavra reservada "inteiro"
    TIPO_DECIMAL,       // Palavra reservada "decimal"
    TIPO_TEXTO,         // Palavra reservada "texto"

    TK_IDENTIFICADOR,   // Nomes de variáveis e funções definidas pelo usuário
    TK_VARIAVEL,        // Especificamente para identificar um lexema como variável (ex: !nome)
                        // Pode ser redundante com TK_IDENTIFICADOR dependendo do uso.
                        // A tabela de símbolos distinguirá melhor a categoria.

    // Literais
    TK_LITERAL_INT,     // Ex: 123, 0, 42
    TK_LITERAL_DEC,     // Ex: 1.0, 0.5, .5, 5.
    TK_LITERAL_STRING,  // Conteúdo de uma string, ex: "ola mundo" (sem as aspas delimitadoras)

    // Operadores (nomes mais descritivos e prefixados com TK_OP_)
    TK_OP_ATRIBUICAO,   // =
    TK_OP_SOMA,         // +
    TK_OP_SUBTRACAO,    // -
    TK_OP_MULTIPLICACAO,// *
    TK_OP_DIVISAO,      // /
    TK_OP_POTENCIA,     // ^ (Exponenciação)

    TK_OP_IGUAL,        // == (Comparação de igualdade)
    TK_OP_DIFERENTE,    // <>
    TK_OP_MENOR,        // <
    TK_OP_MENOR_IGUAL,  // <=
    TK_OP_MAIOR,        // >
    TK_OP_MAIOR_IGUAL,  // >=

    TK_OP_LOGICO_E,     // &&
    TK_OP_LOGICO_OU,    // ||

    // Delimitadores (nomes mais descritivos e prefixados com TK_DELIM_)
    TK_DELIM_ABRE_PAREN,    // (
    TK_DELIM_FECHA_PAREN,   // )
    TK_DELIM_ABRE_COLCH,    // [
    TK_DELIM_FECHA_COLCH,   // ]
    TK_DELIM_ABRE_CHAVES,   // {
    TK_DELIM_FECHA_CHAVES,  // }
    TK_DELIM_PONTO_VIRGULA, // ;
    TK_DELIM_VIRGULA,       // ,
    TK_DELIM_PONTO,         // . (Pode ser usado em números decimais, ou como delimitador em outros contextos - raro)
    TK_DELIM_ASPAS_DUPLAS,  // " (Para marcar início/fim de string literal)

    TK_EOF              // Marcador de Fim de Arquivo (End Of File)
} TokenType;

typedef struct {
  TokenType type;
  char *word;     // O lexema do token (alocado dinamicamente)
  int line;
} Token;

typedef struct {
  const char* word;
  TokenType type;
} ReservedWordMapping;

extern const ReservedWordMapping reserved_words[];
extern const ReservedWordMapping VALID_OPERATORS[];

extern const int ACTUAL_NUM_RESERVED_WORDS;
extern const int ACTUAL_NUM_VALID_OPERATORS;

// Declaração da função para converter TokenType em string
const char* token_type_to_string(TokenType type);

#endif // TOKENS_H
