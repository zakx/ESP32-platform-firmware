#ifndef _DRIVER_FRAMEBUFFER_COMPOSITOR_H_
#define _DRIVER_FRAMEBUFFER_COMPOSITOR_H_
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "driver_framebuffer.h"
#include "esp_system.h"

union Color {
	uint32_t color;
	uint8_t RGBA[4];
}

struct Cwindow {
	Cwindow* _prevWindow;
	Cwindow* _nextWindow;
	/* Position */
	bool visible;                  // Visible or hidden
	int16_t x, y;                  // Position (x,y)
	int16_t width, height;         // Size
	int16_t moveX, moveY;          // Amount of pixels to move on each step
	uint8_t moveDiv, _currMoveDiv; // Move every X steps
	bool moveLoop;                 // Warp to the other side of the screen when moving ouf of bounds

	/* Data buffer */
	Color** frames;                // Array of frames
	uint16_t numFrames, _currFrame; // Amount of frames

	/* Animation */
	uint8_t aniDiv, _currAniDiv;   // Go to next frame every X steps
	bool aniLoop;                  // Loop animation
};

#endif //_DRIVER_FRAMEBUFFER_H_
