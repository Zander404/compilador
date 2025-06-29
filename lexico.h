#ifndef LEXICO_H
#define LEXICO_H


typedef struct Stack{
  int *data;
  int top;
  int size;
} Stack;

void checkLine(const char *line, int num_line);

#endif
