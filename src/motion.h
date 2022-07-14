#ifndef _MOTION_H_
#define _MOTION_H_

void syncRaDecBySecs(long raSecs, long decSecs);
void slewRaDecBySecs(long raSecs, long decSecs);
long getRaSecs();
long getDecSecs();

void motion_halt();
void motion_track();
void move_ra_ms(long ms);
void move_dec_ms(long ms);

#endif
