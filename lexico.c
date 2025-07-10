#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h> 
#include <limits.h>
#include <float.h>


#include "memory_controller.h"
#include "lexico.h"
#include "ascii_table.h"
#include "tokens.h"

/* External declarations from tokens.c */
extern const ReservedWord reserved_words[];
extern const ReservedWord VALID_OPERATORS[];



/* Converter o TOKEN para sua Representação em String */
static const char* token_type_to_string_name(TokenType type) {
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


/* --- Funções para Lidar com a Pilha de TOKEN_LIST --- */
TokenList* create_token_list() {
    TokenList *list = (TokenList*)MALLOC(sizeof(TokenList));
    if (list == NULL) {
        perror("Erro ao alocar TokenList");
        return NULL;
    }
    list->count = 0;
    list->capacity = 10;
    list->tokens = (Token**)MALLOC(sizeof(Token*) * list->capacity);
    if (list->tokens == NULL) {
        perror("Erro ao alocar array de tokens na lista");
        FREE(list);
        return NULL;
    }
    return list;
}


void add_token_to_list(TokenList *list, Token *token) {
    if (list == NULL || token == NULL) return;

    if (list->count == list->capacity) {
        list->capacity *= 2;

        Token **new_tokens = (Token**)realloc(list->tokens, sizeof(Token*) * list->capacity);
        if (new_tokens == NULL) {
            perror("Erro ao realocar lista de tokens");

            FREE(token->word);
            FREE(token);
            return;
        }
        list->tokens = new_tokens;
    }

    list->tokens[list->count++] = token;
}


void destroy_token_list(TokenList *list) {
    size_t i;
    if (list == NULL) return;

    for (i = 0; i < list->count; i++) {
        if (list->tokens[i] != NULL) {
            FREE(list->tokens[i]->word);
            FREE(list->tokens[i]);
        }
    }
    FREE(list->tokens);
    FREE(list);
}

/* Imprimir a Tabela de TOKENS (Todos os elementos reconhecido dado a regras) */
void print_token_list(TokenList *list) {
    size_t i;
    if (list == NULL) {
        printf("Lista de tokens vazia ou nula.\n");
        return;
    }
    printf("\n--- Lista de Tokens Gerados (%zu tokens) ---\n", list->count);
    for (i = 0; i < list->count; i++) {
        Token *t = list->tokens[i];
        if (t == NULL) {
            printf("[%3zu] <NULL TOKEN>\n", i);
            continue;
        }
       
        printf("[%3zu] Linha: %d, Tipo: %s (%s)", i, t->line, token_type_to_string_name(t->type), t->word);
        
       
        if (t->type == TK_NUM_INT) {
            printf(" (Valor Inteiro: %ld)", t->value.int_val);
        } else if (t->type == TK_NUM_DEC) {
            printf(" (Valor Decimal: %lf)", t->value.dec_val);
        }
        printf("\n");
    }
    printf("-------------------------------------------\n");
}



/* Criar Token para coloca na Tabela de TokenList */
static Token* create_new_token(TokenType type, const char* word, int num_line) {
    Token *token = (Token*)MALLOC(sizeof(Token));
    if (token == NULL) {
        perror("Erro ao alocar novo token");
        return NULL;
    }
    token->line = num_line;
    token->type = type;
    token->word = STRDUP(word);
    if (token->word == NULL) {
        perror("Erro ao alocar word para novo token");
        FREE(token);
        return NULL;
    }
   
    token->value.int_val = 0;
    return token;
}

/* Verificar Variavel */
void checkVariable(const char* word, int num_line, TokenList *list){
    Token *token = create_new_token(TK_VARIAVEL, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token);
}


/* Verificar Função */
void checkFunction(const char* word, int num_line, TokenList *list){
    Token *token = create_new_token(TK_FUNCAO, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token);
}


/* Verificar Palavra Reservada */ 
void checkReservedWord(const char* word, int num_line, TokenList *list){
    Token *token = NULL;
    int i;
    TokenType found_type = TK_ERROR;
    for(i = 0; i < NUM_RESERVED_WORDS; i++){
        if (strcmp(reserved_words[i].word, word) == 0){
            found_type = reserved_words[i].type;
            break;
        }
    }
    token = create_new_token(found_type, word, num_line);
    if(found_type == TK_ERROR){
      fprintf(stderr, "[LINHA] %d | [ERROR] PALAVRA RESERVADA NÃO INDENTIFICADO: '%s'\n", num_line, token->word);
      token->type = TK_ERROR;
      exit(1);
    }

    if (token == NULL) return;
    add_token_to_list(list, token);
}


/*Verificar Numero */
void checkNumber(const char *word, int num_line, TokenList *list){
    Token *token = (Token*)MALLOC(sizeof(Token));
    if (token == NULL) {
        perror("Erro ao alocar token para numero");
        return;
    }
    token->line = num_line;
    token->type = TK_ERROR;
    token->word = STRDUP(word);
    if (token->word == NULL) {
        perror("Erro ao alocar word para token de numero");
        FREE(token);
        return;
    }

    if (strchr(token->word, PERIOD) != NULL) {
        double dec_value;
        char *endptr;
        errno = 0;

        dec_value = strtod(token->word, &endptr);

        if (endptr == token->word || *endptr != '\0' || errno == ERANGE) {
            fprintf(stderr, "[LINHA] %d | [ERROR] Formato de DECIMAL inválido ou fora do limite: '%s'\n", num_line, token->word);
            token->type = TK_ERROR;
            exit(1);

        } else {
            token->type = TK_NUM_DEC;
            token->value.dec_val = dec_value;
        }
    } else {
        long int_value;
        char *endptr;
        errno = 0;

        int_value = strtol(token->word, &endptr, 10);

        if (endptr == token->word || *endptr != '\0' || errno == ERANGE) {
            fprintf(stderr, "[LINHA] %d | [ERROR] Formato de INTEGER inválido ou fora do limite: '%s'\n", num_line, token->word);
            token->type = TK_ERROR;
          
        } else {
            token->type = TK_NUM_INT;
            token->value.int_val = int_value; 
        }
    }

    if (token->type != TK_ERROR) {
        add_token_to_list(list, token);
    } else {
        FREE(token->word);
        FREE(token);
        exit(1);

    }
}


void checkString(const char *word, int num_line, TokenList *list){
    Token *token = create_new_token(TK_STRING, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token); 
}

void checkOperator(const char *word, int num_line, TokenList *list){
    Token *token = NULL;
    int i;
    TokenType found_type = TK_ERROR;

    for(i = 0; i < NUM_VALID_OPERATORS; i++){
        if(strcmp(VALID_OPERATORS[i].word, word) == 0){
            found_type = VALID_OPERATORS[i].type;
            break;
        }
    }

    token = create_new_token(found_type, word, num_line);
    if (token == NULL) return;

    if (found_type != TK_ERROR) {
        add_token_to_list(list, token);
    } else {
        fprintf(stderr, "[LINHA] %d | [ERROR] Operador inválido ou não reconhecido: '%s'\n", num_line, token->word);
        FREE(token->word);
        FREE(token);
        exit(1);
    }
}

/*
 * Validador da LINHA (FUNÇÂO PRINCIPAL desse arquivo)
 * A analise da Linha para definir se o Token é Valido e atribuir seu tipo (FUNÇÂO, VARIAVEL, OPERADOR, TEXTO, INTEIRO e DECIMAL)
 * */ 
void checkLine(const char *line, int num_line, TokenList *list){
    int i = 0;
    int k = 0;
    char lexema[100];

    while(line[i] != '\0'){

       /* Limpar o vetor de lexema para a próxima analise de TOKEN */
        k = 0;
        memset(lexema, 0, sizeof(lexema));

        /* Caso o Simbolo atual for um espaço, ir para o próximo caracter */
        while(isspace(line[i])) {
            i++;
        }
        
        /* Caso for o Simbolo de fim de linha sair do while e esperar a próxima linha para ser processada */
        if (line[i] == '\0') break;

        /* Caso, for algum desses simbolos apenas Criar um TOKEN para a lista, para analise futura pelo
         *Duplo balancemento
         * 
         */
        if (line[i] == OPEN_PAREN || line[i] == CLOSE_PAREN ||
            line[i] == OPEN_BRACE || line[i] == CLOSE_BRACE ||
            line[i] == COMMA || line[i] == SEMICOLON ||
            line[i] == OPEN_BRACKET || line[i] == CLOSE_BRACKET) {
            
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            lexema[k] = '\0';
            
            Token *token = create_new_token(TK_DELIM, lexema, num_line);
            if (token == NULL) return;
            add_token_to_list(list, token);
            continue;
        }



        /* Encontroy uma '!', Indicio de ser uma Variável */
        if (line[i] == EXCLAMATION) {
            if (k < sizeof(lexema) -1) { lexema[k++] = line[i++]; }
            while(line[i] != '\0' && (isalnum(line[i]) || line[i] == '_') ) {
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkVariable(lexema, num_line, list);
            continue;
        }

        /* Achou um '__' indica o inicio de uma FUNÇÂO */
        else if(line[i] == '_' && line[i+1] == '_'){
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            while(line[i] != '\0' && isalnum(line[i])){
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkFunction(lexema, num_line, list);
            continue;
        }

        /* Começou com caracter Alphanumerico (a,b,c,d,e,...), indica que é uma PALAVRA RESERVADA*/
        else if(isalpha(line[i])){
            while(line[i] != '\0' && isalnum(line[i])){
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkReservedWord(lexema, num_line, list);
            continue;
        }

        /* Começa com ' " ' (Double QUOTE), indica que é inicio de uma STRING */
        else if(line[i] == DQUOTE){
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            while(line[i] != '\0' && line[i] != DQUOTE){
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            if (line[i] == DQUOTE) {
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            } else {
                fprintf(stderr, "[LINHA] %d | [ERROR] String literal nao terminada (aspas dupla ausente) na linha %d: '%s'\n", num_line, num_line, lexema);
                exit(1);
            }
            lexema[k] = '\0';
            checkString(lexema, num_line, list);
            continue;
        }

        /* Se começar com um DIGITO (1, 2, 3) ou (1.1, 2.1, 3.3), indica que é um DECIMAL ou INTEIRO */
        else if (isdigit(line[i])){
            int has_period = 0;
            while (line[i] != '\0' && (isdigit(line[i]) || (line[i] == PERIOD && !has_period))){
                if (line[i] == PERIOD) has_period = 1;
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkNumber(lexema, num_line, list);
            continue;
        }

        /* Qualquer tipo de operador matemático, Verificar se é um operador valido */
        if ( line [i] == EQUAL ||  line [i] == PLUS ||  line [i] == MINUS ||  line [i] == AMP ||  line [i] == ASTERISK ||  line [i] == CARET ||  line [i] == LT ||  line [i] == GT ||  line [i] == PIPE) {
          lexema[k++] = line[i++];
          while(!isspace(line[i]) && !isalnum(line[i]) && line[i] != '\0' && line[i] != SEMICOLON){
            lexema[k++] = line[i++];
          }
          lexema[k] = '\0';
          checkOperator(lexema, num_line, list);
          continue;
        }



        /* --- Se for um caracter não valido ou desconhecido, emitir erro --- */
        fprintf(stderr, "[LINHA] %d | [ERROR] Caractere ou inicio de token nao reconhecido: '%c' (ASCII: %d)\n", num_line, line[i], line[i]);
        exit(1);
    }
}
