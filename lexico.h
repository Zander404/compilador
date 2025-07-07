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

void checkReservedWord(const char *word, int num_line);
void checkVariable(const char *word, int num_line);
void checkInteger(const char *word, int num_line);
void checkDecimal(const char *word, int num_line);
void checkString(const char *word, int num_line);

void invalidToken();


#endif
