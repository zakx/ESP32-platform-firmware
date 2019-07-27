#ifndef _DRIVER_FRAMEBUFFER_ORIENTATION_H_
#define _DRIVER_FRAMEBUFFER_ORIENTATION_H_

#include <stdint.h>
#include <stdbool.h>

enum Orientation {
	landscape,
	portrait,
	reverse-landscape,
	reverse-portrait
}

enum Orientation driver_framebuffer_get_orientation(struct Frame* frame);
/* Get the orientation of the window that contains the provided frame */

void driver_framebuffer_set_orientation(struct Frame* frame, enum Orientation newOrientation);
/* Set the orientation of the window that contains the provided frame */

uint16_t driver_framebuffer_get_orientation_angle(struct Frame* frame);
/* Get the orientation of the window that contains the provided frame as angle */

void driver_framebuffer_set_orientation_angle(struct Frame* frame, uint16_t angle);
/* Set the orientation of the window that contains the provided frame as angle */

inline bool driver_framebuffer_orientation_apply(struct Frame* frame, int16_t* x, int16_t* y);
/* Apply the orientation of the window that contains the provided frame to the provided coordinates. (the provided coordinates are from the users perspective) */

inline void driver_framebuffer_orientation_revert(struct Frame* frame, int16_t* x, int16_t* y);
/* Revert the orientation of the window that contains the provided frame to the provided coordinates. (the provided coordinates are from the internal perspective) */

#endif
