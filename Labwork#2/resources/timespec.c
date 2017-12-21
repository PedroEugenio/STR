#include "timespec.h"


struct timespec DIFF(struct timespec t2, struct timespec t1) {

  struct timespec deltaTime;

  deltaTime.tv_nsec = t2.tv_nsec - t1.tv_nsec;
  deltaTime.tv_sec = t2.tv_sec - t1.tv_sec;

  /* In case where t1 is greater than t2 */
  if (t1.tv_nsec > t2.tv_nsec) {

    deltaTime.tv_nsec = deltaTime.tv_nsec + S2NS;
    deltaTime.tv_sec = deltaTime.tv_sec - 1;
  }
  return deltaTime;
}

struct timespec SUM(struct timespec t2, struct timespec t1) {

  struct timespec deltaTime;

  deltaTime.tv_nsec = t2.tv_nsec + t1.tv_nsec;
  deltaTime.tv_sec = t2.tv_sec + t1.tv_sec;

  if( deltaTime.tv_nsec > (int)S2NS-1) {

    deltaTime.tv_nsec = deltaTime.tv_nsec % (int)S2NS;
    deltaTime.tv_sec = deltaTime.tv_sec + 1;
  }
  return deltaTime;
}

struct timespec SET(int s, int ns) {

  struct timespec deltaTime;

  deltaTime.tv_nsec = (long)(ns % (int)S2NS);
  deltaTime.tv_sec = (time_t)(s + (int)(ns / (int)S2NS));

  return deltaTime;
}

long double time2ms(struct timespec t) {

  return (long double)(t.tv_sec * (long double)S2MS + t.tv_nsec / (long double)MS2NS);

}

long double time2s(struct timespec t) {

	return (long double)(t.tv_sec + t.tv_nsec / 1E9);

}

int IF_UPPER(struct timespec t2, struct timespec t1) {

  if (t2.tv_sec > t1.tv_sec) {
    return 1;
  }else{
    if (t2.tv_sec == t1.tv_sec) {
      if (t2.tv_nsec > t1.tv_nsec) {
        return 1;
      }else{
        return 0;
      }
    }else{
      return 0;
    }
  }
}

int IF_LOWER(struct timespec t2, struct timespec t1) {

  if (t2.tv_sec < t1.tv_sec) {
    return 1;
  }else{
    if (t2.tv_sec == t1.tv_sec) {
      if (t2.tv_nsec < t1.tv_nsec) {
        return 1;
      }else{
        return 0;
      }
    }else{
      return 0;
    }
  }
}

int IF_EQUAL(struct timespec t2, struct timespec t1) {

  if (t2.tv_sec == t1.tv_sec && t2.tv_nsec == t1.tv_nsec) {
    return 1;
  }
  return 0;
}

int IF_UPPER_OR_EQUAL(struct timespec t2, struct timespec t1) {

  if (IF_UPPER(t2, t1) || IF_EQUAL(t2, t1)) {
    return 1;
  }
  return 0;
}

int IF_LOWER_OR_EQUAL(struct timespec t2, struct timespec t1) {

  if (IF_LOWER(t2, t1) || IF_EQUAL(t2, t1)) {
    return 1;
  }
  return 0;
}
