//
// Created by al on 23.07.22.
//

#ifndef DELAY_COUNTER_H
#define DELAY_COUNTER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define DEFAULT_DELAY_VALUE		100

typedef struct delay_counter_t delay_counter_t;
struct delay_counter_t {
	int start_flag;
	int end_flag;
	int cnt;
	int delay_cnt;
};

void counter_init(delay_counter_t* counter);
int counter_tick(delay_counter_t* counter);
void counter_reset(delay_counter_t* counter);
int counter_is_started(delay_counter_t* counter);
void counter_start(delay_counter_t* counter);
int counter_state(delay_counter_t* counter);

#endif // DELAY_COUNTER_H
