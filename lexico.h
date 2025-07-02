#ifndef LEXICO_H
#define LEXICO_H

#include "symbols.h"

void checkLine(const char *line, int num_line);

void checkReservedWord();
void checkVariable();
void checkInteger();
void checkDecimal();
void checkString(); 


#endif
