
#include "timespec.h"

#ifndef rtErrorExit
#define rtErrorExit(msg)    do { perror(msg); return -1; } while (0)
#endif

typedef struct {

  pthread_t ID;
  struct timespec period;
  struct timespec next_period;
} RT_TASK;

int rt_task_make_periodic(pthread_t, struct timespec, struct timespec);
int rt_task_make_periodic_relative_ns(pthread_t, struct timespec, struct timespec);
int rt_task_wait_period();
int rt_task_get_next_period();
