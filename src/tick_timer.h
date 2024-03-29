#ifndef _TICK_TIMER_H_
#define _TICK_TIMER_H_

#include <stdint.h>

void timer_init();
void timer_status();


void timer_reset_stepper_after_ms(long ms);

void timer_set_interval_ra(float freq);
void timer_set_interval_dec(float freq);
long time_get_secs();
void timer_stop_ra();
void timer_stop_dec();
void timer_status();
uint8_t timer_slew_is_active();

#endif
