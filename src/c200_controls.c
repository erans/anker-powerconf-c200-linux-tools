#include "c200_controls.h"

#include <time.h>
#include <linux/videodev2.h>
#include <strings.h>

static const struct c200_control_info C200_CONTROLS[] = {
	{"fov", C200_CONTROL_KIND_VENDOR_FOV, 0x10, "Vendor FOV preset/value"},
	{"hdr", C200_CONTROL_KIND_VENDOR_BOOL, 0x13, "Vendor HDR toggle"},
	{"horizontal_flip", C200_CONTROL_KIND_VENDOR_BOOL, 0x11, "Vendor horizontal mirror"},
	{"vertical_screen", C200_CONTROL_KIND_VENDOR_BOOL, 0x0a, "Vendor vertical screen mode"},
	{"anti_flicker", C200_CONTROL_KIND_VENDOR_U8, 0x12, "Vendor anti-flicker mode"},
	{"brightness", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_BRIGHTNESS, "Standard UVC brightness"},
	{"contrast", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_CONTRAST, "Standard UVC contrast"},
	{"saturation", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_SATURATION, "Standard UVC saturation"},
	{"hue", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_HUE, "Standard UVC hue"},
	{"white_balance_automatic", C200_CONTROL_KIND_V4L2_BOOL, V4L2_CID_AUTO_WHITE_BALANCE, "Standard UVC auto white balance"},
	{"gamma", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_GAMMA, "Standard UVC gamma"},
	{"power_line_frequency", C200_CONTROL_KIND_V4L2_MENU, V4L2_CID_POWER_LINE_FREQUENCY, "Standard UVC power-line frequency"},
	{"white_balance_temperature", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_WHITE_BALANCE_TEMPERATURE, "Standard UVC white balance temperature"},
	{"sharpness", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_SHARPNESS, "Standard UVC sharpness"},
	{"auto_exposure", C200_CONTROL_KIND_V4L2_MENU, V4L2_CID_EXPOSURE_AUTO, "Standard UVC auto exposure mode"},
	{"exposure_time_absolute", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_EXPOSURE_ABSOLUTE, "Standard UVC exposure time"},
	{"pan_absolute", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_PAN_ABSOLUTE, "Standard UVC pan"},
	{"tilt_absolute", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_TILT_ABSOLUTE, "Standard UVC tilt"},
	{"focus_absolute", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_FOCUS_ABSOLUTE, "Standard UVC focus"},
	{"focus_automatic_continuous", C200_CONTROL_KIND_V4L2_BOOL, V4L2_CID_FOCUS_AUTO, "Standard UVC autofocus"},
	{"zoom_absolute", C200_CONTROL_KIND_V4L2_INT, V4L2_CID_ZOOM_ABSOLUTE, "Standard UVC zoom"},
};

const struct c200_control_info *c200_find_control(const char *name)
{
	size_t i = 0;

	for (i = 0; i < sizeof(C200_CONTROLS) / sizeof(C200_CONTROLS[0]); ++i) {
		if (strcasecmp(C200_CONTROLS[i].name, name) == 0) {
			return &C200_CONTROLS[i];
		}
	}

	return NULL;
}

const struct c200_control_info *c200_all_controls(size_t *count)
{
	if (count != NULL) {
		*count = sizeof(C200_CONTROLS) / sizeof(C200_CONTROLS[0]);
	}

	return C200_CONTROLS;
}

bool c200_parse_bool(const char *text, bool *value)
{
	if (text == NULL || value == NULL) {
		return false;
	}

	if (strcasecmp(text, "1") == 0 || strcasecmp(text, "on") == 0 || strcasecmp(text, "true") == 0 || strcasecmp(text, "yes") == 0) {
		*value = true;
		return true;
	}

	if (strcasecmp(text, "0") == 0 || strcasecmp(text, "off") == 0 || strcasecmp(text, "false") == 0 || strcasecmp(text, "no") == 0) {
		*value = false;
		return true;
	}

	return false;
}

const char *c200_format_bool(bool value)
{
	return value ? "on" : "off";
}
