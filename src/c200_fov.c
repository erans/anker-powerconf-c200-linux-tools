#include "c200_fov.h"
#include "c200_vendor.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int c200_fov_get(int fd, uint16_t *value)
{
	return c200_vendor_get_u16(fd, C200_FOV_SELECTOR, value, 2);
}

int c200_fov_set(int fd, uint16_t value)
{
	return c200_vendor_set_u16(fd, C200_FOV_SELECTOR, value, 2);
}

void c200_fov_encode_set_payload(uint8_t payload[C200_FOV_PAYLOAD_SIZE], uint16_t value)
{
	memset(payload, 0, C200_FOV_PAYLOAD_SIZE);
	payload[0] = 0x00;
	payload[1] = 0x01;
	payload[2] = value & 0xff;
	payload[3] = value >> 8;
	payload[4] = 0x00;
}

uint16_t c200_fov_decode_payload(const uint8_t payload[C200_FOV_PAYLOAD_SIZE])
{
	return (uint16_t)payload[2] | ((uint16_t)payload[3] << 8);
}

bool c200_fov_parse_value(const char *text, uint16_t *value)
{
	char *end = NULL;
	unsigned long parsed = 0;

	if (text == NULL || value == NULL) {
		return false;
	}

	if (strcasecmp(text, "narrow") == 0) {
		*value = 65;
		return true;
	}

	if (strcasecmp(text, "medium") == 0) {
		*value = 78;
		return true;
	}

	if (strcasecmp(text, "wide") == 0) {
		*value = 95;
		return true;
	}

	errno = 0;
	parsed = strtoul(text, &end, 10);
	if (errno != 0 || end == text || *end != '\0' || parsed > UINT16_MAX) {
		return false;
	}

	*value = (uint16_t)parsed;
	return true;
}

const char *c200_fov_describe_value(uint16_t value)
{
	static char buffer[32];

	switch (value) {
	case 65:
		return "65 (narrow)";
	case 78:
		return "78 (medium)";
	case 95:
		return "95 (wide)";
	default:
		snprintf(buffer, sizeof(buffer), "%u", value);
		return buffer;
	}
}
