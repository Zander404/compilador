#ifndef LEXICO_H
#define LEXICO_H

#include "tokens.h"

typedef struct {
  char *text;
  size_t lenght;
} Line;


typedef struct {
  Line *lines;
  int num_lines;
  
} SourceCode;


void checkLine(const char *line, int num_line);

void checkReservedWord();
void checkVariable();
void checkInteger();
void checkDecimal();
void checkString();

void invalidToken();


#endif
