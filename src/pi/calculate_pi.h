#ifndef PI_CALCULATE_PI_H
#define PI_CALCULATE_PI_H

#include "../lib/threading/commons.h"

typedef struct range_t {
	long double initial_value;
	long double range;
} range_t;

typedef struct partial_area_futures_t {
	unsigned int count;
	future_t* futures;
} partial_area_futures_t;

int pi_map(threading_cancel_token_t cancel_token, void* arguments, void** return_value);
int pi_reduce(threading_cancel_token_t cancel_token, void* arguments, void** return_value);

#endif