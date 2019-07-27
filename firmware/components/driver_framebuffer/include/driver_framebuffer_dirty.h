#ifndef _DRIVER_FRAMEBUFFER_DIRTY_H_
#define _DRIVER_FRAMEBUFFER_DIRTY_H_

#include <stdint.h>
#include <stdbool.h>

inline bool driver_framebuffer_is_dirty();
/* Returns true if an area of the framebuffer is marked dirty */

void driver_framebuffer_set_dirty_area(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, bool force);
/* Set which area of the framebuffer is dirty. Forcing allows for directly setting the dirty area. Normally function only increments the dirty area. */

void driver_framebuffer_get_dirty_area(int16_t* x0, int16_t* y0, int16_t* x1, int16_t* y1);
/* Get which area of the framebuffer is dirty. */

//Note: functions in this API do not account for orientation. Please apply orientation first before using these functions.

#endif
