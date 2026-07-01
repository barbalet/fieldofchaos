CC ?= cc
AR ?= ar
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -Iinclude -Ilib

BUILD_DIR := build
LIB_OBJ := $(BUILD_DIR)/fieldofchaosgold.o
LIB := $(BUILD_DIR)/libfieldofchaosgold.a
CLI := $(BUILD_DIR)/fieldofchaosgold
TEST_SRCS := $(wildcard tests/test_*.c)
TEST_BINS := $(patsubst tests/%.c,$(BUILD_DIR)/%,$(TEST_SRCS))

.PHONY: all lib cli test clean

all: lib cli

lib: $(LIB)

cli: $(CLI)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_OBJ): lib/fieldofchaosgold.c include/fieldofchaosgold.h lib/fieldofchaosgold_internal.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c lib/fieldofchaosgold.c -o $@

$(LIB): $(LIB_OBJ)
	$(AR) rcs $@ $<

$(CLI): cli/fieldofchaosgold.c $(LIB) include/fieldofchaosgold.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) cli/fieldofchaosgold.c $(LIB) -o $@

$(BUILD_DIR)/test_%: tests/test_%.c $(LIB) include/fieldofchaosgold.h lib/fieldofchaosgold_internal.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LIB) -o $@

test: $(TEST_BINS)
	@for test_binary in $(TEST_BINS); do \
		./$$test_binary || exit 1; \
	done

clean:
	rm -rf $(BUILD_DIR)
