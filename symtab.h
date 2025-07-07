#ifndef SYMTAB_H
#define SYMTAB_H

#include <stddef.h> // Para size_t
#include "tokens.h" // Para TokenType (usado em DataType se necessário, ou para referência)

// Categoria do símbolo
typedef enum {
    SYM_NONE = 0,    // Indefinido ou para inicialização
    SYM_VARIAVEL,
    SYM_FUNCAO
    // Poderíamos adicionar SYM_PARAMETRO, SYM_CONSTANTE etc. no futuro
} SymbolCategory;

// Tipo de dado do símbolo (para variáveis) ou identificador geral para funções
typedef enum {
    TYPE_NONE = 0,     // Indefinido, ou não aplicável (ex: para um label)
    TYPE_INTEIRO,
    TYPE_TEXTO,
    TYPE_DECIMAL,
    TYPE_FUNCAO_DEF, // Usado para marcar a entrada de uma função na tabela
    TYPE_VOID        // Ex: para tipo de retorno de função, se aplicável
    // Outros tipos podem ser adicionados (ex: TYPE_BOOLEAN)
} DataType;

// Estrutura para uma entrada na Tabela de Símbolos
typedef struct {
    char* name;             // Nome do símbolo (ex: "!minhaVar", "__minhaFunc")
    SymbolCategory category; // VARIAVEL ou FUNCAO
    DataType data_type;      // Tipo da variável, ou TYPE_FUNCAO_DEF para funções
    char* dimensions;       // String para dimensões, ex: "[10]", "[3.2]", ou NULL
    int line_declared;      // Linha da declaração
    char* scope;            // Escopo (ex: "global", nome da função)
                            // (Inicialmente, podemos simplificar para "global" ou nome da função atual)
    // Campos futuros potenciais:
    // TokenType return_type; // Para funções, o tipo de dado que retorna
    // struct SymbolEntry* params; // Lista de parâmetros para funções
    // int num_params;
    // void* value_ptr; // Ponteiro para o valor real (para um interpretador)
    // int address;     // Endereço de memória (para um compilador gerando código)
} SymbolEntry;

// Estrutura para a Tabela de Símbolos (array dinâmico)
typedef struct {
    SymbolEntry *entries;   // Ponteiro para o array de entradas
    size_t size;            // Número atual de símbolos na tabela
    size_t capacity;        // Capacidade atual do array
} SymbolTable;

// --- Interface da Tabela de Símbolos ---

SymbolTable* create_symbol_table(size_t initial_capacity);

// Adiciona um símbolo à tabela. Faz cópias gerenciadas de name, dimensions e scope.
void add_symbol(SymbolTable *table,
                const char* name,
                SymbolCategory category,
                DataType data_type,
                const char* dimensions, // Pode ser NULL
                int line_num,
                const char* scope);     // Pode ser NULL para global, ou nome da função

// Imprime a tabela de símbolos de forma formatada.
void print_symbol_table(const SymbolTable *table);

// Libera toda a memória associada à tabela de símbolos e suas entradas.
void free_symbol_table(SymbolTable *table);

// --- Funções Auxiliares de Conversão para String (para impressão) ---
const char* symbol_category_to_string(SymbolCategory category);
const char* data_type_to_string(DataType type);

#endif // SYMTAB_H
