#ifndef _STEPPER_H_
#define _STEPPER_H_

void stepper_init();
void stepper_status();

int stepper_ra_step();
int stepper_dec_step();

void stepper_ra_set_dir(int direction);
void stepper_dec_set_dir(int direction);

long stepper_ra_get_steps();
long stepper_dec_get_steps();
void stepper_ra_set_steps(long steps);
void stepper_dec_set_steps(long steps);

void stepper_set_ra_micro_stepping(int divider);
void stepper_set_dec_micro_stepping(int divider);

void stepper_ra_set_power(int mode);
void stepper_dec_set_power(int mode);

void stepper_ra_move_steps(long steps);
void stepper_dec_move_steps(long steps);

#endif
