
#ifndef TIMESPEC_H
#define TIMESPEC_H

#include <time.h>

#define S2NS		1E9
#define MS2NS		1E6
#define S2MS    1E3

struct timespec DIFF(struct timespec, struct timespec);

struct timespec SUM(struct timespec, struct timespec);

struct timespec SET(int, int);

long double time2ms(struct timespec);

long double time2s(struct timespec);

int IF_UPPER(struct timespec, struct timespec);

int IF_LOWER(struct timespec, struct timespec);

int IF_EQUAL(struct timespec, struct timespec);

int IF_UPPER_OR_EQUAL(struct timespec, struct timespec);

int IF_LOWER_OR_EQUAL(struct timespec, struct timespec);

#endif
