#ifndef _MOTION_H_
#define _MOTION_H_

void syncRaDecBySecs(long raSecs, long decSecs);
void slewRaDecBySecs(long raSecs, long decSecs);

void motion_halt();
void motion_track();
void move_ra_ms(long ms);
void move_dec_ms(long ms);
void start_move_ra(int dir);
void start_move_dec(int dir);

#endif
