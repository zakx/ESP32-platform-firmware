/*
 * The functions in this file allow for managing frames
 * frames are small buffers that represent the contents
 * of a window during one animation step
 */

#include "include/driver_framebuffer.h"
#include "include/driver_framebuffer_orientation.h"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

/* Variables */

/* Private functions */
inline struct Frame* _create_frame(struct Window* window)
{
	struct Frame* frame = malloc(sizeof(struct Frame));
	if (!frame) return NULL;
	memset(frame, 0, sizeof(struct Frame));
	#ifdef CONFIG_DRIVER_FRAMEBUFFER_SPIRAM
		frame->buffer                       = heap_caps_malloc((window->width*window->height*PIXEL_SIZE)/8, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		#ifdef FB_ALPHA_ENABLED
			if (frame->buffer) frame->alpha = heap_caps_malloc(window->width*window->height, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		#endif
	#else
		frame->buffer                       = heap_caps_malloc((window->width*window->height*PIXEL_SIZE)/8, MALLOC_CAP_8BIT);
		#ifdef FB_ALPHA_ENABLED
			if (frame->buffer) frame->alpha = heap_caps_malloc(window->width*window->height, MALLOC_CAP_8BIT);
		#endif
	#endif
	if (!frame->buffer) {
		free(frame);
		return NULL;
	}
	#ifdef FB_ALPHA_ENABLED
		if (!frame->alpha) {
			free(frame->buffer);
			free(frame);
			return NULL;
		}
	#endif
	return frame;
}

/* Public functions */

struct Frame* driver_framebuffer_add_frame_to_window(struct Window* window)
{
	if (!window) return NULL;
	struct Frame* lastFrame = window->firstFrame;
	while (lastFrame && lastFrame->_nextFrame) lastFrame = lastFrame->_nextFrame;
	struct Frame* newFrame = _create_frame(window);
	if (!newFrame) return NULL;
	newFrame->_prevFrame = lastFrame;
	if (lastFrame) lastFrame->_nextFrame = newFrame;
	return newFrame;
}

void driver_framebuffer_compositor_delete_frame(struct Frame* frame)
{
	if (frame == NULL) return;
	struct Frame* nextFrame = frame->_nextFrame;
	struct Frame* prevFrame = frame->_prevFrame;
	if (prevFrame) prevFrame->_nextFrame = nextFrame; //Link previous to next
	if (nextFrame) nextFrame->_prevFrame = prevFrame; //Link next to previous
	if (frame->buffer) free(frame->buffer);
	if (frame->alpha) free(frame->alpha);
	free(frame);
}

void driver_framebuffer_remove_all_frames_from_window(struct Window* window)
{
	if (!window) return NULL;
	struct Frame* frame = window->firstFrame;
	window->firstFrame = NULL;
	
	while (frame) {
		struct Frame* nextFrame = frame->_nextFrame;
		if (frame->buffer) free(frame->buffer);
		if (frame->alpha) free(frame->alpha);
		free(frame);
		frame = nextFrame;
	}
}

#endif /* CONFIG_DRIVER_FRAMEBUFFER_ENABLE */
