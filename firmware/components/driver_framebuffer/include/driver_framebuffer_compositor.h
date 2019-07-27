#ifndef _DRIVER_FRAMEBUFFER_COMPOSITOR_H_
#define _DRIVER_FRAMEBUFFER_COMPOSITOR_H_
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "driver_framebuffer_devices.h"
#include "driver_framebuffer.h"
#include "esp_system.h"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

struct Frame {
	/* Linked list */
	struct Frame* _prevFrame;
	struct Frame* _nextFrame;
	
	/* Parent */
	struct Window* window;
	
	/* This frame */
	uint8_t* buffer;
};

struct Window {
	/* Linked list */
	struct Window* _prevWindow;
	struct Window* _nextWindow;
	
	/* Identifier */
	uint16_t id;
	
	/* Placement */
	int16_t x, y;                   // Position (x,y)
	uint16_t width, height;         // Size
	bool visible;                   // Visible or hidden
	
	/* Transparency */
	uint8_t tpEnabled;              // Enable transparency (0 = completely visible, 255 = completely transparent)
	uint8_t tpValue;                // Which value (on or off) is transparent
	
	/* Movement */
	int16_t moveX, moveY;           // Amount of pixels to move on each step
	uint8_t moveDiv, _currMoveDiv;  // Move every X steps
	bool moveLoop;                  // Warp to the other side of the screen when moving ouf of bounds

	/* Frames */
	struct Frame* firstFrame;       //Starting point of linked list of frames
	
	/* Animation */
	uint8_t aniDiv, _currAniDiv;    // Go to next frame every X steps
	bool aniLoop;                   // Loop animation
	
	/* Text */
	const GFXfont *font;            // Text drawing font
	uint8_t textScaleX;             // Horizontal text scale
	uint8_t textScaleY;             // Vertical text scale
	int16_t textCursorX;            // Current horizontal position
	int16_t textCursorX0;           // Starting position after newline
	int16_t textCursorY;            // Current vertical position
	bool textWrap;                  // Wrap text when reaching border of buffer
	uint32_t textColor;             // Text drawing color
};

#endif
#endif //_DRIVER_FRAMEBUFFER_H_
