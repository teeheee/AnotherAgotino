#include <Arduino.h>
#include "motion.h"
#include "tick_timer.h"
#include "stepper.h"
#include "config.h"

unsigned long STEP_DELAY_SLEW = 1200;   // Slewing Pulse timing in micros (the higher the pulse, the slower the speed)
                                        // don't change this to too low values otherwise your scope may take off as an helicopter.

void slewRaDecBySteps(unsigned long raSteps, unsigned long decSteps);


void syncRaDecBySecs(long raSecs, long decSecs)
{
    stepper_dec_set_steps(SEC_TO_MICROSTEPS_DEC(decSecs));
    stepper_ra_set_steps(SEC_TO_MICROSTEPS_RA(raSecs));
}

long getRaSecs()
{
    long steps = stepper_ra_get_steps();
    long raSecs = MICROSTEPS_TO_SEC_RA(steps)-time_get_secs();
    while(raSecs < 0)
    {
      raSecs = DAY_SECONDS+raSecs;
    }
    return raSecs%DAY_SECONDS;
}

long getDecSecs()
{
    long steps = stepper_dec_get_steps();
    long decSecs = MICROSTEPS_TO_SEC_DEC(steps);
    return decSecs;
}

void slewRaDecBySecs(long raSecs, long decSecs){
  if(raSecs != 0)
  {
    stepper_set_ra_micro_stepping(MICROSTEPS_RA_FAST);
    timer_set_interval_ra(FREQ_RA_1_HZ*SLEW_SPEED_FREQ);
    if(raSecs > 0)
    {
      stepper_ra_set_dir(1);
    }
    else
    {
      raSecs = -raSecs;
      stepper_ra_set_dir(-1);
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
}

void motion_halt(){
  timer_stop_ra();
  timer_stop_dec();
}

void move_dec_ms(long ms)
{
    long microsteps = MS_TO_MICROSTEPS_DEC(ms);
    if(ms > 0)
    {
      timer_set_interval_dec(FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ);
      stepper_dec_set_dir(1);
      stepper_dec_move_steps(microsteps);
    }
    else
    {
      timer_set_interval_dec(FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ);
      stepper_dec_set_dir(-1);
      stepper_dec_move_steps(-microsteps);
    }
}

void move_ra_ms(long ms)
{
    long microsteps = MS_TO_MICROSTEPS_RA(ms);
    if(ms > 0)
    {
      timer_set_interval_ra(FREQ_DEC_1_HZ+FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ);
      stepper_ra_set_dir(1);
      stepper_ra_move_steps(microsteps);
    }
    else
    {
      float freq = FREQ_DEC_1_HZ - FREQ_DEC_1_HZ*GUIDE_SPEED_FREQ;
      if(freq < 0)
      {
        timer_set_interval_ra(-freq);
        stepper_ra_set_dir(-1);
        stepper_ra_move_steps(-microsteps);
      }
      else if(freq==0)
      {
        timer_set_interval_ra(1000.0/ms);
        stepper_ra_move_steps(1);
      }
      else
      {
        timer_set_interval_dec(freq);
        stepper_dec_set_dir(1);
        stepper_dec_move_steps(-microsteps);
      }
    }
}

