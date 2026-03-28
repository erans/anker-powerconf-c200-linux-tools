#include "c200_fov.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void test_encode_set_payload(void)
{
	uint8_t payload[C200_FOV_PAYLOAD_SIZE];

	memset(payload, 0xaa, sizeof(payload));
	c200_fov_encode_set_payload(payload, 95);

	assert(payload[0] == 0x00);
	assert(payload[1] == 0x01);
	assert(payload[2] == 0x5f);
	assert(payload[3] == 0x00);
	assert(payload[4] == 0x00);
	assert(payload[5] == 0x00);
	assert(payload[63] == 0x00);
}

static void test_decode_payload(void)
{
	uint8_t payload[C200_FOV_PAYLOAD_SIZE] = {0};

	payload[2] = 78;
	payload[3] = 0;
	assert(c200_fov_decode_payload(payload) == 78);
}

static void test_parse_presets(void)
{
	uint16_t value = 0;

	assert(c200_fov_parse_value("65", &value));
	assert(value == 65);

	assert(c200_fov_parse_value("narrow", &value));
	assert(value == 65);

	assert(c200_fov_parse_value("medium", &value));
	assert(value == 78);

	assert(c200_fov_parse_value("wide", &value));
	assert(value == 95);

	assert(!c200_fov_parse_value("banana", &value));
}

static void test_describe_value(void)
{
	assert(strcmp(c200_fov_describe_value(65), "65 (narrow)") == 0);
	assert(strcmp(c200_fov_describe_value(78), "78 (medium)") == 0);
	assert(strcmp(c200_fov_describe_value(95), "95 (wide)") == 0);
	assert(strcmp(c200_fov_describe_value(123), "123") == 0);
}

int main(void)
{
	test_encode_set_payload();
	test_decode_payload();
	test_parse_presets();
	test_describe_value();
	return 0;
}
