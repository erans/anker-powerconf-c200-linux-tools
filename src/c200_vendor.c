#include "c200_vendor.h"

#include <errno.h>
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>
#include <string.h>
#include <sys/ioctl.h>

static int c200_vendor_query(int fd, uint8_t selector, uint8_t query, uint16_t size, uint8_t *payload)
{
	struct uvc_xu_control_query ctrl = {
		.unit = C200_VENDOR_UNIT_ID,
		.selector = selector,
		.query = query,
		.size = size,
		.data = payload,
	};

	return ioctl(fd, UVCIOC_CTRL_QUERY, &ctrl);
}

static int c200_vendor_get_length(int fd, uint8_t selector, uint16_t *size)
{
	uint8_t len_bytes[2] = {0};

	if (size == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (c200_vendor_query(fd, selector, UVC_GET_LEN, sizeof(len_bytes), len_bytes) < 0) {
		return -1;
	}

	*size = (uint16_t)len_bytes[0] | ((uint16_t)len_bytes[1] << 8);
	if (*size == 0 || *size > C200_VENDOR_MAX_PAYLOAD_SIZE) {
		errno = EMSGSIZE;
		return -1;
	}

	return 0;
}

static int c200_vendor_get_payload(int fd, uint8_t selector, uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE], uint16_t *size)
{
	if (c200_vendor_get_length(fd, selector, size) < 0) {
		return -1;
	}

	memset(payload, 0, C200_VENDOR_MAX_PAYLOAD_SIZE);
	return c200_vendor_query(fd, selector, UVC_GET_CUR, *size, payload);
}

static int c200_vendor_set_payload(int fd, uint8_t selector, uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE], uint16_t size)
{
	return c200_vendor_query(fd, selector, UVC_SET_CUR, size, payload);
}

int c200_vendor_get_bool(int fd, uint8_t selector, bool *value)
{
	uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE];
	uint16_t size = 0;

	if (value == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (c200_vendor_get_payload(fd, selector, payload, &size) < 0) {
		return -1;
	}

	*value = payload[0] == 0x01;
	return 0;
}

int c200_vendor_set_bool(int fd, uint8_t selector, bool value)
{
	uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE];
	uint16_t size = 0;

	if (c200_vendor_get_payload(fd, selector, payload, &size) < 0) {
		return -1;
	}

	memset(payload, 0, C200_VENDOR_MAX_PAYLOAD_SIZE);
	payload[0] = value ? 0x01 : 0x00;
	return c200_vendor_set_payload(fd, selector, payload, size);
}

int c200_vendor_get_u8(int fd, uint8_t selector, uint8_t *value)
{
	uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE];
	uint16_t size = 0;

	if (value == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (c200_vendor_get_payload(fd, selector, payload, &size) < 0) {
		return -1;
	}

	*value = payload[0];
	return 0;
}

int c200_vendor_set_u8(int fd, uint8_t selector, uint8_t value)
{
	uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE];
	uint16_t size = 0;

	if (c200_vendor_get_payload(fd, selector, payload, &size) < 0) {
		return -1;
	}

	memset(payload, 0, C200_VENDOR_MAX_PAYLOAD_SIZE);
	payload[0] = value;
	return c200_vendor_set_payload(fd, selector, payload, size);
}

int c200_vendor_get_u16(int fd, uint8_t selector, uint16_t *value, uint16_t value_offset)
{
	uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE];
	uint16_t size = 0;

	if (value == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (value_offset + 1 >= C200_VENDOR_MAX_PAYLOAD_SIZE) {
		errno = EINVAL;
		return -1;
	}

	if (c200_vendor_get_payload(fd, selector, payload, &size) < 0) {
		return -1;
	}

	*value = (uint16_t)payload[value_offset] | ((uint16_t)payload[value_offset + 1] << 8);
	return 0;
}

int c200_vendor_set_u16(int fd, uint8_t selector, uint16_t value, uint16_t value_offset)
{
	uint8_t payload[C200_VENDOR_MAX_PAYLOAD_SIZE];
	uint16_t size = 0;

	if (value_offset + 1 >= C200_VENDOR_MAX_PAYLOAD_SIZE) {
		errno = EINVAL;
		return -1;
	}

	if (c200_vendor_get_payload(fd, selector, payload, &size) < 0) {
		return -1;
	}

	memset(payload, 0, C200_VENDOR_MAX_PAYLOAD_SIZE);
	payload[0] = 0x00;
	payload[1] = 0x01;
	payload[value_offset] = value & 0xff;
	payload[value_offset + 1] = value >> 8;
	return c200_vendor_set_payload(fd, selector, payload, size);
}
