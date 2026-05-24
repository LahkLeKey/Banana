#include "../../../src/native/include/banana_native_ui_abi.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
	if (actual == expected)
		return 1;

	fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
	return 0;
}

int main(void)
{
	banana_native_ui_frame frame;
	banana_native_ui_viewport viewport = {1280, 720, 2560, 1440, 2.0f};

	banana_native_ui_frame_reset(&frame);
	if (!expect_int("frame reset host", frame.host, 0))
		return 1;

	banana_native_ui_frame_write(
		&frame,
		BANANA_NATIVE_UI_HOST_ELECTRON,
		BANANA_NATIVE_UI_SURFACE_GAME_ENGINE,
		BANANA_NATIVE_UI_STATUS_RUNNING,
		"",
		BANANA_NATIVE_UI_MOVEMENT_KEYBOARD,
		1.0f,
		-1.0f,
		viewport,
		"ready",
		42);

	if (!expect_int("host", frame.host, BANANA_NATIVE_UI_HOST_ELECTRON))
		return 1;
	if (!expect_int("surface", frame.surface, BANANA_NATIVE_UI_SURFACE_GAME_ENGINE))
		return 1;
	if (!expect_int("status", frame.engine_status, BANANA_NATIVE_UI_STATUS_RUNNING))
		return 1;
	if (!expect_int("movement source", frame.movement_source, BANANA_NATIVE_UI_MOVEMENT_KEYBOARD))
		return 1;
	if (!expect_int("viewport width", frame.viewport.css_width, 1280))
		return 1;
	if (!expect_int("viewport pixel width", frame.viewport.pixel_width, 2560))
		return 1;
	if (!expect_int("timestamp", (int)frame.timestamp_ms, 42))
		return 1;

	return 0;
}