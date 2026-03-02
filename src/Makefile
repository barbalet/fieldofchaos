CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -O2
TARGET = fieldofchaos
CHAR_TARGET = character_create

all: $(TARGET) $(CHAR_TARGET)

$(TARGET): fieldofchaos.c
	$(CC) $(CFLAGS) -o $(TARGET) fieldofchaos.c

$(CHAR_TARGET): character_create.c
	$(CC) $(CFLAGS) -o $(CHAR_TARGET) character_create.c

clean:
	rm -f $(TARGET) $(CHAR_TARGET)

.PHONY: all clean
