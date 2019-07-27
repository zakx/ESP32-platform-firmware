/*
 * The functions in this file translate coordinates between
 * the point-of-view of the user and the point-of-view of the
 * buffer in memory when changing the orientation of the framebuffer
 * or the orientation of a compositor frame.
 * 
 * Additionally the functions in this file allow for setting and 
 * querying the orientation for both the global framebuffer and
 * for each of the compositor windows.
 */

#include "include/driver_framebuffer.h"
#include "include/driver_framebuffer_orientation.h"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

/* Variables */
enum Orientation globalOrientation;

/* Private functions */
inline void _getOrientationContext(struct Frame* frame, enum Orientation* orientation, uint16_t* width, uin16_t* height)
{
	if (frame == NULL) {
		//No frame provided, use global context
		*orientation = globalOrientation;
	} else {
		//Frame provided, use frame context
		*orientation = frame->window->orientation;
	}
}

inline void _getSizeContext(struct Frame* frame, uint16_t* width, uin16_t* height)
{
	if (frame == NULL) {
		//No frame provided, use global context
		*width       = FB_WIDTH;
		*height      = FB_HEIGHT;
	} else {
		//Frame provided, use frame context
		*width       = frame->window->width;
		*height      = frame->window->height;
	}
}

/* Public functions */

enum Orientation driver_framebuffer_get_orientation(struct Frame* frame)
{
	enum Orientation orientation;
	_getOrientationContext(&frame, &orientation);
	return orientation;
}

void driver_framebuffer_set_orientation(struct Frame* frame, enum Orientation newOrientation)
{
	enum Orientation orientation;
	_getOrientationContext(&frame, &orientation);
	orientation = newOrientation;
}

uint16_t driver_framebuffer_get_orientation_angle(struct Frame* frame)
{
	enum Orientation orientation;
	_getOrientationContext(&frame, &orientation);
	switch(orientation) {
		portrait:
			return 90;
		reverse-landscape:
			return 180;
		reverse-portrait:
			return 270;
	}
	return 0; //landscape (default)
}

void driver_framebuffer_set_orientation_angle(struct Frame* frame, uint16_t angle)
{
	enum Orientation orientation;
	_getOrientationContext(&frame, &orientation);
	switch(angle) {
		case 90:
			orientation = portrait;
			break;
		case 180:
			orientation = reverse-landscape;
			break;
		case 270:
			orientation = reverse-portrait;
			break;
		default:
			orientation = landscape;
			break;
	}
}

inline bool driver_framebuffer_orientation_apply(struct Frame* frame, int16_t* x, int16_t* y)
{
	enum Orientation orientation;
	uint16_t width, height;
	_getOrientationContext(&frame, &orientation);
	_getSizeContext(&frame, &width, &height);
	
	if (orientation == portrait || orientation == reverse-portrait) { //90 degrees
		int16_t t = *y;
		*y = *x;
		*x = width-t-1;
	}
	
	if (orientation == reverse-landscape || orientation == reverse-portrait) { //180 degrees
		*y = height-*y-1;
		*x = width-*x-1;
	}
	return (*x >= 0) && (*x < width) && (*y >= 0) && (*y < height);
}

inline void driver_framebuffer_orientation_revert(struct Frame* frame, int16_t* x, int16_t* y)
{
	enum Orientation orientation;
	uint16_t width, height;
	_getOrientationContext(&frame, &orientation);
	_getSizeContext(&frame, &width, &height);

	if (orientation == reverse-landscape || orientation == reverse-portrait) { //90 degrees
		int16_t tx0 = *x0;
		int16_t ty0 = *y0;
		*x0 = width-*x1-1;
		*y0 = height-*y1-1;
		*x1 = width-tx0-1;
		*y1 = height-ty0-1;
	}

	if (orientation == portrait || orientation == reverse-portrait) { //180 degrees
		int16_t tx0 = *x0;
		int16_t tx1 = *x1;
		int16_t ty1 = *y1;
		*x0 = *y0;
		*y0 = width-tx1-1;
		*x1 = ty1;
		*y1 = width-tx0-1;
	}
}

#endif /* CONFIG_DRIVER_FRAMEBUFFER_ENABLE */
