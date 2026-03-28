#ifndef C200_VENDOR_H
#define C200_VENDOR_H

#include <stdbool.h>
#include <stdint.h>

#define C200_VENDOR_UNIT_ID 0x06
#define C200_VENDOR_MAX_PAYLOAD_SIZE 64

int c200_vendor_get_bool(int fd, uint8_t selector, bool *value);
int c200_vendor_set_bool(int fd, uint8_t selector, bool value);
int c200_vendor_get_u8(int fd, uint8_t selector, uint8_t *value);
int c200_vendor_set_u8(int fd, uint8_t selector, uint8_t value);
int c200_vendor_get_u16(int fd, uint8_t selector, uint16_t *value, uint16_t value_offset);
int c200_vendor_set_u16(int fd, uint8_t selector, uint16_t value, uint16_t value_offset);

#endif
