#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "tokens.h"

typedef struct {
  TokenType type;
  char *word;
  int line;

} Token;

typedef struct {
  const char* word;
  TokenType type;
} ReservedWord;

extern const ReservedWord reserved_words[];


#endif
