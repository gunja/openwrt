#include "delay_counter.h"

#define RX_BUF_SIZE         64

void counter_init(delay_counter_t* counter)
{
	counter->cnt = 0;
	counter->start_flag = 0;
	counter->end_flag = 0;
	counter->delay_cnt = DEFAULT_DELAY_VALUE;
}

int counter_tick(delay_counter_t* counter)
{
	counter->cnt++;
	if(counter->cnt >= counter->delay_cnt)
	{
		counter->end_flag = 1;
	}
	return counter->end_flag;
}

void counter_reset(delay_counter_t* counter)
{
	counter->start_flag = 0;
	counter->end_flag = 0;
	counter->cnt = 0;
}

int counter_is_started(delay_counter_t* counter)
{
	return counter->start_flag == 1;
}

void counter_start(delay_counter_t* counter)
{
	counter->start_flag = 1;
}

int counter_state(delay_counter_t* counter)
{
	return counter->cnt;
}
