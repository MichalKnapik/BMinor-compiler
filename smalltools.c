#include <stdio.h>
#include "smalltools.h"

void print_with_bar_unless_first(int* first, const char* txt) {
  if (*first != 0) printf("|");
  printf("%s", txt);
  *first = 1;
}

unsigned int get_number_of_positions(unsigned int no) {

  int ctr = 1;
  while ((no = no/10) > 0) ++ctr;

  return ctr;
}

void print_comma_unless_first_entry(int* entrycount) {
  
  if (*entrycount == 0) {
    (*entrycount)++;
  } else printf(",");
  

}
