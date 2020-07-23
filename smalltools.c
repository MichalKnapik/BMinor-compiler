#include <stdio.h>
#include "smalltools.h"

void print_with_bar_unless_first(int* first, const char* txt) {
  if (*first != 0) printf("|");
  printf("%s", txt);
  *first = 1;
}
