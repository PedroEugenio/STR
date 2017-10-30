
#include "timespec.h"
#include "func2.h"

/*
* First periodic Task
*/
void f1(int class, int group) {

  int total = (int)(TIME1 / WAIT_TIME);

  for (int i = 0; i < total; i++) {
    wait_time();
  }
}
/*
* Second periodic Task
*/
void f2(int class, int group) {

  int total = (int)(TIME2 / WAIT_TIME);

  for (int i = 0; i < total; i++) {
    wait_time();
  }

}
/*
* Third periodic Task
*/
void f3(int class, int group) {

  int total = (int)(TIME3 / WAIT_TIME);

  for (int i = 0; i < total; i++) {
    wait_time();
  }

}
/*
* Emulate a task of real computation with the same time of computation of such task
*/
void wait_time() {

  struct timespec finish, current;

  /* Setting Time in nanoseconds */
  finish = SET((int)WAIT_TIME, 0);

  if (clock_gettime(CLOCK_MONOTONIC, &current) == -1) {
    threadErrorExit("Errror on geting time!");
  }

  finish = SUM(current, finish);

  while (IF_UPPER(CLOCK_MONOTONIC, &current) == 1) { // -1 ??
    if (clock_gettime(CLOCK_MONOTONIC, &current) == -1) {
      threadErrorExit("Errror on geting time!");
    }
  }
}
/*
* Error Geting Time
*/
// void error_gettime(struct &current) {
//   if (clock_gettime(CLOCK_MONOTONIC, &current) == -1) {
//     threadErrorExit("Errror on geting time!");
//   }
// }
