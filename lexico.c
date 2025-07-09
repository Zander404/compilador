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

/* Helper function to safely free the dynamically allocated 'word' member
 * of a locally created Token struct.
 */
static void free_local_token_word(Token *t) {
    if (t != NULL && t->word != NULL) {
        FREE(t->word); /* Using the FREE macro for consistency and tracking */
        t->word = NULL; /* Prevent dangling pointer */
    }
}

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
        printf("[%3zu] Linha: %d, Tipo: %d (%s)", i, t->line, t->type, t->word);
        if (t->type == TK_NUM_INT) {
            printf(" (Valor Int: %ld)", t->value.int_val);
        } else if (t->type == TK_NUM_DEC) {
            printf(" (Valor Dec: %lf)", t->value.dec_val);
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
    token->value.int_val = 0; /* Or token->value.dec_val = 0.0 */
    return token;
}

/* Checks parenthesis balance for a "principal" function */
/* Prototype in lexico.h should match: int checkPrincipal(const char* line, int num_line); */
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
    TokenType found_type = TK_IDENTIFICADOR; /* Default */

    for(i = 0; i < NUM_RESERVED_WORDS; i++){
        if (strcmp(reserved_words[i].word, word) == 0){
            found_type = reserved_words[i].type;
            break;
        }
    }

    token = create_new_token(found_type, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token);
}

void checkNumber(const char *word, int num_line, TokenList *list){
    Token *token = (Token*)MALLOC(sizeof(Token)); /* Token is a pointer here */
    if (token == NULL) {
        perror("Erro ao alocar token para numero");
        return;
    }
    token->line = num_line;
    token->type = TK_ERROR; /* Assume error until validated */
    token->word = STRDUP(word);
    if (token->word == NULL) {
        perror("Erro ao alocar word para token de numero");
        FREE(token);
        return;
    }

    if (strchr(token->word, PERIOD) != NULL) { /* Use -> for pointer */
        double dec_value;
        char *endptr;
        errno = 0;

        dec_value = strtod(token->word, &endptr); /* Use -> for pointer */

        if (endptr == token->word || *endptr != '\0' || errno == ERANGE) { /* Use -> for pointer */
            fprintf(stderr, "[LINHA] %d | [ERROR] Formato de DECIMAL inválido ou fora do limite: '%s'\n", num_line, token->word); /* Use -> for pointer */
            token->type = TK_ERROR;
        } else {
            token->type = TK_NUM_DEC;
            token->value.dec_val = dec_value; /* Store converted value */
        }
    } else {
        long int_value;
        char *endptr;
        errno = 0;

        int_value = strtol(token->word, &endptr, 10); /* Use -> for pointer */

        if (endptr == token->word || *endptr != '\0' || errno == ERANGE) { /* Use -> for pointer */
            fprintf(stderr, "[LINHA] %d | [ERROR] Formato de INTEGER inválido ou fora do limite: '%s'\n", num_line, token->word); /* Use -> for pointer */
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
        FREE(token->word); /* This is crucial if token->word was allocated but type is ERROR */
        FREE(token);       /* Free the token struct itself */
    }
}


void checkString(const char *word, int num_line, TokenList *list){
    Token *token = create_new_token(TK_STRING, word, num_line);
    if (token == NULL) return;
    add_token_to_list(list, token);
}

void checkOperator(const char *word, int num_line, TokenList *list){
    Token *token = NULL;
    int i; /* Declared at beginning of block for C90 */
    TokenType found_type = TK_ERROR; /* Default to error */

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
        FREE(token->word); /* Free the word before freeing the token */
        FREE(token);
    }
}

/* Main function to check and tokenize a single line of code, adding tokens to list */
/* Prototype in lexico.h should match: void checkLine(const char *line, int num_line, TokenList *list); */
void checkLine(const char *line, int num_line, TokenList *list){
    int i = 0; /* Current character index in the line */
    int k = 0; /* Current character index in the lexema buffer */
    char lexema[100]; /* Temporary buffer for the current token's text */

    while(line[i] != '\0'){
        k = 0;
        memset(lexema, 0, sizeof(lexema)); /* Clear lexema buffer for each new token */

        /* Skip whitespace and structural delimiters (parentheses, braces, brackets, comma, semicolon) */
        /* If these delimiters are meant to be separate tokens, you'd add else-if branches here. */
        while(isspace(line[i]) || line[i] == OPEN_PAREN || line[i] == CLOSE_PAREN ||
              line[i] == OPEN_BRACE || line[i] == CLOSE_BRACE ||
              line[i] == COMMA || line[i] == SEMICOLON ||
              line[i] == OPEN_BRACKET || line[i] == CLOSE_BRACKET) {
            i++;
        }

        if (line[i] == '\0') break; /* End of line after skipping */

        /* --- Token Recognition Logic --- */

        /* Verify if is a Variable (starts with '!') */
        if (line[i] == EXCLAMATION) {
            if (k < sizeof(lexema) -1) { lexema[k++] = line[i++]; } /* Capture '!' */
            while(line[i] != '\0' && (isalnum(line[i]) || line[i] == '_') ) { /* Capture alphanumeric + underscore for var name */
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkVariable(lexema, num_line, list); /* Pass the list */
            continue;
        }

        /* Verify if a Function (starts with '__') */
        /* Corrected: This should be 'line[i+1] == _' (single underscore) if meant for '__' */
        /* Assuming OPEN_PAREN is not literally '_' */
        else if(line[i] == '_' && line[i+1] == '_'){ /* Corrected: '__' */
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; } /* Capture first '_' */
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; } /* Capture second '_' */
            while(line[i] != '\0' && isalnum(line[i])){ /* Capture alphanumeric function name */
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkFunction(lexema, num_line, list); /* Pass the list */
            continue;
        }

        /* Verify if is a ReservedWord or Identifier (starts with letter) */
        else if(isalpha(line[i])){
            while(line[i] != '\0' && isalnum(line[i])){ /* Capture alphanumeric characters for keywords/identifiers */
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkReservedWord(lexema, num_line, list); /* Pass the list */
            continue;
        }

        /* Verify if is a String (starts with DQUOTE) */
        else if(line[i] == DQUOTE){
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; } /* Capture opening quote */
            while(line[i] != '\0' && line[i] != DQUOTE){
                /* TODO: Add logic here to handle escaped quotes like \" */
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            if (line[i] == DQUOTE) { /* Capture closing quote if found */
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            } else {
                fprintf(stderr, "[LINHA] %d | [ERROR] String literal nao terminada (aspas dupla ausente): '%s'\n", num_line, lexema);
                /* The checkString function will create an error token or print an error. */
            }
            lexema[k] = '\0';
            checkString(lexema, num_line, list); /* Pass the list */
            continue;
        }

        /* Verify if a Number (starts with digit) */
        else if (isdigit(line[i])){
            int has_period = 0;
            /* This loop captures digits and a single period. It does NOT handle your [N.N] format. */
            while (line[i] != '\0' && (isdigit(line[i]) || (line[i] == PERIOD && !has_period))){
                if (line[i] == PERIOD) has_period = 1; /* Mark that a period has been seen */
                if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
            }
            lexema[k] = '\0';
            checkNumber(lexema, num_line, list); /* Pass the list */
            continue;
        }

        /* Verify if an Operator */
        else if (line[i] == EQUAL || line[i] == PLUS || line[i] == MINUS || line[i] == AMP ||
                 line[i] == ASTERISK || line[i] == CARET || line[i] == LT || line[i] == GT ||
                 line[i] == PIPE) {
            /* Capture the first character of the potential operator */
            if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }

            /* Check for common two-character operators based on the first character */
            if (line[i] != '\0' && k > 0) {
                char first_char = lexema[0];
                char next_char = line[i];

                if ((first_char == EQUAL && next_char == EQUAL) || /* == */
                    (first_char == AMP && next_char == AMP) ||     /* && */
                    (first_char == PIPE && next_char == PIPE) ||   /* || */
                    (first_char == LT && next_char == EQUAL) ||    /* <= */
                    (first_char == GT && next_char == EQUAL) ||    /* >= */
                    (first_char == LT && next_char == GT)          /* <> */
                   ) {
                    if (k < sizeof(lexema) - 1) { lexema[k++] = line[i++]; } else { break; }
                }
            }
            lexema[k] = '\0';
            checkOperator(lexema, num_line, list); /* Pass the list */
            continue;
        }

        /* --- Unrecognized Character / Error Handling --- */
        /* If execution reaches here, the character at line[i] did not match any token pattern. */
        fprintf(stderr, "[LINHA] %d | [ERROR] Caractere ou inicio de token nao reconhecido: '%c' (ASCII: %d)\n", num_line, line[i], line[i]);
        i++; /* Advance to the next character to avoid infinite loop */
    }
}
