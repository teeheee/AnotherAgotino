# AnotherAgotino

This project is a rework of the [aGotino](https://github.com/mappite/aGotino) telescope mount firmware.
It only implements lx200 command set and supports slew and guide commands.
I use it with arduino nano every in combination with a TMC modded cnc shield v4. But the firmware can be easily adapted to other arduino stepper driver combination.
I also switched the project from arduino to platformio


Many thanks to mappite for the aGotino Project

# Supported Commands

* :GR# -> 00:00:00#
* :GD# -> +90*00:00#
* :Sr 22:48:07# -> 1
* :Sd +90:00:00# -> 1
* :Mn# -> //Start Moving
* :Ms# -> //Start Moving
* :Mw# -> //Start Moving
* :Me# -> //Start Moving
* :Mgnxxxx#
* :Mgsxxxx#
* :Mgwxxxx#
* :Mgexxxx#
* :Qn# -> //Stop Moving
* :Qs# -> //Stop Moving
* :Qw# -> //Stop Moving
* :Qe# -> //Stop Moving
* :Q# -> //Stop Moving
* :MS# -> 0 //Slew to target
* :GC# -> MM/DD/YY# //Get current Date
* :SCMM/DD/YY# 
* :GM# -> Ulm# //Get Site 1 Name
* :SMstring# 
* :GT# -> TT.T# //Get Tracking Rate 60Hz=Sideral Rate
* :Gt# -> sDD*MM# //Get Current Site Latitdue
* :GG# -> sHH# //Get UTC offset time
* :GL# -> HH:MM:SS# //Get Local Time in 24 hour format
* :Gc# -> 24# //time format 

# Command TODO ?
* :SG -02#:
* :Sg-10:00#
* :D#

