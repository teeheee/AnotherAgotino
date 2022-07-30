#include <Arduino.h>
#include "motion.h"
#include "tick_timer.h"
#include "stepper.h"
#include "config.h"
#include "coords.h"

unsigned long STEP_DELAY_SLEW = 1200;   // Slewing Pulse timing in micros (the higher the pulse, the slower the speed)
                                        // don't change this to too low values otherwise your scope may take off as an helicopter.

void slewRaDecBySteps(unsigned long raSteps, unsigned long decSteps);


void syncRaDecBySecs(long raSecs, long decSecs)
{
    coord_set_dec_sec(decSecs);
    coord_set_ra_sec(raSecs);
}


void slewRaDecBySecs(long raSecs, long decSecs){
  if(raSecs != 0)
  {
    stepper_set_ra_micro_stepping(MICROSTEPS_RA_FAST);
    timer_set_interval_ra(FREQ_RA_1_HZ*SLEW_SPEED_FREQ);
    if(raSecs > 0)
    {
      stepper_ra_set_dir(-1);
    }
    else
    {
      raSecs = -raSecs;
      stepper_ra_set_dir(1);
    }
    stepper_ra_move_steps(SEC_TO_MICROSTEPS_RA(raSecs));
  }
  if(decSecs != 0)
  {
    stepper_set_dec_micro_stepping(MICROSTEPS_DEC_FAST);
    timer_set_interval_dec(FREQ_DEC_1_HZ*SLEW_SPEED_FREQ);
    if(decSecs > 0)
    {
      stepper_dec_set_dir(1);
    }
    else
    {
      decSecs = -decSecs;
      stepper_dec_set_dir(-1);
    }
    stepper_dec_move_steps(SEC_TO_MICROSTEPS_DEC(decSecs));
  }
}


void motion_track(){
  timer_set_interval_ra(FREQ_RA_1_HZ);
  timer_stop_dec();
  stepper_dec_set_dir(1);
  stepper_ra_set_dir(1);
  stepper_set_ra_micro_stepping(MICROSTEPS_RA);
  stepper_set_ra_micro_stepping(MICROSTEPS_DEC);
}

void motion_halt(){
  timer_stop_ra();
  timer_stop_dec();
}

void move_dec_ms(long ms)
{
    if(ms > 0)
    {
      stepper_dec_set_dir(1);
    }
    else
    {
      stepper_dec_set_dir(-1);
    }
    timer_set_interval_dec(FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ);
    timer_reset_stepper_after_ms(labs(ms));
}

void move_ra_ms(long ms)
{
    if(ms > 0)
    {
      timer_set_interval_ra(FREQ_DEC_1_HZ+FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ);
      stepper_ra_set_dir(1);
    }
    else
    {
      float freq = FREQ_DEC_1_HZ - FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ;
      if(freq < 0)
      {
        timer_set_interval_ra(fabs(freq));
        stepper_ra_set_dir(-1);
      }
      else if(freq==0)
      {
        timer_stop_ra();
      }
      else
      {
        timer_set_interval_ra(freq);
        stepper_ra_set_dir(1);
      }
    }
    timer_reset_stepper_after_ms(labs(ms));
}

void start_move_ra(int dir)
{
    stepper_set_dec_micro_stepping(MICROSTEPS_DEC_FAST);
    timer_set_interval_ra(FREQ_DEC_1_HZ*SLEW_SPEED_FREQ);
    stepper_ra_set_dir(dir);
}

void start_move_dec(int dir)
{
    stepper_set_dec_micro_stepping(MICROSTEPS_DEC_FAST);
    timer_set_interval_dec(FREQ_DEC_1_HZ*SLEW_SPEED_FREQ);
    stepper_dec_set_dir(dir);
}
