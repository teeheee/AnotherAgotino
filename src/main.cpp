/*
   aGotino a simple Goto with Arduino (Nano/Uno)

      * tracking 
      * cycle among forward and backward speeds on RA&Dec at buttons press
      * listen on serial port for basic LX200 commands (INDI LX200 Basic, SkySafari, Stellarium etc)
      * listen on serial port for aGotino commands
      * ST4 port support

    Command set and latest updates at https://github.com/mappite/aGotino
    
    by gspeed @ astronomia.com / qde / cloudynights.com forum
    This code is free software under GPL v3 License use at your risk and fun ;)

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

   How to calculate STEP_DELAY to drive motor at right sidereal speed for your mount

   Worm Ratio                  90   // 144 eq5/exos2, 135 heq5, 130 eq3-2
   Other (Pulley/Gear) Ratio   2.4 // depends on your pulley setup 48T/20T
   Steps per revolution        400   // or usually 200 depends on your motor
   Microstep                   128   // depends on driver

   MICROSTEPS_PER_DEGREE_RA  30720   // = WormRatio*OtherRatio*StepsPerRevolution*Microsteps/360
                                     // = number of microsteps to rotate the scope by 1 degree

   STEP_DELAY                7791   // = (86164/360)/(MicroSteps per Degree)*1000000
                                     // = microseconds to advance a microstep at 1x
                                     // 86164 is the number of secs for earth 360deg rotation (23h56m04s)

 * Update the values below to match your mount/gear ratios and your preferences: 
 * * * * * * */



#include <Arduino.h>
#include "tick_timer.h"
#include "stepper.h"
#include "config.h"
#include "motion.h"
#include "coords.h"

/*
 * It is safe to keep the below untouched
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



// Current coords in Secs (default to true north)
long currRA  = 0;     
long currDEC = NORTH_DEC_SEC;

// Serial Input (New) coords
long inRA    = 0;
long inDEC   = 0;

// Current position in Meade lx200 format, see updateLx200Coords()
String lx200RA  = "00:00:00#";
String lx200DEC = "+90*00:00#";

String _aGotino = "AGT";

String currentDate = "01/01/2022";
String currentTime = "01:00:00";
String currentLocation = "Ulm";
String currentUTCOffset = "-02";

const unsigned long _ver = 050722;

// Serial Input
char input[20];     // stores serial input
int  in = 0;        // current char in serial input

#define PRINT_VAR(XX)  {Serial.print(#XX": ");Serial.println(XX);}
void main_status()
{
    PRINT_VAR(currRA);
    PRINT_VAR(currDEC);
    PRINT_VAR(inRA);
    PRINT_VAR(inDEC);
}

/* Update lx200 RA&DEC string coords so polling 
 * (:GR# and :GD#) gets processed faster
 */
void updateLx200Coords() {
  long raSecs = coord_get_ra_sec();
  long decSecs =  coord_get_dec_sec();
  unsigned long pp = raSecs/3600;
  unsigned long mi = (raSecs/60)%60;
  unsigned long ss = raSecs%60;
  lx200RA = "";
  if (pp<10) lx200RA.concat("0");
  lx200RA.concat(pp);lx200RA.concat(":");
  if (mi<10) lx200RA.concat("0");
  lx200RA.concat(mi);lx200RA.concat(":");
  if (ss<10) lx200RA.concat("0");
  lx200RA.concat(ss);lx200RA.concat("#");
  pp = labs(decSecs)/3600; //TODO this division looses data
  mi = (labs(decSecs)-pp*3600)/60;
  ss = (labs(decSecs)-mi*60-pp*3600);
  lx200DEC = "";
  lx200DEC.concat(decSecs>0?"+":"-");
  if (pp<10) lx200DEC.concat("0");
  lx200DEC.concat(pp);lx200DEC.concat("*"); // FIXME: may be just * nowadays
  if (mi<10) lx200DEC.concat("0"); 
  lx200DEC.concat(mi);lx200DEC.concat(":");
  if (ss<10) lx200DEC.concat("0");
  lx200DEC.concat(ss);lx200DEC.concat("#");
} 



/* 
 *  Basic Meade LX200 protocol
 */
void lx200(String s) { // all :.*# commands are passed here 
  if (s.charAt(2) == 'D')
  {
    if(timer_slew_is_active())
    {
      Serial.print("\x7f#");
    }
    else
    {
      Serial.print("#");
    }
  }
  if (s.substring(1,3).equals("GR")) { // :GR# 
    printLog("GR");
    // send current RA to computer
    updateLx200Coords(); // recompute strings
    Serial.print(lx200RA);
  } else if (s.substring(1,3).equals("GD")) { // :GD# 
    printLog("GD");
    // send current DEC to computer
    updateLx200Coords(); // recompute strings
    Serial.print(lx200DEC);
  } else if (s.substring(1,3).equals("GV")) { // :GV*# Get Version *
    char c = s.charAt(3); 
    if ( c == "P") {// GVP - Product name
       Serial.print(_aGotino);  
    } else if (c == "N") { // GVN - firmware version
       Serial.print(_ver);  
    }
    Serial.print("#");
  } else if (s.substring(1,3).equals("Gc")) {  
    Serial.print("24#");
  } else if (s.substring(1,3).equals("GC")) { 
    Serial.print(currentDate);  
    Serial.print("#");
  } else if (s.substring(1,3).equals("GM")) { 
    Serial.print(currentLocation);  
    Serial.print("#");
  } else if (s.substring(1,3).equals("GT")) {  
    Serial.print("60.0#");
  } else if (s.substring(1,3).equals("Gt")) {  
    Serial.print("+48*24#");
  } else if (s.substring(1,3).equals("Gg")) {  
    Serial.print("-009*58#");
  } else if (s.substring(1,3).equals("GG")) {  
    Serial.print(currentUTCOffset);
    Serial.print("#");
  } else if (s.substring(1,3).equals("GL")) {  
    Serial.print(currentTime);  
    Serial.print("#");
  } else if (s.substring(1,3).equals("SL")) { 
    int end = s.indexOf("#");
    currentTime = s.substring(3,end);
    Serial.print(1);
  } else if (s.substring(1,3).equals("SC")) { 
    int end = s.indexOf("#");
    currentDate = s.substring(3,end);
    Serial.print(1);
  } else if (s.substring(1,3).equals("SG")) { 
    int end = s.indexOf("#");
    currentUTCOffset = s.substring(3,end);
    Serial.print(1);
  } else if (s.substring(1,3).equals("SM")) { 
    int end = s.indexOf("#");
    currentLocation = s.substring(3,end);
    Serial.print(1);
  } else if (s.substring(1,3).equals("Sr")) { // :SrHH:MM:SS# or :SrHH:MM.T# // no blanks after :Sr as per Meade specs
    printLog("Sr");
    // this is INITAL step for setting position (RA)
    long hh = s.substring(3,5).toInt();
    long mi = s.substring(6,8).toInt();
    long ss = 0;
    if (s.charAt(8) == '.') { // :SrHH:MM.T#
      ss = (s.substring(9,10).toInt())*60/10;
    } else {
      ss = s.substring(9,11).toInt();
    }
    inRA = hh*3600+mi*60+ss;
    Serial.print(1); // FIXME: input is not validated
  } else if (s.substring(1,3).equals("Sd")) { // :SdsDD*MM:SS# or :SdsDD*MM#
    printLog("Sd");
    // this is the FINAL step of setting a pos (DEC) 
    long dd = s.substring(4,6).toInt();
    long mi = s.substring(7,9).toInt();
    long ss = 0;
    if (s.charAt(9) == ':') { ss = s.substring(10,12).toInt(); }
    inDEC = (dd*3600+mi*60+ss)*(s.charAt(3)=="-"?-1:1);
    Serial.print(1); // FIXME: input is not validated
  } else if (s.charAt(1) == 'M') { // MOVE:  :MS# (slew), :Mx# (slow move)
    if (s.charAt(2) == 'S' ) { // SLEW
      printLog("MS");
      // assumes Sr and Sd have been processed hence
      // inRA and inDEC have been set, now it"s time to move
      long deltaRaSecs  = currRA-inRA;
      if(deltaRaSecs > DAY_SECONDS/2)
      {
        deltaRaSecs = -(DAY_SECONDS - labs(deltaRaSecs));
      }
      else if(deltaRaSecs < -DAY_SECONDS/2)
      {
        deltaRaSecs = (DAY_SECONDS - labs(deltaRaSecs));
      }
      long deltaDecSecs = currDEC-inDEC;
      slewRaDecBySecs(-deltaRaSecs, -deltaDecSecs);
      syncRaDecBySecs(inRA, inDEC);
      currRA = inRA;
      currDEC = inDEC;
      Serial.print(0); // slew is possible 
    } else if (s.charAt(2) == 'g' ) {
      printLog("Mg");
      int ms = 500;
      ms = s.substring(4,8).toInt();
      switch (s.charAt(3)) {
          case 'n':
            move_ra_ms(-ms);
            break;
          case 's':
            move_ra_ms(ms);
            break;
          case 'w':
            move_dec_ms(ms);
            break;
          case 'e':
            move_dec_ms(-ms);
            break;
        } // default is ignored;
    }else  {
      switch (s.charAt(2)) {
          case 'n':
            start_move_ra(-1);
            break;
          case 's':
            start_move_ra(1);
            break;
          case 'w':
            start_move_dec(1);
            break;
          case 'e':
            start_move_dec(-1);
            break;
        } // default is ignored;
    }
  } else if (s.charAt(1) == 'Q') { // :Q# or :Qx# stop Dec Motor and set RA to Tracking
    motion_track();
  } else if (s.substring(1,3).equals("CM")) { // :CM# sync
    // assumes Sr and Sd have been processed
    // sync current position with input
    printLog("CM");
    currRA  = inRA;
    currDEC = inDEC;
    Serial.print("Synced#");
    syncRaDecBySecs(currRA, currDEC);
  }else if (s.charAt(1) == 'Z') { // :Q# or :Qx# stop Dec Motor and set RA to Tracking
    stepper_status();
    timer_status();
    coord_status();
    main_status();
    Serial.println("###");
  }
}


void setup() {  
  Serial.begin(9600);               // initialize hardware serial for debugging

  delay(1000);

  stepper_init();
  timer_init();
  stepper_set_dec_micro_stepping(MICROSTEPS_DEC);
  stepper_set_ra_micro_stepping(MICROSTEPS_RA);

  syncRaDecBySecs(currRA, currDEC);
  
  
  Serial.print(_aGotino);
  // init Arduino->Driver Motors pins as Outputs
  // set Dec asleep:
  timer_set_interval_ra(FREQ_RA_1_HZ);
  /* set interrupt for RA motor and start tracking */
  lx200DEC[3] = char(223); // set correct char in string as per earlier specs - FIXME: this should not be needed anymore
  Serial.println(" ready.");
}

/*
 * main loop
 */
 
void loop() {
  process_stepper();
  // Check if message on serial input
  if (Serial.available() > 0) {
    input[in] = Serial.read(); 

    // discard blanks. Meade LX200 specs states :Sd and :Sr are
    // not followed by a blank but some implementation does include it.
    // also this allows aGoto commands to be typed with blanks
    if (input[in] == " ") return; 
    
    // acknowledge LX200 ACK signal (char(6)) for software that tries to autodetect protocol (i.e. Stellarium Plus)
    if (input[in] == char(6)) { Serial.print("P"); return; } // P = Polar

    if (input[in] == "#" || input[in] == "\n") { // after a # or a \n it is time to check what is in the buffer
      if (input[0] == ":") { // it"s lx200 protocol
        printLog(input);
        lx200(input);
      } else {
        // unknown command, print message only
        // if buffer contains more than one char
        // since stellarium seems to send extra #"s
        if (in > 0) {
          String s = input;
          Serial.print(s.substring(0,in));
          Serial.println(" unknown. Expected lx200 or aGotino commands");
        }
      }
      in = 0; // reset buffer // WARNING: the whole input buffer is passed anyway
    } else {
      if (in++>20) in = 0; // prepare for next char or reset buffer if max lenght reached
    } 
  }
}
