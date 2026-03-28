#include "c200_controls.h"
#include "c200_fov.h"
#include "c200_vendor.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

static void usage(FILE *stream, const char *argv0)
{
	fprintf(stream, "Usage: %s [--device /dev/videoN] list-controls\n", argv0);
	fprintf(stream, "       %s [--device /dev/videoN] get <control>\n", argv0);
	fprintf(stream, "       %s [--device /dev/videoN] set <control> <value>\n", argv0);
	fprintf(stream, "Compatibility aliases:\n");
	fprintf(stream, "       %s [--device /dev/videoN] get-fov\n", argv0);
	fprintf(stream, "       %s [--device /dev/videoN] set-fov <65|78|95|narrow|medium|wide|raw>\n", argv0);
}

static int open_device(const char *path, int flags)
{
	int fd = open(path, flags);

	if (fd < 0) {
		fprintf(stderr, "failed to open %s: %s\n", path, strerror(errno));
	}

	return fd;
}

static int v4l2_get_value(int fd, uint32_t id, int32_t *value)
{
	struct v4l2_control control = {.id = id};

	if (value == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (ioctl(fd, VIDIOC_G_CTRL, &control) < 0) {
		return -1;
	}

	*value = control.value;
	return 0;
}

static int v4l2_set_value(int fd, uint32_t id, int32_t value)
{
	struct v4l2_control control = {.id = id, .value = value};

	return ioctl(fd, VIDIOC_S_CTRL, &control);
}

static int parse_i32(const char *text, int32_t *value)
{
	char *end = NULL;
	long parsed = 0;

	if (text == NULL || value == NULL) {
		return -1;
	}

	errno = 0;
	parsed = strtol(text, &end, 10);
	if (errno != 0 || end == text || *end != '\0') {
		errno = EINVAL;
		return -1;
	}

	*value = (int32_t)parsed;
	return 0;
}

static int handle_get_control(int fd, const struct c200_control_info *control)
{
	bool bool_value = false;
	uint8_t u8_value = 0;
	uint16_t u16_value = 0;
	int32_t v4l2_value = 0;

	switch (control->kind) {
	case C200_CONTROL_KIND_VENDOR_BOOL:
		if (c200_vendor_get_bool(fd, (uint8_t)control->id, &bool_value) < 0) {
			return -1;
		}
		printf("%s\n", c200_format_bool(bool_value));
		return 0;
	case C200_CONTROL_KIND_VENDOR_U8:
		if (c200_vendor_get_u8(fd, (uint8_t)control->id, &u8_value) < 0) {
			return -1;
		}
		printf("%u\n", u8_value);
		return 0;
	case C200_CONTROL_KIND_VENDOR_FOV:
		if (c200_fov_get(fd, &u16_value) < 0) {
			return -1;
		}
		printf("%s\n", c200_fov_describe_value(u16_value));
		return 0;
	case C200_CONTROL_KIND_V4L2_BOOL:
		if (v4l2_get_value(fd, control->id, &v4l2_value) < 0) {
			return -1;
		}
		printf("%s\n", c200_format_bool(v4l2_value != 0));
		return 0;
	case C200_CONTROL_KIND_V4L2_INT:
	case C200_CONTROL_KIND_V4L2_MENU:
		if (v4l2_get_value(fd, control->id, &v4l2_value) < 0) {
			return -1;
		}
		printf("%d\n", v4l2_value);
		return 0;
	}

	errno = EINVAL;
	return -1;
}

static int handle_set_control(int fd, const struct c200_control_info *control, const char *value_text)
{
	bool bool_value = false;
	uint8_t u8_value = 0;
	uint16_t u16_value = 0;
	int32_t v4l2_value = 0;

	switch (control->kind) {
	case C200_CONTROL_KIND_VENDOR_BOOL:
		if (!c200_parse_bool(value_text, &bool_value)) {
			errno = EINVAL;
			return -1;
		}
		return c200_vendor_set_bool(fd, (uint8_t)control->id, bool_value);
	case C200_CONTROL_KIND_VENDOR_U8:
		if (parse_i32(value_text, &v4l2_value) < 0 || v4l2_value < 0 || v4l2_value > 255) {
			errno = EINVAL;
			return -1;
		}
		u8_value = (uint8_t)v4l2_value;
		return c200_vendor_set_u8(fd, (uint8_t)control->id, u8_value);
	case C200_CONTROL_KIND_VENDOR_FOV:
		if (!c200_fov_parse_value(value_text, &u16_value)) {
			errno = EINVAL;
			return -1;
		}
		return c200_fov_set(fd, u16_value);
	case C200_CONTROL_KIND_V4L2_BOOL:
		if (!c200_parse_bool(value_text, &bool_value)) {
			errno = EINVAL;
			return -1;
		}
		return v4l2_set_value(fd, control->id, bool_value ? 1 : 0);
	case C200_CONTROL_KIND_V4L2_INT:
	case C200_CONTROL_KIND_V4L2_MENU:
		if (parse_i32(value_text, &v4l2_value) < 0) {
			errno = EINVAL;
			return -1;
		}
		return v4l2_set_value(fd, control->id, v4l2_value);
	}

	errno = EINVAL;
	return -1;
}

static void list_controls(void)
{
	size_t count = 0;
	size_t i = 0;
	const struct c200_control_info *controls = c200_all_controls(&count);

	for (i = 0; i < count; ++i) {
		printf("%-28s %s\n", controls[i].name, controls[i].description);
	}
}

int main(int argc, char **argv)
{
	const char *device = "/dev/video0";
	const char *command = NULL;
	const struct c200_control_info *control = NULL;
	const char *value_text = NULL;
	int argi = 1;
	int fd = -1;

	while (argi < argc && strcmp(argv[argi], "--device") == 0) {
		if (argi + 1 >= argc) {
			usage(stderr, argv[0]);
			return 2;
		}
		device = argv[argi + 1];
		argi += 2;
	}

	if (argi >= argc) {
		usage(stderr, argv[0]);
		return 2;
	}

	command = argv[argi++];
	if (strcmp(command, "list-controls") == 0) {
		if (argi != argc) {
			usage(stderr, argv[0]);
			return 2;
		}

		list_controls();
		return 0;
	}

	if (strcmp(command, "get-fov") == 0) {
		command = "get";
		control = c200_find_control("fov");
	}

	if (strcmp(command, "set-fov") == 0) {
		command = "set";
		control = c200_find_control("fov");
	}

	if (strcmp(command, "get") == 0) {
		if (control == NULL) {
			if (argi + 1 != argc) {
				usage(stderr, argv[0]);
				return 2;
			}
			control = c200_find_control(argv[argi]);
		}

		if (control == NULL) {
			fprintf(stderr, "unknown control: %s\n", argv[argi]);
			return 2;
		}

		fd = open_device(device, O_RDWR);
		if (fd < 0) {
			return 1;
		}

		if (handle_get_control(fd, control) < 0) {
			fprintf(stderr, "failed to read %s from %s: %s\n", control->name, device, strerror(errno));
			close(fd);
			return 1;
		}

		close(fd);
		return 0;
	}

	if (strcmp(command, "set") == 0) {
		if (control == NULL) {
			if (argi + 2 != argc) {
				usage(stderr, argv[0]);
				return 2;
			}
			control = c200_find_control(argv[argi]);
			value_text = argv[argi + 1];
		} else {
			if (argi + 1 != argc) {
				usage(stderr, argv[0]);
				return 2;
			}
			value_text = argv[argi];
		}

		if (control == NULL) {
			fprintf(stderr, "unknown control: %s\n", argv[argi]);
			return 2;
		}

		fd = open_device(device, O_RDWR);
		if (fd < 0) {
			return 1;
		}

		if (handle_set_control(fd, control, value_text) < 0) {
			fprintf(stderr, "failed to set %s on %s: %s\n", control->name, device, strerror(errno));
			close(fd);
			return 1;
		}

		if (handle_get_control(fd, control) < 0) {
			fprintf(stderr, "warning: set succeeded but readback for %s failed: %s\n", control->name, strerror(errno));
			close(fd);
			return 1;
		}

		close(fd);
		return 0;
	}

	usage(stderr, argv[0]);
	return 2;
}
