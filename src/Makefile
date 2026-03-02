CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -O2
TARGET = fieldofchaos

all: $(TARGET)

$(TARGET): fieldofchaos.c
	$(CC) $(CFLAGS) -o $(TARGET) fieldofchaos.c

clean:
	rm -f $(TARGET)

.PHONY: all clean
