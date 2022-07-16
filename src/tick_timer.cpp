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
#define USE_TIMER_2     false
#define USE_TIMER_3     false

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "megaAVR_TimerInterrupt.h"


// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "megaAVR_ISR_Timer.h"

#include <SimpleTimer.h>              // https://github.com/jfturcot/SimpleTimer

#include "config.h"
#include "stepper.h"

ISR_Timer ISR_Timer_Object;

int timer_handle_ra = 0;
int timer_handle_dec = 0;
int timer_handle_timeout = 0;

void TimerHandler()
{
  ISR_Timer_Object.run();
}

float ra_freq = 0;
float dec_freq = 0;


#define PRINT_VAR(XX)  {Serial.print(#XX": ");Serial.println(XX);}

void timer_status()
{
    PRINT_VAR(ra_freq);
    PRINT_VAR(dec_freq);
}

void timer_handle_reset_stepper()
{
    timer_set_interval_ra(FREQ_RA_1_HZ);
    timer_stop_dec();
    stepper_set_dec_micro_stepping(MICROSTEPS_DEC);
    stepper_dec_set_dir(1);
    stepper_set_ra_micro_stepping(MICROSTEPS_RA);
    stepper_ra_set_dir(1);
    ISR_Timer_Object.disable(timer_handle_dec);
}

void timer_handle_ra_callback()
{
  if(stepper_ra_step())
  {
    timer_set_interval_ra(FREQ_RA_1_HZ);
    stepper_set_ra_micro_stepping(MICROSTEPS_RA);
    stepper_ra_set_dir(1);
  }
}

void timer_handle_dec_callback()
{
  if(stepper_dec_step())
  {
    timer_stop_dec();
    stepper_set_dec_micro_stepping(MICROSTEPS_DEC);
    stepper_dec_set_dir(1);
  }
}

void timer_set_interval_ra(float freq)
{
  ra_freq = freq;
  if(freq==0)
  {
    ISR_Timer_Object.disable(timer_handle_ra);
  }
  else
  {
    timer_handle_ra = ISR_Timer_Object.setInterval(1000.0/freq, timer_handle_ra_callback);
  }
}

void timer_set_interval_dec(float freq)
{
  dec_freq = freq;
  if(freq==0)
  {
    ISR_Timer_Object.disable(timer_handle_dec);
  }
  else
  {
    timer_handle_dec = ISR_Timer_Object.setInterval(1000.0/freq, timer_handle_dec_callback);
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
  ITimer1.attachInterruptInterval(1, TimerHandler);
}

void timer_reset_stepper_after_ms(long ms)
{
  ISR_Timer_Object.setTimeout(ms, timer_handle_reset_stepper);
}