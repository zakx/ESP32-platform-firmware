/*
 * The functions in this file serve as a simple way of
 * storing which area of the framebuffer needs to be
 * sent to the display during the next flush
 * 
 * (This only applies to the main framebuffer and not to the compositor frames!)
 */

#include "include/driver_framebuffer_dirty.h"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

/* Variables */
int16_t dirty_x0 = 0,          dirty_y0 = 0;           // Top-left corner of the "dirty" area
int16_t dirty_x1 = FB_WIDTH-1, dirty_y1 = FB_HEIGHT-1; // Bottom-right corner of the "dirty" area

/* Public functions */
inline bool driver_framebuffer_is_dirty()
{
	return (dirty_x1 > dirty_x0) || (dirty_y1 > dirty_y0);
}

void driver_framebuffer_set_dirty_area(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, bool force)
{
	if (force) {
		//Just set the dirty area
		dirty_x0 = x0;
		dirty_y0 = y0;
		dirty_x1 = x1;
		dirty_y1 = y1;
	} else {
		//Only increase the dirty area
		if (x0 < dirty_x0) dirty_x0 = x;
		if (y0 < dirty_y0) dirty_y0 = y;
		if (x1 > dirty_x1) dirty_x1 = x;
		if (y1 > dirty_y1) dirty_y1 = y;
	}
}

void driver_framebuffer_get_dirty_area(int16_t* x0, int16_t* y0, int16_t* x1, int16_t* y1)
{
	*x0 = dirty_x0;
	*y0 = dirty_y0;
	*x1 = dirty_x1;
	*y1 = dirty_y1;
}
	/*if (flip180) {
		int16_t tx0 = *x0;
		int16_t ty0 = *y0;
		*x0 = FB_WIDTH-*x1-1;
		*y0 = FB_HEIGHT-*y1-1;
		*x1 = FB_WIDTH-tx0-1;
		*y1 = FB_HEIGHT-ty0-1;
	}

	if (orientation) {
		int16_t tx0 = *x0;
		int16_t tx1 = *x1;
		int16_t ty1 = *y1;
		*x0 = *y0;
		*y0 = FB_WIDTH-tx1-1;
		*x1 = ty1;
		*y1 = FB_WIDTH-tx0-1;
	}*/

#endif /* CONFIG_DRIVER_FRAMEBUFFER_ENABLE */
