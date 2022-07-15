#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <limits.h>

#define EN_PIN             8 // Enable
#define DIR_PIN_X          2 // Direction
#define STEP_PIN_X         5 // Step 
#define DIR_PIN_Y          3 // Direction
#define STEP_PIN_Y         6 // Step 

#define DRIVER_ADDRESS_X   0b00   // TMC2209 Driver address according to MS1 and MS2
#define DRIVER_ADDRESS_Y   0b01   // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f           // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)

#define MICROSTEPS_RA  32 // RA  Driver Microsteps
#define MICROSTEPS_DEC 32 // DEC Driver Microsteps

#define MICROSTEPS_RA_FAST  1 // RA  Driver Microsteps
#define MICROSTEPS_DEC_FAST 1 // DEC Driver Microsteps

#define SERIAL_SPEED    9600          // serial interface baud. Make sure your computer/phone matches this

#define MAX_RANGE       1800                   // default max slew range in deg minutes (1800'=30°). See +range command

#define  WORM_RATIO_RA     140.0
#define  WORM_RATIO_DEC    90.0
#define  GEAR_HIGH         48.0
#define  GEAR_LOW          20.0
#define  STEPS_PER_REV     200.0

#define MS_PER_DAY_SIDERAL 86164100.0
#define DAY_SECONDS             (24.0*60.0*60.0)
#define MS_PER_DAY              (DAY_SECONDS*1000.0)
#define SECONDS_PER_360_DEG     (360.0*60.0*60.0)
#define MS_PER_360_DEG          (SECONDS_PER_360_DEG*1000.0)

#define MICROSTEPS_RA_DIV       (MICROSTEPS_RA/MICROSTEPS_RA_FAST)
#define MICROSTEPS_DEC_DIV      (MICROSTEPS_DEC/MICROSTEPS_DEC_FAST)

#define MICROSTEPS_PER_DEGREE_RA ((WORM_RATIO_RA*GEAR_HIGH*STEPS_PER_REV*MICROSTEPS_RA)/(GEAR_LOW*360.0))
#define MICROSTEPS_PER_DEGREE_DEC ((WORM_RATIO_RA*GEAR_HIGH*STEPS_PER_REV*MICROSTEPS_RA)/(GEAR_LOW*360.0))


#define ROUND(x) ((x)+0.5)

#define FREQ_RA_1_HZ ((1000.0*MICROSTEPS_PER_DEGREE_RA*360.0)/(MS_PER_DAY_SIDERAL))
#define FREQ_DEC_1_HZ ((1000.0*MICROSTEPS_PER_DEGREE_DEC*360.0)/(MS_PER_DAY_SIDERAL))

#define GUIDE_SPEED_FREQ 2
#define SLEW_SPEED_FREQ  10

#define NORTH_DEC_SEC     324000L // 90°


// Number of Microsteps to move RA by 1hour
#define MICROSTEPS_PER_HOUR_RA    ROUND(MICROSTEPS_PER_DEGREE_RA * 360UL / 24UL)
#define MICROSTEPS_TO_SEC_RA(x)   ROUND((x*DAY_SECONDS) / (MICROSTEPS_PER_DEGREE_RA*360UL))
#define MICROSTEPS_TO_SEC_DEC(x)    ROUND((x*SECONDS_PER_360_DEG) / (MICROSTEPS_PER_DEGREE_DEC*360UL))
#define SEC_TO_MICROSTEPS_RA(x)   ROUND((x*MICROSTEPS_PER_DEGREE_RA*360UL) / DAY_SECONDS) 
#define SEC_TO_MICROSTEPS_DEC(x)     ROUND((x*MICROSTEPS_PER_DEGREE_DEC*360UL) / SECONDS_PER_360_DEG) 
#define MS_TO_MICROSTEPS_RA(x)   ROUND((x*MICROSTEPS_PER_DEGREE_RA*360UL) / MS_PER_DAY) 
#define MS_TO_MICROSTEPS_DEC(x)     ROUND((x*MICROSTEPS_PER_DEGREE_DEC*360UL) / MS_PER_360_DEG) 

//#define DEBUG

#ifdef DEBUG 
#define printLog(s)      {Serial.print(s);} 
#define printLogL(l)      {Serial.println(l);}   
#define printLogUL(ul)   {Serial.println(ul);}
#else
#define printLog(s)      
#define printLogL(l)       
#define printLogUL(ul)   
#endif


#endif