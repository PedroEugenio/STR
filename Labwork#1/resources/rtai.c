#include "rtai.h"

/*
*
*
*
*/

RT_TASK TASK[MAX_RT_TASKS];
int 	num_tasks = 0;


int rt_task_make_periodic(pthread_t _pthread_id, struct timespec _start, struct timespec _period) {

  if (num_tasks >= MAX_RT_TASKS) {
    return -1;
  }
  for (int i = 0; i <= num_tasks; i++) {
    if (num_tasks == i) {
      TASK[i].ID = _pthread_id;
      num_tasks++;
    }
    if (num_tasks == i || pthread_equal(pthread_id, TASK[i].ID)) {
      TASK[i].NEXT_PERIOD = _start;
      TASK[i].PERIOD = _period;
      break;
    }
  }
  return 0;
}

int rt_task_make_periodic_relative_ns(pthread_t _pthread_id, struct timespec _start, struct timespec _period) {

  struct timespec _current_time;

  if (num_tasks >= MAX_RT_TASKS) {
    return -1;
  }
  for (nt i = 0; i < num_tasks; i++) {
    if(num_tasks == i) {
      TASK[i].ID = _pthread_id;
      num_tasks++;
    }
    if (num_tasks == i || pthread_equal(pthread_id, TASK[i].ID)) {

      if (clock_gettime(CLOCK_MONOTONIC, & _current_time) == -1)
        Error_RT("rt_task_make_periodic_relative_ns->clock_gettime");

      TASK[i].NEXT_PERIOD = _start;
      TASK[i].PERIOD = _period;

      return 0;
    }
  }
  return -1;
}

int rt_task_wait_period() {

  struct timespec _current_time;

  if (clock_gettime(CLOCK_MONOTONIC, & _current_time) == -1)
    Error_RT("rt_task_wait_period->clock_gettime");

  pthread_t current_thread_ID = pthread_self();

  for (int i = 0; i < count; i++) {
    if (pthread_equal(current_thread_ID, TASK[i].ID)) {

      if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(TASK[i].NEXT_PERIOD), NULL) != 0)
          Error_RT("rt_task_wait_period->clock_gettime");

      TASK[i].NEXT_PERIOD = SUM(TASK[i].NEXT_PERIOD, TASK[i].PERIOD);

      return 0;
    }
  }
  return -1;
}

int rt_task_get_next_period(struct timespec *NEXT_PERIOD) {

  pthread_t current_thread_ID = pthread_self();

  for (int i = 0; i < num_tasks; i++) {
    if (pthread_equal(current_thread_ID, TASK[i].ID)) {
      *NEXT_PERIOD = TASK[i].NEXT_PERIOD;
      return 0;
    }
  }
  return -1;
}
