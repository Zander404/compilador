#ifndef TOKENS_H
#define TOKENS_H

// Definição dos tipos de token.
// É uma boa prática prefixar todos os membros de enum com algo como TK_ ou TOKEN_
// para evitar conflitos de nome. TIPO_INTEIRO, TIPO_DECIMAL, TIPO_TEXTO
// podem ser renomeados para TK_TIPO_INTEIRO, etc., para consistência.
typedef enum {
    TK_ERRO = 0,          // Token de erro ou indefinido

    // Palavras Reservadas de Comandos e Estruturas
    TK_PRINCIPAL,       // principal
    TK_FUNCAO,          // funcao
    TK_RETORNO,         // retorno
    TK_LEIA,            // leia
    TK_ESCREVA,         // escreva
    TK_SE,              // se
    TK_SENAO,           // senao
    TK_PARA,            // para

    // Palavras Reservadas de Tipos de Dados
    TK_TIPO_INTEIRO,    // inteiro
    TK_TIPO_DECIMAL,    // decimal
    TK_TIPO_TEXTO,      // texto

    // Identificadores e Literais
    TK_IDENTIFICADOR,   // Nomes de variáveis e funções não classificadas como palavras reservadas
    TK_VARIAVEL,        // Especificamente um identificador usado como variável (pode ser redundante com TK_IDENTIFICADOR)
    TK_LITERAL_INT,     // Literal numérico inteiro (ex: 123)
    TK_LITERAL_DEC,     // Literal numérico decimal (ex: 1.23)
    TK_LITERAL_STRING,  // Literal string (ex: "ola")

    // Operadores
    TK_OP_ATRIBUICAO,   // =
    TK_OP_SOMA,         // +
    TK_OP_SUBTRACAO,    // -
    TK_OP_MULTIPLICACAO,// *
    TK_OP_DIVISAO,      // /
    TK_OP_POTENCIA,     // ^

    TK_OP_IGUAL,        // == (comparação)
    TK_OP_DIFERENTE,    // <>
    TK_OP_MENOR,        // <
    TK_OP_MENOR_IGUAL,  // <=
    TK_OP_MAIOR,        // >
    TK_OP_MAIOR_IGUAL,  // >=

    TK_OP_LOGICO_E,     // &&
    TK_OP_LOGICO_OU,    // ||

    // Delimitadores
    TK_DELIM_ABRE_PAREN,    // (
    TK_DELIM_FECHA_PAREN,   // )
    TK_DELIM_ABRE_COLCH,   // [
    TK_DELIM_FECHA_COLCH,  // ]
    TK_DELIM_ABRE_CHAVES,  // {
    TK_DELIM_FECHA_CHAVES, // }
    TK_DELIM_PONTO_VIRGULA, // ;
    TK_DELIM_VIRGULA,       // ,
    TK_DELIM_PONTO,         // . (usado em decimal, mas pode ser um token se usado em outro contexto)
    TK_DELIM_ASPAS_DUPLAS, // " (pode não ser necessário como token individual se strings são TK_LITERAL_STRING)

    TK_EOF              // Fim do arquivo (End Of File)
} TokenType;


// Estrutura para representar um token.
// O campo 'word' armazenará uma cópia do lexema do token,
// e essa memória deve ser gerenciada (alocada/liberada).
typedef struct {
  TokenType type; // Tipo do token (da enum TokenType)
  char *word;     // O lexema (string) do token. Ex: "principal", "variavel1", "123", "+"
  int line;       // Número da linha onde o token aparece
  // Outras informações podem ser adicionadas aqui, como coluna, valor (para números), etc.
} Token;


// Estrutura auxiliar usada para mapear strings (palavras reservadas, operadores)
// para seus respectivos tipos de token.
typedef struct {
  const char* word; // A representação em string (ex: "se", "==")
  TokenType type;   // O TokenType correspondente
} ReservedWordMapping; // Renomeado de ReservedWord para clareza, já que também é usado para operadores.


// Declarações externas das listas de palavras reservadas e operadores.
// Estas listas são definidas em tokens.c.
extern const ReservedWordMapping reserved_words[];
extern const ReservedWordMapping VALID_OPERATORS[];

// Constantes para o número de elementos nessas listas.
// É mais seguro calcular isso em tokens.c e talvez expor como const int,
// ou usar sizeof(array)/sizeof(array[0]) diretamente onde for necessário.
// As definições anteriores NUM_RESERVED_WORDS e NUM_VALID_OPERATORS eram problemáticas.
extern const int ACTUAL_NUM_RESERVED_WORDS;
extern const int ACTUAL_NUM_VALID_OPERATORS;


#endif // TOKENS_H
