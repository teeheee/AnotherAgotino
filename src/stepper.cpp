#include "stepper.h"
#include "config.h"
#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper
#include <SoftwareSerial.h>     // Software serial for the UART to TMC2209 - https://www.arduino.cc/en/Reference/softwareSerial


SoftwareSerial SoftSerial(SW_RX, SW_TX);                     // Be sure to connect RX to TX and TX to RX between both devices
TMC2209Stepper TMCdriverx(&SoftSerial, R_SENSE, DRIVER_ADDRESS_X);   // Create TMC driver
TMC2209Stepper TMCdrivery(&SoftSerial, R_SENSE, DRIVER_ADDRESS_Y);   // Create TMC driver

long ra_counter = 0;
long dec_counter = 0;

int ra_direction = 1;
int dec_direction = 1;

int ra_multiplier = 1;
int dec_multiplier = 1;

long ra_move_until = 0;
long dec_move_until = 0;

int current_ra_divider = 0;
int current_dec_divider = 0;

#define PRINT_VAR(XX)  {Serial.print(#XX": ");Serial.println(XX);}
void stepper_status()
{
    PRINT_VAR(ra_counter);
    PRINT_VAR(dec_counter);
    PRINT_VAR(ra_direction);
    PRINT_VAR(dec_direction);
    PRINT_VAR(ra_multiplier);
    PRINT_VAR(dec_multiplier);
    PRINT_VAR(ra_move_until);
    PRINT_VAR(dec_move_until);
    PRINT_VAR(current_ra_divider);
    PRINT_VAR(current_dec_divider);
}

void stepper_init()
{
  // Set pinmodes
  pinMode(SW_RX, INPUT);
  pinMode(SW_TX, OUTPUT);
  pinMode(EN_PIN, OUTPUT);           
  pinMode(STEP_PIN_X, OUTPUT);
  pinMode(DIR_PIN_X, OUTPUT);
  pinMode(STEP_PIN_Y, OUTPUT);
  pinMode(DIR_PIN_Y, OUTPUT);
  // Enable TMC2209 board
  digitalWrite(EN_PIN, LOW);   
  digitalWrite(STEP_PIN_X, LOW);
  digitalWrite(STEP_PIN_Y, LOW);
  stepper_ra_set_dir(1);
  stepper_dec_set_dir(1);

  SoftSerial.begin(9600);           // initialize software serial for UART motor control
  SoftSerial.listen();

  TMCdriverx.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdriverx.toff(5);                 // Enables driver in software
  TMCdriverx.rms_current(800);        // Set motor RMS current
  TMCdriverx.en_spreadCycle(false);
  TMCdriverx.TPWMTHRS(7);     // Needed for stealthChop

  TMCdrivery.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdrivery.toff(5);                 // Enables driver in software
  TMCdrivery.rms_current(800);        // Set motor RMS current
  TMCdrivery.en_spreadCycle(false);
  TMCdrivery.TPWMTHRS(7);     // Needed for stealthChop
}

int stepper_ra_step()
{
    if(ra_move_until==-1)
    {
        ra_move_until = 0;
        return 1;
    }
    digitalWrite(DIR_PIN_X, HIGH);
    ra_counter+=ra_direction*ra_multiplier;
    int ret = 0;
    if(ra_move_until)
    {
        ra_move_until -= ra_multiplier;
        if(ra_move_until <= 0)
        {
            ra_move_until = 0;
            ret = 1;
        }
    }
    delayMicroseconds(1);
    digitalWrite(DIR_PIN_X, LOW);
    return ret;
}

int stepper_dec_step()
{
    if(dec_move_until==-1)
    {
        ra_move_until = 0;
        return 1;
    }
    digitalWrite(DIR_PIN_X, HIGH);
    dec_counter+=dec_direction*dec_multiplier;
    int ret = 0;
    if(dec_move_until)
    {
        dec_move_until-=dec_multiplier;
        if(dec_move_until<=0)
        {
            dec_move_until = 0;
            ret = 1;
        }
    }
    delayMicroseconds(1);
    digitalWrite(DIR_PIN_X, LOW);
    return ret;
}

long stepper_ra_get_steps()
{
    return ra_counter;
}

long stepper_dec_get_steps()
{
    return dec_counter;
}

void stepper_ra_set_steps(long steps)
{
    ra_counter = steps;
}

void stepper_dec_set_steps(long steps)
{
    dec_counter = steps;
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

void stepper_set_ra_micro_stepping(int divider)
{
  TMCdrivery.microsteps(divider);          // Set microsteps 
  ra_multiplier = MICROSTEPS_RA/divider;
}

void stepper_set_dec_micro_stepping(int divider)
{
  TMCdriverx.microsteps(divider);          // Set microsteps 
  dec_multiplier = MICROSTEPS_DEC/divider;
}

void stepper_ra_set_power(int mode)
{

}

void stepper_dec_set_power(int mode)
{

}


void stepper_ra_move_steps(long steps)
{
    if(steps == 0)
        ra_move_until = -1;
    else
        ra_move_until = steps;
}

void stepper_dec_move_steps(long steps)
{
    if(steps == 0)
        dec_move_until = -1;
    else
        dec_move_until = steps;
}
