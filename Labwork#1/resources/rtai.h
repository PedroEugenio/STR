
#include "timespec.h"

#ifndef Error_RT
#define Error_RT(msg)    do { perror(msg); return -1; } while (0)
#endif

#define MAX_RT_TASKS 50

typedef struct {

  pthread_t ID;
  struct timespec PERIOD;
  struct timespec NEXT_PERIOD;
} RT_TASK;

int rt_task_make_periodic(pthread_t, struct timespec, struct timespec);
int rt_task_make_periodic_relative_ns(pthread_t, struct timespec, struct timespec);
int rt_task_wait_period();
int rt_task_get_next_period(struct timespec *);
