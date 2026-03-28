CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Werror -std=c11

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests

COMMON_SRCS := $(SRC_DIR)/c200_fov.c
COMMON_SRCS += $(SRC_DIR)/c200_vendor.c $(SRC_DIR)/c200_controls.c
CLI_SRCS := $(COMMON_SRCS) $(SRC_DIR)/c200_fov_cli.c
TEST_SRCS := $(COMMON_SRCS) $(TEST_DIR)/test_c200_fov.c

.PHONY: all test clean

all: $(BUILD_DIR)/anker-powerconf-c200-linux-tools

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/anker-powerconf-c200-linux-tools: $(CLI_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(CLI_SRCS) -o $@

$(BUILD_DIR)/test_c200_fov: $(TEST_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(TEST_SRCS) -o $@

test: $(BUILD_DIR)/test_c200_fov
	$(BUILD_DIR)/test_c200_fov

clean:
	rm -rf $(BUILD_DIR)
