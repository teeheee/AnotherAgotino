#include "stepper.h"
#include "config.h"
#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper
#include <Arduino.h>  

TMC2209Stepper TMC_driver_ra(&Serial1, R_SENSE, DRIVER_ADDRESS_X);   // Create TMC driver
TMC2209Stepper TMC_driver_dec(&Serial1, R_SENSE, DRIVER_ADDRESS_Y);   // Create TMC driver

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
  pinMode(SIDE_OF_PIER_PIN, INPUT_PULLUP);
  pinMode(EN_PIN, OUTPUT);           
  pinMode(STEP_PIN_RA, OUTPUT);
  pinMode(DIR_PIN_RA, OUTPUT);
  pinMode(STEP_PIN_DEC, OUTPUT);
  pinMode(DIR_PIN_DEC, OUTPUT);
  // Enable TMC2209 board
  digitalWrite(EN_PIN, LOW);   
  digitalWrite(STEP_PIN_RA, LOW);
  digitalWrite(STEP_PIN_DEC, LOW);
  digitalWrite(DIR_PIN_RA, LOW);
  digitalWrite(DIR_PIN_DEC, LOW);
 
   delay(100);

  TMC_driver_ra.begin();                                                                                                                                                                                                                                                                                                                          // UART: Init SW UART (if selected) with default 115200 baudrate
  TMC_driver_ra.toff(5);                 // Enables driver in software
  TMC_driver_ra.rms_current(1100);        // Set motor RMS current
  TMC_driver_ra.en_spreadCycle(false);
  TMC_driver_ra.pwm_autoscale(true);     // Needed for stealthChop
  TMC_driver_ra.TPWMTHRS(7);     // Needed for stealthChop
  TMC_driver_ra.microsteps(MICROSTEPS_RA);  


  TMC_driver_dec.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
  TMC_driver_dec.toff(5);                 // Enables driver in software
  TMC_driver_dec.rms_current(1100);        // Set motor RMS current
  TMC_driver_dec.en_spreadCycle(false);
  TMC_driver_dec.TPWMTHRS(7);     // Needed for stealthChop
  TMC_driver_dec.microsteps(MICROSTEPS_DEC);  

  
  stepper_ra_set_dir(1);
  stepper_dec_set_dir(1);
  Serial1.end();
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
}

void process_stepper()
{
    if(update_microsteps_ra && update_microsteps_dec) //TODO should be or but driver could not have different microstep at the same time
    {
        Serial1.begin(9600); 
        if(update_microsteps_ra)
        {
            TMC_driver_ra.microsteps(ra_microsteps);          // Set microsteps
            update_microsteps_ra = 0;
        }
        if(update_microsteps_dec)
        {
            TMC_driver_dec.microsteps(dec_microsteps);          // Set microsteps
            update_microsteps_dec = 0;
        }
        Serial1.end();
        digitalWrite(0, LOW);
        digitalWrite(1, LOW);
    }
}

int stepper_ra_step()
{
    digitalWrite(STEP_PIN_RA, HIGH);
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
    digitalWrite(STEP_PIN_RA, LOW);
    return ret;
}

int stepper_dec_step()
{
    digitalWrite(STEP_PIN_DEC, HIGH);
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
    digitalWrite(STEP_PIN_DEC, LOW);
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
    if(direction > 0)
        digitalWrite(DIR_PIN_RA, LOW);
    else
        digitalWrite(DIR_PIN_RA, HIGH);
}


void stepper_dec_set_dir(int direction)
{
    dec_direction = direction;
    if(digitalRead(SIDE_OF_PIER_PIN))
        direction *= -1;
    if(direction > 0)
        digitalWrite(DIR_PIN_DEC, HIGH);
    else
        digitalWrite(DIR_PIN_DEC, LOW);
}

void stepper_set_ra_micro_stepping(int microsteps)
{
  if(ra_microsteps==microsteps)
    return;
  update_microsteps_ra = 1; 
  ra_microsteps = microsteps;
}

void stepper_set_dec_micro_stepping(int microsteps)
{
  if(dec_microsteps==microsteps)
    return;
  update_microsteps_dec = 1; 
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