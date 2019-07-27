#ifndef _DRIVER_FRAMEBUFFER_FRAME_H_
#define _DRIVER_FRAMEBUFFER_FRAME_H_

#include <stdint.h>
#include "driver_framebuffer.h"
#include "driver_framebuffer_devices.h"
#include "driver_framebuffer_window.h"

struct Frame {
	/* Linked list */
	struct Frame* _prevFrame;
	struct Frame* _nextFrame;
	
	/* Parent */
	struct Window* window;
	
	/* This frame */
	uint8_t* buffer;
	uint8_t* alpha;
};

#endif /* _DRIVER_FRAMEBUFFER_FRAME_H_ */
