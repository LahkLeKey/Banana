#ifndef BANANA_NATIVE_UI_ABI_H
#define BANANA_NATIVE_UI_ABI_H

#include <stdint.h>
#include <string.h>

typedef enum banana_native_ui_host_kind {
	BANANA_NATIVE_UI_HOST_WEB = 0,
	BANANA_NATIVE_UI_HOST_ELECTRON = 1,
	BANANA_NATIVE_UI_HOST_REACT_NATIVE = 2,
	BANANA_NATIVE_UI_HOST_NATIVE = 3,
} banana_native_ui_host_kind;

typedef enum banana_native_ui_surface_kind {
	BANANA_NATIVE_UI_SURFACE_GAME_ENGINE = 0,
} banana_native_ui_surface_kind;

typedef enum banana_native_ui_engine_status {
	BANANA_NATIVE_UI_STATUS_IDLE = 0,
	BANANA_NATIVE_UI_STATUS_LOADING = 1,
	BANANA_NATIVE_UI_STATUS_RUNNING = 2,
	BANANA_NATIVE_UI_STATUS_ERROR = 3,
	BANANA_NATIVE_UI_STATUS_UNAVAILABLE = 4,
} banana_native_ui_engine_status;

typedef enum banana_native_ui_movement_source {
	BANANA_NATIVE_UI_MOVEMENT_NONE = 0,
	BANANA_NATIVE_UI_MOVEMENT_KEYBOARD = 1,
	BANANA_NATIVE_UI_MOVEMENT_RADIAL = 2,
} banana_native_ui_movement_source;

typedef struct banana_native_ui_viewport {
	int css_width;
	int css_height;
	int pixel_width;
	int pixel_height;
	float device_pixel_ratio;
} banana_native_ui_viewport;

typedef struct banana_native_ui_frame {
	int host;
	int surface;
	int engine_status;
	char error[256];
	int movement_source;
	float move_x;
	float move_z;
	banana_native_ui_viewport viewport;
	char interaction_message[256];
	int64_t timestamp_ms;
} banana_native_ui_frame;

static inline void banana_native_ui_frame_reset(banana_native_ui_frame *frame)
{
	if (!frame) {
		return;
	}

	memset(frame, 0, sizeof(*frame));
}

static inline void banana_native_ui_frame_write(
	banana_native_ui_frame *frame,
	banana_native_ui_host_kind host,
	banana_native_ui_surface_kind surface,
	banana_native_ui_engine_status engine_status,
	const char *error,
	banana_native_ui_movement_source movement_source,
	float move_x,
	float move_z,
	banana_native_ui_viewport viewport,
	const char *interaction_message,
	int64_t timestamp_ms)
{
	if (!frame) {
		return;
	}

	frame->host = (int)host;
	frame->surface = (int)surface;
	frame->engine_status = (int)engine_status;
	frame->movement_source = (int)movement_source;
	frame->move_x = move_x;
	frame->move_z = move_z;
	frame->viewport = viewport;
	frame->timestamp_ms = timestamp_ms;

	if (error && error[0] != '\0') {
		strncpy(frame->error, error, sizeof(frame->error) - 1);
		frame->error[sizeof(frame->error) - 1] = '\0';
	} else {
		frame->error[0] = '\0';
	}

	if (interaction_message && interaction_message[0] != '\0') {
		strncpy(frame->interaction_message, interaction_message, sizeof(frame->interaction_message) - 1);
		frame->interaction_message[sizeof(frame->interaction_message) - 1] = '\0';
	} else {
		frame->interaction_message[0] = '\0';
	}
}

#endif