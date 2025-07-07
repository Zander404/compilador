#ifndef SYMTAB_H
#define SYMTAB_H

#include <stddef.h> // Para size_t

// Categoria do símbolo (Variável ou Função)
typedef enum {
    SYM_NONE,       // Categoria não definida ou erro
    SYM_VARIAVEL,
    SYM_FUNCAO
} SymbolCategory;

// Tipo de dado para variáveis, ou tipo de retorno para funções (se aplicável)
// TYPE_FUNCAO pode ser usado para a 'data_type' de um símbolo de função em si.
// TYPE_NONE pode ser usado para variáveis sem tipo especificado (se permitido) ou outros casos.
typedef enum {
    TYPE_NONE,      // Sem tipo / Não aplicável
    TYPE_INTEIRO,
    TYPE_TEXTO,
    TYPE_DECIMAL,
    TYPE_FUNCAO,    // Usado para a 'data_type' de um SymbolEntry que é uma SYM_FUNCAO
    TYPE_VOID       // Para funções que podem não ter um tipo de retorno explícito na tabela (embora a regra diga que têm)
} DataType;

// Estrutura para uma entrada na Tabela de Símbolos
typedef struct {
    char* name;             // Nome do símbolo (ex: "!minhaVar", "__minhaFunc")
    SymbolCategory category; // Se é VARIAVEL ou FUNCAO
    DataType data_type;      // Para SYM_VARIAVEL: tipo da variável (INTEIRO, TEXTO, DECIMAL)
                            // Para SYM_FUNCAO: pode ser TYPE_FUNCAO ou o tipo de retorno se quisermos armazená-lo aqui.
    char* dimensions;       // Para variáveis tipo TEXTO ou DECIMAL (ex: "[10]", "[3.2]")
                            // NULL se não aplicável.
    int line_declared;      // Linha onde o símbolo foi declarado
    // Futuramente:
    // char* scope;         // Escopo do símbolo (ex: "global", "__nomeDaFuncaoPai")
    // struct SymbolEntry* parameters; // Lista de parâmetros para funções
    // size_t num_parameters;
    // DataType return_type_for_function; // Se data_type for usado apenas para vars
} SymbolEntry;

// Estrutura para a Tabela de Símbolos (array dinâmico de SymbolEntry)
typedef struct {
    SymbolEntry *entries;   // Ponteiro para o array de entradas de símbolo
    size_t size;            // Número atual de entradas na tabela
    size_t capacity;        // Capacidade atual do array de entradas
} SymbolTable;

// Funções para gerenciar a Tabela de Símbolos
SymbolTable* create_symbol_table(size_t initial_capacity);
void add_symbol(SymbolTable *table, const char* name, SymbolCategory category, DataType data_type, const char* dimensions, int line_num);
void print_symbol_table(const SymbolTable *table);
void free_symbol_table(SymbolTable *table);

// Funções auxiliares para converter enums em strings para impressão
const char* symbol_category_to_string(SymbolCategory category);
const char* data_type_to_string(DataType type);

#endif // SYMTAB_H
