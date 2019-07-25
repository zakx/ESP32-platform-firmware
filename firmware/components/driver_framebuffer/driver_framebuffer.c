/*
 * BADGE.TEAM framebuffer driver
 * Uses parts of the Adafruit GFX Arduino libray
 */

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

#include "include/driver_framebuffer.h"
#include "include/driver_framebuffer_devices.h"

//Displays
#include "driver_ssd1306.h"
#include "driver_erc12864.h"
#include "driver_eink.h"
#include "driver_ili9341.h"

#include "compositor.h"

#include "esp_heap_caps.h"
#include "esp_log.h"

#define TAG "fb"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

bool isDirty = true;
int16_t dirty_x0 = 0;
int16_t dirty_y0 = 0;
int16_t dirty_x1 = 0;
int16_t dirty_y1 = 0;

#define ORIENTATION_LANDSCAPE 0
#define ORIENTATION_PORTRAIT  1

bool orientation = ORIENTATION_LANDSCAPE;
bool flip180     = false;
bool useGreyscale = false;

#ifdef CONFIG_DRIVER_FRAMEBUFFER_DOUBLE_BUFFERED
uint8_t* framebuffer1;
uint8_t* framebuffer2;
#endif

uint8_t* framebuffer;

/* Color space conversions */

inline uint16_t rgbTo565(uint32_t in)
{
	uint8_t r = (in>>16)&0xFF;
	uint8_t g = (in>>8)&0xFF;
	uint8_t b = in&0xFF;
	uint16_t out = ((b & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (r >> 3);
	return out;
}

inline uint8_t rgbToGrey(uint32_t in)
{
	uint8_t r = (in>>16)&0xFF;
	uint8_t g = (in>>8)&0xFF;
	uint8_t b = in&0xFF;
	return ( r + g + b + 1 ) / 3;
}

inline bool greyToBw(uint8_t in)
{
	return in >= 128;
}

bool driver_framebuffer_is_dirty()
{
	return isDirty;
}

void driver_framebuffer_set_dirty(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	isDirty = (x1 > x0) || (y1 > y0);
	dirty_x0 = x0;
	dirty_y0 = y0;
	dirty_x1 = x1;
	dirty_y1 = y1;
}

uint16_t driver_framebuffer_get_orientation()
{
	return orientation*90 + flip180*180;
}

void driver_framebuffer_set_orientation(uint16_t angle)
{
	switch(angle) {
		case 90:
			orientation = ORIENTATION_PORTRAIT;
			flip180     = false;
			break;
		case 180:
			orientation = ORIENTATION_LANDSCAPE;
			flip180     = true;
			break;
		case 270:
			orientation = ORIENTATION_PORTRAIT;
			flip180     = true;
			break;
		default:
			orientation = ORIENTATION_LANDSCAPE;
			flip180     = false;
			break;
	}
}

esp_err_t driver_framebuffer_init()
{
	static bool driver_framebuffer_init_done = false;
	if (driver_framebuffer_init_done) return ESP_OK;
	ESP_LOGD(TAG, "init called");
	
	#ifdef CONFIG_DRIVER_FRAMEBUFFER_DOUBLE_BUFFERED
		ESP_LOGI(TAG, "Allocating %u bytes for framebuffer 1", FB_SIZE);
		#ifdef CONFIG_DRIVER_FRAMEBUFFER_SPIRAM
			framebuffer1 = heap_caps_malloc(FB_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		#else
			//framebuffer1 = malloc(FB_SIZE);
			framebuffer1 = heap_caps_malloc(FB_SIZE, MALLOC_CAP_8BIT);
		#endif
		if (!framebuffer1) return ESP_FAIL;
		ESP_LOGI(TAG, "Allocating %u bytes for framebuffer 2", FB_SIZE);
		#ifdef CONFIG_DRIVER_FRAMEBUFFER_SPIRAM
			framebuffer2 = heap_caps_malloc(FB_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		#else
			//framebuffer2 = malloc(FB_SIZE);
			framebuffer2 = heap_caps_malloc(FB_SIZE, MALLOC_CAP_8BIT);
		#endif
		if (!framebuffer2) return ESP_FAIL;
		framebuffer = framebuffer1;
	#else
		ESP_LOGI(TAG, "Allocating %u bytes for the framebuffer", FB_SIZE);
		#ifdef CONFIG_DRIVER_FRAMEBUFFER_SPIRAM
		framebuffer = heap_caps_malloc(FB_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		#else
		//framebuffer = malloc(FB_SIZE);
		framebuffer = heap_caps_malloc(FB_SIZE, MALLOC_CAP_8BIT);
		#endif
		
		if (!framebuffer) {
			ESP_LOGE(TAG, "Unable to allocate memory for the framebuffer.");
			return ESP_FAIL;
		}
	#endif
	
	driver_framebuffer_setFont(&freesans9pt7b);
		
	driver_framebuffer_fill(COLOR_FILL_DEFAULT); //1st framebuffer
	driver_framebuffer_setTextColor(COLOR_TEXT_DEFAULT);
	driver_framebuffer_flush(FB_FLAG_FORCE | FB_FLAG_FULL);
	driver_framebuffer_fill(COLOR_FILL_DEFAULT); //2nd framebuffer
	
	#ifdef CONFIG_DRIVER_HUB75_ENABLE
	compositor_enable();
	#endif
	
	driver_framebuffer_init_done = true;
	ESP_LOGD(TAG, "init done");
	return ESP_OK;
}

#if defined(FB_TYPE_1BPP)
void driver_framebuffer_fill(uint32_t value)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "fill without alloc!");
		return;
	}
	value = greyToBw(rgbToGrey(value));
	isDirty = true;
	dirty_x0 = 0;
	dirty_y0 = 0;
	dirty_x1 = FB_WIDTH;
	dirty_y1 = FB_HEIGHT;
	memset(framebuffer, value ? 0xFF : 0x00, FB_SIZE);
}

void driver_framebuffer_setPixel(int16_t x, int16_t y, uint32_t value)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "setPixel without alloc!");
		return;
	}
	value = greyToBw(rgbToGrey(value));
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return;
	if (x < 0) return;
	if (y >= FB_HEIGHT) return;
	if (y < 0) return;
	
	isDirty = true;
	if (x < dirty_x0) dirty_x0 = x;
	if (y < dirty_y0) dirty_y0 = y;
	if (x > dirty_x1) dirty_x1 = x;
	if (y > dirty_y1) dirty_y1 = y;

	#ifndef FB_1BPP_VERT
		uint32_t position = (y * FB_WIDTH) + (x / 8);
		uint8_t  bit      = x % 8;
	#else
		uint32_t position = ( (y / 8) * FB_WIDTH) + x;
		uint8_t  bit      = y % 8;
	#endif
	framebuffer[position] ^= (-value ^ framebuffer[position]) & (1UL << bit);
}

uint32_t driver_framebuffer_getPixel(int16_t x, int16_t y)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "getPixel without alloc!");
		return 0;
	}
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return 0;
	if (x < 0) return 0;
	if (y >= FB_HEIGHT) return 0;
	if (y < 0) return 0;

	#ifndef FB_1BPP_VERT
		uint32_t position = (y * FB_WIDTH) + (x / 8);
		uint8_t  bit      = x % 8;
	#else
		uint32_t position = ( (y / 8) * FB_WIDTH) + x;
		uint8_t  bit      = y % 8;
	#endif
	if ((framebuffer[position] >> bit) && 0x01) {
		return 0xFFFFFF;
	} else {
		return 0x000000;
	}
}
#elif defined(FB_TYPE_8BPP)
void driver_framebuffer_fill(uint32_t value)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "fill without alloc!");
		return;
	}
	value = rgbToGrey(value);
	isDirty = true;
	dirty_x0 = 0;
	dirty_y0 = 0;
	dirty_x1 = FB_WIDTH;
	dirty_y1 = FB_HEIGHT;
	memset(framebuffer, value, FB_SIZE);
}

void driver_framebuffer_setPixel(int16_t x, int16_t y, uint32_t value)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "setPixel without alloc!");
		return;
	}
	value = rgbToGrey(value);
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return;
	if (x < 0) return;
	if (y >= FB_HEIGHT) return;
	if (y < 0) return;

	isDirty = true;
	if (x < dirty_x0) dirty_x0 = x;
	if (y < dirty_y0) dirty_y0 = y;
	if (x > dirty_x1) dirty_x1 = x;
	if (y > dirty_y1) dirty_y1 = y;

	uint32_t position = (y * FB_WIDTH) + x;
	framebuffer[position] = value;
}

uint32_t driver_framebuffer_getPixel(int16_t x, int16_t y)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "getPixel without alloc!");
		return 0;
	}
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return 0;
	if (x < 0) return 0;
	if (y >= FB_HEIGHT) return 0;
	if (y < 0) return 0;

	uint32_t position = (y * FB_WIDTH) + x;
	return (framebuffer[position] << 16) + (framebuffer[position]<<8) + framebuffer[position];
}
#elif defined(FB_TYPE_16BPP)
void driver_framebuffer_fill(uint32_t color)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "fill without alloc!");
		return;
	}
	color = rgbTo565(color);
	isDirty = true;
	dirty_x0 = 0;
	dirty_y0 = 0;
	dirty_x1 = FB_WIDTH;
	dirty_y1 = FB_HEIGHT;
	uint8_t c0 = (color>>8)&0xFF;
	uint8_t c1 = color&0xFF;
	
	for (uint32_t i = 0; i < FB_SIZE; i+=2) {
		framebuffer[i + 0] = c0;
		framebuffer[i + 1] = c1;
	}
}

void driver_framebuffer_setPixel(int16_t x, int16_t y, uint32_t color)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "setPixel without alloc!");
		return;
	}
	color = rgbTo565(color);
	uint8_t c0 = (color>>8)&0xFF;
	uint8_t c1 = color&0xFF;

	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return;
	if (x < 0) return;
	if (y >= FB_HEIGHT) return;
	if (y < 0) return;

	isDirty = true;
	if (x < dirty_x0) dirty_x0 = x;
	if (y < dirty_y0) dirty_y0 = y;
	if (x > dirty_x1) dirty_x1 = x;
	if (y > dirty_y1) dirty_y1 = y;

	uint32_t position = (y * FB_WIDTH * 2) + (x * 2);
	framebuffer[position + 0] = c0;
	framebuffer[position + 1] = c1;
}
uint32_t driver_framebuffer_getPixel(int16_t x, int16_t y)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "getPixel without alloc!");
		return 0;
	}
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return 0;
	if (x < 0) return 0;
	if (y >= FB_HEIGHT) return 0;
	if (y < 0) return 0;

	uint32_t position = (y * FB_WIDTH * 2) + (x * 2);
	uint32_t color = (framebuffer[position] << 8) + (framebuffer[position + 1]);
	uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;

	uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;

	uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
	return r << 16 | g << 8 | b;;
}
#elif defined(FB_TYPE_24BPP)
void driver_framebuffer_fill(uint32_t color)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "fill without alloc!");
		return;
	}
	isDirty = true;
	dirty_x0 = 0;
	dirty_y0 = 0;
	dirty_x1 = FB_WIDTH;
	dirty_y1 = FB_HEIGHT;
	uint8_t r = (color>>16)&0xFF;
	uint8_t g = (color>>8)&0xFF;
	uint8_t b = color&0xFF;

	for (uint32_t i = 0; i < FB_SIZE; i+=3) {
		framebuffer[i + 0] = r;
		framebuffer[i + 1] = g;
		framebuffer[i + 2] = b;
	}
}

void driver_framebuffer_setPixel(int16_t x, int16_t y, uint32_t color)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "setPixel without alloc!");
		return;
	}
	uint8_t r = (color>>16)&0xFF;
	uint8_t g = (color>>8)&0xFF;
	uint8_t b = color&0xFF;

	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return;
	if (x < 0) return;
	if (y >= FB_HEIGHT) return;
	if (y < 0) return;

	isDirty = true;
	if (x < dirty_x0) dirty_x0 = x;
	if (y < dirty_y0) dirty_y0 = y;
	if (x > dirty_x1) dirty_x1 = x;
	if (y > dirty_y1) dirty_y1 = y;

	uint32_t position = (y * FB_WIDTH * 3) + (x * 3);
	framebuffer[position + 0] = r;
	framebuffer[position + 1] = g;
	framebuffer[position + 2] = b;
}
uint32_t driver_framebuffer_getPixel(int16_t x, int16_t y)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "getPixel without alloc!");
		return 0;
	}
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return 0;
	if (x < 0) return 0;
	if (y >= FB_HEIGHT) return 0;
	if (y < 0) return 0;
	
	uint32_t position = (y * FB_WIDTH * 3) + (x * 3);
	return (framebuffer[position] << 16) + (framebuffer[position+1] << 8) + (framebuffer[position + 2]);
}
#elif defined(FB_TYPE_32BPP)
void driver_framebuffer_fill(uint32_t color)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "fill without alloc!");
		return;
	}
	isDirty = true;
	dirty_x0 = 0;
	dirty_y0 = 0;
	dirty_x1 = FB_WIDTH;
	dirty_y1 = FB_HEIGHT;
	uint8_t r = (color>>16)&0xFF;
	uint8_t g = (color>>8)&0xFF;
	uint8_t b = color&0xFF;
	uint8_t a = 0xFF;

	for (uint32_t i = 0; i < FB_SIZE; i+=4) {
		framebuffer[i + 0] = a;
		framebuffer[i + 1] = b;
		framebuffer[i + 2] = g;
		framebuffer[i + 3] = r;
	}
}

void driver_framebuffer_setPixel(int16_t x, int16_t y, uint32_t color)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "setPixel without alloc!");
		return;
	}
	uint8_t a = 0xFF;
	uint8_t r = (color>>16)&0xFF;
	uint8_t g = (color>>8)&0xFF;
	uint8_t b = color&0xFF;

	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return;
	if (x < 0) return;
	if (y >= FB_HEIGHT) return;
	if (y < 0) return;

	isDirty = true;
	if (x < dirty_x0) dirty_x0 = x;
	if (y < dirty_y0) dirty_y0 = y;
	if (x > dirty_x1) dirty_x1 = x;
	if (y > dirty_y1) dirty_y1 = y;

	uint32_t position = (y * FB_WIDTH * 4) + (x * 4);
	framebuffer[position + 0] = a;
	framebuffer[position + 1] = b;
	framebuffer[position + 2] = g;
	framebuffer[position + 3] = r;
}
uint32_t driver_framebuffer_getPixel(int16_t x, int16_t y)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "getPixel without alloc!");
		return 0;
	}
	if (orientation) { int16_t t = y; y = x; x = FB_WIDTH-t-1; }
	if (flip180)     { y = FB_HEIGHT-y-1;    x = FB_WIDTH-x-1; }
	if (x >= FB_WIDTH) return 0;
	if (x < 0) return 0;
	if (y >= FB_HEIGHT) return 0;
	if (y < 0) return 0;

	uint32_t position = (y * FB_WIDTH * 4) + (x * 4);
	return (framebuffer[position+3] << 16) + (framebuffer[position+2] << 8) + (framebuffer[position + 1]);
}
#else
#error "Framebuffer can not be enabled without valid output configuration."
#endif

void driver_framebuffer_get_dirty(int16_t* x0, int16_t* y0, int16_t* x1, int16_t* y1)
{
	*x0 = dirty_x0;
	*y0 = dirty_y0;
	*x1 = dirty_x1;
	*y1 = dirty_y1;

	if (flip180) {
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
	}
}

void driver_framebuffer_flush(uint32_t flags)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "flush without alloc!");
		return;
	}

	uint8_t eink_flags = 0;

	if (flags & FB_FLAG_FORCE) {
		isDirty = true;
	}
	if (flags & FB_FLAG_FULL || flags & FB_FLAG_FORCE) {
		dirty_x0 = 0;
		dirty_y0 = 0;
		dirty_x1 = FB_WIDTH-1;
		dirty_y1 = FB_HEIGHT-1;
		#ifdef DISPLAY_FLAG_LUT_BIT
			eink_flags |= DRIVER_EINK_LUT_FULL << DISPLAY_FLAG_LUT_BIT;
		#endif
	}

	if (!isDirty) return; //No need to update
	
	#ifdef CONFIG_DRIVER_HUB75_ENABLE
	compositor_disable();
	#endif
	
	#if defined(FB_TYPE_8BPP) && defined(DISPLAY_FLAG_8BITPIXEL)
		eink_flags |= DISPLAY_FLAG_8BITPIXEL;
	#endif

	if (dirty_x0 < 0) dirty_x0 = 0;
	if (dirty_x0 > FB_WIDTH) dirty_x0 = FB_WIDTH-1;
	if (dirty_x1 < 0) dirty_x1 = 0;
	if (dirty_x1 > FB_WIDTH) dirty_x1 = FB_WIDTH-1;
	if (dirty_y0 < 0) dirty_y0 = 0;
	if (dirty_y0 > FB_WIDTH) dirty_y0 = FB_HEIGHT-1;
	if (dirty_y1 < 0) dirty_y1 = 0;
	if (dirty_y1 > FB_WIDTH) dirty_y1 = FB_HEIGHT-1;

	#ifdef DISPLAY_FLAG_LUT_BIT
		if (flags & FB_FLAG_LUT_NORMAL) eink_flags |= DRIVER_EINK_LUT_NORMAL << DISPLAY_FLAG_LUT_BIT;
		if (flags & FB_FLAG_LUT_FAST) eink_flags |= DRIVER_EINK_LUT_FASTER << DISPLAY_FLAG_LUT_BIT;
		if (flags & FB_FLAG_LUT_FASTEST) eink_flags |= DRIVER_EINK_LUT_FASTEST << DISPLAY_FLAG_LUT_BIT;
	#endif

	#ifdef FB_FLUSH_GS
	if (flags & FB_FLAG_LUT_GREYSCALE) {
		FB_FLUSH_GS(framebuffer, eink_flags);
	} else {
	#endif
		FB_FLUSH(framebuffer,eink_flags,dirty_x0,dirty_y0,dirty_x1,dirty_y1);
	#ifdef FB_FLUSH_GS
	}
	#endif
	
	#ifdef CONFIG_DRIVER_FRAMEBUFFER_DOUBLE_BUFFERED
	if (framebuffer==framebuffer1) {
		framebuffer = framebuffer2;
		memcpy(framebuffer, framebuffer1, FB_SIZE);
	} else {
		framebuffer = framebuffer1;
		memcpy(framebuffer, framebuffer2, FB_SIZE);
	}
	#endif

	dirty_x0 = FB_WIDTH-1;
	dirty_y0 = FB_HEIGHT-1;
	dirty_x1 = 0;
	dirty_y1 = 0;
	isDirty = false;
}

esp_err_t driver_framebuffer_png(int16_t x, int16_t y, lib_reader_read_t reader, void* reader_p)
{
	if (!framebuffer) {
		ESP_LOGE(TAG, "png without alloc!");
		return ESP_FAIL;
	}
	struct lib_png_reader *pr = lib_png_new(reader, reader_p);
	if (pr == NULL) {
		printf("Out of memory.\n");
		return ESP_FAIL;
	}
	
	int res = lib_png_read_header(pr);
	if (res < 0) {
		lib_png_destroy(pr);
		printf("Can not read header.\n");
		return ESP_FAIL;
	}
	
	int width = pr->ihdr.width;
	int height = pr->ihdr.height;
	//int bit_depth = pr->ihdr.bit_depth;
	//int color_type = pr->ihdr.color_type;
	
	isDirty = true;
	if (x < dirty_x0) dirty_x0 = x;
	if (y < dirty_y0) dirty_y0 = y;
	if (x+width > dirty_x1) dirty_x1 = x+width;
	if (y+height > dirty_y1) dirty_y1 = y+height;
	
	uint32_t dst_min_x = x < 0 ? -x : 0;
	uint32_t dst_min_y = y < 0 ? -y : 0;
			
	res = lib_png_load_image(pr, x, y, dst_min_x, dst_min_y, FB_WIDTH - x, FB_HEIGHT - y, FB_WIDTH, 8, driver_framebuffer_setPixel);

	lib_png_destroy(pr);

	if (res < 0) {
		printf("Failed to load image.\n");
		return ESP_FAIL;
	}
	
	return ESP_OK;
}

uint16_t driver_framebuffer_getWidth(void)
{
	return orientation ? FB_HEIGHT : FB_WIDTH;
}

uint16_t driver_framebuffer_getHeight(void)
{
	return orientation ? FB_WIDTH : FB_HEIGHT;
}

#else
esp_err_t driver_framebuffer_init() { return ESP_OK; }
#endif
