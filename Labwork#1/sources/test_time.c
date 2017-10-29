#include "timespec.c"
#include <stdio.h>


int main() {

  struct timespec t1, t2;

  printf("Testing timespec librarie!\n");

  t1.tv_sec = 1;
  t2.tv_sec = 5;

  // %LF - long double
  printf("DIFF: %LF s\n", time2s(DIFF(t2, t1)));

  printf("DIFF: %LF ms\n", time2ms(DIFF(t2, t1)));

  printf("SUM: %LF ms\n", time2ms(SUM(t2, t1)));

  printf("IF_UPPER: %d\n", IF_UPPER(t2, t1));

  printf("IF_LOWER: %d\n", IF_LOWER(t2, t1));

  printf("IF_EQUAL: %d\n", IF_EQUAL(t2, t1));

  printf("IF_UPPER_OR_EQUAL: %d \n", IF_UPPER_OR_EQUAL(t2, t1));

  printf("IF_LOWER_OR_EQUAL: %d\n", IF_LOWER_OR_EQUAL(t2, t1));


  return 0;
}
