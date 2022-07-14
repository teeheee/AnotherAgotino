#include "tick_timer.h"
#if !( defined(__AVR_ATmega4809__) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_AVR_NANO_EVERY) || \
      defined(ARDUINO_AVR_ATmega4809) || defined(ARDUINO_AVR_ATmega4808) || defined(ARDUINO_AVR_ATmega3209) || \
      defined(ARDUINO_AVR_ATmega3208) || defined(ARDUINO_AVR_ATmega1609) || defined(ARDUINO_AVR_ATmega1608) || \
      defined(ARDUINO_AVR_ATmega809) || defined(ARDUINO_AVR_ATmega808) )
  #error This is designed only for Arduino or MegaCoreX megaAVR board! Please check your Tools->Board setting
#endif

// These define's must be placed at the beginning before #include "megaAVR_TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select USING_16MHZ     == true for  16MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_8MHZ      == true for   8MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_250KHZ    == true for 250KHz to Timer TCBx => shorter timer, but better accuracy
// Not select for default 250KHz to Timer TCBx => longer timer,  but worse accuracy
#define USING_16MHZ     true
#define USING_8MHZ      false
#define USING_250KHZ    false

#define USE_TIMER_0     false
#define USE_TIMER_1     true
#define USE_TIMER_2     true
#define USE_TIMER_3     false

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "megaAVR_TimerInterrupt.h"
#include <SimpleTimer.h>
#include "config.h"
#include "stepper.h"


float ra_freq = 0;
float dec_freq = 0;

int tick_compensation = 0; 

long start_time = 0;

#define PRINT_VAR(XX)  {Serial.print(#XX": ");Serial.println(XX);}
void timer_status()
{
    PRINT_VAR(tick_compensation);
    PRINT_VAR(ra_freq);
    PRINT_VAR(dec_freq);
}

void timer_handle_ra()
{
  if(stepper_ra_step())
  {
    timer_set_interval_ra(FREQ_RA_1_HZ);
    stepper_set_ra_micro_stepping(1);
    stepper_ra_set_dir(1);
  }
}

void timer_handle_dec()
{
  if(stepper_dec_step())
  {
    timer_stop_dec();
    stepper_set_dec_micro_stepping(1);
    stepper_dec_set_dir(1);
  }
}

void timer_set_interval_ra(float freq)
{
  ra_freq = freq;
  if(freq==0)
  {
    ITimer1.detachInterrupt();
  }
  else
  {
    ITimer1.attachInterrupt(freq, timer_handle_ra);
  }
}

void timer_set_interval_dec(float freq)
{
  dec_freq = freq;
  if(freq==0)
  {
    ITimer2.detachInterrupt();
  }
  else
  {
    ITimer2.attachInterrupt(freq, timer_handle_dec);
  }
}

void timer_stop_ra()
{
  timer_set_interval_ra(0);
}

void timer_stop_dec()
{
  timer_set_interval_dec(0);
}

void timer_default()
{
  timer_set_interval_ra(FREQ_RA_1_HZ);
  timer_stop_dec();
}

void timer_init()
{
  ITimer1.init();
  ITimer2.init();
  start_time = millis();
}

long time_get_secs()
{
  return (millis()-start_time)/1000;
}