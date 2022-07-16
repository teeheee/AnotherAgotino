#include "stepper.h"
#include "config.h"
#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper
#include <Arduino.h>  

TMC2209Stepper TMCdriverx(&Serial1, R_SENSE, DRIVER_ADDRESS_X);   // Create TMC driver
TMC2209Stepper TMCdrivery(&Serial1, R_SENSE, DRIVER_ADDRESS_Y);   // Create TMC driver

int ra_direction = 1;
int dec_direction = 1;

int ra_microsteps = MICROSTEPS_RA;
int dec_microsteps = MICROSTEPS_DEC;

long ra_move_until = 0;
long dec_move_until = 0;

int update_microsteps_ra = 0;
int update_microsteps_dec = 0;


#define PRINT_VAR(XX)  {Serial.print(#XX": ");Serial.println(XX);}
void stepper_status()
{
    PRINT_VAR(ra_direction);
    PRINT_VAR(dec_direction);
    PRINT_VAR(ra_microsteps);
    PRINT_VAR(dec_microsteps);
    PRINT_VAR(ra_move_until);
    PRINT_VAR(dec_move_until);
}

void stepper_init()
{
  // Set pinmodes
  Serial1.begin(9600); 
  pinMode(EN_PIN, OUTPUT);           
  pinMode(STEP_PIN_X, OUTPUT);
  pinMode(DIR_PIN_X, OUTPUT);
  pinMode(STEP_PIN_Y, OUTPUT);
  pinMode(DIR_PIN_Y, OUTPUT);
  // Enable TMC2209 board
  digitalWrite(EN_PIN, LOW);   
  digitalWrite(STEP_PIN_X, LOW);
  digitalWrite(STEP_PIN_Y, LOW);
  digitalWrite(DIR_PIN_X, LOW);
  digitalWrite(DIR_PIN_Y, LOW);
 
   delay(100);

  TMCdriverx.begin();                                                                                                                                                                                                                                                                                                                          // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdriverx.toff(5);                 // Enables driver in software
  TMCdriverx.rms_current(800);        // Set motor RMS current
  TMCdriverx.en_spreadCycle(false);
  TMCdriverx.pwm_autoscale(true);     // Needed for stealthChop
  TMCdriverx.TPWMTHRS(7);     // Needed for stealthChop
  TMCdriverx.microsteps(MICROSTEPS_RA);  


  TMCdrivery.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdrivery.toff(5);                 // Enables driver in software
  TMCdrivery.rms_current(800);        // Set motor RMS current
  TMCdrivery.en_spreadCycle(false);
  TMCdrivery.TPWMTHRS(7);     // Needed for stealthChop
  TMCdrivery.microsteps(MICROSTEPS_DEC);  

  
  stepper_ra_set_dir(1);
  stepper_dec_set_dir(1);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
}

void process_stepper()
{
    if(update_microsteps_ra)
    {
        TMCdriverx.microsteps(ra_microsteps);          // Set microsteps
    }
    if(update_microsteps_dec)
    {
        TMCdrivery.microsteps(dec_microsteps);          // Set microsteps
    }
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);
}

int stepper_ra_step()
{
    digitalWrite(STEP_PIN_X, HIGH);
    int ret = 0;
    if(ra_move_until)
    {
        ra_move_until -= MICROSTEPS_RA/ra_microsteps;
        if(ra_move_until <= 0)
        {
            ra_move_until = 0;
            ret = 1;
        }
    }
    delayMicroseconds(5);
    digitalWrite(STEP_PIN_X, LOW);
    return ret;
}

int stepper_dec_step()
{
    digitalWrite(STEP_PIN_Y, HIGH);
    int ret = 0;
    if(dec_move_until)
    {
        dec_move_until -= MICROSTEPS_DEC/dec_microsteps;
        if(dec_move_until<=0)
        {
            dec_move_until = 0;
            ret = 1;
        }
    }
    delayMicroseconds(5);
    digitalWrite(STEP_PIN_Y, LOW);
    return ret;
}

long stepper_ra_get_remaining_secs()
{
    return MICROSTEPS_TO_SEC_RA(ra_move_until);
}

long stepper_dec_get_remaining_secs()
{
    return MICROSTEPS_TO_SEC_DEC(dec_move_until);
}


void stepper_ra_set_dir(int direction)
{
    ra_direction = direction;
    if(direction == 1)
        digitalWrite(HIGH, DIR_PIN_Y);
    if(direction == -1)
        digitalWrite(LOW, DIR_PIN_Y);
}


void stepper_dec_set_dir(int direction)
{
    dec_direction = direction;
    if(direction == 1)
        digitalWrite(HIGH, DIR_PIN_X);
    if(direction == -1)
        digitalWrite(LOW, DIR_PIN_X);
}

void stepper_set_ra_micro_stepping(int microsteps)
{
  if(ra_microsteps==microsteps)
    return;
  update_microsteps_dec = 1; 
  ra_microsteps = microsteps;
}

void stepper_set_dec_micro_stepping(int microsteps)
{
  if(dec_microsteps==microsteps)
    return;
  update_microsteps_ra = 1; 
  dec_microsteps = microsteps;
}

void stepper_ra_set_power(int mode)
{

}

void stepper_dec_set_power(int mode)
{

}


void stepper_ra_move_steps(long steps)
{
    ra_move_until = steps;
}

void stepper_dec_move_steps(long steps)
{
    dec_move_until = steps;
}

int stepper_ra_get_dir()
{
    return ra_direction;
}

int stepper_dec_get_dir()
{
    return dec_direction;
}