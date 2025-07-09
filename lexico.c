#include <stdio.h>    /* For printf, fprintf, stderr, perror, NULL */
#include <stdlib.h>   /* For malloc, free, strtol, strtod, atoi, atof, exit */
#include <ctype.h>    /* For isspace, isalpha, isalnum, isdigit, ispunct */
#include <string.h>   /* For strlen, strcpy, strcspn, strchr, memset */
#include <errno.h>    /* For errno, ERANGE */
#include <limits.h>   /* For LONG_MAX, LONG_MIN */
#include <float.h>    /* For HUGE_VAL */

/* Include your project-specific headers */
#include "memory_controller.h" /* For MALLOC, FREE, STRDUP */
#include "lexico.h"            /* For TokenList, Token, and prototypes */
#include "ascii_table.h"       /* For character constants */
#include "tokens.h"            /* For Token struct, TokenType enum, ReservedWord struct, etc. */

/* External declarations from tokens.c */
extern const ReservedWord reserved_words[];
extern const ReservedWord VALID_OPERATORS[];
/* The #define's for NUM_RESERVED_WORDS and NUM_VALID_OPERATORS should be in tokens.h. */

/* Helper function to convert TokenType enum to its string representation */
/* This function must be defined here or in a .c file and prototyped in its .h */
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

/* Helper function to safely free the dynamically allocated 'word' member
 * of a locally created Token struct.
 * NOTE: This function is not used for tokens added to TokenList, as TokenList
 * manages their freeing. It would be for ephemeral tokens not stored.
 * I'm removing its direct use from here as tokens are now persistent.
 */
/*
static void free_local_token_word(Token *t) {
    if (t != NULL && t->word != NULL) {
        FREE(t->word);
        t->word = NULL;
    }
}
*/

/* --- TokenList Management Functions --- */

/* Creates and initializes a new TokenList */
TokenList* create_token_list() {
    TokenList *list = (TokenList*)MALLOC(sizeof(TokenList));
    if (list == NULL) {
        perror("Erro ao alocar TokenList");
        return NULL;
    }
    list->count = 0;
    list->capacity = 10; /* Initial capacity, will grow dynamically */
    list->tokens = (Token**)MALLOC(sizeof(Token*) * list->capacity);
    if (list->tokens == NULL) {
        perror("Erro ao alocar array de tokens na lista");
        FREE(list);
        return NULL;
    }
    return list;
}

/* Adds a Token to the TokenList, reallocating if necessary */
void add_token_to_list(TokenList *list, Token *token) {
    if (list == NULL || token == NULL) return;

    /* Resize the array if capacity is reached */
    if (list->count == list->capacity) {
        list->capacity *= 2; /* Double the capacity */
        /* Use realloc (which you defined in memory_controller.h to use MALLOC internally if needed) */
        Token **new_tokens = (Token**)realloc(list->tokens, sizeof(Token*) * list->capacity);
        if (new_tokens == NULL) {
            perror("Erro ao realocar lista de tokens");
            /* Handle fatal error: cannot add token, memory full. */
            FREE(token->word); /* Free token's word before losing it */
            FREE(token);       /* Free token itself */
            return;
        }
        list->tokens = new_tokens;
    }

    list->tokens[list->count++] = token; /* Add token and increment count */
}

/* Frees all memory associated with the TokenList and its Tokens */
void destroy_token_list(TokenList *list) {
    size_t i; /* Declared at beginning of block for C90 */
    if (list == NULL) return;

    for (i = 0; i < list->count; i++) {
        if (list->tokens[i] != NULL) {
            FREE(list->tokens[i]->word); /* Free the lexeme string */
            FREE(list->tokens[i]);       /* Free the Token struct itself */
        }
    }
    FREE(list->tokens); /* Free the array of pointers */
    FREE(list);         /* Free the TokenList struct */
}

/* Prints the contents of the TokenList (for debugging) */
void print_token_list(TokenList *list) {
    size_t i; /* Declared at beginning of block for C90 */
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
        /* CORRECTED LINE: Using token_type_to_string_name for type name */
        printf("[%3zu] Linha: %d, Tipo: %s (%s)", i, t->line, token_type_to_string_name(t->type), t->word);
        
        /* Print value union on the same line if possible, or indented */
        if (t->type == TK_NUM_INT) {
            printf(" (Valor Inteiro: %ld)", t->value.int_val);
        } else if (t->type == TK_NUM_DEC) {
            printf(" (Valor Decimal: %lf)", t->value.dec_val);
        }
        printf("\n");
    }
    printf("-------------------------------------------\n");
}

/* --- Check Functions (Modified to Create and Add Tokens) --- */

/* Helper to create a Token* on the heap */
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
    /* Initialize value union to zero for safety */
    token->value.int_val = 0; /* Set a default for unused union members */
    return token;
}

/* Checks parenthesis balance for a "principal" function */
int checkPrincipal(const char* line, int num_line){
    int i = 0;
    int opened = 0;

    while(line[i] != '\0'){
        if(line[i] == OPEN_PAREN){
            opened ++;
        }else if(line[i] == CLOSE_PAREN){
            opened --;
        }
        i++;
    }

    if(opened == 0){
        return 1; /* Balanced */
    }

    fprintf(stderr, "[LINHA] %d | [ERROR] Funcao 'PRINCIPAL' MAL FORMULADA (parenteses desbalanceados)\n", num_line);
    return -1; /* Unbalanced */
}

void checkVariable(const char* word, int num_line, TokenList *list){
    Token *token = create_new_token(TK_VARIAVEL, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token);
}

void checkFunction(const char* word, int num_line, TokenList *list){
    Token *token = create_new_token(TK_FUNCAO, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token);
}

void checkReservedWord(const char* word, int num_line, TokenList *list){
    Token *token = NULL;
    int i;
    TokenType found_type = TK_ERROR; /* Default */
    for(i = 0; i < 11; i++){
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

void checkNumber(const char *word, int num_line, TokenList *list){
    Token *token = (Token*)MALLOC(sizeof(Token)); /* Allocate the Token struct */
    if (token == NULL) {
        perror("Erro ao alocar token para numero");
        return;
    }
    token->line = num_line;
    token->type = TK_ERROR; /* Assume error until validated */
    token->word = STRDUP(word); /* Allocate and copy the word */
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
            token->value.dec_val = dec_value; /* Store converted value */
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
            token->value.int_val = int_value; /* Store converted value */
        }
    }

    /* Only add valid tokens to the list. If TK_ERROR, free it immediately. */
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
    TokenType found_type = TK_ERROR; /* Default */

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

/* Main function to check and tokenize a single line of code, adding tokens to list */
void checkLine(const char *line, int num_line, TokenList *list){
    int i = 0;
    int k = 0;
    char lexema[100];

    while(line[i] != '\0'){
        k = 0;
        memset(lexema, 0, sizeof(lexema));

        while(isspace(line[i])) {
            i++;
        }

        if (line[i] == '\0') break;

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

        /* --- Step 3: Other Token Recognition Logic (as per your existing code) --- */

        /* Verify if is a Variable (starts with '!') */
        if (line[i] == EXCLAMATION) {
            if (k < sizeof(lexema) -1) { lexema[k++] = line[i++]; }
            while(line[i] != '\0' && (isalnum(line[i]) || line[i] == '_') ) {
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkVariable(lexema, num_line, list);
            continue;
        }

        /* Verify if a Function (starts with '__') */
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

        /* Verify if is a ReservedWord or Identifier (starts with letter) */
        else if(isalpha(line[i])){
            while(line[i] != '\0' && isalnum(line[i])){
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkReservedWord(lexema, num_line, list);
            continue;
        }

        /* Verify if is a String (starts with DQUOTE) */
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

        /* Verify if a Number (starts with digit) */
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

        /* Check if is operator */
        if ( line [i] == EQUAL ||  line [i] == PLUS ||  line [i] == MINUS ||  line [i] == AMP ||  line [i] == ASTERISK ||  line [i] == CARET ||  line [i] == LT ||  line [i] == GT ||  line [i] == PIPE) {
          lexema[k++] = line[i++];
          while(!isspace(line[i]) && !isalnum(line[i]) && line[i] != '\0'){
            lexema[k++] = line[i++];
          }
          lexema[k] = '\0';
          checkOperator(lexema, num_line, list);
          continue;
        }



        /* --- Unrecognized Character / Error Handling --- */
        fprintf(stderr, "[LINHA] %d | [ERROR] Caractere ou inicio de token nao reconhecido: '%c' (ASCII: %d)\n", num_line, line[i], line[i]);
        exit(1);
    }
}
