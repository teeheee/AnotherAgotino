#include "coords.h"
#include "stepper.h"

long current_ra_sec=0;
long current_dec_sec=0;

long coord_get_ra_sec()
{
    long remaining_secs = stepper_ra_get_remaining_secs();
    remaining_secs *= stepper_ra_get_dir()==1 ? 1 : -1;
    return current_ra_sec+remaining_secs;
}

long coord_get_dec_sec()
{
    long remaining_secs = stepper_dec_get_remaining_secs();
    remaining_secs *= stepper_dec_get_dir()==1 ? 1 : -1;
    return current_dec_sec+remaining_secs;
}

void coord_set_ra_sec(long secs)
{
    current_ra_sec = secs;
}

void coord_set_dec_sec(long secs)
{
    current_dec_sec = secs;
}
