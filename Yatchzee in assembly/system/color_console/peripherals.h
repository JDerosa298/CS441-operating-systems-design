#ifndef __PERIPHERALS_H__
#define __PERIPHERALS_H__

#include "miniat/miniat.h"

#define KEYB_REFRESH_PERIOD 10000


extern int keyb_update_counter;


extern void peripherals_cleanup();
extern void peripherals_clock(miniat *m);


#endif /*  __PERIPHERALS_H__ */
