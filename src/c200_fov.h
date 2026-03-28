#ifndef C200_FOV_H
#define C200_FOV_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define C200_FOV_UNIT_ID 0x06
#define C200_FOV_SELECTOR 0x10
#define C200_FOV_PAYLOAD_SIZE 64

int c200_fov_get(int fd, uint16_t *value);
int c200_fov_set(int fd, uint16_t value);

void c200_fov_encode_set_payload(uint8_t payload[C200_FOV_PAYLOAD_SIZE], uint16_t value);
uint16_t c200_fov_decode_payload(const uint8_t payload[C200_FOV_PAYLOAD_SIZE]);
bool c200_fov_parse_value(const char *text, uint16_t *value);
const char *c200_fov_describe_value(uint16_t value);

#endif
